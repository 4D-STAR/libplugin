#include "fourdst/plugin/plugin.h"
#include "mock_interfaces.h"

class FunctorPlugin final : public IExampleFunctor {
    ExampleContext operator()(const ExampleContext& input) const override {
        return {input.value * 2, input.threshold + 1.0};
    }
};

FOURDST_DECLARE_PLUGIN(FunctorPlugin, "FunctorPlugin", "1.0.0");