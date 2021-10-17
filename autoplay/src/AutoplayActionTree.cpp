#include "AutoplayActionTree.h"

namespace APlay {

    namespace ActionTree {

        #define __prefix "ActionTree err -> "
        #define __badType(entt) __prefix entt ": bad type"
        #define __badFields(entt) __prefix entt ": bad fields"
        #define __badCollision(entt) __prefix entt ": bad collision"

        AMethod __detectMethod(std::string str) {
            if (str.compare("TEMPLATE_MATCHING")) return AMethod::AMethod_TemplateMatching;
            else return AMethod::AMethod_None;
        }

        int ATarget::operator<<(std::string name) {
            _name = name;
            return 0;
        }

        int ATarget::operator<<(Json::Value component) {

            if (!component["image"].empty() && !component["text"].empty() ) {
                throw ActionTreeException(__badCollision("text/iamge"));
            }

            // Image field
            if (!component["image"].empty() && component["image"].isString() ) { 
                // FIXME: Load image
                _type = ATargetType::ATargetType_Resource;
                // _image = reinterpret_cast<const unsigned char*>(component["image"].asCString());
            }
            else if (!component["image"].empty() && !component["image"].isString() ) {
                throw ActionTreeException(__badType("image"));
            }
            else if (component["image"].empty() && component["text"].empty() ) {
                throw ActionTreeException(__badFields("image/text"));
            }

            // Text field
            if (!component["text"].empty() && component["text"].isString() ) { 
                _type = ATargetType::ATargetType_Text;
                _text = component["text"].asString();           
            }
            else if (!component["text"].empty() && !component["text"].isString() ) {
                throw ActionTreeException(__badType("text"));
            }
            else if (component["image"].empty() && component["text"].empty() ) {
                throw ActionTreeException(__badFields("image/text"));
            }

            return 0;
        }

        int ARule::operator<<(const char* name) {
            _name = name;
            return 0;
        }

        int ARule::operator<<(Json::Value component) {

            if (!component["target"].empty() ) {
                if (component["target"].isString()) {
                    _targetName.push_back(component["target"].asString());
                }
                else if (component["target"].isArray() && component["target"][0].isString()) {
                    for (int i = 0; i < component["target"].size(); i++ ) {
                        _targetName.push_back(component["target"][i].asString());
                    }
                }
                else {
                    throw ActionTreeException(__badFields("target"));
                }
            }
            else {
                throw ActionTreeException(__badFields("target"));
            }

            // method
            if (!component["method"].empty() && component["method"].isString() ) {
                _method = __detectMethod(component["method"].asString());
            }
            else if (!component["method"].empty() && !component["method"].isString() ) {
                throw ActionTreeException(__badType("method"));
            }
            else {
                _method = AMethod::AMethod_None;
            }

            // Action
            if (!component["do"].empty() && component["do"].isString() ) {
                _do = component["do"].asString();
            }
            else if (!component["do"].empty() && !component["do"].isString()) {
                throw ActionTreeException(__badType("do"));
            }
            else {
                throw ActionTreeException(__badFields("do"));
            }

            return 0;
        }

        int ActionTree::operator<<(std::shared_ptr<APlay::AutoplayJsonConfig> config) {
            for (int i = 0; i < config->GetRoot("")["targets"].getMemberNames().size(); i++ ) {
                ATarget target;
                target << config->GetRoot("")["targets"][config->GetRoot("")["targets"].getMemberNames().at(i)];
                target << config->GetRoot("")["targets"].getMemberNames()[i];
                _targetTree._targets.insert({ {target._name, target} } );
            }

            for (int i = 0; i < config->GetRoot("")["rules"].getMemberNames().size(); i++ ) {
                ARule rule;
                rule << config->GetRoot("")["rules"][config->GetRoot("")["rules"].getMemberNames().at(i)];
                rule << config->GetRoot("")["rules"].getMemberNames().at(i).c_str();
                _ruleTree._rules.insert({ {rule._name, rule} } );
            }            
        }


        std::shared_ptr<ActionTree> CreateActionTree(std::shared_ptr<APlay::AutoplayJsonConfig> config) {
            auto tree = std::make_shared<ActionTree>();

            for (int i = 0; i < config->GetRoot("")["targets"].getMemberNames().size(); i++ ) {
                ATarget target;
                target << config->GetRoot("")["targets"][config->GetRoot("")["targets"].getMemberNames().at(i)];
                target << config->GetRoot("")["targets"].getMemberNames()[i];

                tree->_targetTree._targets.insert({ {target._name, target} } );
            }

            for (int i = 0; i < config->GetRoot("")["rules"].getMemberNames().size(); i++ ) {
                ARule rule;
                rule << config->GetRoot("")["rules"][config->GetRoot("")["rules"].getMemberNames().at(i)];
                rule << config->GetRoot("")["rules"].getMemberNames().at(i).c_str();

                tree->_ruleTree._rules.insert({ {rule._name, rule} } );
            }


            return tree;
        }

    }

}

