using System;
using System.Runtime.CompilerServices;

namespace BSAEngine
{
    // Oyun scriptlerinin ana sinifi
    public class Entity
    {
        public ulong ID { get; private set; }
        
        protected Entity() { ID = 0; }

        internal Entity(ulong id)
        {
            ID = id;
        }

        // --- Ornek Component Erisimleri ---

        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 result);
                return result;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(ID, ref value);
            }
        }

        // --- Yorumlanan C++ Eventleri (Motor bu metodlari cagirir) ---
        protected virtual void OnCreate() {}
        protected virtual void OnUpdate(float ts) {}
        protected virtual void OnDestroy() {}
    }
}
