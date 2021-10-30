#include "AutoplayJsonConfig.h"
#include "AutoplayClient.h"
#include "ConfigValidator.h"

namespace APlay {

    enum _JsonValueType {
        _value_none = 0,
        _value_singel,
        _value_array
    };

    template<typename TReal>
    void initValue(const char* id, Json::Value& json, Ecs::Entity* registry, const short& type ) {
        if (!json[id].empty() ) {
            auto realType = Ecs::__getComponentTypeID<TReal>();

            if (type & _JsonValueType::_value_singel && json[id].is<TReal>() ) {
                if (realType == Ecs::__getComponentTypeID<bool>() ) { registry->addComponent<Ecs::Bool>(id, json[id].asBool()); }
                else if (realType == Ecs::__getComponentTypeID<int>() ) { registry->addComponent<Ecs::Int>(id, json[id].asInt()); }
                else if (realType == Ecs::__getComponentTypeID<float>() ) { registry->addComponent<Ecs::Float>(id, json[id].asFloat()); }
                else if (realType == Ecs::__getComponentTypeID<std::string>() ) { registry->addComponent<Ecs::String>(id, json[id].as<std::string>()); }
            }

            else if (type & _JsonValueType::_value_array && json[id].isArray() && json[id][0].is<TReal>() ) {
                std::vector<TReal> _arr;

                for (int i = 0; i < json[id].size(); i++ ) {
                    _arr.push_back(json[id][i].as<TReal>() );
                }

                registry->addComponent<Ecs::Array<TReal>>(id, _arr );               
            }

            else if (type & _JsonValueType::_value_array && !json[id].isArray() && json[id].is<TReal>()) {
                std::vector<TReal> _arr;
                _arr.push_back(json[id].as<TReal>());

                registry->addComponent<Ecs::Array<TReal>>(id, _arr );
            }
        }
    }

    JsonReader JsonParser::Init() {
        _root = std::make_shared<Ecs::Entity>();

        std::ifstream configFile(_file.c_str());
        configFile >> _jsvalue;
        configFile.close();

        ConfigValidator valid(_file, _jsvalue);

        initValue<std::string>("title", _jsvalue, _root.get(),
            _JsonValueType::_value_singel
        );

        initValue<std::string>("author", _jsvalue, _root.get(),
            _JsonValueType::_value_singel | _JsonValueType::_value_array
        );

        initValue<std::string>("edition", _jsvalue, _root.get(),
            _JsonValueType::_value_singel
        );

        initValue<std::string>("version", _jsvalue, _root.get(),
            _JsonValueType::_value_singel | _JsonValueType::_value_array
        );

        initValue<int>("interval", _jsvalue, _root.get(),
            _JsonValueType::_value_singel
        );

        initValue<bool>("recover", _jsvalue, _root.get(),
            _JsonValueType::_value_singel
        );

        initValue<std::string>("type", _jsvalue["adapter"], _root.get(),
            _JsonValueType::_value_singel
        );

        initValue<std::string>("window", _jsvalue["adapter"], _root.get(),
            _JsonValueType::_value_array
        );

        initValue<std::string>("method", _jsvalue["adapter"], _root.get(),
            _JsonValueType::_value_singel 
        );

        return JsonReader{};
    }

    std::string AutoplayJsonConfig::GetTitle() {
        return _root->getComponent<Ecs::String>("title")->GetValue();
    }

    std::vector<std::string> AutoplayJsonConfig::GetAuthor() {
        return _root->getComponent<Ecs::Array<std::string>>("author")->GetValue();
    }

    std::string AutoplayJsonConfig::GetEdition() {
        return _root->getComponent<Ecs::String>("edition")->GetValue();
    }

    std::string AutoplayJsonConfig::GetVersion() {
        return _root->getComponent<Ecs::String>("version")->GetValue();
    }

    int AutoplayJsonConfig::GetInterval() {
        if (_root->hasComponent<Ecs::Int>("interval") ) {
            return _root->getComponent<Ecs::Int>("interval")->GetValue();
        }
        else {
            return _cmdconfig->GetInterval();
        }
    }

    bool AutoplayJsonConfig::GetRecover() {
        if (_root->hasComponent<Ecs::Bool>("recover") ) return _root->getComponent<Ecs::Bool>("recover")->GetValue();
        else return _cmdconfig->GetRecover();
    }

    int AutoplayJsonConfig::GetAdapterType() {
        if (_root->getComponent<Ecs::String>("type")->GetValue().compare("desktop"))
            return AutoplayAdapterType::AdapterDesktop;
        else
            return AutoplayAdapterType::AdapterNone;
    }

    std::vector<std::string> AutoplayJsonConfig::GetWindowTitle() {
        return _root->getComponent<Ecs::Array<std::string>>("window")->GetValue();
    }

    std::string AutoplayJsonConfig::GetFindTitleMethod() {
        return _root->getComponent<Ecs::String>("method")->GetValue();
    }

    int AutoplayJsonConfig::GetFindTitleMethodInt() {
        if (_root->hasComponent<Ecs::String>("method") ) {
            if (_root->getComponent<Ecs::String>("method")->GetValueLower().compare("WITH_KEYWORDS") ) return (int)AutoplayClientFindWindowMethod::WithKeywords;
            else if (_root->getComponent<Ecs::String>("method")->GetValueLower().compare("WITH_KEYWORDS") ) return (int)AutoplayClientFindWindowMethod::Match;
        }
        else {
            return (int)AutoplayClientFindWindowMethod::None;
        }
    }



    std::shared_ptr<IParser<JsonReader>> CreateParser(const char* file) {
        return std::make_shared<JsonParser>(file);
    }

    std::shared_ptr<AutoplayJsonConfig> CreateJsonConfig(std::string file) {
        return std::make_shared<AutoplayJsonConfig>(file);
    }

    std::shared_ptr<AutoplayJsonConfig> CreateJsonConfig(const std::shared_ptr<AutoplayCmdConfig>& config) {
        return std::make_shared<AutoplayJsonConfig>(config);
    }

}
