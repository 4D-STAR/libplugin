/**
 * @file data_pipeline.cpp
 * @brief Data processing pipeline application using processor plugins
 * 
 * This application demonstrates loading and chaining data processing plugins
 * to create flexible data transformation pipelines.
 */

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <random>
#include <chrono>
#include <iomanip>
#include <map>

#include "fourdst/plugin/manager/plugin_manager.h"
#include "fourdst/plugin/exception/exceptions.h"
#include "../include/data_interfaces.h"

/**
 * @brief Generate synthetic data for testing
 * 
 * @param num_points Number of data points to generate
 * @param noise_level Amount of noise to add (0.0 = no noise, 1.0 = high noise)
 * @return DataSeries The generated synthetic data
 */
DataSeries generate_synthetic_data(size_t num_points, double noise_level = 0.2) {
    std::vector<DataPoint> points;
    points.reserve(num_points);
    
    // Random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> noise_dist(0.0, noise_level);
    std::uniform_real_distribution<double> trend_dist(0.8, 1.2);
    
    auto start_time = std::chrono::system_clock::now();
    
    for (size_t i = 0; i < num_points; ++i) {
        // Base signal: sine wave with trend
        double base_value = std::sin(2.0 * M_PI * i / 50.0) * 3.0 + i * 0.01;
        
        // Add noise
        double noisy_value = base_value + noise_dist(gen);
        
        // Add occasional outliers (5% chance)
        if (std::uniform_real_distribution<double>(0.0, 1.0)(gen) < 0.05) {
            noisy_value += (std::uniform_real_distribution<double>(0.0, 1.0)(gen) > 0.5 ? 10.0 : -10.0);
        }
        
        // Create timestamp
        auto timestamp = start_time + std::chrono::milliseconds(i * 100);
        
        // Create metadata
        std::map<std::string, std::string> metadata;
        metadata["index"] = std::to_string(i);
        metadata["synthetic"] = "true";
        
        points.emplace_back(noisy_value, timestamp, std::move(metadata));
    }
    
    DataSeries series(std::move(points), "synthetic_generator", "Synthetic data for testing");
    series.sort_by_time();
    
    return series;
}

/**
 * @brief Print data series statistics
 * 
 * @param series The data series to analyze
 * @param prefix Prefix for output lines
 */
void print_series_stats(const DataSeries& series, const std::string& prefix = "") {
    if (series.empty()) {
        std::cout << prefix << "Empty data series\n";
        return;
    }
    
    std::cout << prefix << "Data points: " << series.size() << "\n";
    std::cout << prefix << "Range: [" << std::fixed << std::setprecision(3) 
              << series.min_value() << ", " << series.max_value() << "]\n";
    std::cout << prefix << "Mean: " << std::fixed << std::setprecision(3) 
              << series.mean_value() << "\n";
    std::cout << prefix << "Std Dev: " << std::fixed << std::setprecision(3) 
              << series.std_deviation() << "\n";
    
    if (!series.processing_history.empty()) {
        std::cout << prefix << "Processing steps:\n";
        for (const auto& [step, info] : series.processing_history) {
            std::cout << prefix << "  - " << step << ": " << info << "\n";
        }
    }
}

/**
 * @brief Data processing pipeline application
 */
class DataPipeline {
private:
    fourdst::plugin::manager::PluginManager& m_manager = fourdst::plugin::manager::PluginManager::getInstance();
    std::vector<IDataSeriesProcessor*> m_processors;
    
public:
    /**
     * @brief Load a processor plugin
     * 
     * @param plugin_path Path to the plugin file
     * @return bool True if loaded successfully
     */
    bool load_processor(const std::filesystem::path& plugin_path) {
        try {
            std::cout << "Loading: " << plugin_path.filename() << "...";
            
            m_manager.load(plugin_path);
            
            // Try to get as data series processor
            try {
                auto* processor = m_manager.get<IDataSeriesProcessor>(plugin_path.stem().string());
                m_processors.push_back(processor);
                std::cout << " ✓ (DataSeries processor)\n";
                return true;
            } catch (const fourdst::plugin::exception::PluginTypeError&) {
                std::cout << " ✗ (not a DataSeries processor)\n";
                return false;
            } catch (const fourdst::plugin::exception::PluginNotLoadedError&) {
                std::cout << " ✗ (plugin not found)\n";
                return false;
            }
            
        } catch (const fourdst::plugin::exception::PluginError& e) {
            std::cout << " ✗\n";
            std::cerr << "  Error: " << e.what() << "\n";
            return false;
        }
    }
    
