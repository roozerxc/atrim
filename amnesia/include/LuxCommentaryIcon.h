#ifndef LUX_COMMENTARY_ICON_H
#define LUX_COMMENTARY_ICON_H

//----------------------------------------------

#include "LuxEntity.h"

//----------------------------------------------

class cLuxCommentaryIcon_SaveData : public iLuxEntity_SaveData
{
    kSerializableClassInit(cLuxCommentaryIcon_SaveData)
public:
    virtual ~cLuxCommentaryIcon_SaveData();

    tString msFileName;
    cMatrixf m_mtxOnLoadTransform;
    cVector3f mvOnLoadScale;

    tString msTalker;
    tString msTopic;
    tString msSoundFile;

    iLuxEntity* CreateEntity(cLuxMap *apMap);
};

//----------------------------------------------

class cLuxCommentaryIcon : public iLuxEntity
{
    typedef iLuxEntity super_class;
    friend class cLuxCommentaryIconLoader;
public:
    cLuxCommentaryIcon(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxCommentaryIcon();

    //////////////////////
    //General
    void SetupAfterLoad(cWorld *apWorld);

    void OnUpdate(float afTimeStep);

    virtual float OnInteractDebugDraw(cGuiSet *apSet,iFontData *apFont, float afStartY)
    {
        return afStartY;
    }

    //////////////////////
    //Actions
    void GiveDamage(float afAmount, int alStrength) {}

    bool CanInteract(iPhysicsBody *apBody)
    {
        return true;
    }
    bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

    eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
    {
        return eLuxFocusCrosshair_Grab;
    }
    virtual iEntity3D* GetAttachEntity()
    {
        return mvBodies[0];
    }

    tWString GetFocusText();

    //////////////////////
    //Properies
    void SetPlayingSound(bool abX);
    bool GetPlayingSound()
    {
        return mbPlayingSound;
    }

    //////////////////////
    //Data
    cMeshEntity * GetMeshEntity()
    {
        return mpMeshEntity;
    }
    virtual cMeshEntity* GetEffectMeshEntity()
    {
        return mpMeshEntity;
    }

    int GetBodyNum()
    {
        return (int)mvBodies.size();
    }
    iPhysicsBody* GetBody(int alIdx)
    {
        return mvBodies[alIdx];
    }

    iPhysicsBody* GetMainBody()
    {
        return mvBodies[0];
    }

    //////////////////////
    //Connection callbacks
    void OnConnectionStateChange(iLuxEntity *apEntity, int alState) {}

    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData* CreateSaveData();
    virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);

protected:
    void OnSetActive(bool abX);

    void LoadCommentaryFile(const tString& asFile);

    //////////////
    //Variables
    tString msTalker;
    tString msTopic;
    tString msSoundFile;

    bool mbPlayingSound;
    cVector3f mvRotAngle;

    ////////////////
    //Data
    cMeshEntity *mpMeshEntity;
    std::vector<iLight*> mvLights;
    std::vector<cParticleSystem*> mvParticleSystems;
    std::vector<cBillboard*> mvBillboards;

    std::vector<iPhysicsBody*> mvBodies;

    std::vector<cMatrixf> mvSubMeshMatrix;

    tString msRotateSubMesh;


    //////////////
    //Save specific
    tString msFileName;
    cMatrixf m_mtxOnLoadTransform;
    cVector3f mvOnLoadScale;
};

//----------------------------------------------

class cLuxCommentaryIconLoader : public cEntityLoader_Object
{
public:
    cLuxCommentaryIconLoader(const tString& asName);
    virtual ~cLuxCommentaryIconLoader() {}

    void BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);
    void AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);

protected:
    float mfDefaultMaxFocusDistance;
};

//----------------------------------------------


#endif // LUX_COMMENTARY_ICON_H
