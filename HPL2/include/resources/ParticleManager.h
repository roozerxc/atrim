#ifndef HPL_PARTICLE_SYSTEM_MANAGER_H
#define HPL_PARTICLE_SYSTEM_MANAGER_H

#include "resources/ResourceManager.h"

#include "math/MathTypes.h"

class TiXmlElement;

namespace hpl {

    class cGraphics;
    class cResources;
    class cParticleSystem;
    class cParticleSystemData;
    class cXmlElement;

    //----------------------------------------------------
    
    typedef std::map<tString, cParticleSystemData*> tParticleSystemData3DMap;
    typedef tParticleSystemData3DMap::iterator tParticleSystemData3DMapIt;


    //----------------------------------------------------

    class cParticleManager : public iResourceManager
    {
    public:
        cParticleManager(cGraphics* apGraphics,cResources *apResources);
        ~cParticleManager();
        
        cParticleSystem* CreatePS(const tString& asName,const tString& asType, cVector3f avSize);

        /////////////////////////////////////////////////
        // This method is a hack, just so everyone knows
        cParticleSystem* CreatePS(const tString& asName, const tString& asDataName, cXmlElement* apElement,cVector3f avSize);

        void AddData(cParticleSystemData *apData);

        void Preload(const tString& asFile);
        
        void Destroy(iResourceBase* apResource);
        void Unload(iResourceBase* apResource);
    private:
        cGraphics* mpGraphics;
        cResources *mpResources;

        std::list<cParticleSystem*> mlstSystems;
    };

};
#endif // HPL_PARTICLE_SYSTEM_MANAGER_H
