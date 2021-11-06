#include "AutoplayCmdVisializer.h"
#include "AutoplayLog.h"

#include <indicators/indeterminate_progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <indicators/termcolor.hpp>
#include <indicators/terminal_size.hpp>

namespace APlay {

    class AutoplayCmdVisualizer : public IAutoplayCmdVisualizer {
        public:
            ~AutoplayCmdVisualizer() { Stop(); }

            AutoplayCmdVisualizer(const std::shared_ptr<AutoplayJsonConfig>& _config) {
                interval = _config->GetInterval();
            }

            virtual void Start() override {
                rootThread = std::thread(
                    [&]() {
                        indicators::IndeterminateProgressBar bar{
                            // indicators::option::HideBarWhenComplete{true},
                            indicators::option::BarWidth{indicators::terminal_width() - 30},
                            indicators::option::Start{" ["},
                            indicators::option::Fill{"·"},
                            indicators::option::Lead{"<==>"},
                            indicators::option::End{"] "},
                            indicators::option::PrefixText{"  In process"},
                            indicators::option::PostfixText{std::to_string(currentInterval) + "/"  + std::to_string(interval) + " (TPS)"},
                            indicators::option::ForegroundColor{indicators::Color::white},
                            // indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
                        };

                        indicators::show_console_cursor(false);

                        while (active) {
                            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                            bar.tick();
                            bar.set_option(
                                indicators::option::PostfixText{std::to_string(1000/currentInterval) + "/"  + std::to_string(1000/interval) + " (TPS)"}
                            );

                        }

                        bar.mark_as_completed();
                        indicators::show_console_cursor(true);
                    }
                );

                rootThread.detach();
            }

            virtual void Stop() override {
                indicators::show_console_cursor(true);
                active = false;
            }

            virtual void PassInterval(const int& _interval) override {
                // AutoplayLogger::Get().log->info("CmdVisualizer pass new interval {}ms", _interval);
                currentInterval = _interval;
            }

        private:
            bool active = true;

            std::thread rootThread;
            int interval = 0;
            int currentInterval = 0;
    };


    std::shared_ptr<IAutoplayCmdVisualizer> CreateCmdVisualizer(const std::shared_ptr<AutoplayJsonConfig>& _config) {
        APLAY_PROFILE_FUNCTION();
        return std::make_shared<AutoplayCmdVisualizer>(_config);
    }

}

