#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <atomic>

#include "fourdst/plugin/plugin.h"
#include "mocks/mock_interfaces.h"

// DEFINE the global variable here, in the test executable's compilation unit.
std::atomic<bool> g_destructor_called = false;

// Test Fixture for PluginManager tests
class PluginManagerTest : public ::testing::Test {
protected:
    fourdst::plugin::manager::PluginManager& manager = fourdst::plugin::manager::PluginManager::getInstance();
    std::filesystem::path valid_plugin_path;
    std::filesystem::path no_factory_plugin_path;
    std::filesystem::path other_plugin_path;
    std::filesystem::path functor_plugin_path;
    std::filesystem::path non_existent_path = "non_existent_plugin.so";
    std::filesystem::path invalid_lib_path = "invalid_library.txt";

    void SetUp() override {
        #ifdef VALID_PLUGIN_PATH
            valid_plugin_path = VALID_PLUGIN_PATH;
        #endif
        #ifdef NO_FACTORY_PLUGIN_PATH
            no_factory_plugin_path = NO_FACTORY_PLUGIN_PATH;
        #endif
        #ifdef OTHER_PLUGIN_PATH
            other_plugin_path = OTHER_PLUGIN_PATH;
        #endif
        #ifdef FUNCTOR_PLUGIN_PATH
            functor_plugin_path = FUNCTOR_PLUGIN_PATH;
        #endif

        std::ofstream invalid_file(invalid_lib_path);
        invalid_file << "This is not a shared library.";
        invalid_file.close();
    }

    void TearDown() override {
        std::filesystem::remove(invalid_lib_path);
    }
};

// --- R1: Plugin Loading ---

TEST_F(PluginManagerTest, R1_1_SuccessfullyLoadsValidPlugin) {
    ASSERT_TRUE(std::filesystem::exists(valid_plugin_path)) << "Valid plugin library not found at " << valid_plugin_path;
    EXPECT_NO_THROW(manager.load(valid_plugin_path));
}

TEST_F(PluginManagerTest, R1_2_ThrowsWhenFileDoesNotExist) {
    EXPECT_THROW(manager.load(non_existent_path), fourdst::plugin::exception::PluginLoadError);
}

TEST_F(PluginManagerTest, R1_3_ThrowsWhenFileIsNotAValidLibrary) {
    EXPECT_THROW(manager.load(invalid_lib_path), fourdst::plugin::exception::PluginLoadError);
}

TEST_F(PluginManagerTest, R1_4_ThrowsWhenFactoryFunctionIsMissing) {
    ASSERT_TRUE(std::filesystem::exists(no_factory_plugin_path)) << "No-factory plugin library not found.";
    EXPECT_THROW(manager.load(no_factory_plugin_path), fourdst::plugin::exception::PluginSymbolError);
}

// --- R2: Plugin Instantiation and Management ---

TEST_F(PluginManagerTest, R2_1_R2_2_InstantiatesAndStoresPluginByName) {
    auto* plugin = manager.get<fourdst::plugin::IPlugin>("ValidPlugin");
    ASSERT_NE(plugin, nullptr);
    EXPECT_STREQ(plugin->get_name(), "ValidPlugin");
    EXPECT_STREQ(plugin->get_version(), "1.0.0");
}

TEST_F(PluginManagerTest, R2_3_ThrowsOnDuplicatePluginName) {
    EXPECT_THROW(manager.load(valid_plugin_path), fourdst::plugin::exception::PluginNameCollisionError);
}

TEST_F(PluginManagerTest, R2_5_ReturnsNullptrForNonExistentPlugin) {
    EXPECT_THROW(manager.get<fourdst::plugin::IPlugin>("NonExistentPlugin"), fourdst::plugin::exception::PluginNotLoadedError);
}

TEST_F(PluginManagerTest, R2_6_7_BooleanCheckForPluginExistence) {
    EXPECT_FALSE(manager.has("NonExistentPlugin"));
    EXPECT_TRUE(manager.has("ValidPlugin"));
}

// --- R3: Type-Safe Access ---
TEST_F(PluginManagerTest, R3_1_R3_2_TypeSafeGetSucceedsForCorrectType) {
    auto* plugin = manager.get<IValidPlugin>("ValidPlugin");
    ASSERT_NE(plugin, nullptr);
    EXPECT_EQ(plugin->get_magic_number(), 42);
}

TEST_F(PluginManagerTest, R3_3_TypeSafeGetThrowsForIncorrectType) {
    manager.load(other_plugin_path);
    EXPECT_THROW(manager.get<IValidPlugin>("OtherPlugin"), fourdst::plugin::exception::PluginTypeError);
}

// --- R4: Plugin Lifecycle and Unloading ---

TEST_F(PluginManagerTest, R4_1_R4_2_UnloadCallsDestructorAndReleasesLibrary) {
    g_destructor_called = false;

    EXPECT_NO_THROW(manager.unload("ValidPlugin"));
    EXPECT_TRUE(g_destructor_called);
    EXPECT_THROW(manager.get<fourdst::plugin::IPlugin>("ValidPlugin"), fourdst::plugin::exception::PluginNotLoadedError);
}

// --- R5: Plugin Authoring Experience ---

TEST_F(PluginManagerTest, R5_1_R5_2_MacroAndBaseClassProvideCorrectMetadata) {
    manager.load(valid_plugin_path);
    auto* plugin = manager.get<fourdst::plugin::IPlugin>("ValidPlugin");
    ASSERT_NE(plugin, nullptr);
    EXPECT_STREQ(plugin->get_name(), "ValidPlugin");
    EXPECT_STREQ(plugin->get_version(), "1.0.0");
}

TEST_F(PluginManagerTest, R5_3_FunctorPluginWorksAsExpected) {
    manager.load(functor_plugin_path);
    auto* functor_plugin = manager.get<IExampleFunctor>("FunctorPlugin");
    ASSERT_NE(functor_plugin, nullptr);

    constexpr ExampleContext input{42, 3.14};
    auto [value, threshold] = (*functor_plugin)(input);

    EXPECT_EQ(value, 84);
    EXPECT_DOUBLE_EQ(threshold, 4.14);
}
