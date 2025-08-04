/**
 * @file plugin.h
 * @brief Main include header for the FourDST plugin system
 * 
 * This is the primary header file that provides access to all components
 * of the FourDST plugin system. It serves as a convenience header for
 * users who want to include the entire plugin framework in their projects.
 * 
 * By including this single header, developers get access to:
 * - Core plugin interfaces (IPlugin)
 * - Plugin factory and creation macros
 * - Plugin manager for loading and managing plugins
 * - Utility functions for plugin development
 * - Exception classes for error handling
 * - Template classes for specialized plugin types
 * 
 * @note This header is designed for convenience. For better compilation times
 *       in large projects, consider including only the specific headers you need.
 */

#pragma once

#include "fourdst/plugin/iplugin.h"
#include "fourdst/plugin/factory/plugin_factory.h"
#include "fourdst/plugin/manager/plugin_manager.h"
#include "fourdst/plugin/utils/plugin_utils.h"
#include "fourdst/plugin/exception/exceptions.h"
#include "fourdst/plugin/templates/functor.h"

/**
 * @brief Main namespace for the FourDST plugin system
 * 
 * This namespace contains all classes, functions, and types related to the
 * FourDST plugin framework. It is organized into several sub-namespaces:
 * 
 * - fourdst::plugin::exception - Exception classes for error handling
 * - fourdst::plugin::manager - Plugin management functionality
 * - fourdst::plugin::templates - Template classes for specialized plugins
 * 
 * The namespace is designed to prevent naming conflicts while providing
 * a clear organizational structure for the plugin system components.
 */
namespace fourdst::plugin{}