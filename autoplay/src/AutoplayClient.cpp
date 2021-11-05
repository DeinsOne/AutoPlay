#include "AutoplayClient.h"

#include "AutoplayActionTree.h"
#include "AutoplayVisualProcessor.hpp"
#include "AutoplayLog.h"

#include <Envi.h>
#include <EnviUtils.h>
#include <thread>

namespace APlay {

    class AutoplayClient : public IAutoplayClient {
    public:
        AutoplayClient(std::shared_ptr<AutoplayJsonConfig> config) : _config(config) {
            _actionTree = CreateActionTree(_config);
            _visualProcessor = std::make_shared<AutoplayVisualProcessor<AutoplayTextDetectorEAST, AutoplayTextRecognizerCRNN>>(config, _actionTree);
        }

        ~AutoplayClient() {
            _terminate = true;
        }

        virtual void Pause() override {
            APLAY_PROFILE_FUNCTION();
            _captureManager->Pause();
            _paused = true;
        }

        virtual bool IsPaused() override {
            return _captureManager->IsPaused();
        }

        virtual void Resume() override {
            APLAY_PROFILE_FUNCTION();
            _captureManager->Resume();
        }

        void _Start() {
            _thread = std::thread([&]() {
                {
                    APLAY_PROFILE_SCOPE("void AuoplayClient::_Start threaded lambda");
                    printf("Configuration: %s v%s\n", _config->GetTitle().c_str(), _config->GetVersion().c_str() );
                    printf("%s %s\n", _config->GetAuthor().at(0).c_str(), _config->GetEdition().c_str() );
                    printf("Hardware threads: %d\n\n", std::thread::hardware_concurrency());

                    #if defined(_DEBUG)
                        APLAY_CONSOLE_INFO("Action tree initialized with {} targets, {} rules",
                            _actionTree->_registry->getComponentArraySize<TargetComponent>(),
                            _actionTree->_registry->getComponentArraySize<RuleComponent>()
                        );
                        APLAY_CONSOLE_INFO("Expected refresh rate {}(tps) | {}(ms)",
                            (int)(1000 / _config->GetInterval()),
                            _config->GetInterval()
                        );
                        printf("\n");
                    #endif

                    auto capConfig = Envi::CreateWindowCaptureConfiguration([&]() -> std::vector<Envi::Window> {
                        APLAY_PROFILE_SCOPE("void AutoplayClient::_Start CreateWindowCaptureConfiguration");
                        std::vector<Envi::Window> windows;
                        static bool _firstCall = true;

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
                        if (windows.size()) {
                            if (_firstCall) {
                                APLAY_CONSOLE_INFO("Found window: {}", windows.at(0).Name);
                                printf("\n");
                            }
                            _firstCall = false;
                        }
                        return windows;
                    });

                    capConfig->SetTickInterval(_config->GetInterval());
                    capConfig->SetRecoverImages(_config->GetRecover());

                    capConfig->OnNewFrame([&](const Envi::Image &img, const Envi::Window &window) {
                        APLAY_PROFILE_FUNCTION();
                        _visualProcessor->ProcessImage(img);
                    });

                    _captureManager = capConfig->startCapturing();
                }

                while (!_terminate) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(_config->GetInterval()));
                }
            });

            _thread.detach();
            _paused = false;
            _terminate = false;
        }

    private:
        bool _paused = false;
        bool _terminate = false;

        std::thread _thread;

        std::shared_ptr<AutoplayJsonConfig> _config;
        std::shared_ptr<Envi::ICapturerManager> _captureManager;

        std::shared_ptr<ActionTree> _actionTree;

        std::shared_ptr<AutoplayVisualProcessor<AutoplayTextDetectorEAST, AutoplayTextRecognizerCRNN>> _visualProcessor;

    };

    std::shared_ptr<IAutoplayClient> CreateAutoplayClient(std::shared_ptr<AutoplayJsonConfig> config) {
        APLAY_PROFILE_FUNCTION();
        auto client = std::make_shared<AutoplayClient>(config);
        client->_Start();
        return client;
    }

}
