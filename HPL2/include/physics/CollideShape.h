#ifndef HPL_COLLIDE_SHAPE_H
#define HPL_COLLIDE_SHAPE_H

#include "physics/PhysicsTypes.h"
#include "math/MathTypes.h"
#include "math/BoundingVolume.h"

namespace hpl {
    
    //-----------------------------------------------

    class iPhysicsWorld;

    //-----------------------------------------------
    
    class iCollideShape
    {
    public:
        iCollideShape(iPhysicsWorld *apWorld) : mlUserCount(0), mpWorld(apWorld) {}
        virtual ~iCollideShape(){}

        virtual iCollideShape* GetSubShape(int alIdx)=0;
        virtual int GetSubShapeNum()=0;
        
        const cVector3f& GetSize(){ return mvSize;}
        
        float GetRadius(){ return mvSize.x;}
        float GetHeight(){ return mvSize.y;}
        float GetWidth(){ return mvSize.x;}
        float GetDepth(){ return mvSize.z;}

        const cMatrixf& GetOffset(){ return m_mtxOffset;}

        eCollideShapeType GetType(){ return mType;}

        void IncUserCount(){ mlUserCount++;}
        void DecUserCount(){ mlUserCount--;}

        bool HasUsers(){ return mlUserCount>0;}
        int GetUserCount(){return mlUserCount;}

        float GetVolume(){ return mfVolume;}

        cBoundingVolume& GetBoundingVolume(){ return mBoundingVolume;}
    protected:
        cVector3f mvSize;
        eCollideShapeType mType;
        cMatrixf m_mtxOffset;

        int mlUserCount;

        iPhysicsWorld *mpWorld;
        float mfVolume;

        cBoundingVolume mBoundingVolume;
    };
};
#endif // HPL_COLLIDE_SHAPE_H
