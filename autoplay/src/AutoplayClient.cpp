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

                    printf("\nConfiguration: %s v%s\n", _config->GetTitle().c_str(), _config->GetVersion().c_str() );
                    printf("%s %s\n\n", _config->GetAuthor().at(0).c_str(), _config->GetEdition().c_str() );

                    // _actionTree = ActionTree::CreateActionTree(_config);

                    auto capConfig = Envi::CreateWindowCaptureConfiguration([&]() -> std::vector<Envi::Window> {
                        std::vector<Envi::Window> windows;

                        switch (_config->GetFindTitleMethodInt()) {
                            case (AutoplayClientFindWindowMethod::WithKeywords): {
                                windows = Envi::GetWindowsWithNameKeywords(_config->GetWindowTitle());
                                break;
                            }
                            case (AutoplayClientFindWindowMethod::Match): {
                                windows = Envi::GetWindowsWithNameContains(_config->GetWindowTitle().at(0));
                                break;
                            }
                            case (AutoplayClientFindWindowMethod::None): {
                                windows = Envi::GetWindowsWithNameKeywords(_config->GetWindowTitle());
                                break;
                            }
                        }

                        if (windows.size()) { printf("Found: %s\n", windows.at(0).Name ); }
                        return windows;
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

            // std::shared_ptr<ActionTree::ActionTree> _actionTree;

    };

    std::shared_ptr<IAutoplayClient> CreateAutoplayClient(std::shared_ptr<AutoplayJsonConfig> config) {
        auto client = std::make_shared<AutoplayClient>(config);
        client->_Start();
        return client;
    }

}
