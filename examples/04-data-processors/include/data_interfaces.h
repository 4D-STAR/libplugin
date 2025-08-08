/**
 * @file data_interfaces.h
 * @brief Data structures and interfaces for data processing plugins
 * 
 * This file defines data types and functor interfaces for data processing
 * plugins. It demonstrates the use of the FourDST functor template system
 * for real-world data transformation scenarios.
 */

#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>
#include "fourdst/plugin/plugin.h"

/**
 * @brief A single data point with value, timestamp, and metadata
 * 
 * Represents a single measurement or observation with associated
 * temporal and contextual information.
 */
struct DataPoint {
    double value;                                           ///< The numeric value
    std::chrono::system_clock::time_point timestamp;       ///< When the data was recorded
    std::map<std::string, std::string> metadata;           ///< Additional metadata
    
    /**
     * @brief Default constructor
     */
    DataPoint() : value(0.0), timestamp(std::chrono::system_clock::now()) {}
    
    /**
     * @brief Constructor with value
     * @param val The data value
     */
    explicit DataPoint(double val) 
        : value(val), timestamp(std::chrono::system_clock::now()) {}
    
    /**
     * @brief Constructor with value and timestamp
     * @param val The data value
     * @param ts The timestamp
     */
    DataPoint(double val, std::chrono::system_clock::time_point ts)
        : value(val), timestamp(ts) {}
    
    /**
     * @brief Constructor with value, timestamp, and metadata
     * @param val The data value
     * @param ts The timestamp
     * @param meta The metadata map
     */
    DataPoint(double val, std::chrono::system_clock::time_point ts, 
              std::map<std::string, std::string> meta)
        : value(val), timestamp(ts), metadata(std::move(meta)) {}
    
    // Default copy/move constructors and assignment operators
    DataPoint(const DataPoint&) = default;
    DataPoint& operator=(const DataPoint&) = default;
    DataPoint(DataPoint&&) = default;
    DataPoint& operator=(DataPoint&&) = default;
};

/**
 * @brief A time-ordered collection of data points with processing metadata
 * 
 * Represents a time series dataset with comprehensive metadata tracking
 * for processing history and data provenance.
 */
struct DataSeries {
    std::vector<DataPoint> points;                          ///< The data points (should be time-ordered)
    std::map<std::string, std::string> processing_history; ///< Processing steps applied
    std::string source;                                     ///< Data source identifier
    std::string description;                                ///< Human-readable description
    
    /**
     * @brief Default constructor
     */
    DataSeries() = default;
    
    /**
     * @brief Constructor with points
     * @param pts The data points
     */
    explicit DataSeries(std::vector<DataPoint> pts) : points(std::move(pts)) {}
    
    /**
     * @brief Constructor with points and source
     * @param pts The data points
     * @param src The data source identifier
     */
    DataSeries(std::vector<DataPoint> pts, std::string src) 
        : points(std::move(pts)), source(std::move(src)) {}
    
    /**
     * @brief Constructor with full metadata
     * @param pts The data points
     * @param src The data source identifier
     * @param desc The description
     */
    DataSeries(std::vector<DataPoint> pts, std::string src, std::string desc)
        : points(std::move(pts)), source(std::move(src)), description(std::move(desc)) {}
    
    // Default copy/move constructors and assignment operators
    DataSeries(const DataSeries&) = default;
    DataSeries& operator=(const DataSeries&) = default;
    DataSeries(DataSeries&&) = default;
    DataSeries& operator=(DataSeries&&) = default;
    
    /**
     * @brief Get the number of data points
     * @return size_t Number of points in the series
     */
    size_t size() const { return points.size(); }
    
    /**
     * @brief Check if the dataset is empty
     * @return bool True if no data points exist
     */
    bool empty() const { return points.empty(); }
    
    /**
     * @brief Get the minimum value in the series
     * @return double The minimum value, or NaN if empty
     */
    double min_value() const {
        if (empty()) return std::numeric_limits<double>::quiet_NaN();
        auto it = std::min_element(points.begin(), points.end(),
            [](const DataPoint& a, const DataPoint& b) { return a.value < b.value; });
        return it->value;
    }
    
    /**
     * @brief Get the maximum value in the series
     * @return double The maximum value, or NaN if empty
     */
    double max_value() const {
        if (empty()) return std::numeric_limits<double>::quiet_NaN();
        auto it = std::max_element(points.begin(), points.end(),
            [](const DataPoint& a, const DataPoint& b) { return a.value < b.value; });
        return it->value;
    }
    
    /**
     * @brief Calculate the mean value of the series
     * @return double The mean value, or NaN if empty
     */
    double mean_value() const {
        if (empty()) return std::numeric_limits<double>::quiet_NaN();
        double sum = std::accumulate(points.begin(), points.end(), 0.0,
            [](double acc, const DataPoint& p) { return acc + p.value; });
        return sum / static_cast<double>(points.size());
    }
    
    /**
     * @brief Calculate the standard deviation of the series
     * @return double The standard deviation, or NaN if empty or single point
     */
    double std_deviation() const {
        if (points.size() < 2) return std::numeric_limits<double>::quiet_NaN();
        
        double mean = mean_value();
        double variance = std::accumulate(points.begin(), points.end(), 0.0,
            [mean](double acc, const DataPoint& p) {
                double diff = p.value - mean;
                return acc + diff * diff;
            }) / static_cast<double>(points.size() - 1);
        
        return std::sqrt(variance);
    }
    
    /**
     * @brief Add a processing step to the history
     * @param step_name The name of the processing step
     * @param step_info Additional information about the step
     */
    void add_processing_step(const std::string& step_name, const std::string& step_info) {
        processing_history[step_name] = step_info;
    }
    
    /**
     * @brief Sort points by timestamp
     */
    void sort_by_time() {
        std::sort(points.begin(), points.end(),
            [](const DataPoint& a, const DataPoint& b) {
                return a.timestamp < b.timestamp;
            });
    }
};

/**
 * @brief Functor interface for processing individual data points
 * 
 * This interface allows plugins to transform single data points.
 * The transformation can modify the value, timestamp, or metadata.
 * Useful for point-wise operations like scaling, unit conversion, etc.
 */
class IDataPointProcessor : public fourdst::plugin::templates::FunctorPlugin_T<DataPoint> {
public:
    /**
     * @brief Virtual destructor
     */
    using FunctorPlugin_T<DataPoint>::FunctorPlugin_T;
    virtual ~IDataPointProcessor() = default;
};

/**
 * @brief Functor interface for processing entire data series
 * 
 * This interface allows plugins to transform entire datasets.
 * This is useful for operations that need to consider multiple
 * data points together (e.g., filtering, smoothing, trend analysis).
 */
class IDataSeriesProcessor : public fourdst::plugin::templates::FunctorPlugin_T<DataSeries> {
public:
    using FunctorPlugin_T<DataSeries>::FunctorPlugin_T;
    /**
     * @brief Virtual destructor
     */
    virtual ~IDataSeriesProcessor() = default;
};
