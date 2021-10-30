#pragma once
#include <fstream>
#include <cxxopts.hpp>
#include "AutoplayCore.h"


namespace APlay {

    struct CommandLineArguments {
        int argc;
        char** argv;
    };

    class CmdParser : public IParser<CommandLineArguments> {
        public:
            CmdParser(int argc, char** argv) : _input{argc,argv} {  }

            virtual CommandLineArguments Init() override;

            template<typename T>
            T* GetArgument(std::string id) { return _root->getComponent<T>(id); }

        protected:
            CommandLineArguments _input;
    };

    class AutoplayCmdConfig : public CmdParser {
        public:
            AutoplayCmdConfig(int argc, char** argv) : CmdParser(argc, argv) { Init(); }

            std::string GetCfgFile();

            int GetInterval();

            int GetRecover();
    };

    std::shared_ptr<IParser<CommandLineArguments>> CreateParser(int argc, char** argv);

    std::shared_ptr<AutoplayCmdConfig> CreateCmdConfig(int argc, char** argv);

}