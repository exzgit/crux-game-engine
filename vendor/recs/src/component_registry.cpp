#include "recs/component_registry.h"

ComponentRegistry& ComponentRegistry::instance() {
    static ComponentRegistry inst;
    return inst;
}

const ComponentTypeInfo& ComponentRegistry::info(ComponentTypeID id) const {
    assert(id < infos.size());
    return infos[id];
}
// Note: `type_id<T>()` is a template and defined in the header
// so that it is available to all translation units.
