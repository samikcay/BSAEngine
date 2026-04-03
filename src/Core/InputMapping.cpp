#include "BSAEngine/Core/InputMapping.h"
#include "BSAEngine/Core/Input.h"

namespace BSA {

    std::map<std::string, std::vector<InputBinding>> InputMapping::s_ActionMappings;

    void InputMapping::AddAction(const std::string& actionName, InputType type, int code) {
        s_ActionMappings[actionName].push_back({ type, code });
    }

    bool InputMapping::IsActionPressed(const std::string& actionName) {
        if (s_ActionMappings.find(actionName) == s_ActionMappings.end())
            return false;

        for (const auto& binding : s_ActionMappings[actionName]) {
            if (binding.Type == InputType::Keyboard) {
                if (Input::IsKeyPressed(binding.Code))
                    return true;
            } else if (binding.Type == InputType::MouseButton) {
                if (Input::IsMouseButtonPressed(binding.Code))
                    return true;
            }
        }

        return false;
    }

    void InputMapping::Clear() {
        s_ActionMappings.clear();
    }

} // namespace BSA
