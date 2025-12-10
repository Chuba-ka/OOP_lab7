#pragma once
#include <string>
#include <mutex>
#include <fstream>

extern std::mutex g_cout_mutex;

class Observer
{
public:
    virtual ~Observer() = default;
    virtual void onKill(const std::string &message) = 0;
};

class ConsoleObserver : public Observer
{
public:
    void onKill(const std::string &message) override;
};

class FileObserver : public Observer
{
public:
    void onKill(const std::string &message) override;
};