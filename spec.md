# Core Requirements & Specification

## R1: Plugin Loading

- R1.1: The PluginManager must successfully load a valid shared library (.so or .dylib) from a given filesystem path.
- R1.2: The PluginManager must throw a specific, identifiable exception if the file at the given path does not exist.
- R1.3: The PluginManager must throw a specific, identifiable exception if the file is not a valid shared library that can be loaded by the operating system.
- R1.4: The PluginManager must throw a specific, identifiable exception if the library does not contain the required C-style factory function (e.g., create_plugin).

## R2: Plugin Instantiation and Management

- R2.1: Upon successful loading, the PluginManager must call the factory function to create an instance of the plugin.
- R2.2: The PluginManager must store the created plugin instance, associated with the plugin's self-reported name.
- R2.3: Attempting to load a plugin with a name that is already managed should fail with a specific exception to prevent name collisions.
- R2.4: The PluginManager must provide a method to retrieve a raw IPlugin* pointer to a loaded plugin by its name.
- R2.5: Retrieving a non-existent plugin should return a nullptr.

## R3: Type-Safe Access

- R3.1: The PluginManager must provide a templated get<T>() method for type-safe retrieval of a plugin.
- R3.2: The get<T>() method must successfully return a valid pointer of type T* if the stored plugin is of a compatible type (verified via dynamic_cast).
- R3.3: The get<T>() method must throw a specific exception if the stored plugin cannot be safely cast to T*.

## R4: Plugin Lifecycle and Unloading

- R4.1: The PluginManager must correctly call the destructor of a plugin instance when it is unloaded.
- R4.2: The PluginManager must successfully unload the associated shared library from memory.
- R4.3: The PluginManager destructor must automatically unload all managed plugins to prevent resource leaks.

## R5: Plugin Authoring Experience

- R5.1: The DECLARE_PLUGIN macro must correctly generate a non-mangled create_plugin factory function.
- R5.2: The PluginBase helper class must correctly provide the get_name() and get_version() implementations based on the macro parameters.
- R5.3: The FunctorPlugin class must correctly work with TypeErasure to allow plugins to be used without knowing their exact type at compile time.