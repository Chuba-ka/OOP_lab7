#include <gtest/gtest.h>
#include "DungeonEditor.h"
#include "Factory.h"
#include <cstdio>
#include <shared_mutex>

TEST(DungeonEditorTest, AddNPCValidationAndSaveLoad)
{
    DungeonEditor editor;
    // valid add
    EXPECT_TRUE(editor.addNPC("Странствующий рыцарь", "A", 0, 0));
    // duplicate name
    EXPECT_FALSE(editor.addNPC("Эльф", "A", 1, 1));
    // out of bounds
    EXPECT_FALSE(editor.addNPC("Эльф", "B", -1, 0));
    EXPECT_FALSE(editor.addNPC("Эльф", "C", DungeonEditor::MAP_WIDTH, 0));
    EXPECT_FALSE(editor.addNPC("Эльф", "D", 0, DungeonEditor::MAP_HEIGHT));

    // Add another valid
    EXPECT_TRUE(editor.addNPC("Эльф", "E", 10, 10));

    // Save to file
    const char *fname = "test_save.txt";
    editor.saveToFile(fname);

    // Clear internal container by loading a different (empty) file
    // But first ensure the file exists by loading it into a fresh editor
    DungeonEditor editor2;
    editor2.loadFromFile(fname);

    // Check that at least 2 NPCs exist (A and E) and are alive
    {
        std::shared_lock<std::shared_mutex> lock(editor2.npcs_mutex);
        int alive_count = 0;
        for (auto &u : editor2.accessNPCs())
        {
            if (u->isAlive())
                ++alive_count;
        }
        EXPECT_GE(alive_count, 2);
    }

    std::remove(fname);
}

TEST(NPCTest, DistanceCalculation)
{
    NPCFactory factory;
    auto a = factory.create("Странствующий рыцарь", "a", 0, 0);
    auto b = factory.create("Эльф", "b", 3, 4);
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    double d = a->distanceTo(b.get());
    EXPECT_DOUBLE_EQ(d, 5.0);
}