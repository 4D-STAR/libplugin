/**
 * @file main.cpp
 * @brief Simple host application demonstrating plugin loading and usage
 * 
 * This application shows how to create a host that loads and manages plugins
 * using the FourDST plugin system. It demonstrates proper error handling,
 * plugin discovery, and interactive usage.
 */

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

// Include the main plugin header and the greeter interface
#include "fourdst/plugin/manager/plugin_manager.h"
#include "fourdst/plugin/exception/exceptions.h"
#include "../include/greeter_interface.h"

/**
 * @brief Display usage information
 * 
 * @param program_name The name of the program executable
 */
void print_usage(const std::string& program_name) {
    std::cout << "Usage: " << program_name << " <plugin_file> [plugin_file2] ...\n";
    std::cout << "       " << program_name << " --help\n\n";
    std::cout << "Load and test greeting plugins.\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  plugin_file    Path to a shared library containing a greeting plugin\n";
    std::cout << "  --help         Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " ./libsimple_greeter.so\n";
    std::cout << "  " << program_name << " plugin1.so plugin2.so plugin3.so\n";
}

/**
 * @brief Interactive greeting session with a plugin
 * 
 * @param plugin Pointer to the greeting plugin
 * @param plugin_name Name of the plugin for display purposes
 */
void interactive_greeting_session(IGreeter* plugin, const std::string& plugin_name) {
    std::cout << "\n=== Interactive Greeting Session ===\n";
    std::cout << "Using plugin: " << plugin_name << "\n\n";
    
    const std::vector<std::string> styles = {"casual", "formal", "friendly", "professional"};
    
    std::cout << "Available greeting styles:\n";
    for (size_t i = 0; i < styles.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << styles[i] << "\n";
    }
    std::cout << "\n";

    std::string name;
    while (true) {
        std::cout << "Enter your name (or 'quit' to exit): ";
        std::getline(std::cin, name);
        
        if (name == "quit" || name == "q") {
            break;
        }
        
        if (name.empty()) {
            std::cout << "Please enter a valid name.\n";
            continue;
        }

        // Get style choice
        int style_choice;
        std::cout << "Enter style (1-" << styles.size() << "): ";
        std::cin >> style_choice;
        std::cin.ignore(); // Clear the newline from input buffer
        
        if (style_choice < 1 || style_choice > static_cast<int>(styles.size())) {
            std::cout << "Invalid style choice. Using 'casual'.\n";
            style_choice = 1;
        }

        try {
            // Set the greeting style
            plugin->set_greeting_style(styles[style_choice - 1]);
            
            // Generate and display the greeting
            std::string greeting = plugin->greet(name);
            std::cout << styles[style_choice - 1] << " greeting: " << greeting << "\n\n";
            
        } catch (const std::exception& e) {
            std::cerr << "Error generating greeting: " << e.what() << "\n\n";
        }
    }
}

/**
 * @brief Load and test a single plugin
 * 
 * @param manager Reference to the plugin manager
 * @param plugin_path Path to the plugin file
 * @return bool True if the plugin was loaded and tested successfully
 */
bool load_and_test_plugin(fourdst::plugin::manager::PluginManager& manager, 
                         const std::filesystem::path& plugin_path) {
    try {
        std::cout << "Loading plugin: " << plugin_path << "\n";
        
        // Check if file exists
        if (!std::filesystem::exists(plugin_path)) {
            std::cerr << "Error: Plugin file does not exist: " << plugin_path << "\n";
            return false;
        }
        
        // Load the plugin
        manager.load(plugin_path);
        
        // The plugin name is determined by the plugin itself, so we need to
        // know it to retrieve the plugin. For this example, we'll assume
        // it's "simple_greeter" based on our plugin implementation.
        // In a real application, you might maintain a registry or use
        // other mechanisms to discover plugin names.
        
        auto* greeter_plugin = manager.get<IGreeter>("simple_greeter");
        
        std::cout << "Successfully loaded plugin: " << greeter_plugin->get_name() 
                  << " v" << greeter_plugin->get_version() << "\n";
        
        // Run interactive session
        interactive_greeting_session(greeter_plugin, greeter_plugin->get_name());
        
        return true;
        
    } catch (const fourdst::plugin::exception::PluginLoadError& e) {
        std::cerr << "Plugin load error: " << e.what() << "\n";
        return false;
    } catch (const fourdst::plugin::exception::PluginNotLoadedError& e) {
        std::cerr << "Plugin not loaded error: " << e.what() << "\n";
        return false;
    } catch (const fourdst::plugin::exception::PluginTypeError& e) {
        std::cerr << "Plugin type error: " << e.what() << "\n";
        std::cerr << "Note: This plugin does not implement the IGreeter interface.\n";
        return false;
    } catch (const fourdst::plugin::exception::PluginSymbolError& e) {
        std::cerr << "Plugin symbol error: " << e.what() << "\n";
        std::cerr << "Note: The plugin file may not be a valid FourDST plugin.\n";
        return false;
    } catch (const fourdst::plugin::exception::PluginNameCollisionError& e) {
        std::cerr << "Plugin name collision: " << e.what() << "\n";
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return false;
    }
}

/**
 * @brief Main application entry point
 */
int main(int argc, char* argv[]) {
    // Handle help request
    if (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
        print_usage(argv[0]);
        return 0;
    }
    
    // Check arguments
    if (argc < 2) {
        std::cerr << "Error: No plugin files specified.\n\n";
        print_usage(argv[0]);
        return 1;
    }

    std::cout << "FourDST Simple Host Application\n";
    std::cout << "================================\n\n";

    // Create plugin manager
    fourdst::plugin::manager::PluginManager manager;
    
    int successful_loads = 0;
    int total_plugins = argc - 1;

    // Load and test each plugin
    for (int i = 1; i < argc; ++i) {
        std::filesystem::path plugin_path(argv[i]);
        
        if (load_and_test_plugin(manager, plugin_path)) {
            successful_loads++;
        }
        
        // Add separator between plugins if testing multiple
        if (i < argc - 1) {
            std::cout << "\n" << std::string(50, '-') << "\n\n";
        }
    }

    // Summary
    std::cout << "\n=== Summary ===\n";
    std::cout << "Successfully loaded " << successful_loads << " out of " 
              << total_plugins << " plugins.\n";

    if (successful_loads == 0) {
        std::cout << "\nNo plugins were successfully loaded. Please check:\n";
        std::cout << "1. Plugin files exist and are readable\n";
        std::cout << "2. Plugin files are valid FourDST plugins\n";
        std::cout << "3. Plugins implement the IGreeter interface\n";
        return 1;
    }

    std::cout << "\nThank you for using the FourDST plugin system!\n";
    return 0;
}
