#pragma once
#include "AutoplayCore.h"
#include "AutoplayJsonConfig.h"

namespace APlay {

    class IAutoplayClient {
        public:
            virtual void Pause() = 0;

            virtual bool IsPaused() = 0;

            virtual void Resume() = 0;

    };

    std::shared_ptr<IAutoplayClient> CreateAutoplayClient(std::shared_ptr<AutoplayJsonConfig> config);

}
