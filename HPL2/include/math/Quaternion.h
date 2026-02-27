#ifndef HPL_QUATERNION_H
#define HPL_QUATERNION_H

#include "math/Vector3.h"
#include "math/Matrix.h"

namespace hpl {

    class cQuaternion
    {
    public:
        cVector3<float> v;
        float w;
        
        cQuaternion();
        cQuaternion(float afAngle, const cVector3<float> &avAxis);
        cQuaternion(float afW, float afX, float afY, float afZ);
        cQuaternion(const cMatrix<float> &a_mtxRot);

        void Normalize();
        void ToRotationMatrix(cMatrix<float> &a_mtxDest) const;
        void FromRotationMatrix(const cMatrix<float> &a_mtxRot);

        /**
         * Set the quaternion from and angle and axis.
         * \param afAngle 
         * \param &avAxis MUST be unit length!
         */
        void FromAngleAxis(float afAngle, const cVector3<float> &avAxis);


        cQuaternion operator+(const cQuaternion &aqB) const;
        cQuaternion operator-(const cQuaternion &aqB) const;
        cQuaternion operator*(const cQuaternion &aqB) const;
        cQuaternion operator/(const cQuaternion &aqB) const;
        cQuaternion operator*(float afScalar) const;

        static const cQuaternion Identity;

    private:
    };
};
#endif // HPL_QUATERNION_H
