/**
 * @file math_calculator.cpp
 * @brief Interactive calculator using math plugins
 * 
 * This application demonstrates loading and using multiple types of math
 * plugins to create a flexible calculator system.
 */

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <filesystem>

#include "fourdst/plugin/manager/plugin_manager.h"
#include "fourdst/plugin/exception/exceptions.h"
#include "../include/math_interfaces.h"

/**
 * @brief Calculator application using plugin-based math operations
 */
class MathCalculator {
private:
    fourdst::plugin::manager::PluginManager& m_manager = fourdst::plugin::manager::PluginManager::getInstance();
    std::vector<IMathOperation*> m_operation_plugins;
    std::vector<IAdvancedMath*> m_function_plugins;

public:
    /**
     * @brief Load a plugin from file
     */
    bool load_plugin(const std::filesystem::path& plugin_path) {
        try {
            std::cout << "Loading: " << plugin_path.filename() << "...";
            m_manager.load(plugin_path);
            
            // Try to cast to different plugin types
            bool loaded_something = false;
            
            // Try as IMathOperation
            try {
                auto* op_plugin = m_manager.get<IMathOperation>(plugin_path.stem().string());
                m_operation_plugins.push_back(op_plugin);
                std::cout << " [IMathOperation]";
                loaded_something = true;
            } catch (const fourdst::plugin::exception::PluginTypeError&) {
            } catch (const fourdst::plugin::exception::PluginNotLoadedError&) {
            }
            
            // Try as IAdvancedMath
            try {
                auto* func_plugin = m_manager.get<IAdvancedMath>(plugin_path.stem().string());
                m_function_plugins.push_back(func_plugin);
                std::cout << " [IAdvancedMath]";
                loaded_something = true;
            } catch (const fourdst::plugin::exception::PluginTypeError&) {
            } catch (const fourdst::plugin::exception::PluginNotLoadedError&) {
            }
            
            if (loaded_something) {
                std::cout << " ✓\n";
                return true;
            } else {
                std::cout << " (no compatible interfaces) ✗\n";
                return false;
            }
            
        } catch (const fourdst::plugin::exception::PluginError& e) {
            std::cout << " ✗\n";
            std::cerr << "  Error: " << e.what() << "\n";
            return false;
        }
    }

    /**
     * @brief Show available operations and functions
     */
    void show_available_commands() const {
        std::cout << "\nAvailable Commands:\n";
        std::cout << "==================\n";
        
        if (!m_operation_plugins.empty()) {
            std::cout << "\nBinary Operations (syntax: <operation> <num1> <num2>):\n";
            for (const auto* plugin : m_operation_plugins) {
                auto operations = plugin->get_supported_operations();
                std::cout << "  " << plugin->get_name() << ": ";
                for (size_t i = 0; i < operations.size(); ++i) {
                    std::cout << operations[i];
                    if (i < operations.size() - 1) std::cout << ", ";
                }
                std::cout << "\n";
            }
        }
        
        if (!m_function_plugins.empty()) {
            std::cout << "\nUnary Functions (syntax: <function> <num>):\n";
            for (const auto* plugin : m_function_plugins) {
                auto functions = plugin->get_supported_functions();
                std::cout << "  " << plugin->get_name() << ": ";
                for (size_t i = 0; i < functions.size(); ++i) {
                    std::cout << functions[i];
                    if (i < functions.size() - 1) std::cout << ", ";
                }
                std::cout << "\n";
            }
        }
        
        std::cout << "\nSpecial Commands:\n";
        std::cout << "  help - Show this help\n";
        std::cout << "  domain <function> - Show domain for a function\n";
        std::cout << "  quit - Exit calculator\n\n";
    }

    /**
     * @brief Process a single command
     */
    bool process_command(const std::string& input) const {
        std::istringstream iss(input);
        std::string command;
        iss >> command;
        
        if (command == "quit" || command == "q") {
            return false;
        }
        
        if (command == "help" || command == "h") {
            show_available_commands();
            return true;
        }
        
        if (command == "domain") {
            std::string function_name;
            iss >> function_name;
            
            for (const auto* plugin : m_function_plugins) {
                if (plugin->supports_function(function_name)) {
                    try {
                        std::cout << "Domain of " << function_name << ": " 
                                  << plugin->get_function_domain(function_name) << "\n";
                        return true;
                    } catch (const std::exception& e) {
                        std::cerr << "Error getting domain: " << e.what() << "\n";
                        return true;
                    }
                }
            }
            std::cerr << "Function '" << function_name << "' not found\n";
            return true;
        }
        
        // Try as binary operation
        double operand1, operand2;
        if (iss >> operand1 >> operand2) {
            for (const auto* plugin : m_operation_plugins) {
                if (plugin->supports_operation(command)) {
                    try {
                        double result = plugin->perform_operation(command, operand1, operand2);
                        std::cout << std::fixed << std::setprecision(6) << result << "\n";
                        return true;
                    } catch (const std::exception& e) {
                        std::cerr << "Error: " << e.what() << "\n";
                        return true;
                    }
                }
            }
        }
        
        // Try as unary function
        iss.clear();
        iss.str(input);
        iss >> command;
        double argument;
        if (iss >> argument) {
            for (const auto* plugin : m_function_plugins) {
                if (plugin->supports_function(command)) {
                    try {
                        double result = plugin->evaluate_function(command, argument);
                        std::cout << std::fixed << std::setprecision(6) << result << "\n";
                        return true;
                    } catch (const std::exception& e) {
                        std::cerr << "Error: " << e.what() << "\n";
                        return true;
                    }
                }
            }
        }
        
        std::cerr << "Unknown command or invalid syntax: " << command << "\n";
        std::cerr << "Type 'help' for usage information.\n";
        return true;
    }

    /**
     * @brief Run the interactive calculator
     */
    void run() const {
        std::cout << "Math Calculator\n";
        std::cout << "===============\n";
        std::cout << "Type 'help' for available commands, 'quit' to exit.\n\n";
        
        std::string input;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, input);
            
            if (input.empty()) continue;
            
            if (!process_command(input)) {
                break;
            }
        }
        
        std::cout << "Goodbye!\n";
    }
};

/**
 * @brief Main application entry point
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <plugin1.so> [plugin2.so] ...\n";
        std::cerr << "Load math plugins and run an interactive calculator.\n";
        return 1;
    }

    MathCalculator calculator;
    
    std::cout << "Loading plugins:\n";
    int loaded_count = 0;
    
    for (int i = 1; i < argc; ++i) {
        if (calculator.load_plugin(argv[i])) {
            loaded_count++;
        }
    }
    
    if (loaded_count == 0) {
        std::cerr << "\nNo plugins loaded successfully. Exiting.\n";
        return 1;
    }
    
    std::cout << "\nLoaded " << loaded_count << " plugin(s) successfully.\n";
    calculator.run();
    
    return 0;
}
