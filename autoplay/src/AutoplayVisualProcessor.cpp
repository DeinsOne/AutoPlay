#include "AutoplayVisualProcessor.h"
#include "EnviUtils.h"

#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>

void APlay::AutoplayVisualProcessor::ProcessImage(const Envi::Image &img) {
    APLAY_PROFILE_FUNCTION();

    auto converted = std::make_shared<cv::Mat>(Envi::Width(img), Envi::Height(img), CV_8UC4 );
    {
        APLAY_PROFILE_SCOPE("ExtractAndConvertToRGBA Envi::Image -> cv::Mat");
        Envi::ExtractAndConvertToRGBA(img, converted->data, 0);
    }

}

