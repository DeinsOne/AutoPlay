#pragma once
#include "AutoplayCore.h"
#include "AutoplayJsonConfig.h"

namespace APlay {

    class IAutoplayCmdVisualizer {
        public:
            virtual void Start() = 0;

            virtual void Stop() = 0;

            virtual void PassInterval(const int& _interval) = 0;
    };

    std::shared_ptr<IAutoplayCmdVisualizer> CreateCmdVisualizer(const std::shared_ptr<AutoplayJsonConfig>& _config);

}

