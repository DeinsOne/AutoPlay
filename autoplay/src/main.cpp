#include <cstdio>
#include <Envi.h>
#include <EnviUtils.h>
#include <memory>

#include "AutoplayCore.h"
#include "AutoplayCmdConfig.h"
#include "AutoplayJsonConfig.h"

int main(int argc, char** argv) {

    APlay::AutoplayCmdConfig ccfg(argc, argv);
    APlay::AutoplayJsonConfig jcfg(std::make_shared<APlay::AutoplayCmdConfig>(ccfg));

    printf("Configuration: %s v%s\n", jcfg.GetCfgName(), jcfg.GetVersion() );
    printf("%s %s\n\n", jcfg.GetAuthor(), jcfg.GetEdition() );

    auto CaptureCfg = Envi::CreateWindowCaptureConfiguration([&]() -> std::vector<Envi::Window> {
        if (strcmp(jcfg.GetFindTitleMethod(), "WITH_KEYWORDS") == 0) {
            auto wnds = Envi::GetWindowsWithNameKeywords(jcfg.GetWindowTitle());
            if (wnds.size()) { printf("Found: %s\n", wnds.at(0).Name ); }
            return wnds;
        }
        else if (strcmp(jcfg.GetFindTitleMethod(), "MATCH") == 0) {
            auto wnds = Envi::GetWindowsWithNameContains(jcfg.GetWindowTitle().at(0) );
            if (wnds.size()) { printf("Found: %s\n", wnds.at(0).Name ); }
            return wnds;
        }
    });

    // Start capturing
    auto CaptureManager = CaptureCfg->SetTickInterval(jcfg.GetInterval() )->SetRecoverImages(jcfg.GetRecover() )->startCapturing();

    while (true) {
        if (Envi::IsKeyPressed(Envi::KeyCodes::KEY_LeftControl) &&
            Envi::IsKeyPressed(Envi::KeyCodes::KEY_W)
        ) { break; }

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    return 0;
}
