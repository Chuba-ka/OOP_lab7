#pragma once
#include "NPC.h"
#include <memory>
#include <string>

class NPCFactory
{
public:
    std::unique_ptr<NPC> create(const std::string &type,
                                const std::string &name,
                                int x, int y) const;
};