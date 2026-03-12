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
        virtual void OnUpdate(float ts) {}

    private:
        Entity m_Entity;
        friend class Scene; // Scene, Instantiate asamasinda Entity referansini ayarlayacak
    };

}
