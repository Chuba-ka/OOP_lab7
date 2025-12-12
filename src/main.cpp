#include "DungeonSimulation.h"
#include <iostream>

std::mutex g_cout_mutex;

int main()
{
    try
    {
        DungeonSimulation simulation;
        simulation.run(std::chrono::seconds(30));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Произошла ошибка в симуляции: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}