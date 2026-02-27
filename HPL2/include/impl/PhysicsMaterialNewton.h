#ifndef HPL_PHYSICS_MATERIAL_NEWTON_H
#define HPL_PHYSICS_MATERIAL_NEWTON_H

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#endif
#include <Newton.h>

#include "physics/PhysicsMaterial.h"

namespace hpl {

    class iPhysicsBody;
    
    //------------------------------------------

    class cNewtonLockBodyUntilReturn
    {
    public:
        cNewtonLockBodyUntilReturn(const NewtonBody* apNewtonBody);
        ~cNewtonLockBodyUntilReturn();
    private: 
        const NewtonBody* mpNewtonBody;
    };


    //------------------------------------------

    class cPhysicsMaterialNewton : public iPhysicsMaterial
    {
    public:
        cPhysicsMaterialNewton(const tString &asName, iPhysicsWorld *apWorld, int alMatId=-1);
        ~cPhysicsMaterialNewton();

        void SetElasticity(float afElasticity);
        float GetElasticity() const;
        void SetStaticFriction(float afElasticity);
        float GetStaticFriction() const;
        void SetKineticFriction(float afElasticity);
        float GetKineticFriction() const;

        void SetFrictionCombMode(ePhysicsMaterialCombMode aMode);
        ePhysicsMaterialCombMode GetFrictionCombMode() const;
        void SetElasticityCombMode(ePhysicsMaterialCombMode aMode);
        ePhysicsMaterialCombMode GetElasticityCombMode() const;

        void UpdateMaterials();

        int GetId(){ return mlMaterialId;}
    private:
        float Combine(ePhysicsMaterialCombMode aMode, float afX, float afY);

        static int OnAABBOverlapCallback(const NewtonMaterial* apMaterial,
                                        const NewtonBody* apBody0, const NewtonBody* apBody1, int alThreadIndex);

        static void ContactsProcessCallback(const NewtonJoint* apContactJoint, dFloat afTimestep, int alThreadIndex);

        /*static int BeginContactCallback(const NewtonMaterial* apMaterial,
                                        const NewtonBody* apBody0, const NewtonBody* apBody1, int alThreadIndex);
        
        static int ProcessContactCallback(    const NewtonMaterial* apMaterial,
                                            const NewtonBody* apBody0, const NewtonBody* apBody1, 
                                            dFloat afTimestep, int alThreadIndex);
        
        static void EndContactCallback(        const NewtonMaterial* apMaterial, const NewtonBody* apBody0, const NewtonBody* apBody1, 
                                            int alThreadIndex);*/

           NewtonWorld *mpNewtonWorld;

        int mlMaterialId;

        ePhysicsMaterialCombMode mFrictionMode;
        ePhysicsMaterialCombMode mElasticityMode;

        float mfElasticity;
        float mfStaticFriction;
        float mfKineticFriction;

    };
};
#endif // HPL_PHYSICS_MATERIAL_NEWTON_H
