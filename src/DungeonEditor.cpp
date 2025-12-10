#include "DungeonEditor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

bool DungeonEditor::isNameUnique(const std::string &name) const
{
    std::shared_lock<std::shared_mutex> lock(npcs_mutex);
    return std::none_of(npcs.begin(), npcs.end(),
                        [&name](const auto &npc)
                        { return npc->getName() == name; });
}

void DungeonEditor::notifyObservers(const std::string &msg)
{
    for (auto *obs : observers)
    {
        obs->onKill(msg);
    }
}

void DungeonEditor::addObserver(Observer *obs)
{
    observers.push_back(obs);
}

bool DungeonEditor::addNPC(const std::string &type, const std::string &name, int x, int y)
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
    {
        std::cout << "Координаты вне допустимого диапазона!\n";
        return false;
    }
    if (!isNameUnique(name))
    {
        std::cout << "Имя уже занято!\n";
        return false;
    }

    auto npc = factory.create(type, name, x, y);
    if (!npc)
    {
        std::cout << "Неизвестный тип NPC: " << type << "\n";
        return false;
    }

    {
        std::lock_guard<std::shared_mutex> lock(npcs_mutex);
        npcs.push_back(std::move(npc));
    }
    return true;
}

void DungeonEditor::printAll() const
{
    std::shared_lock<std::shared_mutex> lock(npcs_mutex);
    std::cout << "=== Список NPC ===\n";
    bool any = false;
    for (const auto &npc : npcs)
    {
        if (!npc->isAlive())
            continue;
        any = true;
        std::cout << npc->getType() << " | " << npc->getName()
                  << " | (" << npc->getX() << ", " << npc->getY() << ")\n";
    }
    if (!any)
    {
        std::cout << "(пусто)\n";
    }
    std::cout << "==================\n";
}

void DungeonEditor::saveToFile(const std::string &filename) const
{
    std::shared_lock<std::shared_mutex> lock(npcs_mutex);
    std::ofstream f(filename);
    if (!f)
    {
        std::cout << "Ошибка записи файла: " << filename << "\n";
        return;
    }
    for (const auto &npc : npcs)
    {
        if (!npc->isAlive())
            continue;
        f << '"' << npc->getType() << "\" "
          << npc->getName() << " "
          << npc->getX() << " "
          << npc->getY() << '\n';
    }
    std::cout << "Сохранено в " << filename << "\n";
}

void DungeonEditor::loadFromFile(const std::string &filename)
{
    std::ifstream f(filename);
    if (!f)
    {
        std::cout << "Файл не найден: " << filename << "\n";
        return;
    }

    std::string line;
    int count = 0;

    while (std::getline(f, line))
    {
        if (line.empty())
            continue;

        size_t start = line.find('"');
        size_t end = line.find('"', start + 1);
        if (start == std::string::npos || end == std::string::npos)
            continue;

        std::string type = line.substr(start + 1, end - start - 1);
        std::istringstream rest(line.substr(end + 1));

        std::string name;
        int x, y;
        if (rest >> name >> x >> y)
        {
            if (addNPC(type, name, x, y))
            {
                count++;
            }
        }
    }

    std::cout << "Загружено " << count << " NPC из " << filename << "\n";
}