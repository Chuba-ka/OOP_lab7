#pragma once
#include "NPC.h"
#include "Factory.h"
#include "Observer.h"
#include <vector>
#include <memory>
#include <set>
#include <shared_mutex>

class DungeonEditor
{
public:
    DungeonEditor() = default;
    ~DungeonEditor() = default;

    void addObserver(Observer *obs);

    bool addNPC(const std::string &type, const std::string &name, int x, int y);

    void printAll() const;

    void saveToFile(const std::string &filename) const;
    void loadFromFile(const std::string &filename);

    std::vector<std::unique_ptr<NPC>> &accessNPCs() { return npcs; }
    const std::vector<Observer *> &accessObservers() const { return observers; }

    mutable std::shared_mutex npcs_mutex;

    static constexpr int MAP_WIDTH = 200;
    static constexpr int MAP_HEIGHT = 200;

private:
    std::vector<std::unique_ptr<NPC>> npcs;
    std::vector<Observer *> observers;
    NPCFactory factory;

    bool isNameUnique(const std::string &name) const;

    void notifyObservers(const std::string &msg);
};