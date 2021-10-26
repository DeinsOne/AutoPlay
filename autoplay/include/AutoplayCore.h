#ifndef __AUTOPLAY_CORE
#define __AUTOPLAY_CORE

#include <memory>
#include "ecs/uecs.h"
#include "ecs/components.h"


namespace APlay {

    template<typename T>
    class IParser {
        public:
            virtual T Init() = 0;

            virtual std::shared_ptr<Ecs::Entity> GetRoot() { return _root; }

        protected:
            std::shared_ptr<Ecs::Entity> _root;
    };

}


#endif // __AUTOPLAY_CORE
