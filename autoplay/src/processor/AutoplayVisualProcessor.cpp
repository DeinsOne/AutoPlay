#include "AutoplayVisualProcessor.hpp"
#include "EnviUtils.h"

#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>

namespace Envi {
    void ExtractAndConvertToRGB(const Envi::Image &img, unsigned char *dst) {
        auto imgsrc = StartSrc(img);
        auto imgdist = dst;
        for (auto h = 0; h < Height(img); h++) {
            auto startimgsrc = imgsrc;
            for (auto w = 0; w < Width(img); w++) {
                *imgdist++ = imgsrc->R;
                *imgdist++ = imgsrc->G;
                *imgdist++ = imgsrc->B;
                imgsrc++;
            }
            imgsrc = Envi::GotoNextRow(img, startimgsrc);
        }
    }
}

namespace APlay {

    void _AutoplayVisualProcessorProcessImage(
        const Envi::Image &img,
        const _Tree& actionTree,
        std::shared_ptr<unsigned char>& convertedData,
        const std::shared_ptr<BaseTextProcessor>& textProcessor
    ) {
        convertedData = std::shared_ptr<unsigned char>(
            (unsigned char*)malloc(Envi::Height(img) * Envi::Width(img) * sizeof(Envi::ImageBGRA))
        );

        cv::Mat converted(Envi::Height(img), Envi::Width(img), CV_8UC3, convertedData.get());
        {
            APLAY_PROFILE_SCOPE("ExtractAndConvertToRGBA Envi::Image -> cv::Mat");
            Envi::ExtractAndConvertToRGB(img, convertedData.get());
        }

        textProcessor->ProcessImage(converted);
    }

}
