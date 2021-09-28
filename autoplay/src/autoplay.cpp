#include <cstdio>
#include <Envi.h>
#include <EnviUtils.h>

int main(int argc, char** argv) {
    printf("Autoplay...\n");
    printf("Try capture Visual Studion...\n");

    auto CaptureCfg = Envi::CreateWindowCaptureConfiguration([&]() -> std::vector<Envi::Window> {
        auto wnds = Envi::GetWindowsWithNameKeywords({ "visual studio", "code", "autoplay" });
        if (wnds.size()) { printf("Found: %s\n", wnds.at(0).Name ); }
        return wnds;
    });

    CaptureCfg->SetTickInterval(20)->SetRecoverImages(true);
    auto CaptureManager = CaptureCfg->startCapturing();

    std::this_thread::sleep_for(std::chrono::milliseconds(30) );

    return 0;
}
