#define _USE_MATH_DEFINES

#include "DungeonSimulation.h"
#include <iostream>
#include <algorithm>
#include <thread>

extern std::mutex g_cout_mutex;

using namespace std::chrono_literals;

DungeonSimulation::DungeonSimulation()
{
    editor.addObserver(&console);
    editor.addObserver(&fileLog);
    initializeNPCs(50);
}

DungeonSimulation::~DungeonSimulation()
{
    if (mover_thread.joinable())
        mover_thread.join();
    if (fighter_thread.joinable())
        fighter_thread.join();
}

void DungeonSimulation::initializeNPCs(int count)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist_pos_x(0, DungeonEditor::MAP_WIDTH - 1);
    std::uniform_int_distribution<int> dist_pos_y(0, DungeonEditor::MAP_HEIGHT - 1);
    std::uniform_int_distribution<int> dist_type(0, 2);

    const std::vector<std::string> types = {
        "Странствующий рыцарь",
        "Эльф",
        "Друид"};

    for (int i = 0; i < count; ++i)
    {
        std::ostringstream name;
        name << "NPC" << i;
        editor.addNPC(types[dist_type(rng)], name.str(), dist_pos_x(rng), dist_pos_y(rng));
    }
}

// поток Mover
void DungeonSimulation::moverLogic()
{
    std::mt19937 loc_rng(std::random_device{}());
    std::uniform_real_distribution<double> dist_angle(0.0, 2.0 * M_PI);

    while (!stopFlag.load())
    {
        {
            std::lock_guard<std::shared_mutex> lock(editor.npcs_mutex);
            auto &npcs = editor.accessNPCs();

            for (auto &u : npcs)
            {
                if (!u->isAlive())
                    continue;

                int R = u->getMoveDistance();
                double angle = dist_angle(loc_rng);
                int dx = static_cast<int>(R * std::cos(angle));
                int dy = static_cast<int>(R * std::sin(angle));

                int nx = u->getX() + dx;
                int ny = u->getY() + dy;

                nx = std::max(0, std::min(nx, DungeonEditor::MAP_WIDTH - 1));
                ny = std::max(0, std::min(ny, DungeonEditor::MAP_HEIGHT - 1));

                u->setPosition(nx, ny);
            }

            for (size_t i = 0; i < npcs.size(); ++i)
            {
                NPC *a = npcs[i].get();
                if (!a->isAlive())
                    continue;

                for (size_t j = i + 1; j < npcs.size(); ++j)
                {
                    NPC *b = npcs[j].get();
                    if (!b->isAlive())
                        continue;

                    double effective_kill_distance = std::max(a->getKillDistance(), b->getKillDistance());

                    if (a->distanceTo(b) <= effective_kill_distance)
                    {
                        std::uintptr_t key = pair_key(a, b);

                        {
                            std::lock_guard<std::mutex> clock(cooldowns_mutex);
                            if (fight_cooldowns.count(key) &&
                                fight_cooldowns.at(key) > clock::now())
                            {
                                continue;
                            }
                        }

                        {
                            std::lock_guard<std::mutex> tlock(tasks_mutex);
                            if (tasks_keys.find(key) == tasks_keys.end())
                            {
                                tasks.emplace(a, b);
                                tasks_keys.insert(key);
                                tasks_cv.notify_one();
                            }
                        }
                    }
                }
            }
        }

        std::this_thread::sleep_for(100ms);
    }
}

// поток Fighter
void DungeonSimulation::fighterLogic()
{
    std::set<NPC *> local_dead;
    std::vector<Observer *> obs = editor.accessObservers();
    FightVisitor visitor(local_dead, obs);

    while (!stopFlag.load() || !tasks.empty())
    {
        FightTask task;
        {
            std::unique_lock<std::mutex> tlock(tasks_mutex);
            tasks_cv.wait_for(tlock, 200ms, [this]()
                              { return !tasks.empty() || stopFlag.load(); });

            if (tasks.empty())
            {
                if (stopFlag.load())
                    break;
                continue;
            }
            task = tasks.front();
            tasks.pop();
            tasks_keys.erase(pair_key(task.a, task.b));
        }

        {
            std::shared_lock<std::shared_mutex> rlock(editor.npcs_mutex);
            if (!task.a->isAlive() || !task.b->isAlive())
                continue;
        }

        local_dead.clear();

        {
            std::shared_lock<std::shared_mutex> rlock(editor.npcs_mutex);
            visitor.fight_pair(*task.a, *task.b);
        }

        if (!local_dead.empty())
        {
            std::lock_guard<std::shared_mutex> wlock(editor.npcs_mutex);
            for (NPC *p : local_dead)
            {
                if (p->isAlive())
                {
                    p->kill();
                }
            }
        }
        else
        {
            std::lock_guard<std::mutex> clock(cooldowns_mutex);
            std::uintptr_t key = pair_key(task.a, task.b);

            fight_cooldowns[key] = clock::now() + FIGHT_COOLDOWN_MS;
        }
    }
}

void DungeonSimulation::printMap(int seconds_left)
{
    std::lock_guard<std::mutex> cout_lock(g_cout_mutex);
    std::cout << "\n=== MAP (" << seconds_left << "s left) ===\n";

    std::shared_lock<std::shared_mutex> lock(editor.npcs_mutex);
    int count = 0;
    for (const auto &u : editor.accessNPCs())
    {
        if (!u->isAlive())
            continue;
        ++count;
        std::cout << u->getType() << " | " << u->getName()
                  << " | (" << u->getX() << ", " << u->getY() << ")\n";
    }
    if (count == 0)
        std::cout << "(пусто)\n";
    std::cout << "=========================================\n";
}

// Основной цикл
void DungeonSimulation::run(std::chrono::seconds duration)
{
    mover_thread = std::thread(&DungeonSimulation::moverLogic, this);
    fighter_thread = std::thread(&DungeonSimulation::fighterLogic, this);

    const auto start = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - start < duration)
    {
        auto elapsed = std::chrono::steady_clock::now() - start;
        printMap(duration.count() - std::chrono::duration_cast<std::chrono::seconds>(elapsed).count());
        std::this_thread::sleep_for(1s);
    }

    stopFlag.store(true);
    tasks_cv.notify_all();

    {
        std::lock_guard<std::mutex> cout_lock(g_cout_mutex);
        std::cout << "\n=== Выжившие NPC ===\n";
        std::shared_lock<std::shared_mutex> lock(editor.npcs_mutex);
        for (const auto &u : editor.accessNPCs())
        {
            if (!u->isAlive())
                continue;
            std::cout << u->getType() << " | " << u->getName()
                      << " | (" << u->getX() << ", " << u->getY() << ")\n";
        }
        std::cout << "====================\n";
    }

    editor.saveToFile("dungeon_save.txt");
}