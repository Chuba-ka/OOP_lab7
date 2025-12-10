#include <gtest/gtest.h>
#include "Factory.h"
#include "NPC.h"
#include <memory>

TEST(FactoryTest, CreatesKnightElfDruid)
{
    NPCFactory factory;
    auto k = factory.create("Странствующий рыцарь", "Arthur", 1, 2);
    ASSERT_NE(k, nullptr);
    EXPECT_EQ(k->getType(), "Странствующий рыцарь");
    EXPECT_EQ(k->getName(), "Arthur");
    EXPECT_EQ(k->getX(), 1);
    EXPECT_EQ(k->getY(), 2);

    auto e = factory.create("Эльф", "Legolas", 3, 4);
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(e->getType(), "Эльф");

    auto d = factory.create("Друид", "Merlin", 5, 6);
    ASSERT_NE(d, nullptr);
    EXPECT_EQ(d->getType(), "Друид");

    auto n = factory.create("НеИзвестно", "N", 0, 0);
    EXPECT_EQ(n, nullptr);
}