#include "fourdst/plugin/plugin.h"
#include "mock_interfaces.h"

#include <atomic>

// Global flag to check if the destructor was called (for R4.1)
extern std::atomic<bool> g_destructor_called;

class ValidPlugin final : public IValidPlugin {
public:
    ~ValidPlugin() override {
        g_destructor_called = true;
    }
    // Custom method for type-safe access test (R3.2)
    [[nodiscard]] int get_magic_number() const override { return 42; }
};

FOURDST_DECLARE_PLUGIN(ValidPlugin, "ValidPlugin", "1.0.0");
