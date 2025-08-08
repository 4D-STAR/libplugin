#include "fourdst/plugin/bundle/bundle.h"
#include "include/interface.h"

#include <string>

void print_usage(const std::string& program_name) {
    std::cout << "Usage: " << program_name << " <bundle_file>\n";
    std::cout << "       " << program_name << " --help\n\n";
    std::cout << "Load and verify a plugin bundle.\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  bundle_file    Path to a .fbundle file containing plugins\n";
    std::cout << "  --help         Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " ./example.fbundle\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 2 || std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
        print_usage(argv[0]);
        return 1;
    }

    try {
        fourdst::plugin::manager::PluginManager& manager = fourdst::plugin::manager::PluginManager::getInstance();
        fourdst::plugin::bundle::PluginBundle bundle(argv[1]);
        std::cout << "Successfully loaded plugin bundle: " << argv[1] << "\n";

        for (const auto& name : bundle.getPluginNames()) {
            std::cout << " - Loaded plugin: " << name << "\n";
            std::cout << " - Calling say_hello() from plugin...\n";
            auto plugin = manager.get<Interface>(name);
            plugin->say_hello();
        }



    } catch (const std::exception& e) {
        std::cerr << "Error loading plugin bundle: " << e.what() << "\n";
        return 1;
    }

    return 0;
}