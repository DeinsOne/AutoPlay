#ifndef __JSON_CONFIG
#define __JSON_CONFIG

#include <fstream>
#include <json/json.h>
#include "AutoplayCore.h"
#include "AutoplayCmdConfig.h"

namespace APlay {

    struct JsonReader {};

    class JsonParser : public IParser<JsonReader> {
        public:
            JsonParser(const char* file) : _file(file) { }

            // FIXME: Add file validator
            virtual JsonReader Init() override;

        protected:
            Json::Value _jsvalue;
            std::string _file;
    };

    std::shared_ptr<IParser<JsonReader>> CreateParser(const char* file);

    class AutoplayJsonConfig : public JsonParser {
        private:
            std::shared_ptr<AutoplayCmdConfig> _cmdconfig;

        public:
            AutoplayJsonConfig(std::string file) : JsonParser(file.c_str()) {
                Init();
                // AutoplayLogger::Get().log->info("JsonConfig created and initialized");
            }

            AutoplayJsonConfig(const std::shared_ptr<AutoplayCmdConfig>& config) : JsonParser(config->GetCfgFile().c_str()), _cmdconfig(config) { Init(); }

            std::string GetTitle(); 
            std::vector<std::string> GetAuthor();
            std::vector<std::string> GetWindowTitle();
            std::string GetFindTitleMethod();
            std::string GetEdition();
            std::string GetVersion();
            int GetAdapterType();
            int GetFindTitleMethodInt();
            int GetInterval();
            bool GetRecover();

            inline Json::Value GetJson() const {
                return _jsvalue;
            }
    };

    std::shared_ptr<AutoplayJsonConfig> CreateJsonConfig(std::string file);

    std::shared_ptr<AutoplayJsonConfig> CreateJsonConfig(const std::shared_ptr<AutoplayCmdConfig>& config);

}


#endif // __JSON_CONFIG