    /**
     * @brief Process data through the loaded processors
     * 
     * @param input_data The input data series
     * @return DataSeries The processed data series
     */
    DataSeries process_data(const DataSeries& input_data) {
        DataSeries current_data = input_data;
        
        std::cout << "\nProcessing pipeline:\n";
        for (size_t i = 0; i < m_processors.size(); ++i) {
            auto* processor = m_processors[i];
            
            auto start_time = std::chrono::high_resolution_clock::now();
            current_data = (*processor)(current_data);
            auto end_time = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            
            std::cout << "  " << (i + 1) << ". " << processor->get_name() 
                      << " (" << duration.count() << "μs)\n";
        }
        
        return current_data;
    }
    
    /**
     * @brief Get the number of loaded processors
     * 
     * @return size_t Number of processors
     */
    size_t get_processor_count() const {
        return m_processors.size();
    }
    
    /**
     * @brief List loaded processors
     */
    void list_processors() {
        if (m_processors.empty()) {
            std::cout << "No processors loaded.\n";
            return;
        }
        
        std::cout << "Loaded processors:\n";
        for (size_t i = 0; i < m_processors.size(); ++i) {
            auto* processor = m_processors[i];
            std::cout << "  " << (i + 1) << ". " << processor->get_name() 
                      << " v" << processor->get_version() << "\n";
        }
    }
};

/**
 * @brief Print usage information
 */
void print_usage(const std::string& program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS] <processor1.so> [processor2.so] ...\n\n";
    std::cout << "Options:\n";
    std::cout << "  --interactive, -i   Interactive mode for custom processing\n";
    std::cout << "  --points, -p NUM    Number of synthetic data points (default: 1000)\n";
    std::cout << "  --noise, -n LEVEL   Noise level (0.0-1.0, default: 0.2)\n";
    std::cout << "  --help, -h          Show this help\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " processors/*.so\n";
    std::cout << "  " << program_name << " -p 500 -n 0.1 noise_filter.so moving_average.so\n";
    std::cout << "  " << program_name << " --interactive processors/*.so\n";
}

/**
 * @brief Main application entry point
 */
int main(int argc, char* argv[]) {
    // Parse command line arguments
    [[maybe_unused]] bool interactive = false;
    size_t num_points = 1000;
    double noise_level = 0.2;
    std::vector<std::string> plugin_paths;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        } else if (arg == "--interactive" || arg == "-i") {
            interactive = true;
        } else if (arg == "--points" || arg == "-p") {
            if (i + 1 < argc) {
                num_points = static_cast<size_t>(std::stoul(argv[++i]));
            } else {
                std::cerr << "Error: --points requires a number\n";
                return 1;
            }
        } else if (arg == "--noise" || arg == "-n") {
            if (i + 1 < argc) {
                noise_level = std::stod(argv[++i]);
                if (noise_level < 0.0 || noise_level > 1.0) {
                    std::cerr << "Error: noise level must be between 0.0 and 1.0\n";
                    return 1;
                }
            } else {
                std::cerr << "Error: --noise requires a number\n";
                return 1;
            }
        } else if (arg.find(".so") != std::string::npos || 
                   arg.find(".dylib") != std::string::npos ||
                   arg.find(".dll") != std::string::npos) {
            plugin_paths.push_back(arg);
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            print_usage(argv[0]);
            return 1;
        }
    }
    
    if (plugin_paths.empty()) {
        std::cerr << "Error: No processor plugins specified\n\n";
        print_usage(argv[0]);
        return 1;
    }
    
    std::cout << "Data Processing Pipeline\n";
    std::cout << "========================\n\n";
    
    // Create pipeline and load processors
    DataPipeline pipeline;
    int loaded_count = 0;
    
    std::cout << "Loading processors:\n";
    for (const auto& path : plugin_paths) {
        if (pipeline.load_processor(path)) {
            loaded_count++;
        }
    }
    
    if (loaded_count == 0) {
        std::cerr << "\nNo processors loaded successfully. Exiting.\n";
        return 1;
    }
    
    std::cout << "\n";
    pipeline.list_processors();
    
    // Generate synthetic data
    std::cout << "\nGenerating " << num_points << " synthetic data points with noise level " 
              << noise_level << "...\n";
    DataSeries original_data = generate_synthetic_data(num_points, noise_level);
    
    std::cout << "\nOriginal data:\n";
    print_series_stats(original_data, "  ");
    
    // Process data
    DataSeries processed_data = pipeline.process_data(original_data);
    
    std::cout << "\nProcessed data:\n";
    print_series_stats(processed_data, "  ");
    
    std::cout << "\nProcessing complete!\n";
    
    return 0;
}
