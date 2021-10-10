#pragma once
#include <fstream>
#include <json/json.h>
#include "AutoplayCore.h"
#include "AutoplayCmdConfig.h"

namespace APlay {

    struct JsonReader {};

    class JsonParser : public IParser<JsonReader> {
        public:
            JsonParser(const char* file) : _file(file) { }

            inline virtual JsonReader Init() override {
                std::ifstream configFile(_file.c_str());
                configFile >> _root;
                configFile.close();
                return JsonReader{};
            }

            inline const Json::Value& GetRoot(const char* name) { return _root; }

        protected:
            Json::Value _root;
            std::string _file;
    };

    inline const Json::Value& GetParserArgument(std::shared_ptr<IParser<JsonReader>> parser, const char* name) {
        return dynamic_cast<JsonParser*>(parser.get())->GetRoot(name);
    }
    inline std::shared_ptr<IParser<JsonReader>> CreateParser(const char* file) {
        return std::make_shared<JsonParser>(file);
    }


    class AutoplayJsonConfig : public JsonParser {
        private:
            std::shared_ptr<AutoplayCmdConfig> _cmdconfig;

        public:
            AutoplayJsonConfig(std::string file) : JsonParser(file.c_str()) { Init(); }
            AutoplayJsonConfig(const std::shared_ptr<AutoplayCmdConfig>& config) : JsonParser(config->GetCfgFile()), _cmdconfig(config) { Init(); }

            const char* GetCfgName() {
                if (!_root["cfgname"].empty())
                    return _root["cfgname"].asCString();
                else return "";
            }

            const char* GetAuthor() {
                if (!_root["author"][0].empty())
                    return _root["author"][0].asCString();
                else return "";
            }

            const char* GetEdition() {
                if (!_root["edition"].empty())
                    return _root["edition"].asCString();
                else return "";
            }

            const char* GetVersion() {
                if (!_root["version"].empty())
                    return _root["version"].asCString();
                else return "";
            }

            int GetInterval() {
                if (!_root["cfg"].empty() && _root["cfg"].isObject() ) {
                    if (!_root["cfg"]["interval"].empty() && _root["cfg"]["interval"].isInt())
                        return _root["cfg"]["interval"].asInt();
                    else return _cmdconfig->GetInterval();
                }
                else return _cmdconfig->GetInterval();
            }

            bool GetRecover() {
                if (!_root["cfg"].empty() && _root["cfg"].isObject() ) {
                    if (!_root["cfg"]["recover"].empty() && _root["cfg"]["recover"].isBool())
                        return _root["cfg"]["recover"].asBool();
                    else return _cmdconfig->GetRecover();
                }
                else return _cmdconfig->GetRecover();
            }

            std::vector<std::string> GetWindowTitle() {
                if (!_root["cfg"].empty() && _root["cfg"].isObject() ) {
                    if (!_root["cfg"]["window"]["title"].empty() && _root["cfg"]["window"]["title"].isArray()) {
                        std::vector<std::string> _title;
                        for (auto i : _root["cfg"]["window"]["title"] ) {
                            _title.emplace_back(i.asCString());
                        }
                        return _title;
                    }
                    return {};
                }
                else return {};
            }

            const char* GetFindTitleMethod() {
                if (!_root["cfg"].empty() && _root["cfg"].isObject() ) {
                    if (!_root["cfg"]["window"]["method"].empty() && _root["cfg"]["window"]["method"].isString())
                        return _root["cfg"]["window"]["method"].asCString();
                    else return "WITH_KEYWORDS";
                }
                else return "WITH_KEYWORDS";
            }

    };


}

