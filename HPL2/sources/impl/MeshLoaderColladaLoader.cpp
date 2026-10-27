#include "impl/MeshLoaderCollada.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/System.h"
#include "system/Platform.h"

#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/Material.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Bone.h"
#include "graphics/Skeleton.h"
#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"

#include "resources/MaterialManager.h"

#include "impl/XmlPugi.h"

#include "math/Math.h"

namespace hpl
{

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// FILL STRUCTURES
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cMeshLoaderCollada::FillStructures(const tWString &asFile,
                                        tColladaImageVec *apColladaImageVec,
                                        tColladaTextureVec *apColladaTextureVec,
                                        tColladaMaterialVec *apColladaMaterialVec,
                                        tColladaLightVec *apColladaLightVec,
                                        tColladaGeometryVec *apColladaGeometryVec,
                                        tColladaControllerVec *apColladaControllerVec,
                                        tColladaAnimationVec *apColladaAnimVec,
                                        cColladaScene *apColladaScene, bool abCache)
{
    bool bLoadCache = false;
    //abCache = false;
    //Log("Loading %s\n",asFile.c_str());

    tWString sCacheFile = cString::SetFileExtW(asFile, _W("dae_cache"));

    if(    abCache &&
            cPlatform::FileExists(sCacheFile) &&
            cPlatform::FileExists(asFile))
    {
        cDate colladaDate = cPlatform::FileModifiedDate(asFile);
        cDate cacheDate = cPlatform::FileModifiedDate(sCacheFile);

        //Check if cache is newer
        if(cacheDate > colladaDate)
        {
            bLoadCache = true;
        }
    }

    /////////////////////////////////////////////////
    // LOAD CACHE
    if(bLoadCache)
    {
        return LoadStructures(sCacheFile,
                              apColladaImageVec,
                              apColladaTextureVec,
                              apColladaMaterialVec,
                              apColladaLightVec,
                              apColladaGeometryVec,
                              apColladaControllerVec,
                              apColladaAnimVec,
                              apColladaScene);
    }

    Log("Cache out of date! Reloading collada file '%s'\n", cString::To8Char(asFile).c_str());

    /////////////////////////////////////////////////
    // LOAD THE DOCUMENT
    //unsigned long lStartTime = mpSystem->GetLowLevel()->GetTime();

    if(cPlatform::FileExists(asFile) == false)
    {
        return false;
    }

    //pugixml parses in place: the file is read once and the tree points into that buffer.
    pugi::xml_document xmlDoc;
    if(Xml::LoadFile(xmlDoc, asFile) == false)
    {
        Error("Couldn't load Collada XML file '%s'!\n", cString::To8Char(asFile).c_str());
        return false;
    }

    //unsigned long lTime = mpSystem->GetLowLevel()->GetTime() - lStartTime;
    //Log("Loading collada XML file for '%s' took: %d ms\n",asFile.c_str(),lTime);

    //Get the root.
    pugi::xml_node pRootElem = xmlDoc.document_element();

    /////////////////////////////////////////////////
    // FIRST SETUP

    //Set up axis as Y.
    mbZToY = false;
    mfUnitScale = 1.0f;

    //Load assets element.
    pugi::xml_node pAssetElem = pRootElem.child("asset");
    if(pAssetElem)
    {
        pugi::xml_node pUpAxisElem = pAssetElem.child("up_axis");
        if(pUpAxisElem)
        {
            if(Xml::TextCondensed(pUpAxisElem) == "Z_UP")
            {
                mbZToY = true;
                //Log("!!!!!!!Z IS UP!!!!!!");
            }
        }


        bool bGetUnit = false;

        // Only get unit from 3ds max files!
        pugi::xml_node pCont = pAssetElem.child("contributor");

        if(pCont)
        {
            pCont = pCont.child("authoring_tool");
        }
        else
        {
            pCont = pAssetElem.child("authoring_tool");
        }

        if(pCont)
        {
            tString sName = Xml::TextCondensed(pCont);

            if(sName == "FBX COLLADA exporter")
            {
                bGetUnit = true;
            }
        }

        pugi::xml_node pUnitElem = pAssetElem.child("unit");

        if(pUnitElem && bGetUnit && mbConvertUnit)
        {
            float fUnitTypeScale = 1.0f;
            float fUnitValue = 1.0f;

            pugi::xml_attribute firstAttri = pUnitElem.first_attribute();

            for(; firstAttri; firstAttri = firstAttri.next_attribute())
            {
                tString sName = cString::ToString(firstAttri.name(), "");

                if(sName == "meter")
                {
                    //////////////
                    // Read the scale
                    fUnitValue = (float)firstAttri.as_double();
                }
            }

            mfUnitScale = fUnitValue;
        }

    }
    else
    {
        Error("Couldn't find asset element!\n");
    }

    //mbZToY = false;
    //mfUnitScale = 1.0f;


    /////////////////////////////////////////////////
    // ITERATE THE LIBRARIES BEFORE SCENE LIBRARIES
    pugi::xml_node pLibraryElem = Xml::FirstElem(pRootElem);
    while(pLibraryElem)
    {
        tString sType = cString::ToString(Xml::Attr(pLibraryElem, "type"), "");
        tString sValue = cString::ToString(pLibraryElem.name(), "");

        // Lights
        if((sType == "LIGHT" || sValue == "library_lights") && apColladaLightVec)
        {
            ///Log("Loading type: %s\n",sType.c_str());
            LoadLights(pLibraryElem, *apColladaLightVec);
            //Log(" --- \n");
        }

        pLibraryElem = Xml::NextElem(pLibraryElem);
    }

    //////////////////////////////////////////////
    // LOAD SCENE
    if(apColladaScene)
    {
        pugi::xml_node pSceneElem = pRootElem.child("library_visual_scenes");
        if(pSceneElem)
        {
            pSceneElem = pSceneElem.child("visual_scene");
            if(!pSceneElem)
            {
                Warning("No visual scene element found!\n");
            }
        }

        if(!pSceneElem)
        {
            pSceneElem = pRootElem.child("scene");
        }

        if(!pSceneElem)
        {
            Warning("No scene element found!\n");
        }
        else
        {
            //Get start and end time (MAYA ONLY!)
            pugi::xml_node pExtraElem = pSceneElem.child("extra");
            if(pExtraElem)
            {
                // Get techniques
                pugi::xml_node pExtraTechElem = pExtraElem.child("technique");
                for(; pExtraTechElem; pExtraTechElem = pExtraTechElem.next_sibling("technique"))
                {
                    //Check for maya profile
                    tString sProfile = cString::ToString(Xml::Attr(pExtraTechElem, "profile"), "");
                    if(sProfile == "MAYA" || sProfile == "FCOLLADA")
                    {
                        //Iterate params
                        pugi::xml_node pParam = Xml::FirstElem(pExtraTechElem);
                        for(; pParam; pParam = Xml::NextElem(pParam))
                        {
                            tString sName = cString::ToString(Xml::Attr(pParam, "name"), "");
                            if(sName == "")
                            {
                                sName = cString::ToString(pParam.name(), "");
                            }

                            sName = cString::ToLowerCase(sName);

                            const char *pText = Xml::TextOrEmpty(pParam);
                            float fValue = cString::ToFloat(pText, 0);

                            if(sName == "start_time")
                            {
                                apColladaScene->mfStartTime = fValue;
                            }
                            else if(sName == "end_time")
                            {
                                apColladaScene->mfEndTime = fValue;
                            }
                        }
                    }

                    apColladaScene->mfDeltaTime = apColladaScene->mfEndTime - apColladaScene->mfStartTime;
                }

                //Log("Anim Start: %f End: %f\n",apColladaScene->mfStartTime, apColladaScene->mfEndTime);
            }
            else
            {
                apColladaScene->mfStartTime = 0;
                apColladaScene->mfEndTime = 0;
                apColladaScene->mfDeltaTime = -1;
                Warning("No 'extra scene' element found!\n");
            }

            //Load all nodes in the scene.
            pugi::xml_node pNodeElem = pSceneElem.child("node");
            while(pNodeElem)
            {
                LoadColladaScene(pNodeElem, &apColladaScene->mRoot, apColladaScene, apColladaLightVec);

                pNodeElem = pNodeElem.next_sibling("node");
            }
        }
    }

    /////////////////////////////////////////////////
    // ITERATE THE AFTER SCENE LIBRARIES

    pLibraryElem = Xml::FirstElem(pRootElem);
    while(pLibraryElem)
    {
        tString sValue = cString::ToString(pLibraryElem.name(), "");
        tString sType = cString::ToString(Xml::Attr(pLibraryElem, "type"), "");

        // Image
        if((sType == "IMAGE" || sValue == "library_images") && apColladaImageVec)
        {
            //Loads all images files available
            LoadImages(pLibraryElem, *apColladaImageVec);
        }
        // Effects / Texture (texture is name in older versions)
        else if((sType == "TEXTURE" || sValue == "library_effects") && apColladaTextureVec)
        {
            //Loads all texture and their settings
            LoadTextures(pLibraryElem, *apColladaTextureVec);
        }
        // Material
        else if((sType == "MATERIAL" || sValue == "library_materials") && apColladaMaterialVec)
        {
            //All materials used are loaded
            LoadMaterials(pLibraryElem, *apColladaMaterialVec);
        }
        // Geometry
        else if((sType == "GEOMETRY" || sValue == "library_geometries") && apColladaGeometryVec)
        {
            //geometry for all objects in the scene are loaded.
            LoadGeometry(pLibraryElem, *apColladaGeometryVec);
        }
        // Animation
        else if((sType == "ANIMATION" || sValue == "library_animations") && apColladaScene && apColladaAnimVec)
        {
            LoadAnimations(pLibraryElem, *apColladaAnimVec, apColladaScene);
        }
        // Controller
        else if((sType == "CONTROLLER" || sValue == "library_controllers") && apColladaGeometryVec && apColladaControllerVec)
        {
            LoadControllers(pLibraryElem, *apColladaControllerVec, apColladaGeometryVec);
        }


        //Log(" ---- \n",sType.c_str());

        pLibraryElem = Xml::NextElem(pLibraryElem);
    }

    /*if(abCache && mbLoadAndSaveMSHFormat==false)
    {
        SaveStructures(sCacheFile,apColladaImageVec,
                        apColladaTextureVec,
                        apColladaMaterialVec,
                        apColladaLightVec,
                        apColladaGeometryVec,
                        apColladaControllerVec,
                        apColladaAnimVec,
                        apColladaScene);
    }*/
    return true;
}

//--------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE COLLADA DATA
//////////////////////////////////////////////////////////////////////////

static pugi::xml_node CreateXMLChild(pugi::xml_node aParent, const char* asName)
{
    return aParent.append_child(asName);
}

static pugi::xml_node CreateXMLTextChild(pugi::xml_node aParent, const char* asText)
{
    pugi::xml_node textNode = aParent.append_child(pugi::node_pcdata);
    textNode.set_value(asText);
    return textNode;
}

//-----------------------------------------------------------------------

static void SaveImageVec(pugi::xml_node apRootElem, tColladaImageVec *apColladaImageVec);
static void SaveTextureVec(pugi::xml_node apRootElem, tColladaTextureVec *apColladaTextureVec);
static void SaveMaterialVec(pugi::xml_node apRootElem, tColladaMaterialVec *apColladaMaterialVec);
static void SaveLightVec(pugi::xml_node apRootElem, tColladaLightVec *apColladaLightVec);
static void SaveAnimationVec(pugi::xml_node apRootElem, tColladaAnimationVec *apColladaAnimationVec);
static void SaveControllerVec(pugi::xml_node apRootElem, tColladaControllerVec *apColladaControllerVec);
static void SaveGeometryVec(pugi::xml_node apRootElem, tColladaGeometryVec *apColladaGeometryVec);
static void SaveScene(pugi::xml_node apRootElem, cColladaScene *apColladaScene);

bool cMeshLoaderCollada::SaveStructures(const tWString &asFile,
                                        tColladaImageVec *apColladaImageVec,
                                        tColladaTextureVec *apColladaTextureVec,
                                        tColladaMaterialVec *apColladaMaterialVec,
                                        tColladaLightVec *apColladaLightVec,
                                        tColladaGeometryVec *apColladaGeometryVec,
                                        tColladaControllerVec *apColladaControllerVec,
                                        tColladaAnimationVec *apColladaAnimVec,
                                        cColladaScene *apColladaScene)
{
    pugi::xml_document xmlDoc;

    pugi::xml_node pRootElem = CreateXMLChild(xmlDoc, "ColladaCache");

    if(apColladaImageVec)
    {
        SaveImageVec(pRootElem, apColladaImageVec);
    }
    if(apColladaTextureVec)
    {
        SaveTextureVec(pRootElem, apColladaTextureVec);
    }
    if(apColladaMaterialVec)
    {
        SaveMaterialVec(pRootElem, apColladaMaterialVec);
    }
    if(apColladaLightVec)
    {
        SaveLightVec(pRootElem, apColladaLightVec);
    }
    if(apColladaAnimVec)
    {
        SaveAnimationVec(pRootElem, apColladaAnimVec);
    }
    if(apColladaControllerVec)
    {
        SaveControllerVec(pRootElem, apColladaControllerVec);
    }
    if(apColladaGeometryVec)
    {
        SaveGeometryVec(pRootElem, apColladaGeometryVec);
    }
    if(apColladaScene)
    {
        SaveScene(pRootElem, apColladaScene);
    }

    if(Xml::SaveFile(xmlDoc, asFile) == false)
    {
        Error("Couldn't save XML file %s\n", cString::To8Char(asFile).c_str());
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------

static void SaveImageVec(pugi::xml_node apRootElem, tColladaImageVec *apColladaImageVec)
{
    pugi::xml_node pImageRootElem =  CreateXMLChild(apRootElem, "ImageRoot");

    Xml::SetAttr(pImageRootElem, "Size", (int)apColladaImageVec->size());

    for(size_t i = 0; i < apColladaImageVec->size(); ++i)
    {
        cColladaImage *pImage = &(*apColladaImageVec)[i];

        pugi::xml_node pImageElem = CreateXMLChild(pImageRootElem, "Image");

        Xml::SetAttr(pImageElem, "Id", pImage->msId.c_str());
        Xml::SetAttr(pImageElem, "Name", pImage->msName.c_str());
        Xml::SetAttr(pImageElem, "Source", cString::GetFileName(pImage->msSource.c_str()).c_str());
    }
}

//-----------------------------------------------------------------------

static void SaveTextureVec(pugi::xml_node apRootElem, tColladaTextureVec *apColladaTextureVec)
{
    pugi::xml_node pTextureRootElem =  CreateXMLChild(apRootElem, "TextureRoot");

    Xml::SetAttr(pTextureRootElem, "Size", (int)apColladaTextureVec->size());

    for(size_t i = 0; i < apColladaTextureVec->size(); ++i)
    {
        cColladaTexture *pTexture = &(*apColladaTextureVec)[i];

        pugi::xml_node pTextureElem = CreateXMLChild(pTextureRootElem, "Texture");

        Xml::SetAttr(pTextureElem, "Id", pTexture->msId.c_str());
        Xml::SetAttr(pTextureElem, "Name", pTexture->msName.c_str());
        Xml::SetAttr(pTextureElem, "Image", pTexture->msImage.c_str());
    }
}

//-----------------------------------------------------------------------

static void SaveMaterialVec(pugi::xml_node apRootElem, tColladaMaterialVec *apColladaMaterialVec)
{
    pugi::xml_node pMaterialRootElem =  CreateXMLChild(apRootElem, "MaterialRoot");

    Xml::SetAttr(pMaterialRootElem, "Size", (int)apColladaMaterialVec->size());

    for(size_t i = 0; i < apColladaMaterialVec->size(); ++i)
    {
        cColladaMaterial *pMaterial = &(*apColladaMaterialVec)[i];

        pugi::xml_node pMaterialElem = CreateXMLChild(pMaterialRootElem, "Material");

        Xml::SetAttr(pMaterialElem, "Id", pMaterial->msId.c_str());
        Xml::SetAttr(pMaterialElem, "Name", pMaterial->msName.c_str());
        Xml::SetAttr(pMaterialElem, "Texture", pMaterial->msTexture.c_str());
        Xml::SetAttr(pMaterialElem, "Color", pMaterial->mDiffuseColor.ToFileString().c_str());
    }
}

//-----------------------------------------------------------------------

static void SaveLightVec(pugi::xml_node apRootElem, tColladaLightVec *apColladaLightVec)
{
    pugi::xml_node pLightRootElem =  CreateXMLChild(apRootElem, "LightRoot");

    Xml::SetAttr(pLightRootElem, "Size", (int)apColladaLightVec->size());

    for(size_t i = 0; i < apColladaLightVec->size(); ++i)
    {
        cColladaLight *pLight = &(*apColladaLightVec)[i];

        pugi::xml_node pLightElem = CreateXMLChild(pLightRootElem, "Light");

        //Log("Light %s\n",pLight->msName.c_str());

        Xml::SetAttr(pLightElem, "Id", pLight->msId.c_str());
        Xml::SetAttr(pLightElem, "Name", pLight->msName.c_str());
        Xml::SetAttr(pLightElem, "Type", pLight->msType.c_str());

        Xml::SetAttr(pLightElem, "Angle", cString::ToString(pLight->mfAngle).c_str());
        Xml::SetAttr(pLightElem, "Color", pLight->mDiffuseColor.ToFileString().c_str());
    }
}

//-----------------------------------------------------------------------

static void SaveAnimationVec(pugi::xml_node apRootElem, tColladaAnimationVec *apColladaAnimationVec)
{
    pugi::xml_node pAnimationRootElem =  CreateXMLChild(apRootElem, "AnimationRoot");

    Xml::SetAttr(pAnimationRootElem, "Size", (int)apColladaAnimationVec->size());

    for(size_t i = 0; i < apColladaAnimationVec->size(); ++i)
    {
        cColladaAnimation *pAnimation = &(*apColladaAnimationVec)[i];

        pugi::xml_node pAnimationElem = CreateXMLChild(pAnimationRootElem, "Animation");

        Xml::SetAttr(pAnimationElem, "Id", pAnimation->msId.c_str());
        Xml::SetAttr(pAnimationElem, "TargetNode", pAnimation->msTargetNode.c_str());

        /////////////////////////////////////
        // Channels
        {
            pugi::xml_node pChannelVecElem =  CreateXMLChild(pAnimationElem, "ChannelVec");
            Xml::SetAttr(pChannelVecElem, "Size", (int)pAnimation->mvChannels.size());

            for(size_t idx = 0; idx < pAnimation->mvChannels.size(); ++idx)
            {
                cColladaChannel *pChannel = &pAnimation->mvChannels[idx];
                pugi::xml_node pChannelElem =  CreateXMLChild(pChannelVecElem, "Channel");

                Xml::SetAttr(pChannelElem, "Id", pChannel->msId.c_str());
                Xml::SetAttr(pChannelElem, "Target", pChannel->msTarget.c_str());
                Xml::SetAttr(pChannelElem, "Source", pChannel->msSource.c_str());
            }
        }
        /////////////////////////////////////
        // Samplers
        {
            pugi::xml_node pSamplerVecElem =  CreateXMLChild(pAnimationElem, "SamplerVec");
            Xml::SetAttr(pSamplerVecElem, "Size", (int)pAnimation->mvSamplers.size());

            for(size_t idx = 0; idx < pAnimation->mvSamplers.size(); ++idx)
            {
                cColladaSampler *pSampler = &pAnimation->mvSamplers[idx];
                pugi::xml_node pSamplerElem =  CreateXMLChild(pSamplerVecElem, "Sampler");

                Xml::SetAttr(pSamplerElem, "Id", pSampler->msId.c_str());
                Xml::SetAttr(pSamplerElem, "TimeArray", pSampler->msTimeArray.c_str());
                Xml::SetAttr(pSamplerElem, "ValueArray", pSampler->msValueArray.c_str());
                Xml::SetAttr(pSamplerElem, "Target", pSampler->msTarget.c_str());
            }
        }
        /////////////////////////////////////
        // Sources
        {
            pugi::xml_node pSourceVecElem =  CreateXMLChild(pAnimationElem, "SourceVec");
            Xml::SetAttr(pSourceVecElem, "Size", (int)pAnimation->mvSources.size());

            for(size_t idx = 0; idx < pAnimation->mvSources.size(); ++idx)
            {
                cColladaAnimSource *pSource = &pAnimation->mvSources[idx];
                pugi::xml_node pSourceElem =  CreateXMLChild(pSourceVecElem, "Source");

                Xml::SetAttr(pSourceElem, "Id", pSource->msId.c_str());
                tString sData = "";
                for(size_t j = 0; j < pSource->mvValues.size(); ++j)
                {
                    sData += cString::ToString(pSource->mvValues[j]) + " ";
                }

                Xml::SetAttr(pSourceElem, "Values", sData.c_str());
            }
        }
    }
}

//-----------------------------------------------------------------------

static void SaveControllerVec(pugi::xml_node apRootElem, tColladaControllerVec *apColladaControllerVec)
{
    pugi::xml_node pControllerRootElem =  CreateXMLChild(apRootElem, "ControllerRoot");

    Xml::SetAttr(pControllerRootElem, "Size", (int)apColladaControllerVec->size());

    for(size_t i = 0; i < apColladaControllerVec->size(); ++i)
    {
        cColladaController *pController = &(*apColladaControllerVec)[i];

        pugi::xml_node pControllerElem = CreateXMLChild(pControllerRootElem, "Controller");

        Xml::SetAttr(pControllerElem, "Id", pController->msId.c_str());
        Xml::SetAttr(pControllerElem, "Target", pController->msTarget.c_str());

        Xml::SetAttr(pControllerElem, "BindShapeMatrix", pController->m_mtxBindShapeMatrix.ToFileString().c_str());

        Xml::SetAttr(pControllerElem, "JointPairIdx", pController->mlJointPairIdx);
        Xml::SetAttr(pControllerElem, "WeightPairIdx", pController->mlWeightPairIdx);

        ///////////////////////
        // Joints
        {
            pugi::xml_node pJointsElem = CreateXMLChild(pControllerElem, "Joints");
            Xml::SetAttr(pJointsElem, "Size", (int)pController->mvJoints.size());

            tString sData = "";
            sData.reserve(pController->mvJoints.size() * 10);
            for(size_t idx = 0; idx < pController->mvJoints.size(); ++idx)
            {
                sData += pController->mvJoints[idx] + " ";
            }
            CreateXMLTextChild(pJointsElem, sData.c_str());
        }
        ///////////////////////
        // Weights
        {
            pugi::xml_node pWeightsElem = CreateXMLChild(pControllerElem, "Weights");
            Xml::SetAttr(pWeightsElem, "Size", (int)pController->mvWeights.size());

            tString sData = "";
            sData.reserve(pController->mvWeights.size() * 5);
            for(size_t idx = 0; idx < pController->mvWeights.size(); ++idx)
            {
                sData += cString::ToString(pController->mvWeights[idx]) + " ";
            }
            CreateXMLTextChild(pWeightsElem, sData.c_str());
        }
        ///////////////////////
        // Matrices
        {
            pugi::xml_node pMatricesElem = CreateXMLChild(pControllerElem, "Matrices");
            Xml::SetAttr(pMatricesElem, "Size", (int)pController->mvMatrices.size());

            tString sData = "";
            sData.reserve(pController->mvMatrices.size() * 5);
            for(size_t idx = 0; idx < pController->mvMatrices.size(); ++idx)
            {
                sData += pController->mvMatrices[idx].ToFileString() + " ";
            }
            CreateXMLTextChild(pMatricesElem, sData.c_str());
        }
        ///////////////////////
        // Pairs
        {
            pugi::xml_node pPairVecElem = CreateXMLChild(pControllerElem, "PairVec");
            Xml::SetAttr(pPairVecElem, "Size", (int)pController->mvPairs.size());

            tIntVec vPairNumVec;
            vPairNumVec.resize(pController->mvPairs.size(), 0);

            ////////////////////////////////
            //The pair connections
            int lCount = 0;
            pugi::xml_node pPairsElem = CreateXMLChild(pPairVecElem, "Pairs");
            tString sData = "";
            sData.reserve(pController->mvPairs.size() * 4 * 2);
            for(size_t idx = 0; idx < pController->mvPairs.size(); ++idx)
            {
                tColladaJointPairList *pList = &pController->mvPairs[idx];

                for(tColladaJointPairListIt it = pList->begin(); it != pList->end(); ++it)
                {
                    cColladaJointPair &Pair = *it;
                    sData += cString::ToString(Pair.mlJoint) + " " + cString::ToString(Pair.mlWeight) + " ";
                    ++lCount;
                    ++vPairNumVec[idx];
                }
            }
            CreateXMLTextChild(pPairsElem, sData.c_str());
            Xml::SetAttr(pPairsElem, "Size", lCount);

            ////////////////////////////////
            //The pair numbers
            pugi::xml_node pPairNumElem = CreateXMLChild(pPairVecElem, "PairNum");
            sData = "";
            sData.reserve(pController->mvPairs.size() * 4);
            for(size_t idx = 0; idx < pController->mvPairs.size(); ++idx)
            {
                sData += cString::ToString(vPairNumVec[idx]) + " ";
            }
            CreateXMLTextChild(pPairNumElem, sData.c_str());

        }
    }
}

//-----------------------------------------------------------------------

static void SaveGeometryVec(pugi::xml_node apRootElem, tColladaGeometryVec *apColladaGeometryVec)
{
    pugi::xml_node pGeometryRootElem =  CreateXMLChild(apRootElem, "GeometryRoot");

    Xml::SetAttr(pGeometryRootElem, "Size", (int)apColladaGeometryVec->size());

    for(size_t i = 0; i < apColladaGeometryVec->size(); ++i)
    {
        cColladaGeometry *pGeometry = &(*apColladaGeometryVec)[i];

        pugi::xml_node pGeometryElem = CreateXMLChild(pGeometryRootElem, "Geometry");

        /////////////////////////////////7
        //Main properties
        Xml::SetAttr(pGeometryElem, "Id", pGeometry->msId.c_str());
        Xml::SetAttr(pGeometryElem, "Name", pGeometry->msName.c_str());

        Xml::SetAttr(pGeometryElem, "Material", pGeometry->msMaterial.c_str());

        /////////////////////////////////
        // Vertex properties
        // Should not be needed.
        /*Xml::SetAttr(pGeometryElem, "PosIdxNum", pGeometry->mlPosIdxNum);
        Xml::SetAttr(pGeometryElem, "NormIdxNum", pGeometry->mlNormIdxNum);
        Xml::SetAttr(pGeometryElem, "TexIdxNum", pGeometry->mlTexIdxNum);

        Xml::SetAttr(pGeometryElem, "PosArrayIdx", pGeometry->mlPosArrayIdx);
        Xml::SetAttr(pGeometryElem, "NormArrayIdx", pGeometry->mlNormArrayIdx);
        Xml::SetAttr(pGeometryElem, "TexArrayIdx", pGeometry->mlTexArrayIdx); */

        /////////////////////////////////
        // Index Vec
        pugi::xml_node pIndexVecElem = CreateXMLChild(pGeometryElem, "IndexVec");

        Xml::SetAttr(pIndexVecElem, "Size", (int)pGeometry->mvIndexVec.size());

        pugi::xml_node pIndicesDataElem = CreateXMLChild(pIndexVecElem, "Indices");

        tString sIndexVec = "";
        sIndexVec.reserve(pGeometry->mvIndexVec.size() * 5);
        for(size_t j = 0; j < pGeometry->mvIndexVec.size(); ++j)
        {
            sIndexVec += cString::ToString((int)pGeometry->mvIndexVec[j]) + " ";
        }
        pugi::xml_node pIndecVecText = CreateXMLTextChild(pIndicesDataElem, "Data");
        pIndecVecText.set_value(sIndexVec.c_str());

        /////////////////////////////////
        // Vertex vec
        pugi::xml_node pVertexVecElem = CreateXMLChild(pGeometryElem, "VertexVec");
        Xml::SetAttr(pVertexVecElem, "Size", (int)pGeometry->mvVertexVec.size());
        {
            /////////////////////////////////
            // Positions
            tString sData = "";
            sData.reserve(pGeometry->mvVertexVec.size() * 10 * 3);
            for(size_t j = 0; j < pGeometry->mvVertexVec.size(); ++j)
            {
                sData += pGeometry->mvVertexVec[j].pos.ToFileString() + " ";
            }
            pugi::xml_node pDataElem = CreateXMLChild(pVertexVecElem, "Positions");
            pugi::xml_node pVecText = CreateXMLTextChild(pDataElem, "Data");
            pVecText.set_value(sData.c_str());

            /////////////////////////////////
            // Normals
            sData = "";
            sData.reserve(pGeometry->mvVertexVec.size() * 10 * 3);
            for(size_t j = 0; j < pGeometry->mvVertexVec.size(); ++j)
            {
                sData += pGeometry->mvVertexVec[j].norm.ToFileString() + " ";
            }
            pDataElem = CreateXMLChild(pVertexVecElem, "Normals");
            pVecText = CreateXMLTextChild(pDataElem, "Data");
            pVecText.set_value(sData.c_str());

            /////////////////////////////////
            // UVs
            sData = "";
            sData.reserve(pGeometry->mvVertexVec.size() * 10 * 3);
            for(size_t j = 0; j < pGeometry->mvVertexVec.size(); ++j)
            {
                sData += pGeometry->mvVertexVec[j].tex.ToFileString() + " ";
            }
            pDataElem = CreateXMLChild(pVertexVecElem, "UV");
            pVecText = CreateXMLTextChild(pDataElem, "Data");
            pVecText.set_value(sData.c_str());

            /////////////////////////////////
            // Tangents
            sData = "";
            char sTemp[20];
            sData.reserve(pGeometry->mvVertexVec.size() * 10 * 4);
            for(size_t j = 0; j < pGeometry->mvTangents.size(); ++j)
            {
                //sTemp was never filled, so the cache got garbage (and could not be parsed again).
                snprintf(sTemp, sizeof(sTemp), "%g ", pGeometry->mvTangents[j]);
                sData += sTemp;
            }
            pDataElem = CreateXMLChild(pVertexVecElem, "Tangents");
            CreateXMLTextChild(pDataElem, sData.c_str());
        }

        /////////////////////////////////
        // Extra vertex vec
        //if(false)
        {
            pugi::xml_node pExtraVertexVecElem = CreateXMLChild(pGeometryElem, "ExtraVertexVec");
            Xml::SetAttr(pExtraVertexVecElem, "Size", (int)pGeometry->mvExtraVtxVec.size());

            tUIntVec vExtraNum;
            vExtraNum.resize(pGeometry->mvExtraVtxVec.size(), 0);

            ////////////////////////////
            //Extra vertices
            tString sData = "";
            sData.reserve(pGeometry->mvExtraVtxVec.size() * 4 * 4 * 4);
            int lExtraCount = 0;
            pugi::xml_node pExtraElem = CreateXMLChild(pExtraVertexVecElem, "ExtraVertex");
            for(size_t idx = 0; idx < pGeometry->mvExtraVtxVec.size(); ++idx)
            {
                tColladaExtraVtxList *pList = &pGeometry->mvExtraVtxVec[idx];

                tColladaExtraVtxListIt it = pList->begin();
                for(; it != pList->end(); ++it)
                {
                    cColladaExtraVtx& extra = *it;
                    sData +=    cString::ToString(extra.mlVtx) + " " +
                                cString::ToString(extra.mlNorm) + " " +
                                cString::ToString(extra.mlTex) + " " +
                                cString::ToString(extra.mlNewVtx) + " ";
                    ++lExtraCount;
                    ++vExtraNum[idx];
                }

            }
            CreateXMLTextChild(pExtraElem, sData.c_str());
            Xml::SetAttr(pExtraElem, "Size", (int)lExtraCount);

            ////////////////////////////
            //Extra num
            sData = "";
            sData.reserve(pGeometry->mvExtraVtxVec.size() * 4 * 4);
            pugi::xml_node pExtraNumElem = CreateXMLChild(pExtraVertexVecElem, "ExtraNum");
            for(size_t idx = 0; idx < pGeometry->mvExtraVtxVec.size(); ++idx)
            {
                sData += cString::ToString((int)vExtraNum[idx]) + " ";
            }
            CreateXMLTextChild(pExtraNumElem, sData.c_str());
        }
    }

}

//-----------------------------------------------------------------------

static void SaveIterativeNode(pugi::xml_node apParentElem, cColladaNode *apParentNode)
{
    tColladaNodeListIt it = apParentNode->mlstChildren.begin();
    for(; it != apParentNode->mlstChildren.end(); ++it)
    {
        cColladaNode *pNode = *it;
        pugi::xml_node pNodeElem =  CreateXMLChild(apParentElem, "Node");

        Xml::SetAttr(pNodeElem, "Id", pNode->msId.c_str());
        Xml::SetAttr(pNodeElem, "Sid", pNode->msSid.c_str());
        Xml::SetAttr(pNodeElem, "Name", pNode->msName.c_str());
        Xml::SetAttr(pNodeElem, "Type", pNode->msType.c_str());

        Xml::SetAttr(pNodeElem, "InstanceMaterial", pNode->msInstanceMaterial.c_str());

        Xml::SetAttr(pNodeElem, "Source", pNode->msSource.c_str());
        Xml::SetAttr(pNodeElem, "SourceIsFile", pNode->mbSourceIsFile ? "true" : "false");

        Xml::SetAttr(pNodeElem, "Transform", pNode->m_mtxTransform.ToFileString().c_str());
        Xml::SetAttr(pNodeElem, "WorldTransform", pNode->m_mtxWorldTransform.ToFileString().c_str());

        Xml::SetAttr(pNodeElem, "Scale", pNode->mvScale.ToFileString().c_str());

        Xml::SetAttr(pNodeElem, "Count", pNode->mlCount);


        pugi::xml_node pTransformRootElem =  CreateXMLChild(pNodeElem, "TransformRoot");
        //tColladaTransformList mlstTransforms;
        tColladaTransformListIt transIt = pNode->mlstTransforms.begin();
        for(; transIt != pNode->mlstTransforms.end(); ++transIt)
        {
            cColladaTransform & transform = *transIt;
            pugi::xml_node pTransformElem =  CreateXMLChild(pTransformRootElem, "Transform");

            Xml::SetAttr(pTransformElem, "Sid", transform.msSid.c_str());
            Xml::SetAttr(pTransformElem, "Type", transform.msType.c_str());

            tString sValues = "";
            for(size_t i = 0; i < transform.mvValues.size(); ++i)
            {
                sValues += cString::ToString(transform.mvValues[i]) + " ";
            }

            Xml::SetAttr(pTransformElem, "Values", sValues.c_str());
        }

        SaveIterativeNode(pNodeElem, pNode);
    }
}

static void SaveScene(pugi::xml_node apRootElem, cColladaScene *apColladaScene)
{
    pugi::xml_node pSceneElem =  CreateXMLChild(apRootElem, "Scene");

    Xml::SetAttr(pSceneElem, "StartTime", cString::ToString(apColladaScene->mfStartTime).c_str());
    Xml::SetAttr(pSceneElem, "EndTime", cString::ToString(apColladaScene->mfEndTime).c_str());
    Xml::SetAttr(pSceneElem, "DeltaTime", cString::ToString(apColladaScene->mfDeltaTime).c_str());

    pugi::xml_node pSceneRootElem =  CreateXMLChild(pSceneElem, "Root");

    SaveIterativeNode(pSceneRootElem, &apColladaScene->mRoot);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// LOAD COLLADA DATA
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

static void LoadImageVec(pugi::xml_node apRootElem, tColladaImageVec *apColladaImageVec);
static void LoadTextureVec(pugi::xml_node apRootElem, tColladaTextureVec *apColladaTextureVec);
static void LoadMaterialVec(pugi::xml_node apRootElem, tColladaMaterialVec *apColladaMaterialVec);
static void LoadLightVec(pugi::xml_node apRootElem, tColladaLightVec *apColladaLightVec);
static void LoadAnimationVec(pugi::xml_node apRootElem, tColladaAnimationVec *apColladaAnimVec);
static void LoadControllerVec(pugi::xml_node apRootElem, tColladaControllerVec *apColladaControllerVec);
static void LoadGeometryVec(pugi::xml_node apRootElem, tColladaGeometryVec *apColladaGeometryVec);
static void LoadScene(pugi::xml_node apRootElem, cColladaScene *apColladaScene);

bool cMeshLoaderCollada::LoadStructures(const tWString &asFile,
                                        tColladaImageVec *apColladaImageVec,
                                        tColladaTextureVec *apColladaTextureVec,
                                        tColladaMaterialVec *apColladaMaterialVec,
                                        tColladaLightVec *apColladaLightVec,
                                        tColladaGeometryVec *apColladaGeometryVec,
                                        tColladaControllerVec *apColladaControllerVec,
                                        tColladaAnimationVec *apColladaAnimVec,
                                        cColladaScene *apColladaScene)
{
    //unsigned long lStartTime = mpSystem->GetLowLevel()->GetTime();

    if(cPlatform::FileExists(asFile) == false)
    {
        return false;
    }

    pugi::xml_document xmlDoc;
    if(Xml::LoadFile(xmlDoc, asFile) == false)
    {
        Warning("Couldn't open XML file %s\n", cString::To8Char(asFile).c_str());
        return false;
    }

    //unsigned long lTime = mpSystem->GetLowLevel()->GetTime() - lStartTime;
    //Log("Loading cached collada XML file '%s' took: %d ms\n",asFile.c_str(),lTime);

    pugi::xml_node pRootElem = xmlDoc.document_element();

    if(apColladaImageVec)
    {
        LoadImageVec(pRootElem, apColladaImageVec);
    }
    if(apColladaTextureVec)
    {
        LoadTextureVec(pRootElem, apColladaTextureVec);
    }
    if(apColladaMaterialVec)
    {
        LoadMaterialVec(pRootElem, apColladaMaterialVec);
    }
    if(apColladaLightVec)
    {
        LoadLightVec(pRootElem, apColladaLightVec);
    }
    if(apColladaAnimVec)
    {
        LoadAnimationVec(pRootElem, apColladaAnimVec);
    }
    if(apColladaControllerVec)
    {
        LoadControllerVec(pRootElem, apColladaControllerVec);
    }
    if(apColladaGeometryVec)
    {
        LoadGeometryVec(pRootElem, apColladaGeometryVec);
    }
    if(apColladaScene)
    {
        LoadScene(pRootElem, apColladaScene);
    }

    return true;
}

//-----------------------------------------------------------------------

static void LoadImageVec(pugi::xml_node apRootElem, tColladaImageVec *apColladaImageVec)
{
    pugi::xml_node pImageRootElem = apRootElem.child("ImageRoot");

    int lSize = cString::ToInt(Xml::Attr(pImageRootElem, "Size"), 0);

    apColladaImageVec->clear();
    apColladaImageVec->resize(lSize);

    int lCount = 0;
    pugi::xml_node pImageElem = Xml::FirstElem(pImageRootElem);
    for(; pImageElem; pImageElem = Xml::NextElem(pImageElem), ++lCount)
    {
        cColladaImage* pImage = &(*apColladaImageVec)[lCount];

        pImage->msId = Xml::AttrOrEmpty(pImageElem, "Id");
        pImage->msName = Xml::AttrOrEmpty(pImageElem, "Name");
        pImage->msSource = Xml::AttrOrEmpty(pImageElem, "Source");
    }
}

//-----------------------------------------------------------------------

static void LoadTextureVec(pugi::xml_node apRootElem, tColladaTextureVec *apColladaTextureVec)
{
    pugi::xml_node pTextureRootElem = apRootElem.child("TextureRoot");

    int lSize = cString::ToInt(Xml::Attr(pTextureRootElem, "Size"), 0);

    apColladaTextureVec->clear();
    apColladaTextureVec->resize(lSize);

    int lCount = 0;
    pugi::xml_node pTextureElem = Xml::FirstElem(pTextureRootElem);
    for(; pTextureElem; pTextureElem = Xml::NextElem(pTextureElem), ++lCount)
    {
        cColladaTexture* pTexture = &(*apColladaTextureVec)[lCount];

        pTexture->msId = Xml::AttrOrEmpty(pTextureElem, "Id");
        pTexture->msName = Xml::AttrOrEmpty(pTextureElem, "Name");
        pTexture->msImage = Xml::AttrOrEmpty(pTextureElem, "Image");
    }
}

//-----------------------------------------------------------------------

static void LoadMaterialVec(pugi::xml_node apRootElem, tColladaMaterialVec *apColladaMaterialVec)
{
    pugi::xml_node pMaterialRootElem = apRootElem.child("MaterialRoot");

    int lSize = cString::ToInt(Xml::Attr(pMaterialRootElem, "Size"), 0);

    apColladaMaterialVec->clear();
    apColladaMaterialVec->resize(lSize);

    int lCount = 0;
    pugi::xml_node pMaterialElem = Xml::FirstElem(pMaterialRootElem);
    for(; pMaterialElem; pMaterialElem = Xml::NextElem(pMaterialElem), ++lCount)
    {
        cColladaMaterial* pMaterial = &(*apColladaMaterialVec)[lCount];

        pMaterial->msId = Xml::AttrOrEmpty(pMaterialElem, "Id");
        pMaterial->msName = Xml::AttrOrEmpty(pMaterialElem, "Name");
        pMaterial->msTexture = Xml::AttrOrEmpty(pMaterialElem, "Texture");
        pMaterial->mDiffuseColor = cString::ToColor(Xml::Attr(pMaterialElem, "Color"), cColor(0, 0));
    }
}

//-----------------------------------------------------------------------

static void LoadControllerVec(pugi::xml_node apRootElem, tColladaControllerVec *apColladaControllerVec)
{
    pugi::xml_node pControllerRootElem = apRootElem.child("ControllerRoot");

    int lSize = cString::ToInt(Xml::Attr(pControllerRootElem, "Size"), 0);

    apColladaControllerVec->clear();
    apColladaControllerVec->resize(lSize);

    int lCount = 0;
    pugi::xml_node pControllerElem = Xml::FirstElem(pControllerRootElem);
    for(; pControllerElem; pControllerElem = Xml::NextElem(pControllerElem), ++lCount)
    {
        cColladaController* pController = &(*apColladaControllerVec)[lCount];

        pController->msId = Xml::AttrOrEmpty(pControllerElem, "Id");
        pController->msTarget = Xml::AttrOrEmpty(pControllerElem, "Target");

        pController->m_mtxBindShapeMatrix = cString::ToMatrixf(Xml::Attr(pControllerElem, "BindShapeMatrix"), cMatrixf::Identity);

        pController->mlJointPairIdx = cString::ToInt(Xml::Attr(pControllerElem, "JointPairIdx"), 0);
        pController->mlWeightPairIdx = cString::ToInt(Xml::Attr(pControllerElem, "WeightPairIdx"), 0);

        ////////////////////////////
        //Joints
        {
            pugi::xml_node pJointsElem = pControllerElem.child("Joints");
            int lSize = cString::ToInt(Xml::Attr(pJointsElem, "Size"), 0);
            const char *pJointsText = Xml::TextOrEmpty(pJointsElem);

            tString sData = pJointsText;
            tString sSepp = " ";
            pController->mvJoints.reserve(lSize);
            cString::GetStringVec(sData, pController->mvJoints, &sSepp);
        }
        ////////////////////////////
        //Weights
        {
            pugi::xml_node pWeightsElem = pControllerElem.child("Weights");
            int lSize = cString::ToInt(Xml::Attr(pWeightsElem, "Size"), 0);
            const char *pWeightsText = Xml::TextOrEmpty(pWeightsElem);

            //tString sData = pWeightsText;
            //tString sSepp = " "; pController->mvWeights.reserve(lSize);
            //cString::GetFloatVec(sData,pController->mvWeights,&sSepp);

            pController->mvWeights.resize(lSize);
            cString::FloatStringToArray(&pController->mvWeights[0], pWeightsText, lSize);
        }
        ////////////////////////////
        //Matrices
        {
            pugi::xml_node pMatricesElem = pControllerElem.child("Matrices");
            int lSize = cString::ToInt(Xml::Attr(pMatricesElem, "Size"), 0);
            const char *pMatricesText = Xml::TextOrEmpty(pMatricesElem);

            //tString sData = pMatricesText;
            //tString sSepp = " ";
            //cString::GetFloatVec(sData,vRawData,&sSepp);
            tFloatVec vRawData;
            vRawData.resize(lSize * 16);
            cString::FloatStringToArray(&vRawData[0], pMatricesText, lSize * 16);


            float *pData = &vRawData[0];
            pController->mvMatrices.reserve(lSize);
            for(int i = 0; i < lSize; ++i)
            {
                pController->mvMatrices.push_back(
                    cMatrixf(    pData[0], pData[1], pData[2], pData[3],
                                 pData[4], pData[5], pData[6], pData[7],
                                 pData[8], pData[9], pData[10], pData[11],
                                 pData[12], pData[13], pData[14], pData[15]));
                pData += 16;
            }
        }
        ////////////////////////////
        //Pairs
        {
            pugi::xml_node pPairVecElem = pControllerElem.child("PairVec");
            int lSize = cString::ToInt(Xml::Attr(pPairVecElem, "Size"), 0);

            /////////////////////////////////////////
            // Get number of connections per pair
            pugi::xml_node pPairNumElem = pPairVecElem.child("PairNum");
            const char *pPairNumText = Xml::TextOrEmpty(pPairNumElem);

            tUIntVec vPairNum;
            vPairNum.resize(lSize);
            //tString sData = pPairNumText;
            //tString sSepp = " ";
            //cString::GetIntVec(sData,vPairNum,&sSepp);
            cString::UIntStringToArray(&vPairNum[0], pPairNumText, lSize);

            /////////////////////////////////////////
            // Get pairs
            pugi::xml_node pPairsElem = pPairVecElem.child("Pairs");
            int lPairSize = cString::ToInt(Xml::Attr(pPairsElem, "Size"), 0);
            const char *pPairsText = Xml::TextOrEmpty(pPairsElem);

            tUIntVec vPairs;
            vPairs.resize(lPairSize * 2);
            //sData = pPairsText;
            //cString::GetIntVec(sData,vPairs,&sSepp);
            cString::UIntStringToArray(&vPairs[0], pPairsText, lPairSize * 2);


            pController->mvPairs.resize(lSize);

            int lPairCount = 0;
            for(int i = 0; i < lSize; ++i)
            {
                for(unsigned int j = 0; j < vPairNum[i]; ++j)
                {
                    pController->mvPairs[i].push_back(
                        cColladaJointPair(vPairs[lPairCount], vPairs[lPairCount + 1]));
                    lPairCount += 2;
                }
            }
        }
    }
}
//-----------------------------------------------------------------------

static void LoadLightVec(pugi::xml_node apRootElem, tColladaLightVec *apColladaLightVec)
{
    pugi::xml_node pLightRootElem = apRootElem.child("LightRoot");

    int lSize = cString::ToInt(Xml::Attr(pLightRootElem, "Size"), 0);

    apColladaLightVec->clear();
    apColladaLightVec->resize(lSize);

    int lCount = 0;
    pugi::xml_node pLightElem = Xml::FirstElem(pLightRootElem);
    for(; pLightElem; pLightElem = Xml::NextElem(pLightElem), ++lCount)
    {
        cColladaLight* pLight = &(*apColladaLightVec)[lCount];

        pLight->msId = Xml::AttrOrEmpty(pLightElem, "Id");
        pLight->msName = Xml::AttrOrEmpty(pLightElem, "Name");
        pLight->msType = Xml::AttrOrEmpty(pLightElem, "Type");
        pLight->mfAngle = cString::ToFloat(Xml::Attr(pLightElem, "Angle"), 0);
        pLight->mDiffuseColor = cString::ToColor(Xml::Attr(pLightElem, "Color"), cColor(0, 0));
    }
}

//-----------------------------------------------------------------------

static void LoadAnimationVec(pugi::xml_node apRootElem, tColladaAnimationVec *apColladaAnimationVec)
{
    pugi::xml_node pAnimationRootElem = apRootElem.child("AnimationRoot");

    int lSize = cString::ToInt(Xml::Attr(pAnimationRootElem, "Size"), 0);

    apColladaAnimationVec->clear();
    apColladaAnimationVec->resize(lSize);

    int lCount = 0;
    pugi::xml_node pAnimationElem = Xml::FirstElem(pAnimationRootElem);
    for(; pAnimationElem; pAnimationElem = Xml::NextElem(pAnimationElem), ++lCount)
    {
        cColladaAnimation* pAnimation = &(*apColladaAnimationVec)[lCount];

        pAnimation->msId = Xml::AttrOrEmpty(pAnimationElem, "Id");
        pAnimation->msTargetNode = Xml::AttrOrEmpty(pAnimationElem, "TargetNode");

        /////////////////////////////////////
        //Channels
        {
            pugi::xml_node pChannelVecElem = pAnimationElem.child("ChannelVec");
            int lSize = cString::ToInt(Xml::Attr(pChannelVecElem, "Size"), 0);
            pAnimation->mvChannels.resize(lSize);

            int idx = 0;
            pugi::xml_node pChannelElem = pChannelVecElem.child("Channel");
            for(; pChannelElem; pChannelElem = pChannelElem.next_sibling("Channel"), ++idx)
            {
                cColladaChannel *pChannel = &pAnimation->mvChannels[idx];

                pChannel->msId = Xml::AttrOrEmpty(pChannelElem, "Id");
                pChannel->msTarget = Xml::AttrOrEmpty(pChannelElem, "Target");
                pChannel->msSource = Xml::AttrOrEmpty(pChannelElem, "Source");
            }
        }

        /////////////////////////////////////
        //Samplers
        {
            pugi::xml_node pSamplerVecElem = pAnimationElem.child("SamplerVec");
            int lSize = cString::ToInt(Xml::Attr(pSamplerVecElem, "Size"), 0);
            pAnimation->mvSamplers.resize(lSize);

            int idx = 0;
            pugi::xml_node pSamplerElem = pSamplerVecElem.child("Sampler");
            for(; pSamplerElem; pSamplerElem = pSamplerElem.next_sibling("Sampler"), ++idx)
            {
                cColladaSampler *pSampler = &pAnimation->mvSamplers[idx];

                pSampler->msId = Xml::AttrOrEmpty(pSamplerElem, "Id");
                pSampler->msTarget = Xml::AttrOrEmpty(pSamplerElem, "Target");
                pSampler->msTimeArray = Xml::AttrOrEmpty(pSamplerElem, "TimeArray");
                pSampler->msValueArray = Xml::AttrOrEmpty(pSamplerElem, "ValueArray");
            }
        }

        /////////////////////////////////////
        //Sources
        {
            pugi::xml_node pSourceVecElem = pAnimationElem.child("SourceVec");
            int lSize = cString::ToInt(Xml::Attr(pSourceVecElem, "Size"), 0);
            pAnimation->mvSources.resize(lSize);

            int idx = 0;
            pugi::xml_node pSourceElem = pSourceVecElem.child("Source");
            for(; pSourceElem; pSourceElem = pSourceElem.next_sibling("Source"), ++idx)
            {
                cColladaAnimSource *pSource = &pAnimation->mvSources[idx];

                pSource->msId = Xml::AttrOrEmpty(pSourceElem, "Id");
                tString sData = Xml::AttrOrEmpty(pSourceElem, "Values");
                tString sSepp = " ";
                pSource->mvValues.reserve(3);

                cString::GetFloatVec(sData, pSource->mvValues, &sSepp);
            }
        }

    }
}

//-----------------------------------------------------------------------

static void LoadGeometryVec(pugi::xml_node apRootElem, tColladaGeometryVec *apColladaGeometryVec)
{
    pugi::xml_node pGeometryRootElem = apRootElem.child("GeometryRoot");
    int lSize = cString::ToInt(Xml::Attr(pGeometryRootElem, "Size"), 0);

    apColladaGeometryVec->clear();
    apColladaGeometryVec->resize(lSize);

    int lCount = 0;
    pugi::xml_node pGeometryElem = Xml::FirstElem(pGeometryRootElem);
    for(; pGeometryElem; pGeometryElem = Xml::NextElem(pGeometryElem), ++lCount)
    {
        cColladaGeometry* pGeometry = &(*apColladaGeometryVec)[lCount];

        pGeometry->msId = Xml::AttrOrEmpty(pGeometryElem, "Id");
        pGeometry->msName = Xml::AttrOrEmpty(pGeometryElem, "Name");
        pGeometry->msMaterial = Xml::AttrOrEmpty(pGeometryElem, "Material");

        // XXX
        if(pGeometry->msName.empty())
        {
            pGeometry->msName = pGeometry->msId;
            Warning("ColladaGeometry object with id '%s' has empty name! Setting id as name\n", pGeometry->msId.c_str());
        }

        /////////////////////////////////
        // Index Vec
        {
            pugi::xml_node pIndexVecElem = pGeometryElem.child("IndexVec");
            int lSize = cString::ToInt(Xml::Attr(pIndexVecElem, "Size"), 0);

            if(lSize > 0)
            {
                pugi::xml_node pIndicesDataElem = pIndexVecElem.child("Indices");
                const char *pVecText = Xml::TextOrEmpty(pIndicesDataElem);

                pGeometry->mvIndexVec.resize(lSize);

                cString::UIntStringToArray(&pGeometry->mvIndexVec[0], pVecText, lSize);

                //tString sData = pVecText;
                //tString sSepp = " ";
                //cString::GetUIntVec(sData,pGeometry->mvIndexVec,&sSepp);
            }
        }

        /////////////////////////////////
        // Vertex vec
        {
            pugi::xml_node pVertexVecElem = pGeometryElem.child("VertexVec");
            int lSize = cString::ToInt(Xml::Attr(pVertexVecElem, "Size"), 0);

            if(lSize > 0)
            {
                pGeometry->mvVertexVec.resize(lSize);

                tFloatVec vRawData;
                vRawData.resize(lSize * 3);

                /////////////////////////////////
                // Positions
                {
                    pugi::xml_node pDataElem = pVertexVecElem.child("Positions");
                    const char *pVecText = Xml::TextOrEmpty(pDataElem);

                    //tString sData = pVecText;
                    //tString sSepp = " ";
                    //tFloatVec vRawData; vRawData.reserve(lSize * 3);
                    //cString::GetFloatVec(sData,vRawData,&sSepp);

                    cString::FloatStringToArray(&vRawData[0], pVecText, lSize * 3);

                    float *pData = &vRawData[0];
                    for(int i = 0; i < lSize; ++i)
                    {
                        cVector3f &vPos = pGeometry->mvVertexVec[i].pos;
                        vPos.x = pData[0];
                        vPos.y = pData[1];
                        vPos.z = pData[2];
                        pData += 3;
                    }
                }

                /////////////////////////////////
                // Normals
                {
                    pugi::xml_node pDataElem = pVertexVecElem.child("Normals");
                    const char *pVecText = Xml::TextOrEmpty(pDataElem);

                    //tString sData = pVecText;
                    //tString sSepp = " ";
                    //tFloatVec vRawData; vRawData.reserve(lSize * 3);
                    //cString::GetFloatVec(sData,vRawData,&sSepp);

                    cString::FloatStringToArray(&vRawData[0], pVecText, lSize * 3);

                    float *pData = &vRawData[0];
                    for(int i = 0; i < lSize; ++i)
                    {
                        cVector3f &vPos = pGeometry->mvVertexVec[i].norm;
                        vPos.x = pData[0];
                        vPos.y = pData[1];
                        vPos.z = pData[2];
                        pData += 3;
                    }
                }

                /////////////////////////////////
                // UVs
                {
                    pugi::xml_node pDataElem = pVertexVecElem.child("UV");
                    const char *pVecText = Xml::TextOrEmpty(pDataElem);

                    //tString sData = pVecText;
                    //tString sSepp = " ";
                    //tFloatVec vRawData; vRawData.reserve(lSize * 3);
                    //cString::GetFloatVec(sData,vRawData,&sSepp);

                    cString::FloatStringToArray(&vRawData[0], pVecText, lSize * 3);

                    float *pData = &vRawData[0];
                    for(int i = 0; i < lSize; ++i)
                    {
                        cVector3f &vPos = pGeometry->mvVertexVec[i].tex;
                        vPos.x = pData[0];
                        vPos.y = pData[1];
                        vPos.z = pData[2];
                        pData += 3;
                    }
                }

                /////////////////////////////////
                // Tangents
                {
                    pugi::xml_node pDataElem = pVertexVecElem.child("Tangents");
                    const char *pVecText = Xml::TextOrEmpty(pDataElem);

                    //tString sData = pVecText;
                    //tString sSepp = " ";
                    //tFloatVec vRawData; vRawData.reserve(lSize * 3);
                    //cString::GetFloatVec(sData,vRawData,&sSepp);

                    pGeometry->mvTangents.resize(lSize * 4);
                    cString::FloatStringToArray(&pGeometry->mvTangents[0], pVecText, lSize * 4);
                }
            }

        }

        /////////////////////////////////
        // Extra vertex vec
        {
            pugi::xml_node pVertexVecElem = pGeometryElem.child("ExtraVertexVec");
            int lSize = cString::ToInt(Xml::Attr(pVertexVecElem, "Size"), 0);

            if(lSize > 0)
            {
                pGeometry->mvExtraVtxVec.resize(lSize);
                tUIntVec mvExtraNum;
                mvExtraNum.resize(lSize);
                tUIntVec mvExtraVtx;

                //////////////////////////////////////////////
                // Extra num
                {
                    pugi::xml_node pExtraNumElem = pVertexVecElem.child("ExtraNum");
                    const char *pExtraNumText = Xml::TextOrEmpty(pExtraNumElem);

                    cString::UIntStringToArray(&mvExtraNum[0], pExtraNumText, lSize);
                }

                //////////////////////////////////////////////
                // Extra vertex
                {
                    pugi::xml_node pExtraVertexElem = pVertexVecElem.child("ExtraVertex");
                    const char *pExtraVertexText = Xml::TextOrEmpty(pExtraVertexElem);
                    int lExtraVtxSize = cString::ToInt(Xml::Attr(pExtraVertexElem, "Size"), 0);
                    mvExtraVtx.resize(lExtraVtxSize * 4);

                    cString::UIntStringToArray(&mvExtraVtx[0], pExtraVertexText, lExtraVtxSize * 4);
                }

                unsigned int *pData = &mvExtraVtx[0];
                for(int idx = 0; idx < lSize; ++idx)
                {
                    tColladaExtraVtxList *pList = &pGeometry->mvExtraVtxVec[idx];

                    for(unsigned int j = 0; j < mvExtraNum[idx]; ++j)
                    {
                        pList->push_back(cColladaExtraVtx(pData[0], pData[1], pData[2], pData[3]));
                        pData += 4;
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------

static void LoadIterativeNode(pugi::xml_node apNodeElem, cColladaNode *apParentNode, cColladaScene *apColladaScene)
{
    cColladaNode *pNode = apParentNode->CreateChild();
    apColladaScene->mlstNodes.push_back(pNode);

    pNode->msId = Xml::AttrOrEmpty(apNodeElem, "Id");
    pNode->msSid = cString::ToString(Xml::Attr(apNodeElem, "Sid"), pNode->msId);
    pNode->msName = cString::ToString(Xml::Attr(apNodeElem, "Name"), pNode->msId);
    pNode->msType = Xml::AttrOrEmpty(apNodeElem, "Type");

    pNode->msInstanceMaterial = cString::ToString(Xml::Attr(apNodeElem, "InstanceMaterial"), "");

    pNode->msSource  = Xml::AttrOrEmpty(apNodeElem, "Source");
    pNode->mbSourceIsFile = cString::ToBool(Xml::Attr(apNodeElem, "SourceIsFile"), false);

    pNode->m_mtxTransform = cString::ToMatrixf(Xml::Attr(apNodeElem, "Transform"), cMatrixf::Identity);
    pNode->m_mtxWorldTransform = cString::ToMatrixf(Xml::Attr(apNodeElem, "WorldTransform"), cMatrixf::Identity);

    pNode->mvScale = cString::ToVector3f(Xml::Attr(apNodeElem, "Scale"), 1);
    pNode->mlCount = cString::ToInt(Xml::Attr(apNodeElem, "Count"), 1);

    pugi::xml_node pTransformRootElem = apNodeElem.child("TransformRoot");
    pugi::xml_node pTransformElem = pTransformRootElem.child("Transform");
    for(; pTransformElem; pTransformElem = pTransformElem.next_sibling("Transform"))
    {
        pNode->mlstTransforms.push_back(cColladaTransform());
        cColladaTransform &transform = pNode->mlstTransforms.back();

        transform.msSid =  Xml::AttrOrEmpty(pTransformElem, "Sid");
        transform.msType =  Xml::AttrOrEmpty(pTransformElem, "Type");

        tString sData = Xml::AttrOrEmpty(pTransformElem, "Values");
        tString sSepp = " ";
        cString::GetFloatVec(sData, transform.mvValues, &sSepp);
    }

    pugi::xml_node pChildElem = apNodeElem.child("Node");
    for(; pChildElem; pChildElem = pChildElem.next_sibling("Node"))
    {
        LoadIterativeNode(pChildElem, pNode, apColladaScene);
    }
}

//-----------------------------------------------------------------------

static void LoadScene(pugi::xml_node apRootElem, cColladaScene *apColladaScene)
{
    pugi::xml_node pSceneElem = apRootElem.child("Scene");

    //Delete all nodes.
    apColladaScene->ResetNodes();

    apColladaScene->mfStartTime = cString::ToFloat(Xml::Attr(pSceneElem, "StartTime"), 0);
    apColladaScene->mfEndTime = cString::ToFloat(Xml::Attr(pSceneElem, "EndTime"), 0);
    apColladaScene->mfDeltaTime = cString::ToFloat(Xml::Attr(pSceneElem, "DeltaTime"), 0);

    pugi::xml_node pSceneRootElem = pSceneElem.child("Root");
    pugi::xml_node pNodeElem = pSceneRootElem.child("Node");
    for(; pNodeElem; pNodeElem = pNodeElem.next_sibling("Node"))
    {
        LoadIterativeNode(pNodeElem, &apColladaScene->mRoot, apColladaScene);
    }
}



//-----------------------------------------------------------------------
}
