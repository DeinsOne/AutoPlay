#ifndef __AUTOPLAY_CORE
#define __AUTOPLAY_CORE

#include <memory>

namespace APlay {

    // namespace ActionTree {
        // #define __prefix "ActionTree err -> "
// 
        // #define __badFields(name, where) __prefix name ": bad fields"
        // #define __badFieldType(name, where, expected) __prefix name ": bad type"
// 
        // #define __fieldsCollision(names) __prefix names ": bad collision"
        // #define __missedFiled(name, where) ""
    // }

    template<typename T>
    class IParser {
        public:
            virtual T Init() = 0;
    };

}


#endif // __AUTOPLAY_CORE
