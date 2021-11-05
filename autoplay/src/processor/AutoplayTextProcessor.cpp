#include "AutoplayVisualProcessor.hpp"
#include "AutoplayProcessManager.h"
#include "AutoplayLog.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace APlay {

    using _Configuration = std::shared_ptr<AutoplayJsonConfig>;
    using _Tree = std::shared_ptr<ActionTree>;
    using _PointVec = std::vector<std::vector<cv::Point>>;
    using _Dictionary = std::vector<std::string>;

    void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result);

    void AutoplayTextDetectorEAST::Init(const _Configuration& config) {
        APLAY_PROFILE_FUNCTION();
        _model = CreateTextDetectionModelEAST(
            "assets/.model/frozen_east_text_detection.pb",
            0.1, 0.2,
            {320, 736}
        );
    }

    void AutoplayTextRecognizerCRNN::Init(const _Configuration& config) {
        APLAY_PROFILE_FUNCTION();
        _model = CreateTextRecognitionModel(
            "assets/.model/crnn_cs.onnx",
            "assets/.model/alphabet_94.txt"
        );
    }

    void AutoplayTextDetectorEAST::ProcessImage(const cv::Mat& image) {
        APLAY_PROFILE_FUNCTION();
        _regions.clear();
        _model->_model->detect(image, _regions);
        APLAY_CONSOLE_INFO("AutoplayTextDetectorEAST detected {} results", _regions.size());
    }

    void AutoplayTextRecognizerCRNN::ProcessImage(const cv::Mat& image) {
        APLAY_PROFILE_FUNCTION();
        std::mutex _blockData;

        auto manager = CreateProcessManager<_PointVec, _PointVec::iterator>(_regions, [&](_PointVec container) -> int {
            for (auto i : container) {
                cv::Mat cropped;

                std::vector<cv::Point2f> quadrangle_2f;
                for (int j = 0; j < 4; j++)
                    quadrangle_2f.emplace_back(i[j]);

                fourPointsTransform(image, quadrangle_2f.data(), cropped);

                {
                    std::lock_guard<std::mutex> lk(_blockData);
                    _dict.emplace_back(_model->_model->recognize(cropped));
                }
            }
            return 0;
        });
        manager->_execute();
    }

    void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result) {
        const cv::Size outputSize = cv::Size(100, 32);

        cv::Point2f targetVertices[4] = {
            cv::Point(0, outputSize.height - 1),
            cv::Point(0, 0),
            cv::Point(outputSize.width - 1, 0),
            cv::Point(outputSize.width - 1, outputSize.height - 1)
        };
        cv::Mat rotationMatrix = getPerspectiveTransform(vertices, targetVertices);

        cv::warpPerspective(frame, result, rotationMatrix, outputSize);
    }

    void _AutoplayTextProcessorUpdateTree(const std::shared_ptr<AutoplayTextDetector>& detector, const std::shared_ptr<AutoplayTextRecognizer>& recognizer, const _Tree& actionTree) {

    }


}

