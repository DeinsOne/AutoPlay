#include "AutoplayActionTree.h"
#include "AutoplayCore.h"

namespace APlay {

    namespace ActionTree {

        #define __prefix "ActionTree err -> "

        std::string __badFields(std::string name, std::string where) {
            return (__prefix "bad fields. where: " + where + " field: " + name);
        }
        std::string __badFieldType(std::string name, std::string where,std::string expected) {
            return (__prefix "bad type. where: " + where + " field: " + name + " expected: " + expected);
        }

        std::string __fieldsCollision(std::string names) {
            return (__prefix + names + ": bad collision");
        }
        std::string __missedFiled(std::string name, std::string where) {
            (__prefix "missed fields: " + name + " where: " + where);
        }


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
                throw ActionTreeException(__fieldsCollision("text/iamge"));
            }

            // Image field
            if (!component["image"].empty() && component["image"].isString() ) { 
                // FIXME: Load image
                _type = ATargetType::ATargetType_Resource;
                // _image = reinterpret_cast<const unsigned char*>(component["image"].asCString());
            }
            else if (!component["image"].empty() && !component["image"].isString() ) {
                throw ActionTreeException(__badFieldType("image", "targets." + _name + ".image", "string"));
            }
            else if (component["image"].empty() && component["text"].empty() ) {
                throw ActionTreeException(__badFields("image/text", "targets." + _name));
            }

            // Text field
            if (!component["text"].empty() && component["text"].isString() ) { 
                _type = ATargetType::ATargetType_Text;
                _text = component["text"].asString();           
            }
            else if (!component["text"].empty() && !component["text"].isString() ) {
                throw ActionTreeException(__badFieldType("text", "targets." + _name + ".text", "string"));
            }
            else if (component["image"].empty() && component["text"].empty() ) {
                throw ActionTreeException(__badFields("image/text", _name + ".text"));
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
                    throw ActionTreeException(__badFields("target", "rules." + _name + ".do"));
                }
            }
            else {
                throw ActionTreeException(__missedFiled("target", "rules." + _name + ".do"));
            }

            // method
            if (!component["method"].empty() && component["method"].isString() ) {
                _method = __detectMethod(component["method"].asString());
            }
            else if (!component["method"].empty() && !component["method"].isString() ) {
                throw ActionTreeException(__badFieldType("method", "rules." + _name + ".method", "string"));
            }
            else {
                _method = AMethod::AMethod_None;
            }

            // Action
            if (!component["do"].empty() && component["do"].isString() ) {
                _do = component["do"].asString();
            }
            else if (!component["do"].empty() && !component["do"].isString()) {
                throw ActionTreeException(__badFieldType("do", "rules." + _name + ".do", "string"));
            }
            else {
                throw ActionTreeException(__missedFiled("do", "rules." + _name + ".do"));
            }

            return 0;
        }

        int ActionTree::operator<<(std::shared_ptr<APlay::AutoplayJsonConfig> config) {
            for (int i = 0; i < config->GetRoot("")["targets"].getMemberNames().size(); i++ ) {
                ATarget target;
                target << config->GetRoot("")["targets"].getMemberNames()[i];
                target << config->GetRoot("")["targets"][config->GetRoot("")["targets"].getMemberNames().at(i)];
                _targetTree._targets.insert({ {target._name, target} } );
            }

            for (int i = 0; i < config->GetRoot("")["rules"].getMemberNames().size(); i++ ) {
                ARule rule;
                rule << config->GetRoot("")["rules"].getMemberNames().at(i).c_str();
                rule << config->GetRoot("")["rules"][config->GetRoot("")["rules"].getMemberNames().at(i)];
                _ruleTree._rules.insert({ {rule._name, rule} } );
            }            
        }


        std::shared_ptr<ActionTree> CreateActionTree(std::shared_ptr<APlay::AutoplayJsonConfig> config) {
            auto tree = std::make_shared<ActionTree>();

            // for (int i = 0; i < config->GetRoot("")["targets"].getMemberNames().size(); i++ ) {
            //     ATarget target;
            //     target << config->GetRoot("")["targets"].getMemberNames()[i];
            //     target << config->GetRoot("")["targets"][config->GetRoot("")["targets"].getMemberNames().at(i)];

            //     tree->_targetTree._targets.insert({ {target._name, target} } );
            // }

            // for (int i = 0; i < config->GetRoot("")["rules"].getMemberNames().size(); i++ ) {
            //     ARule rule;
            //     rule << config->GetRoot("")["rules"].getMemberNames().at(i).c_str();
            //     rule << config->GetRoot("")["rules"][config->GetRoot("")["rules"].getMemberNames().at(i)];

            //     tree->_ruleTree._rules.insert({ {rule._name, rule} } );
            // }


            return tree;
        }

    }

}

