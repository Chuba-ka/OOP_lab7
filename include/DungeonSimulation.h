#pragma once

#include "DungeonEditor.h"
#include "Observer.h"
#include "FightVisitor.h"
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <unordered_set>
#include <sstream>
#include <cmath>
#include <unordered_map>

struct FightTask
{
    NPC *a;
    NPC *b;
    FightTask(NPC *aa = nullptr, NPC *bb = nullptr) : a(aa), b(bb) {}
};

class DungeonSimulation
{
private:
    using clock = std::chrono::steady_clock;

    DungeonEditor editor;
    ConsoleObserver console;
    FileObserver fileLog;

    std::queue<FightTask> tasks;
    std::mutex tasks_mutex;
    std::condition_variable tasks_cv;
    std::unordered_set<std::uintptr_t> tasks_keys;

    std::atomic<bool> stopFlag{false};

    std::unordered_map<std::uintptr_t, clock::time_point> fight_cooldowns;
    std::mutex cooldowns_mutex;
    const std::chrono::milliseconds FIGHT_COOLDOWN_MS = std::chrono::milliseconds(1000);

    std::thread mover_thread;
    std::thread fighter_thread;

    void initializeNPCs(int count);
    void moverLogic();
    void fighterLogic();
    void printMap(int seconds_left);

    std::uintptr_t pair_key(NPC *a, NPC *b) const
    {
        auto pa = reinterpret_cast<std::uintptr_t>(a);
        auto pb = reinterpret_cast<std::uintptr_t>(b);
        if (pa < pb)
            return (pa << 32) ^ pb;
        return (pb << 32) ^ pa;
    }

public:
    DungeonSimulation();
    ~DungeonSimulation();
    void run(std::chrono::seconds duration);
};