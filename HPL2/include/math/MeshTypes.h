#ifndef HPL_MESH_TYPES_H
#define HPL_MESH_TYPES_H

#include "math/MathTypes.h"
#include "math/BoundingVolume.h"

namespace hpl {

    //----------------------------------------------

    class cTriangleData
    {
    public:
        cVector3f normal;
        bool facingLight;

        cTriangleData(){}
        cTriangleData(const cVector3f& avNormal)
        {
            normal = avNormal;
        }
     };
    
    typedef std::vector<cTriangleData> tTriangleDataVec;
    typedef tTriangleDataVec::iterator tTriangleDataVecIt;

    //----------------------------------------------

    class cTriEdge
    {
    public:
        int point1, point2;
        mutable int tri1, tri2;
        bool invert_tri2;

        cTriEdge(){}
        cTriEdge(int alPoint1, int alPoint2, int alTri1, int alTri2)
        {
            point1 = alPoint1;
            point2 = alPoint2;
            tri1 = alTri1;
            tri2 = alTri2;
        }
    };

    typedef std::vector<cTriEdge> tTriEdgeVec;
    typedef tTriEdgeVec::iterator tTriEdgeVecIt;

    //----------------------------------------------

};
#endif // HPL_MESH_TYPES_H
