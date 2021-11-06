#pragma once
#include "AutoplayCore.h"
#include "AutoplayJsonConfig.h"
#include "ecs/uecs.h"

#include "AutoplayLog.h"

#include <memory>
#include <exception>
#include <iostream>

namespace APlay {

    struct ActionTreeBaseComponent {
        bool _active = true;
    };

    struct Target : public ActionTreeBaseComponent{
        std::string _text;
        std::string _path;
        // TODO: add pointer to loaded image from path
    };

    class TargetComponent : public Ecs::Component, public Target {
        public:
            TargetComponent(std::string id, const Json::Value& target);
    };

    struct Rule : public ActionTreeBaseComponent {
        std::string _command;
        std::string _script;
        std::vector<std::string> _target;
        // TODO: add lua state for rule
    };

    class RuleComponent : public Ecs::Component, public Rule {
        public:
            RuleComponent(std::string id, const Json::Value& rule);
    };

    class ActionTree {
        public:
            ActionTree(const Json::Value& _config) { init(_config); }
            ActionTree(const std::shared_ptr<AutoplayJsonConfig>& _config) { init(_config->GetJson()); }

            std::shared_ptr<Ecs::Entity> _registry;

        private:
            void init(const Json::Value& _config);
    };


    inline std::shared_ptr<ActionTree> CreateActionTree(const Json::Value& config) {
        return std::make_shared<ActionTree>(config);
    }

    inline std::shared_ptr<ActionTree> CreateActionTree(const std::shared_ptr<AutoplayJsonConfig> config) {
        APLAY_PROFILE_FUNCTION();
        return std::make_shared<ActionTree>(config);
    }

}
