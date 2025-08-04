/**
 * @file moving_average.cpp
 * @brief Moving average smoothing plugin for data series
 * 
 * This plugin applies a sliding window moving average to smooth
 * noisy data while preserving overall trends.
 */

#include "../include/data_interfaces.h"
#include <sstream>

/**
 * @brief Plugin that applies moving average smoothing to data series
 * 
 * Uses a sliding window to compute the average of neighboring points,
 * which helps reduce noise while preserving trends. The window size
 * is configurable.
 */
class MovingAverage : public IDataSeriesProcessor {
private:
    size_t m_window_size = 5; ///< Size of the moving average window
    
public:
    /**
     * @brief Apply moving average smoothing to a data series
     * 
     * @param input The input data series
     * @return DataSeries The smoothed data series
     */
    DataSeries operator()(const DataSeries& input) const override {
        if (input.empty() || input.size() < m_window_size) {
            // Return unchanged if too few points
            DataSeries result = input;
            result.add_processing_step("moving_average", "skipped (insufficient data)");
            return result;
        }
        
        std::vector<DataPoint> smoothed_points;
        smoothed_points.reserve(input.points.size());
        
        // Apply moving average
        for (size_t i = 0; i < input.points.size(); ++i) {
            // Determine window boundaries
            size_t start = (i >= m_window_size / 2) ? i - m_window_size / 2 : 0;
            size_t end = std::min(start + m_window_size, input.points.size());
            
            // Adjust start if we're near the end
            if (end - start < m_window_size && end == input.points.size()) {
                start = (end >= m_window_size) ? end - m_window_size : 0;
            }
            
            // Calculate average for the window
            double sum = 0.0;
            size_t count = 0;
            
            for (size_t j = start; j < end; ++j) {
                sum += input.points[j].value;
                count++;
            }
            
            double averaged_value = sum / static_cast<double>(count);
            
            // Create smoothed point with original timestamp and metadata
            DataPoint smoothed_point = input.points[i];
            smoothed_point.value = averaged_value;
            
            // Add metadata about the smoothing
            smoothed_point.metadata["smoothed"] = "true";
            smoothed_point.metadata["window_size"] = std::to_string(count);
            
            smoothed_points.push_back(smoothed_point);
        }
        
        // Create result series
        DataSeries result(std::move(smoothed_points), input.source, input.description);
        result.processing_history = input.processing_history;
        
        // Add processing metadata
        std::ostringstream info;
        info << "applied " << m_window_size << "-point moving average";
        result.add_processing_step("moving_average", info.str());
        
        return result;
    }
    
    /**
     * @brief Set the window size for moving average
     * 
     * @param window_size The size of the moving window (must be odd and >= 3)
     */
    void set_window_size(size_t window_size) {
        if (window_size >= 3) {
            // Ensure odd window size for symmetry
            m_window_size = (window_size % 2 == 0) ? window_size + 1 : window_size;
        }
    }
    
    /**
     * @brief Get the current window size
     * 
     * @return size_t The current window size
     */
    size_t get_window_size() const {
        return m_window_size;
    }
};

FOURDST_DECLARE_PLUGIN(MovingAverage, "libmoving_average", "1.0.0");
