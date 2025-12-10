#include "Observer.h"
#include <iostream>

void ConsoleObserver::onKill(const std::string &message)
{
    std::lock_guard<std::mutex> lock(g_cout_mutex);
    std::cout << "Бой: " << message << std::endl;
}

void FileObserver::onKill(const std::string &message)
{
    std::lock_guard<std::mutex> lock(g_cout_mutex);
    std::ofstream log("log.txt", std::ios::app);
    if (log)
    {
        log << message << std::endl;
    }
}