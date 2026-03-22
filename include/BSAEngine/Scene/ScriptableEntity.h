#pragma once

#include "BSAEngine/Scene/Entity.h"

namespace BSA {

    class ScriptableEntity {
    public:
        virtual ~ScriptableEntity() {}

        template<typename T>
        T& GetComponent() {
            return m_Entity.GetComponent<T>();
        }

    protected:
        // Kullanıcı türetilmiş class'ta (kendi scriptinde) buraları override edebilir
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(float /*ts*/) {}
        
        // Fizik motorundan gelen çarpışma callbackleri
        virtual void OnCollisionBegin(Entity /*other*/) {}
        virtual void OnCollisionEnd(Entity /*other*/) {}

    private:
        Entity m_Entity;
        friend class Scene; // Scene, Instantiate asamasinda Entity referansini ayarlayacak
        friend class PhysicsContactListener; // PhysicsContactListener, carpisma eventlerini cagirabilmeli
    };

}
