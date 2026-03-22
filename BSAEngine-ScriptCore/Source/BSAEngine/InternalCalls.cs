using System;
using System.Runtime.CompilerServices;

namespace BSAEngine
{
    // Bu sınıf C++ tarafındaki fonksiyonları temsil eder.
    // [MethodImpl(MethodImplOptions.InternalCall)] attribute'u, 
    // fonksiyonun gövdesinin C++ tarafında (mono_add_internal_call) olduğunu belirtir.
    public static class InternalCalls
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void NativeLog(string text, int parameter);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Input_IsKeyDown(int keycode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void TransformComponent_GetTranslation(ulong entityID, out Vector3 translation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void TransformComponent_SetTranslation(ulong entityID, ref Vector3 translation);
    }
}
