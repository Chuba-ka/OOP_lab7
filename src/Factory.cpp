#include "Factory.h"
#include <algorithm>
#include <string>

static std::string clean_type(const std::string &str)
{
    std::string s = str;
    s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c)
                           { return c == '\r' || c == '\n' || c == '\t'; }),
            s.end());
    size_t start = s.find_first_not_of(" ");
    size_t end = s.find_last_not_of(" ");
    if (start == std::string::npos)
        return "";
    return s.substr(start, end - start + 1);
}

std::unique_ptr<NPC> NPCFactory::create(const std::string &type,
                                        const std::string &name,
                                        int x, int y) const
{
    std::string t = clean_type(type);
    if (t == "Странствующий рыцарь")
        return std::make_unique<Knight>(name, x, y);
    if (t == "Эльф")
        return std::make_unique<Elf>(name, x, y);
    if (t == "Друид")
        return std::make_unique<Druid>(name, x, y);
    return nullptr;
}