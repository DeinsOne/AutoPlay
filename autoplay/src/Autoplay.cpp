#include <cstdio>
#include <Envi.h>
#include <EnviUtils.h>
#include <memory>

#include "AutoplayCore.h"
#include "AutoplayCmdConfig.h"
#include "AutoplayJsonConfig.h"
#include "AutoplayClient.h"

#include "ecs/uecs.h"

int main(int argc, char** argv) {

    auto ccfg = APlay::CreateCmdConfig(argc, argv);
    auto jcfg = APlay::CreateJsonConfig(ccfg);

    auto aClient = APlay::CreateAutoplayClient(jcfg);

    while (true) {
        if (Envi::IsKeyPressed(Envi::KeyCodes::KEY_LeftControl) &&
            Envi::IsKeyPressed(Envi::KeyCodes::KEY_W)
        ) {
            aClient->Pause();
            printf("\n\ndetected ctrl+w. Exit\n");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(jcfg->GetInterval()));
    }

    return 0;
}
