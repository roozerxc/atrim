#include "math/MathTypes.h"

namespace hpl
{
template <>
const cMatrixf cMatrixf::Identity(1,0,0,0,
                                  0,1,0,0,
                                  0,0,1,0,
                                  0,0,0,1);
template <>
const cMatrixf cMatrixf::Zero(0,0,0,0,
                              0,0,0,0,
                              0,0,0,0,
                              0,0,0,0);
}
