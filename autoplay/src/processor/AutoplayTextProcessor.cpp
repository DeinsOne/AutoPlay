#include "AutoplayVisualProcessor.h"
#include "AutoplayLog.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace APlay {

    std::shared_ptr<AutoplayTextDetectionModel_EAST> AutoplayTextProcessor::_textDetector;
    std::shared_ptr<AutoplayTextRecognitionModel> AutoplayTextProcessor::_textRecognizer;

    void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result);

    void AutoplayTextProcessor::Init() {
        APLAY_PROFILE_FUNCTION();
        _textRecognizer = CreateTextRecognitionModel(
            "assets/.model/crnn_cs.onnx",
            "assets/.model/alphabet_94.txt"
        );

        _textDetector = CreateTextDetectionModelEAST(
            "assets/.model/frozen_east_text_detection.pb",
            0.1, 0.2,
            {320, 736}
        );
    }

    void AutoplayTextProcessor::ProcessImage(cv::Mat& image, const std::shared_ptr<ActionTree>& actionTree) {
        APLAY_PROFILE_FUNCTION();

        if (!_textDetector) {
            APLAY_CONSOLE_CRITICAL("Network is not preloaded!");
        }

        std::vector<std::vector<cv::Point>> detectResults;

        {
            APLAY_PROFILE_SCOPE("_textRecognizer->_model->recognize()");
            _textDetector->_model->detect(image, detectResults);
            APLAY_CONSOLE_INFO("AutoplayTextProcessor detected {} results", detectResults.size());
        }

        {
            APLAY_PROFILE_SCOPE("_textDetector->_model->detect()");

            for (auto i : detectResults) {
                cv::Mat cropped;

                std::vector<cv::Point2f> quadrangle_2f;
                for (int j = 0; j < 4; j++)
                quadrangle_2f.emplace_back(i[j]);

                fourPointsTransform(image, quadrangle_2f.data(), cropped);
                std::string recognitionResult = _textRecognizer->_model->recognize(cropped);

                APLAY_CONSOLE_INFO("AutoplayTextProcessor recognized \"{}\"", recognitionResult);
            }
        }

        // {
            // APLAY_PROFILE_SCOPE("lining");
            // cv::polylines(image, detResults, true, cv::Scalar(0, 255, 0), 2);
        // }

        // cv::imshow("Text Detection", image);
        // cv::waitKey();
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

}

