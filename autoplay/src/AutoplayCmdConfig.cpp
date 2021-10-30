#include "AutoplayCmdConfig.h"

namespace APlay {

    CommandLineArguments CmdParser::Init() {
        _root = std::make_shared<Ecs::Entity>();

        cxxopts::Options options("Autoplay", "Automaticaly play specified game\n");
        options.add_options("Flags")
            ("r,recover", "Recover mode")
            ("help", "Help info");

        options.add_options("Args")
            ("f,file", "Input configuration file", cxxopts::value<std::string>())
            ("i,interval", "Capturer tick interval", cxxopts::value<int>()->default_value("100"));

        cxxopts::ParseResult _parsed = options.parse(_input.argc, _input.argv);

        if (_parsed["help"].count()) {
            printf("%s\n", options.help().c_str());
        }

        if (_parsed["file"].count()) {
            _root->addComponent<Ecs::String>("file", _parsed["file"].as<std::string>());
        } else {
            // TODO: Throw error
        }

        _root->addComponent<Ecs::Bool>("recover", _parsed["recover"].as<bool>());
        _root->addComponent<Ecs::Int>("interval", _parsed["interval"].as<int>());

        return _input;
    }

    std::shared_ptr<IParser<CommandLineArguments>> CreateParser(int argc, char **argv) {
        return std::make_shared<CmdParser>(argc, argv);
    }

    std::string AutoplayCmdConfig::GetCfgFile() { return GetArgument<Ecs::String>("file")->GetValue(); }

    int AutoplayCmdConfig::GetInterval() { return GetArgument<Ecs::Int>("interval")->GetValue(); }

    int AutoplayCmdConfig::GetRecover() { return GetArgument<Ecs::Bool>("recover")->GetValue(); }

    std::shared_ptr<AutoplayCmdConfig> CreateCmdConfig(int argc, char** argv) {
        return std::make_shared<AutoplayCmdConfig>(argc, argv);
    }

}
