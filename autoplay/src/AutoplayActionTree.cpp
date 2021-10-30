#include "AutoplayActionTree.h"
#include "AutoplayCore.h"

namespace APlay {

    TargetComponent::TargetComponent(std::string id, const Json::Value& target) : Component(id) {
        _text = target["text"].asString();
        _path = target["path"].asString();
    }

    RuleComponent::RuleComponent(std::string id, const Json::Value& rule) : Component(id) {
        _command = rule["command"].asString();
        _script = rule["script"].asString();

        for (int i = 0; i < rule["target"].size(); i++) {
            _target.emplace_back(rule["target"][i].asString());
        }
    }

    void ActionTree::init(const Json::Value& _config) {
        _registry = std::make_shared<Ecs::Entity>();

        for (auto i : _config["targets"].getMemberNames() ) {
            _registry->addComponent<TargetComponent>(i, _config["targets"][i.c_str()]);
        }

        for (auto i : _config["rules"].getMemberNames() ) {
            _registry->addComponent<RuleComponent>(i, _config["rules"][i.c_str()]);
        }

    }

}

