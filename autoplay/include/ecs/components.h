#pragma once

#include "uecs.h"

namespace APlay {
    namespace Ecs {

        class Bool : public APlay::Ecs::Component {
            protected:
                bool _value;

            public:
                Bool(std::string id, bool value = false) : Component(id), _value(value) { }

                int GetValue() const { return _value; }

                void SetValue(const int& value) { _value = value; }
        };

        class Int : public APlay::Ecs::Component {
            protected:
                int _value;

            public:
                Int(std::string id, int value = -1) : Component(id), _value(value) { }

                int GetValue() const { return _value; }

                void SetValue(const int& value) { _value = value; }
        };

        class Float : public APlay::Ecs::Component {
            protected:
                float _value;

            public:
                Float(std::string id, float value = -1) : Component(id), _value(value) { }

                float GetValue() const { return _value; }

                void SetValue(const float& value) { _value = value; }
        };

        class String : public APlay::Ecs::Component {
            protected:
                std::string _value;

            public:
                String(std::string id, std::string value = "") : Component(id), _value(value) { }

                std::string GetValue() const { return _value; }

                std::string GetValueLower() const { return _toLower(_value); }

                void SetValue(const std::string& value) { _value = value; }
        };

        template<typename T>
        class Array : public Component {
            protected:
                std::vector<T> _value;

            public:
                Array(std::string id, std::initializer_list<T> value = std::initializer_list<T>() ) : Component(id), _value(value) { }

                Array(std::string id, const std::vector<T>& value ) : Component(id), _value(value) { }


                std::vector<T> GetValue() { return _value; }

                void SetValue(const std::vector<T>& value) { _value = value; }
                void SetValue(const std::initializer_list<T>& value) { _value = value; }
        };

    }

}

