# Data Processors Example

This example demonstrates the use of functor plugins for data processing pipelines. It showcases the FourDST functor template system with real data transformation scenarios.

## Overview

This example includes:
- Data processing interfaces using the `FunctorPlugin_T` template
- Multiple processor plugins implementing different data transformations
- A pipeline host application that chains processors together
- Real-world data processing scenarios (noise filtering, smoothing, scaling)

## Components

### Data Types
- **DataPoint**: Individual measurement with value, timestamp, and metadata
- **DataSeries**: Time-ordered collection of data points with processing history

### Processor Plugins
- **NoiseFilter**: Removes statistical outliers using Z-score analysis
- **MovingAverage**: Applies sliding window smoothing with configurable window size
- **ScaleTransform**: Scales data values by a configurable factor
- **TrendAnalyzer**: Adds trend analysis metadata to data series

### Host Application
- **DataPipeline**: Interactive application that processes data through plugin chains

## Building

```bash
meson setup builddir
meson compile -C builddir
```

## Usage

```bash
# Process with all available processors
./builddir/data_pipeline builddir/processors/*.so

# Process with specific processors in order
./builddir/data_pipeline builddir/processors/libnoise_filter.so builddir/processors/libmoving_average.so

# Interactive mode for custom processing
./builddir/data_pipeline --interactive builddir/processors/*.so
```

## Example Session

```
$ ./builddir/data_pipeline builddir/processors/*.so
Data Processing Pipeline
========================
Loaded processors:
  - noise_filter v1.0.0 (DataSeries processor)
  - moving_average v1.0.0 (DataSeries processor)  
  - scale_transform v1.0.0 (DataSeries processor)
  - trend_analyzer v1.0.0 (DataSeries processor)

Generated 1000 synthetic data points with noise
Original data: range=[0.123, 9.876], std_dev=2.84

Processing pipeline:
1. noise_filter: Removed 23 outliers (2.3%)
2. moving_average: Applied 5-point smoothing
3. scale_transform: Scaled by factor 1.5
4. trend_analyzer: Added trend metadata

Final data: range=[0.184, 14.814], std_dev=1.92
Trend: INCREASING (slope=0.045)
Processing time: 12.3ms
```

## Using with fourdst-cli

Generate a new data processor:

```bash
fourdst-cli plugin init my_processor --header ./include/data_interfaces.h
# Select IDataSeriesProcessor or IDataPointProcessor
```

## Key Features

1. **Functor Pattern**: Plugins implement `operator()` for clean transformation syntax
2. **Type Safety**: Template system ensures input/output type consistency
3. **Pipeline Chaining**: Multiple processors can be chained seamlessly
4. **State Management**: Processors maintain configuration between calls
5. **Metadata Tracking**: Processing history is preserved in data structures

## Real-world Applications

- **Signal Processing**: Filtering and smoothing sensor data
- **Financial Data**: Moving averages and trend analysis
- **Scientific Computing**: Data preprocessing pipelines
- **IoT Systems**: Real-time data transformation

## Files

- `include/data_interfaces.h` - Data types and interface definitions
- `processors/` - Functor plugin implementations
- `src/data_pipeline.cpp` - Host application with interactive mode
- `data/` - Sample data files for testing
- `meson.build` - Build configuration
