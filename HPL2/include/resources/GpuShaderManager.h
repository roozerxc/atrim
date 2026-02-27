#ifndef HPL_GPU_SHADER_MANAGER_H
#define HPL_GPU_SHADER_MANAGER_H

#include "resources/ResourceManager.h"

#include "graphics/GPUShader.h"

namespace hpl {

    //------------------------------------

    class iLowLevelGraphics;
    class cParserVarContainer;
    class cPreprocessParser;

    //------------------------------------

    class cGpuShaderManager : public iResourceManager
    {
    public:
        cGpuShaderManager(cFileSearcher *apFileSearcher, iLowLevelGraphics *apLowLevelGraphics,
                            iLowLevelResources *apLowLevelResources,iLowLevelSystem *apLowLevelSystem);
        ~cGpuShaderManager();

        void CheckFeatureSupport();
                        
        /**
         * Creates a new GPU program
         * \param asName name of the program 
         * \param asEntry the entry point of the program (usually "main")
         * \param aType type of the program
         * \return 
         */
        iGpuShader* CreateShader(const tString& asName,eGpuShaderType aType, cParserVarContainer *apVarContainer);

        void Destroy(iResourceBase* apResource);
        void Unload(iResourceBase* apResource);
    
    private:
        bool IsShaderSupported(const tString& asName, eGpuShaderType aType);

        iLowLevelGraphics *mpLowLevelGraphics;
        cPreprocessParser* mpPreprocessParser;
    };

};
#endif // HPL_GPU_SHADER_MANAGER_H
