#pragma once
#include <opencv4/opencv2/dnn/dnn.hpp>

namespace APlay {

    struct AutoplayTextDetectionModel_EAST {
        std::string _filename;
        std::unique_ptr<cv::dnn::TextDetectionModel_EAST> _model;
        cv::Scalar _detMean = cv::Scalar(123.68, 116.78, 103.94);
    };

    struct AutoplayTextDetectionModel_DB {
        std::string _filename;
        std::unique_ptr<cv::dnn::TextDetectionModel_DB> _model;
        cv::Scalar _mean = cv::Scalar(122.67891434, 116.66876762, 104.00698793);
    };

    struct AutoplayTextRecognitionModel {
        std::string _modelFilename;
        std::string _vaoFilename;
        std::unique_ptr<cv::dnn::TextRecognitionModel> _model;
        cv::Scalar _mean = cv::Scalar(127.5, 127.5, 127.5);
    };

    std::shared_ptr<AutoplayTextDetectionModel_EAST> CreateTextDetectionModelEAST(
        std::string filename,
        float confThreshold = 0.5,
        float nmsThreshold = 0.4,
        cv::Size detInputSize = {320, 320}
    );

    std::shared_ptr<AutoplayTextDetectionModel_DB> CreateTextDetectionModelDB(
        std::string filename,
        float binThresh = 0.3,
        float polyThresh = 0.5,
        uint maxCandidates = 200,
        double unclipRatio = 2.0,
        cv::Size detInputSize = {736, 736}
    );

    std::shared_ptr<AutoplayTextRecognitionModel> CreateTextRecognitionModel(
        std::string modelFilename,
        std::string vocabularyFilename,
        cv::Size detInputSize = {100, 32}
    );

}

