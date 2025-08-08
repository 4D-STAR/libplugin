/**
 * @file scale_transform.cpp
 * @brief Scaling transformation plugin for data series
 * 
 * This plugin scales data values by a configurable factor,
 * useful for unit conversions or normalization.
 */

#include "../include/data_interfaces.h"
#include <sstream>

/**
 * @brief Plugin that scales data values by a configurable factor
 * 
 * Multiplies all data values by a scaling factor while preserving
 * timestamps and metadata. Useful for unit conversions, normalization,
 * or amplification.
 */
class ScaleTransform : public IDataSeriesProcessor {
private:
    double m_scale_factor = 1.0; ///< The scaling factor to apply
    
public:
    using IDataSeriesProcessor::IDataSeriesProcessor;
    /**
     * @brief Apply scaling transformation to a data series
     * 
     * @param input The input data series
     * @return DataSeries The scaled data series
     */
    DataSeries operator()(const DataSeries& input) const override {
        if (input.empty()) {
            return input; // Return empty series unchanged
        }
        
        std::vector<DataPoint> scaled_points;
        scaled_points.reserve(input.points.size());
        
        // Scale each data point
        for (const auto& point : input.points) {
            DataPoint scaled_point = point;
            scaled_point.value *= m_scale_factor;
            
            // Add metadata about the scaling
            scaled_point.metadata["scaled"] = "true";
            scaled_point.metadata["scale_factor"] = std::to_string(m_scale_factor);
            
            scaled_points.push_back(scaled_point);
        }
        
        // Create result series
        DataSeries result(std::move(scaled_points), input.source, input.description);
        result.processing_history = input.processing_history;
        
        // Add processing metadata
        std::ostringstream info;
        info << "scaled by factor " << m_scale_factor;
        result.add_processing_step("scale_transform", info.str());
        
        return result;
    }
    
    /**
     * @brief Set the scaling factor
     * 
     * @param scale_factor The factor to multiply values by
     */
    void set_scale_factor(double scale_factor) {
        m_scale_factor = scale_factor;
    }
    
    /**
     * @brief Get the current scaling factor
     * 
     * @return double The current scaling factor
     */
    double get_scale_factor() const {
        return m_scale_factor;
    }
};

FOURDST_DECLARE_PLUGIN(ScaleTransform, "libscale_transform", "1.0.0");
