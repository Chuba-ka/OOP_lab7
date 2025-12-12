#include "FightVisitor.h"
#include <iostream>
#include <string>

void FightVisitor::notify(const std::string &msg)
{
    for (Observer *obs : observers)
    {
        obs->onKill(msg);
    }
}

void FightVisitor::fight_pair(NPC &a, NPC &b)
{

    bool a_can_kill_b = false;
    bool b_can_kill_a = false;

    if (dynamic_cast<Knight *>(&a) && dynamic_cast<Elf *>(&b))
        a_can_kill_b = true;
    if (dynamic_cast<Knight *>(&b) && dynamic_cast<Elf *>(&a))
        b_can_kill_a = true;

    if (dynamic_cast<Elf *>(&a) && (dynamic_cast<Druid *>(&b) || dynamic_cast<Knight *>(&b)))
        a_can_kill_b = true;
    if (dynamic_cast<Elf *>(&b) && (dynamic_cast<Druid *>(&a) || dynamic_cast<Knight *>(&a)))
        b_can_kill_a = true;

    if (dynamic_cast<Druid *>(&a) && dynamic_cast<Druid *>(&b))
    {
        a_can_kill_b = true;
        b_can_kill_a = true;
    }

    if (!a_can_kill_b && !b_can_kill_a)
        return;

    int a_roll_attack = 0;
    int b_roll_defense = 0;
    int b_roll_attack = 0;
    int a_roll_defense = 0;

    bool a_kills_b = false;
    bool b_kills_a = false;

    if (a_can_kill_b)
    {
        a_roll_attack = dice(rng);
        b_roll_defense = dice(rng);
        if (a_roll_attack > b_roll_defense)
            a_kills_b = true;
    }

    if (b_can_kill_a)
    {
        b_roll_attack = dice(rng);
        a_roll_defense = dice(rng);
        if (b_roll_attack > a_roll_defense)
            b_kills_a = true;
    }

    if (a_kills_b && b_kills_a)
    {
        notify(a.getName() + " - " + a.getType() + " и " + b.getName() + " - " + b.getType() + " убили друг друга!");
        dead.insert(&a);
        dead.insert(&b);
    }
    else if (a_kills_b)
    {
        notify(a.getName() + " - " + a.getType() + " убил " + b.getName() + " - " + b.getType() + " (" + std::to_string(a_roll_attack) + " vs " + std::to_string(b_roll_defense) + ")");
        dead.insert(&b);
    }
    else if (b_kills_a)
    {
        notify(b.getName() + " - " + b.getType() + " убил " + a.getName() + " - " + a.getType() + " (" + std::to_string(b_roll_attack) + " vs " + std::to_string(a_roll_defense) + ")");
        dead.insert(&a);
    }
    else
    {
        notify(a.getName() + " - " + a.getType() + " не смог преодолеть защиту " + b.getName() + " - " + b.getType() + " (" + std::to_string(a_roll_attack) + " vs " + std::to_string(b_roll_defense) + ")");
    }
}