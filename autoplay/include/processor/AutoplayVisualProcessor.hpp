#pragma once
#include "AutoplayActionTree.h"
#include "AutopleyNetwork.h"
#include "Envi.h"

namespace APlay {

    using _Configuration = std::shared_ptr<AutoplayJsonConfig>;
    using _PointVec = std::vector<std::vector<cv::Point>>;

    class AutoplayBaseProcessor {
    public:
        virtual void Init(const _Configuration& config) = 0;

        virtual void ProcessImage(const cv::Mat& image) = 0;
    };

    class AutoplayTextDetector : public AutoplayBaseProcessor {
        public:
            virtual _PointVec GetRegions() = 0;

            virtual void Detect(const cv::Mat& image) = 0;
    };

    using _Dictionary = std::vector<std::string>;

    class AutoplayTextRecognizer : public AutoplayBaseProcessor {
        public:
            virtual _Dictionary GetDictionary() = 0;

            virtual void Recognize(const cv::Mat& image, _PointVec regions) = 0;
    };



    class AutoplayTextDetectorEAST : public AutoplayTextDetector {
    public:
        virtual void Init(const _Configuration& config) override;

        virtual void ProcessImage(const cv::Mat& image) override;

        virtual void Detect(const cv::Mat& image) override {
            ProcessImage(image);
        }

        virtual _PointVec GetRegions() override {
            return _regions;
        }

    private:
        std::shared_ptr<AutoplayTextDetectionModel_EAST> _model;
        _PointVec _regions;
    };

    class AutoplayTextRecognizerCRNN : public AutoplayTextRecognizer {
    public:
        virtual void Init(const _Configuration& config) override;

        virtual void ProcessImage(const cv::Mat& image) override;

        virtual void Recognize(const cv::Mat& image, _PointVec regions) override {
            _dict.clear();
            SetRegions(regions);
            ProcessImage(image);
        }

        void SetRegions(const _PointVec& regions) {
            _regions = regions;
        }

        virtual _Dictionary GetDictionary() override {
            return _dict;
        }

    private:
        std::shared_ptr<AutoplayTextRecognitionModel> _model;
        _Dictionary _dict;
        _PointVec _regions;
    };

    using _Tree = std::shared_ptr<ActionTree>;

    class BaseTextProcessor : public AutoplayBaseProcessor {
        public:
            virtual void Process(const cv::Mat& image, const _Tree& actionTree) = 0;
    };

    // FIXME:
    void _AutoplayTextProcessorUpdateTree(
        const std::shared_ptr<AutoplayTextDetector>& detector,
        const std::shared_ptr<AutoplayTextRecognizer>& recognizer,
        const _Tree& actionTree
    );

    template<typename _Detector, typename _Recognizer>
    class AutoplayTextProcessor : public BaseTextProcessor {
    public:
        AutoplayTextProcessor(const _Configuration& config) {
            _textDetector = std::make_shared<_Detector>();
            _textRecognizer = std::make_shared<_Recognizer>();
            Init(config);
        }

        virtual void Init(const _Configuration& config) override {
            _textDetector->Init(config);
            _textRecognizer->Init(config);
            printf("\n");
        }

        virtual void ProcessImage(const cv::Mat& image) override {
            APLAY_PROFILE_FUNCTION();
            _textDetector->Detect(image);
            _textRecognizer->Recognize(image, _textDetector->GetRegions());
            UpdateTree();
        }

        void SetActionTree(const _Tree& actionTree) {
            _actionTree = actionTree;
        }

        virtual void Process(const cv::Mat& image, const _Tree& actionTree) override {
            SetActionTree(actionTree);
            ProcessImage(image);
        }

    private:
        void UpdateTree() {
            APLAY_PROFILE_FUNCTION();
            _AutoplayTextProcessorUpdateTree(
                static_cast<std::shared_ptr<AutoplayTextDetector>>(_textDetector),
                static_cast<std::shared_ptr<AutoplayTextRecognizer>>(_textRecognizer),
                _actionTree
            );
        }

    private:
        std::shared_ptr<_Detector> _textDetector; 
        std::shared_ptr<_Recognizer> _textRecognizer;
        std::shared_ptr<ActionTree> _actionTree;
    };


    void _AutoplayVisualProcessorProcessImage(
        const Envi::Image &img,
        const _Tree& actionTree,
        std::shared_ptr<unsigned char>& convertedData,
        const std::shared_ptr<BaseTextProcessor>& textProcessor
    );

    template<typename _TextDetector, typename _TextRecognizer>
    class AutoplayVisualProcessor {
    public:
        AutoplayVisualProcessor(const _Configuration& config, const _Tree& _actionTree) : actionTree(_actionTree) {
            textProcessor = std::make_shared<AutoplayTextProcessor<_TextDetector, _TextRecognizer>>(config);
        }

        void ProcessImage(const Envi::Image &img) {
            APLAY_PROFILE_FUNCTION();
            _AutoplayVisualProcessorProcessImage(
                img, actionTree, convertedData,
                static_cast<std::shared_ptr<BaseTextProcessor>>(textProcessor)
            );
        }

    private:
        std::shared_ptr<ActionTree> actionTree;
        std::shared_ptr<unsigned char> convertedData;

        std::shared_ptr<AutoplayTextProcessor<_TextDetector, _TextRecognizer>> textProcessor;        
    };


}

