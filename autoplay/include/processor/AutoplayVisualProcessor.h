#pragma once
#include "AutoplayActionTree.h"
#include "AutopleyNetwork.h"
#include "Envi.h"

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/dnn.hpp>

namespace APlay {

    class AutoplayTextProcessor {
    public:
        static void Init();

        static void ProcessImage(cv::Mat& image, const std::shared_ptr<ActionTree>& actionTree);

    private:
        static std::shared_ptr<AutoplayTextDetectionModel_EAST> _textDetector; 
        static std::shared_ptr<AutoplayTextRecognitionModel> _textRecognizer;

    };

    // class AutoplayImageProcessor : public AutoplayBaseProcessor {
    // };

    class AutoplayVisualProcessor {
    public:
        AutoplayVisualProcessor(const std::shared_ptr<ActionTree>& _actionTree) : actionTree(_actionTree) {
        }

        void ProcessImage(const Envi::Image &img);

    private:
        std::shared_ptr<ActionTree> actionTree;
        std::shared_ptr<unsigned char> convertedData;
    };


}

