#pragma once
#include "AutoplayActionTree.h"
#include "Envi.h"

#include <opencv4/opencv2/core.hpp>

namespace APlay {

    class AutoplayBaseProcessor {
    public:
        virtual void ProcessImage(
            const std::shared_ptr<ActionTree>& actionTree,
            const std::shared_ptr<cv::Mat>& image
        ) = 0;

        virtual std::size_t GetResultsCount() = 0;

        virtual double GetElapsedTime() = 0;
    };

    class AutoplayTextProcessor : public AutoplayBaseProcessor {
    public:
        AutoplayTextProcessor(
            const std::shared_ptr<ActionTree>& actionTree,
            const std::shared_ptr<cv::Mat>& image
        ) {
            ProcessImage(actionTree, image);
        }

        virtual std::size_t GetResultsCount() override;

        virtual double GetElapsedTime() override;

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
    };


}

