#include <cstdio>
#include <Envi.h>
#include <EnviUtils.h>
#include <memory>

#include "AutoplayCore.h"
#include "AutoplayCmdConfig.h"
#include "AutoplayJsonConfig.h"
#include "AutoplayClient.h"
#include "AutoplayActionTree.h"

#include "ecs/uecs.h"

class String : public APlay::Ecs::Component {
    protected:
        std::string _value;

    public:
        String(std::string name, std::string value = "") : Component(name), _value(value) { }

        std::string GetValue() const { return _value; }

        void SetValue(const std::string& value) { _value = value; }

};

int main(int argc, char** argv) {

    auto ccfg = APlay::CreateCmdConfig(argc, argv);
    auto jcfg = APlay::CreateJsonConfig(ccfg);

    auto aClient = APlay::CreateAutoplayClient(jcfg);

    while (true) {
        if (Envi::IsKeyPressed(Envi::KeyCodes::KEY_LeftControl) &&
            Envi::IsKeyPressed(Envi::KeyCodes::KEY_W)
        ) {
            aClient->Pause();
            printf("\ndetected ctrl+w. Exit\n");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(jcfg->GetInterval()));
    }

    return 0;
}
