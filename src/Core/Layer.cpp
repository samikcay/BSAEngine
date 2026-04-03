#include "BSAEngine/Core/Layer.h"
#include <ostream>
#include <string_view>

namespace BSA {

    Layer::Layer(const std::string& debugName)
        : m_DebugName(debugName)
    {
    }

} // namespace BSA
