#pragma once

#include <string>
#include <cmath>
#include <atomic>

class FightVisitor;

class NPC
{
protected:
    std::string name;
    int x, y;
    std::atomic<bool> alive;

public:
    NPC(const std::string &n, int xx, int yy) : name(n), x(xx), y(yy), alive(true) {}
    virtual ~NPC() = default;

    virtual std::string getType() const = 0;
    const std::string &getName() const { return name; }
    int getX() const { return x; }
    int getY() const { return y; }

    virtual int getMoveDistance() const = 0;
    virtual int getKillDistance() const = 0;

    void setPosition(int nx, int ny)
    {
        x = nx;
        y = ny;
    }
    bool isAlive() const { return alive.load(); }
    void kill() { alive.store(false); }

    double distanceTo(const NPC *other) const
    {
        int dx = x - other->x;
        int dy = y - other->y;
        return std::sqrt(dx * dx + dy * dy);
    }

    virtual void accept(FightVisitor &visitor) = 0;
};

class Knight : public NPC
{
public:
    Knight(const std::string &n, int x, int y) : NPC(n, x, y) {}
    std::string getType() const override { return "Странствующий рыцарь"; }
    int getMoveDistance() const override { return 30; }
    int getKillDistance() const override { return 10; }
    void accept(FightVisitor &v) override;
};

class Druid : public NPC
{
public:
    Druid(const std::string &n, int x, int y) : NPC(n, x, y) {}
    std::string getType() const override { return "Друид"; }
    int getMoveDistance() const override { return 10; }
    int getKillDistance() const override { return 10; }
    void accept(FightVisitor &v) override;
};

class Elf : public NPC
{
public:
    Elf(const std::string &n, int x, int y) : NPC(n, x, y) {}
    std::string getType() const override { return "Эльф"; }
    int getMoveDistance() const override { return 10; }
    int getKillDistance() const override { return 50; }
    void accept(FightVisitor &v) override;
};