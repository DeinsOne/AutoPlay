#include "AutopleyNetwork.h"
#include "AutoplayLog.h"

#include <indicators/progress_spinner.hpp>
#include <indicators/cursor_control.hpp>

namespace APlay {


    std::shared_ptr<AutoplayTextDetectionModel_EAST> CreateTextDetectionModelEAST(
        std::string filename,
        float confThreshold,
        float nmsThreshold,
        cv::Size detInputSize
    ) {
        APLAY_PROFILE_FUNCTION();

        indicators::ProgressSpinner spinner{
            indicators::option::PostfixText{"Loading TD network..."},
            indicators::option::PrefixText{" "},
            indicators::option::ForegroundColor{indicators::Color::white},
            indicators::option::SpinnerStates{std::vector<std::string>{"⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂", "⠁"}},
            indicators::option::ShowPercentage{false},
            indicators::option::ShowSpinner{true}
        };

        bool loaded = false;

        std::thread thread([&spinner, &loaded, &filename]() {
            indicators::show_console_cursor(false);

            while (!loaded) {
                spinner.tick();
                spinner.set_progress(2);
                std::this_thread::sleep_for(std::chrono::milliseconds(40));
            }

            spinner.set_option(indicators::option::PrefixText{"[OK]"});
            spinner.set_option(indicators::option::PostfixText{"TD Network preloaded"});
            spinner.set_option(indicators::option::ShowSpinner{false});
            spinner.mark_as_completed();

            indicators::show_console_cursor(true);
        });
        thread.detach();

        auto rt = std::make_shared<AutoplayTextDetectionModel_EAST>();
        rt->_model = std::make_unique<cv::dnn::TextDetectionModel_EAST>(filename);

        rt->_model->setConfidenceThreshold(confThreshold)
             .setNMSThreshold(nmsThreshold)
        ;

        double detScale = 1.0;
        rt->_model->setInputParams(detScale, detInputSize, rt->_detMean, true);

        loaded = true;

        rt->_filename = filename;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return rt;
    }


    std::shared_ptr<AutoplayTextDetectionModel_DB> CreateTextDetectionModelDB(
        std::string filename,
        float binThresh,
        float polyThresh,
        uint maxCandidates,
        double unclipRatio,
        cv::Size detInputSize
    ) {
        APLAY_PROFILE_FUNCTION();

        bool loaded = false;

        indicators::ProgressSpinner spinner{
            indicators::option::PostfixText{"Loading TD network..."},
            indicators::option::PrefixText{" "},
            indicators::option::ForegroundColor{indicators::Color::white},
            indicators::option::SpinnerStates{std::vector<std::string>{"⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂", "⠁"}},
            indicators::option::ShowPercentage{false},
            indicators::option::ShowSpinner{true}
        };

        std::thread thread([&spinner, &loaded, &filename]() {
            indicators::show_console_cursor(false);

            while (!loaded) {
                spinner.tick();
                spinner.set_progress(2);
                std::this_thread::sleep_for(std::chrono::milliseconds(40));
            }

            spinner.set_option(indicators::option::PrefixText{"[OK]"});
            spinner.set_option(indicators::option::PostfixText{"TD Network preloaded"});
            spinner.set_option(indicators::option::ShowSpinner{false});
            spinner.mark_as_completed();

            indicators::show_console_cursor(true);
        });
        thread.detach();

        auto rt = std::make_shared<AutoplayTextDetectionModel_DB>();
        rt->_model = std::make_unique<cv::dnn::TextDetectionModel_DB>(filename);

        rt->_model->setBinaryThreshold(binThresh)
             .setPolygonThreshold(polyThresh)
             .setMaxCandidates(maxCandidates)
             .setUnclipRatio(unclipRatio)
        ;

        rt->_model->setInputParams(1.0 / 255.0, detInputSize, rt->_mean);

        loaded = true;

        rt->_filename = filename;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return rt;
    }


    std::shared_ptr<AutoplayTextRecognitionModel> CreateTextRecognitionModel(
        std::string modelFilename,
        std::string vocabularyFilename,
        cv::Size detInputSize
    ) {
        APLAY_PROFILE_FUNCTION();

        bool loaded = false;

        indicators::ProgressSpinner spinner{
            indicators::option::PostfixText{"Loading TR network..."},
            indicators::option::PrefixText{" "},
            indicators::option::ForegroundColor{indicators::Color::white},
            indicators::option::SpinnerStates{std::vector<std::string>{"⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂", "⠁"}},
            indicators::option::ShowPercentage{false},
            indicators::option::ShowSpinner{true}
        };

        std::thread thread([&spinner, &loaded]() {
            indicators::show_console_cursor(false);

            while (!loaded) {
                spinner.tick();
                spinner.set_progress(2);
                std::this_thread::sleep_for(std::chrono::milliseconds(40));
            }

            spinner.set_option(indicators::option::PrefixText{"[OK]"});
            spinner.set_option(indicators::option::PostfixText{"TR Network preloaded"});
            spinner.set_option(indicators::option::ShowSpinner{false});
            spinner.mark_as_completed();

            indicators::show_console_cursor(true);
        });
        thread.detach();

        auto rt = std::make_shared<AutoplayTextRecognitionModel>();
        rt->_model = std::make_unique<cv::dnn::TextRecognitionModel>(modelFilename);

        std::ifstream vocFile;
        vocFile.open(vocabularyFilename);
        CV_Assert(vocFile.is_open());
        cv::String vocLine;
        std::vector<cv::String> vocabulary;
        while (std::getline(vocFile, vocLine)) {
            vocabulary.push_back(vocLine);
        }

        loaded = true;

        rt->_model->setDecodeType("CTC-greedy");
        rt->_model->setVocabulary(vocabulary);
        rt->_model->setInputParams(1.0 / 127.5, detInputSize, rt->_mean);

        rt->_modelFilename = modelFilename;
        rt->_vaoFilename = vocabularyFilename;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return rt;
    }

}


