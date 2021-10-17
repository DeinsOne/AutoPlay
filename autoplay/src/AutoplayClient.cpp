#include "AutoplayClient.h"
#include "AutoplayActionTree.h"
#include <Envi.h>
#include <EnviUtils.h>
#include <thread>


namespace APlay {

    class AutoplayClient : public IAutoplayClient {
        public:
            AutoplayClient(std::shared_ptr<AutoplayJsonConfig> config) : _config(config) {
            }

            ~AutoplayClient() {
                _terminate = true;
            }

            virtual void Pause() override { _captureManager->Pause(); }

            virtual bool IsPaused() override { return _captureManager->IsPaused(); }

            virtual void Resume() override { _captureManager->Resume(); }

            void _Start() {
                _thread = std::thread([&]() {

                    printf("Configuration: %s v%s\n", _config->GetCfgName(), _config->GetVersion() );
                    printf("%s %s\n\n", _config->GetAuthor(), _config->GetEdition() );

                    _actionTree = ActionTree::CreateActionTree(_config);

                    auto capConfig = Envi::CreateWindowCaptureConfiguration([&]() -> std::vector<Envi::Window> {
                        if (strcmp(_config->GetFindTitleMethod(), "WITH_KEYWORDS") == 0) {
                            auto wnds = Envi::GetWindowsWithNameKeywords(_config->GetWindowTitle());
                            if (wnds.size()) { printf("Found: %s\n", wnds.at(0).Name ); }
                            return wnds;
                        }
                        else if (strcmp(_config->GetFindTitleMethod(), "MATCH") == 0) {
                            auto wnds = Envi::GetWindowsWithNameContains(_config->GetWindowTitle().at(0) );
                            if (wnds.size()) { printf("Found: %s\n", wnds.at(0).Name ); }
                            return wnds;
                        }
                        // Default method WITH_KEYWORDS
                        else {
                            auto wnds = Envi::GetWindowsWithNameKeywords(_config->GetWindowTitle());
                            if (wnds.size()) { printf("Found: %s\n", wnds.at(0).Name ); }
                            return wnds;
                        }
                    });

                    // capConfig->OnNewFrame([&](const Envi::Image &img, const Envi::Window &window) { });

                    capConfig->SetTickInterval(_config->GetInterval());
                    capConfig->SetRecoverImages(_config->GetRecover());

                    _captureManager = capConfig->startCapturing();

                    while (!_terminate) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(_config->GetInterval()));

                    }

                });

                _paused = false;
                _terminate = false;
                _thread.detach();
            }

        private:
            bool _paused = false;
            bool _terminate = false;

            std::thread _thread;

            std::shared_ptr<AutoplayJsonConfig> _config;
            std::shared_ptr<Envi::ICapturerManager> _captureManager;

            std::shared_ptr<ActionTree::ActionTree> _actionTree;

    };

    std::shared_ptr<IAutoplayClient> CreateAutoplayClient(std::shared_ptr<AutoplayJsonConfig> config) {
        auto client = std::make_shared<AutoplayClient>(config);
        client->_Start();
        return client;
    }

}
