#ifndef __AUTOPLAY_CORE
#define __AUTOPLAY_CORE

#include <memory>

namespace APlay {

    template<typename T>
    class IParser {
        public:
            virtual T Init() = 0;
    };

}


#endif // __AUTOPLAY_CORE
