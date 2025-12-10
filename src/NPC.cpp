#include "NPC.h"
#include "FightVisitor.h"

void Knight::accept(FightVisitor &visitor)
{
    visitor.visit(*this);
}

void Druid::accept(FightVisitor &visitor)
{
    visitor.visit(*this);
}

void Elf::accept(FightVisitor &visitor)
{
    visitor.visit(*this);
}