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

            CmdParser(const CmdParser& parser) : _input{parser._input}, _parsed(parser._parsed) {  }

            inline virtual CommandLineArguments Init() override {
                cxxopts::Options options("Autoplay", "Automaticaly play specified game\n");
                options.add_options("Flags")
                    ("r,recover", "Recover mode")
                    ("help", "Help info");

                options.add_options("Args")
                    ("f,file", "Input configuration file", cxxopts::value<std::string>() )
                    ("i,interval", "Capturer tick interval", cxxopts::value<int>()->default_value("100") );

                _parsed = options.parse(_input.argc, _input.argv);

                if (_parsed["help"].count()) {
                    printf("%s\n", options.help().c_str());
                }

                return _input;
            }

            template<typename T>
            inline const T& GetArgument(const char* name) { return _parsed[name].as<T>(); }

        protected:
            CommandLineArguments _input;
            cxxopts::ParseResult _parsed;
    };

    template<typename T>
    inline const T& GetParserArgument(std::shared_ptr<IParser<CommandLineArguments>> parser, const char* name) {
        return dynamic_cast<CmdParser*>(parser.get())->GetArgument<T>(name);
    }

    inline std::shared_ptr<IParser<CommandLineArguments>> CreateParser(int argc, char** argv) {
        return std::make_shared<CmdParser>(argc, argv);
    }


    class AutoplayCmdConfig : public CmdParser {
        public:
            AutoplayCmdConfig(int argc, char** argv) : CmdParser(argc, argv) { Init(); }

            const char* GetCfgFile() { return GetArgument<std::string>("file").c_str(); }

            int GetInterval() { return GetArgument<int>("interval"); }

            int GetRecover() { return GetArgument<bool>("recover"); }

    };

}