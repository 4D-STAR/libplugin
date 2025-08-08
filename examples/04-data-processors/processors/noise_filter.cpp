/**
 * @file noise_filter.cpp
 * @brief Statistical noise filtering plugin for data series
 * 
 * This plugin removes outliers from data series using Z-score analysis.
 * It's configurable and preserves processing metadata.
 */

#include "../include/data_interfaces.h"
#include <cmath>
#include <sstream>
#include <iomanip>

/**
 * @brief Plugin that removes statistical outliers from data series
 * 
 * Uses Z-score analysis to identify and remove data points that are
 * statistical outliers. The threshold is configurable via metadata.
 */
class NoiseFilter : public IDataSeriesProcessor {
private:
    double m_threshold = 2.0; ///< Z-score threshold for outlier detection
    
public:
    using IDataSeriesProcessor::IDataSeriesProcessor;
    /**
     * @brief Apply noise filtering to a data series
     * 
     * @param input The input data series
     * @return DataSeries The filtered data series with outliers removed
     * 
     * @throw std::runtime_error If the input series is too small for statistical analysis
     */
    DataSeries operator()(const DataSeries& input) const override {
        if (input.empty()) {
            return input; // Return empty series unchanged
        }
        
        if (input.size() < 3) {
            // Too few points for meaningful outlier detection
            DataSeries result = input;
            result.add_processing_step("noise_filter", "skipped (insufficient data)");
            return result;
        }
        
        // Calculate mean and standard deviation
        double mean = input.mean_value();
        double std_dev = input.std_deviation();
        
        if (std::isnan(std_dev) || std_dev == 0.0) {
            // No variation in data
            DataSeries result = input;
            result.add_processing_step("noise_filter", "skipped (no variation)");
            return result;
        }
        
        // Filter outliers based on Z-score
        std::vector<DataPoint> filtered_points;
        size_t removed_count = 0;
        
        for (const auto& point : input.points) {
            double z_score = std::abs(point.value - mean) / std_dev;
            
            if (z_score <= m_threshold) {
                filtered_points.push_back(point);
            } else {
                removed_count++;
            }
        }
        
        // Create result series
        DataSeries result(std::move(filtered_points), input.source, input.description);
        result.processing_history = input.processing_history;
        
        // Add processing metadata
        std::ostringstream info;
        info << "removed " << removed_count << " outliers (threshold=" << m_threshold 
             << ", " << std::fixed << std::setprecision(1) 
             << (100.0 * removed_count / input.size()) << "%)";
        result.add_processing_step("noise_filter", info.str());
        
        return result;
    }
    
    /**
     * @brief Configure the Z-score threshold
     * 
     * @param threshold The Z-score threshold (default: 2.0)
     */
    void set_threshold(double threshold) {
        if (threshold > 0.0) {
            m_threshold = threshold;
        }
    }
    
    /**
     * @brief Get the current threshold
     * 
     * @return double The current Z-score threshold
     */
    double get_threshold() const {
        return m_threshold;
    }
};

FOURDST_DECLARE_PLUGIN(NoiseFilter, "libnoise_filter", "1.0.0");
