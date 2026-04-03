#pragma once

#include "BSAEngine/Core/KeyCodes.h"
#include "BSAEngine/Core/MouseButtonCodes.h"
#include <string>
#include <map>
#include <vector>

namespace BSA {

    enum class InputType {
        Keyboard = 0, MouseButton = 1
    };

    struct InputBinding {
        InputType Type;
        int Code;
    };

    class InputMapping {
    public:
        static void AddAction(const std::string& actionName, InputType type, int code);
        static bool IsActionPressed(const std::string& actionName);
        
        static void Clear();

    private:
        static std::map<std::string, std::vector<InputBinding>> s_ActionMappings;
    };

} // namespace BSA
