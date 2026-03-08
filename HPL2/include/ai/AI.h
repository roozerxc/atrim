#ifndef HPL_AI_H
#define HPL_AI_H

#include "system/SystemTypes.h"
#include "engine/EngineTypes.h"

#include "engine/Updateable.h"

namespace hpl
{

class cAINodeGenerator;

class cAI : public iUpdateable
{
public:
    cAI();
    ~cAI();

    void Reset();
    void Update(float afTimeStep);

    void Init();

    cAINodeGenerator *GetNodeGenerator()
    {
        return mpAINodeGenerator;
    }

private:
    cAINodeGenerator *mpAINodeGenerator;
};

};
#endif // HPL_AI_H
