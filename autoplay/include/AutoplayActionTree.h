#pragma once
#include "AutoplayCore.h"
#include "AutoplayJsonConfig.h"
#include <vector>
#include <string>

#include <unordered_map>
#include <memory>
#include <exception>
#include <iostream>

namespace APlay {

    namespace ActionTree {

        enum AMethod {
            AMethod_None = 0,
            AMethod_TemplateMatching // TEMPLATE_MATCHING
        };

        enum ATargetType {
            ATargetType_None = 0,
            ATargetType_Text,
            ATargetType_Resource
        };

        class ANodeBase {
            public:
                AMethod _method;
                std::string _name;
        };

        class ATarget : public ANodeBase {
            public:
                ATargetType _type;
                std::string _text;
                // FIXME: alternative format
                const unsigned char* _image;

                ATarget& operator =(const ATarget& eq) {
                    _type = eq._type;
                    _text = eq._text;  
                    _image = eq._image;
                    _method = eq._method;
                    _name = eq._name;
                }

                int operator<<(Json::Value component);
                int operator<<(std::string name);
        };

        class ARule : public ANodeBase {
            public:
                std::vector<std::string> _targetName;
                // FIXME: provide action type
                std::string _do;

                ARule& operator =(const ARule& eq) {
                    _targetName = eq._targetName;
                    _do = eq._do;  
                    _method = eq._method;
                    _name = eq._name;
                }

                int operator<<(Json::Value component);
                int operator<<(const char* name);

        };

        // inline std::string GetHash(ANodeBase node) { return node._name; };
        // inline ANodeBase  HashToBase(std::string hash) {};
        // inline std::string GetName(std::string hash) { return ""; };
        // inline std::string GetMethod(std::string hash) { return ""; };

        /**
         * Container for tagets. 
         * Hash is lowercase string which represent target name and method. Shapes as: std::string("target._name" + "_" + "target._method").lower()
        */
        class TargetTree {
            public:
                std::unordered_map<std::string, ATarget> _targets;
        };

        class RuleTree {
            public:
                std::unordered_map<std::string, ARule> _rules;
        };

        class ActionTree {
            public:
                TargetTree  _targetTree;
                RuleTree    _ruleTree;

                int operator<<(std::shared_ptr<APlay::AutoplayJsonConfig> config);
        };


        std::shared_ptr<ActionTree> CreateActionTree(std::shared_ptr<APlay::AutoplayJsonConfig> config);

        class ActionTreeException : virtual public std::exception {
            protected:
                std::string msg;

            public:
                explicit ActionTreeException(std::string _what) : msg(_what) {
                }

                virtual ~ActionTreeException() throw () {}

                virtual const char* what() const throw () { return msg.c_str(); }

        };

    }
} // APlay
