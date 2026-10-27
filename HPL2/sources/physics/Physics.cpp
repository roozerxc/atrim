#include "physics/Physics.h"
#include "physics/LowLevelPhysics.h"

#include "physics/PhysicsWorld.h"
#include "physics/SurfaceData.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "impl/XmlPugi.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cPhysics::cPhysics(iLowLevelPhysics *apLowLevelPhysics)  : iUpdateable("HPL_Physics")
{
    mpLowLevelPhysics = apLowLevelPhysics;

    mlMaxImpacts = 6;
    mfImpactDuration = 0.4f;

    mbLog = false;
}

//-----------------------------------------------------------------------

cPhysics::~cPhysics()
{
    Log("Exiting Physics Module\n");
    Log("--------------------------------------------------------\n");

    STLDeleteAll(mlstWorlds);
    STLMapDeleteAll(m_mapSurfaceData);

    Log("--------------------------------------------------------\n\n");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cPhysics::Init(cResources *apResources)
{
    mpResources = apResources;
}

//-----------------------------------------------------------------------

void cPhysics::Update(double adFixedDelta)
{
    UpdateImpactCounts(adFixedDelta);
}

//-----------------------------------------------------------------------

iPhysicsWorld *cPhysics::CreateWorld(bool abAddSurfaceData)
{
    iPhysicsWorld * pWorld = mpLowLevelPhysics->CreateWorld();
    mlstWorlds.push_back(pWorld);

    if(abAddSurfaceData)
    {
        tSurfaceDataMapIt it = m_mapSurfaceData.begin();
        for(; it != m_mapSurfaceData.end(); ++it)
        {
            cSurfaceData *apData = it->second;
            apData->ToMaterial(pWorld);
        }
    }

    return pWorld;
}

//-----------------------------------------------------------------------

void cPhysics::DestroyWorld(iPhysicsWorld* apWorld)
{
    STLFindAndDelete(mlstWorlds, apWorld);
}

//-----------------------------------------------------------------------

bool cPhysics::CanPlayImpact()
{
    if((int)mlstImpactCounts.size() >= mlMaxImpacts)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void cPhysics::AddImpact()
{
    mlstImpactCounts.push_back(cPhysicsImpactCount());
}

//-----------------------------------------------------------------------

cSurfaceData *cPhysics::CreateSurfaceData(const tString& asName)
{
    cSurfaceData *pData = hplNew( cSurfaceData, (asName, this, mpResources) );

    m_mapSurfaceData.insert(tSurfaceDataMap::value_type(asName, pData));

    return pData;
}

cSurfaceData *cPhysics::GetSurfaceData(const tString& asName)
{
    tSurfaceDataMapIt it = m_mapSurfaceData.find(asName);
    if(it == m_mapSurfaceData.end())
    {
        return NULL;
    }

    return it->second;
}

//-----------------------------------------------------------------------

bool cPhysics::LoadSurfaceData(const tString& asFile)
{
    //////////////////////////////////
    //Open document
    pugi::xml_document xmlDoc;
    if(Xml::LoadFile(xmlDoc, cString::To16Char(asFile)) == false)
    {
        Error("Couldn't load XML file '%s'!\n", asFile.c_str());
        return false;
    }

    /////////////////////////
    //Get the root.
    pugi::xml_node pRootElem = xmlDoc.document_element();

    //////////////////////////
    //Iterate children
    pugi::xml_node pChildElem = pRootElem.child("Material");
    for(; pChildElem; pChildElem = pChildElem.next_sibling("Material"))
    {
        tString sName = cString::ToString(Xml::Attr(pChildElem, "Name"), "");
        if(sName == "")
        {
            continue;
        }

        cSurfaceData *pData = CreateSurfaceData(sName);

        //Get properties
        pData->SetElasticity(cString::ToFloat(Xml::Attr(pChildElem, "Elasticity"), 0.5f));
        pData->SetKineticFriction(cString::ToFloat(Xml::Attr(pChildElem, "KineticFriction"), 0.3f));
        pData->SetStaticFriction(cString::ToFloat(Xml::Attr(pChildElem, "StaticFriction"), 0.3f));

        pData->SetPriority(cString::ToInt(Xml::Attr(pChildElem, "Priority"), 0));

        pData->SetElasticityCombMode(GetCombMode(Xml::Attr(pChildElem, "ElasticityMode")));
        pData->SetFrictionCombMode(GetCombMode(Xml::Attr(pChildElem, "FrictionMode")));

        pData->GetStepType(cString::ToString(Xml::Attr(pChildElem, "StepType"), ""));

        pData->SetMinScrapeSpeed(cString::ToFloat(Xml::Attr(pChildElem, "MinScrapeSpeed"), 0.7f));
        pData->SetMinScrapeFreq(cString::ToFloat(Xml::Attr(pChildElem, "MinScrapeFreq"), 0.7f));
        pData->SetMinScrapeFreqSpeed(cString::ToFloat(Xml::Attr(pChildElem, "MinScrapeFreqSpeed"), 0.7f));
        pData->SetMiddleScrapeSpeed(cString::ToFloat(Xml::Attr(pChildElem, "MiddleScrapeSpeed"), 1.2f));
        pData->SetMaxScrapeFreqSpeed(cString::ToFloat(Xml::Attr(pChildElem, "MaxScrapeFreqSpeed"), 3));
        pData->SetMaxScrapeFreq(cString::ToFloat(Xml::Attr(pChildElem, "MaxScrapeFreq"), 2));
        pData->SetMinScrapeContacts(cString::ToInt(Xml::Attr(pChildElem, "MinScrapeContacts"), 4));
        pData->SetScrapeSoundName(cString::ToString(Xml::Attr(pChildElem, "ScrapeSoundName"), ""));

        pData->SetMinRollSpeed(cString::ToFloat(Xml::Attr(pChildElem, "MinRollSpeed"), 0.7f));
        pData->SetMinRollFreq(cString::ToFloat(Xml::Attr(pChildElem, "MinRollFreq"), 0.7f));
        pData->SetMinRollVolume(cString::ToFloat(Xml::Attr(pChildElem, "MinRollVolume"), 0.7f));
        pData->SetMinRollFreqSpeed(cString::ToFloat(Xml::Attr(pChildElem, "MinRollFreqSpeed"), 0.7f));
        pData->SetMiddleRollSpeed(cString::ToFloat(Xml::Attr(pChildElem, "MiddleRollSpeed"), 1.2f));
        pData->SetMaxRollFreqSpeed(cString::ToFloat(Xml::Attr(pChildElem, "MaxRollFreqSpeed"), 3));
        pData->SetMaxRollFreq(cString::ToFloat(Xml::Attr(pChildElem, "MaxRollFreq"), 2));
        pData->SetMaxRollVolume(cString::ToFloat(Xml::Attr(pChildElem, "MaxRollVolume"), 2));
        pData->SetRollSoundName(cString::ToString(Xml::Attr(pChildElem, "RollSoundName"), ""));

        //Axes
        tString sAxisVec = cString::ToString(Xml::Attr(pChildElem, "RollAxis"), "");
        tStringVec vAxes;
        tFlag axisFlags = 0;
        cString::GetStringVec(sAxisVec, vAxes);
        for(size_t i = 0; i < vAxes.size(); ++i)
        {
            tString sAxis = cString::ToLowerCase(vAxes[i]);
            if(sAxis == "x")
            {
                axisFlags |= eRollAxisFlag_X;
            }
            else if(sAxis == "y")
            {
                axisFlags |= eRollAxisFlag_Y;
            }
            else if(sAxis == "z")
            {
                axisFlags |= eRollAxisFlag_Z;
            }
        }
        pData->SetRollAxisFlags(axisFlags);

        /////////////////////////
        //Get Impact data
        pugi::xml_node pImpactElem = pChildElem.child("Impact");
        for(; pImpactElem; pImpactElem = pImpactElem.next_sibling("Impact"))
        {
            float fMinSpeed = cString::ToFloat(Xml::Attr(pImpactElem, "MinSpeed"), 1);

            cSurfaceImpactData *pImpactData = pData->CreateImpactData(fMinSpeed);

            pImpactData->SetSoundName(cString::ToString(Xml::Attr(pImpactElem, "SoundName"), ""));
            pImpactData->SetPSName(cString::ToString(Xml::Attr(pImpactElem, "PSName"), ""));
            pImpactData->SetPSPrio(cString::ToInt(Xml::Attr(pImpactElem, "PSPrio"), 10));
        }

        /////////////////////////
        //Get Hit data
        pugi::xml_node pHitElem = pChildElem.child("Hit");
        for(; pHitElem; pHitElem = pHitElem.next_sibling("Hit"))
        {
            float fMinSpeed = cString::ToFloat(Xml::Attr(pHitElem, "MinSpeed"), 1);

            cSurfaceImpactData *pHitData = pData->CreateHitData(fMinSpeed);

            pHitData->SetSoundName(cString::ToString(Xml::Attr(pHitElem, "SoundName"), ""));
            pHitData->SetPSName(cString::ToString(Xml::Attr(pHitElem, "PSName"), ""));
            pHitData->SetPSPrio(cString::ToInt(Xml::Attr(pHitElem, "PSPrio"), 10));
        }

        /*Log("Added %s e: %f sf: %f kf: %f emode: %d fmode: %d\n", pData->GetName().c_str(),
                pData->GetElasticity(), pData->GetStaticFriction(), pData->GetKineticFriction(),
                pData->GetElasticityCombMode(), pData->GetFrictionCombMode());*/
    }

    return true;
}
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

ePhysicsMaterialCombMode cPhysics::GetCombMode(const char *apName)
{
    if(apName == NULL)
    {
        return ePhysicsMaterialCombMode_Average;
    }

    tString sMode = cString::ToLowerCase(apName);

    if(sMode == "average")
    {
        return ePhysicsMaterialCombMode_Average;
    }
    if(sMode == "min")
    {
        return ePhysicsMaterialCombMode_Min;
    }
    if(sMode == "max")
    {
        return ePhysicsMaterialCombMode_Max;
    }
    if(sMode == "multiply")
    {
        return ePhysicsMaterialCombMode_Multiply;
    }

    return ePhysicsMaterialCombMode_Average;
}

//-----------------------------------------------------------------------

void cPhysics::UpdateImpactCounts(double adFixedDelta)
{
    tPhysicsImpactCountListIt it = mlstImpactCounts.begin();
    while(it != mlstImpactCounts.end())
    {
        it->mfCount += (float)adFixedDelta;
        if(it->mfCount > mfImpactDuration)
        {
            it = mlstImpactCounts.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

//-----------------------------------------------------------------------

}
