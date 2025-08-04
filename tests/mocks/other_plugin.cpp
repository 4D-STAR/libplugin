#include "fourdst/plugin/plugin.h"

#include "mock_interfaces.h"

// A different interface to test R3.3
class OtherPlugin final : public IOtherInterface {
public:
    [[nodiscard]] const char* get_name() const override { return "OtherPlugin"; }
    [[nodiscard]] const char* get_version() const override { return "1.0.0"; }
};

FOURDST_DECLARE_PLUGIN(OtherPlugin, "OtherPlugin", "1.0.0");
