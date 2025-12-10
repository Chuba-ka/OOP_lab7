#pragma once
#include "NPC.h"
#include "Observer.h"
#include <memory>
#include <vector>
#include <set>
#include <random>
#include <shared_mutex>

class FightVisitor
{
private:
    std::set<NPC *> &dead;
    std::vector<Observer *> &observers;

    std::mt19937 rng;
    std::uniform_int_distribution<int> dice;

    void notify(const std::string &msg);

public:
    FightVisitor(std::set<NPC *> &d, std::vector<Observer *> &o)
        : dead(d), observers(o), rng(std::random_device{}()), dice(1, 6) {}

    virtual void visit(Knight &k) {}
    virtual void visit(Druid &d) {}
    virtual void visit(Elf &e) {}

    virtual ~FightVisitor() = default;

    void fight_pair(NPC &a, NPC &b);
};