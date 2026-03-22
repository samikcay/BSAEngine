using System;

namespace BSAEngine
{
    public class Input
    {
        public static bool IsKeyDown(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyDown((int)keycode);
        }
    }

    public enum KeyCode
    {
        // Temel ornek keycode'lar (GLFW'a denk gelen vb.)
        Space = 32,
        A = 65,
        D = 68,
        S = 83,
        W = 87
    }
}
