#ifndef __ECS
#define __ECS

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>
#include <unordered_map>
#include <thread>

namespace APlay {
    namespace Ecs {

        class Entity;

        using ComponentTypeID = std::size_t;

        ComponentTypeID __hashString(const std::string& st) {
            static std::hash<std::string> _hasher;
            return _hasher(st);
        }

        template<typename T>
        ComponentTypeID __getComponentTypeID() {
            return __hashString(typeid(T).name());
        }

        class Component {
            public:
                Component(std::string identifier, Entity* parant = nullptr) : _identifier(identifier), _parent(parant) { }

                std::string getIdentifier() { return _identifier; }
                Entity* getParent() { return _parent; }

                void setParent(Entity* parent) { _parent = parent; }

            protected:
                std::string _identifier;
                Entity* _parent;

        };

        using ComponentArray = std::vector<std::unique_ptr<Component>>;
        using ComponentMap = std::unordered_map<ComponentTypeID, std::shared_ptr<ComponentArray>>;
        using ComponentTypes = std::vector<ComponentTypeID>;


        class Entity : public Component {
            public:
                Entity(std::string identifier = "", Entity* parent = nullptr) : Component(identifier, parent) {
                    componentTypes = std::make_shared<ComponentTypes>();
                    components = std::make_shared<ComponentMap>();
                }

                template<typename T, typename... TArgs>
                T* addComponent(TArgs&&... margs) {
                    auto cmPtr = std::make_unique<T>(std::forward<TArgs>(margs)...);
                    cmPtr->setParent(this);

                    if (std::find(componentTypes->begin(), componentTypes->end(), __getComponentTypeID<T>()) != componentTypes->end() ) {
                        if (!hasComponent<T>(cmPtr->getIdentifier())) {
                            components->at(__getComponentTypeID<T>())->emplace_back(std::move(cmPtr));
                        }
                        else {
                            auto cmp = getComponent<T>(cmPtr->getIdentifier());
                            *cmp = *cmPtr.release();
                        }
                    } else {
                        components->insert( {{ __getComponentTypeID<T>(), std::make_shared<ComponentArray>() }} );
                        components->at(__getComponentTypeID<T>())->emplace_back(std::move(cmPtr));
                        componentTypes->emplace_back(__getComponentTypeID<T>());
                    }

                    return static_cast<T*>(components->at(__getComponentTypeID<T>())->back().get() );
                }

                template<typename T>
                bool hasComponentType() const {
                    return std::find(componentTypes->begin(), componentTypes->end(), __getComponentTypeID<T>()) != componentTypes->end() ? true : false;
                }

                template<typename T>
                bool hasComponent(std::string identifier) const {
                    if (hasComponentType<T>() ) {
                        auto iter = std::find_if(components->at(__getComponentTypeID<T>())->begin(), components->at(__getComponentTypeID<T>())->end(),
                            [&](const std::unique_ptr<Component>& c) {
                                return c->getIdentifier() == identifier;
                            }
                        );
                        return iter != components->at(__getComponentTypeID<T>())->end() ? true : false;
                    }
                }

                template<typename T>
                T* getComponent(std::string identifier) {
                    if (hasComponentType<T>()) {

                        auto iter = std::find_if(components->at(__getComponentTypeID<T>())->begin(), components->at(__getComponentTypeID<T>())->end(),
                            [&](const std::unique_ptr<Component>& c) {
                                return c->getIdentifier() == identifier;
                            }
                        );

                        return iter != components->at(__getComponentTypeID<T>())->end() ? static_cast<T*>(iter->get()) : nullptr;
                    }
                    return nullptr;
                }

                template<typename T>
                std::size_t getComponentArraySize() {
                    return hasComponentType<T>() ? components->at(__getComponentTypeID<T>())->size() : 0;
                }

                template<typename T>
                void removeComponent(std::string identifier) {
                    if (hasComponentType<T>() ) {
                        auto iter = std::find_if(components->at(__getComponentTypeID<T>())->begin(), components->at(__getComponentTypeID<T>())->end(),
                            [&](const std::unique_ptr<Component>& c) {
                                return c->getIdentifier() == identifier;
                            }
                        );

                        if (iter != components->at(__getComponentTypeID<T>())->end()) {
                            components->at(__getComponentTypeID<T>())->erase(iter);
                        }
                    }

                    refresh();
                }

                template<typename T>
                void removeComponentArray() {
                    if (hasComponentType<T>()) {
                        components->at(__getComponentTypeID<T>())->clear();
                        components->at(__getComponentTypeID<T>())->shrink_to_fit();

                        auto it = std::find(componentTypes->begin(), componentTypes->end(), __getComponentTypeID<T>());
                        componentTypes->erase(it);
                    }

                    refresh();
                }

                std::size_t size() {
                    std::size_t _size = 0;

                    if (componentTypes->size() ) {
                        std::for_each(componentTypes->begin(), componentTypes->end(),
                            [&](const ComponentTypeID& id) {
                                _size += components->at(id)->size();
                            }
                        );
                    }

                    return _size;
                }

            protected:
                std::shared_ptr<ComponentTypes> componentTypes;
                std::shared_ptr<ComponentMap> components;

                // Non thread save
                void refresh() {
                    std::thread update(
                        [&]() {
                            std::vector<ComponentTypeID> rmv;

                            std::for_each(componentTypes->begin(), componentTypes->end(),
                                [&](const ComponentTypeID& id) {
                                    if (!components->at(id)->size() ) {
                                        components->erase(id);
                                        rmv.emplace_back(id);
                                    }
                                }
                            );

                            std::for_each(rmv.begin(), rmv.end(),
                                [&](const ComponentTypeID& id) {
                                    auto iter = std::find(componentTypes->begin(), componentTypes->end(), id);
                                    componentTypes->erase(iter);
                                }
                            ); 
                        }
                    );
                    update.detach();
                }
        };

    }

}


#endif // __ECS

