#include <cstdio>
#include <Envi.h>
#include <EnviUtils.h>
#include <memory>

#include "AutoplayCore.h"
#include "AutoplayCmdConfig.h"
#include "AutoplayJsonConfig.h"
#include "AutoplayClient.h"
#include "AutoplayLog.h"

#include "ecs/uecs.h"

int main(int argc, char** argv) {
    APlay::AutoplayLogger::Init();

    // Initiate profiling session for Autoplay initialization.
    // There have to be all of preloaded instances such
    // images, scripts and neural network
    APLAY_PROFILE_BEGIN_SESSION("Initializing", "tmp/init_profile.json");

    // Preloading cmd configuration. It provides several
    // arguments for Autoplay deployment
    auto ccfg = APlay::CreateCmdConfig(argc, argv);

    // Preloading main configuration file. There is config
    // validation which will throw exception with detailed
    // information of occurred error.
    // Contains duplicated variables form cmd config but
    // it's variables have higher priority. Those variables
    // are interval, recovery
    auto jcfg = APlay::CreateJsonConfig(ccfg);

    APLAY_PROFILE_END_SESSION();

    // Initiate runtime session. There are profiling of main APlay
    // systems and main loop
	APLAY_PROFILE_BEGIN_SESSION("Runtime", "tmp/runtime_profile.json");

    // Create Client. CLient is entry point of Autoplay. It
    // Provides image capturing using specified adapter for 
    // desktop or mobile. It processes captured image and
    // execute rules 
    auto aClient = APlay::CreateAutoplayClient(jcfg);

    // Main loop
    while (true) {
        // Shortcut to exit Autoplay
        if (Envi::IsKeyPressed(Envi::KeyCodes::KEY_LeftControl) &&
            Envi::IsKeyPressed(Envi::KeyCodes::KEY_W)
        ) {
            aClient->Pause();
            APLAY_CONSOLE_INFO("detected ctrl+w. Exit");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(jcfg->GetInterval()));
    }

    APLAY_PROFILE_END_SESSION();

    return 0;
}
