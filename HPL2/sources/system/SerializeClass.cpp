#include "system/SerializeClass.h"


#include "system/LowLevelSystem.h"

#include "system/String.h"
#include "system/Platform.h"

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "system/Container.h"

#include "impl/XmlPugi.h"

#include "resources/BinaryBuffer.h"

#define ZLIB_WINAPI
#include <zlib.h>
#include <stdio.h>
#include <cstring>

namespace hpl
{

#define kSavedDataCRCKey (0x12AD11A1)

//////////////////////////////////////////////////////////////////////////
// SERIALIZEABLE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SERIALIZE SAVED CLASS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSerializeSavedClass::cSerializeSavedClass(const char* asName, const char *asParent,
        cSerializeMemberField* apMemberFields, size_t alSize,
        iSerializable * (*apCreateFunc)())
{
    msName = asName;
    msParentName = asParent;
    mpMemberFields = apMemberFields;
    mlSize = alSize;
    mpCreateFunc = apCreateFunc;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SERIALIZE MEMBER FIELD ITERATOR
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSerializeMemberFieldIterator::cSerializeMemberFieldIterator(cSerializeSavedClass *apTopClass)
{
    mlFieldNum = 0;
    mpSavedClass = apTopClass;

    //Make sure that the first field type is not null
    if(mpSavedClass && mpSavedClass->mpMemberFields[mlFieldNum].mType == eSerializeType_NULL)
    {
        if(mpSavedClass->msParentName[0] != '\0')
        {
            cSerializeSavedClass *pClass = cSerializeClass::GetClass(mpSavedClass->msParentName);
            if(pClass)
            {
                mpSavedClass = pClass;
                mlFieldNum = 0;
            }
        }
    }
}

//-----------------------------------------------------------------------

bool cSerializeMemberFieldIterator::HasNext()
{
    if(mpSavedClass == NULL ||
            mpSavedClass->mpMemberFields[mlFieldNum].mType == eSerializeType_NULL)
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------

cSerializeMemberField *cSerializeMemberFieldIterator::GetNext()
{
    cSerializeMemberField *pField = &mpSavedClass->mpMemberFields[mlFieldNum];
    mlFieldNum++;

    if(mpSavedClass->mpMemberFields[mlFieldNum].mType == eSerializeType_NULL)
    {
        if(mpSavedClass->msParentName[0] != '\0')
        {
            cSerializeSavedClass *pClass = cSerializeClass::GetClass(mpSavedClass->msParentName);
            if(pClass)
            {
                mpSavedClass = pClass;
                mlFieldNum = 0;
            }
        }
    }

    return pField;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SERIALIZE CLASS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSerializeSavedClass gvSerializeTempClasses[kMaxSerializeClasses];
int glSerializeTempClassesNum = 0;

//Define static variables
tSerializeSavedClassMap cSerializeClass::m_mapSavedClasses;
bool cSerializeClass::mbDataSetup = false;
tString cSerializeClass::msTempString = "";
char cSerializeClass::msTempCharArray[2048];

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC SERIALIZE CLASS METHODS
//////////////////////////////////////////////////////////////////////////

static bool gbLog = false;
static int glTabs = 0;
static tString gsTabString = "";

const char    *GetTabs()
{
    gsTabString = "";
    for(int i = 0; i < glTabs; ++i)
    {
        gsTabString += "   ";
    }
    return gsTabString.c_str();
}

cSerializeClass::cSerializeClass(const char* asName, const char* asParent,
                                 cSerializeMemberField* apMemberFields, size_t alSize,
                                 iSerializable * (*apCreateFunc)())
{
    gvSerializeTempClasses[glSerializeTempClassesNum] = cSerializeSavedClass(asName, asParent,
        apMemberFields, alSize, apCreateFunc);

    glSerializeTempClassesNum++;
}

//-----------------------------------------------------------------------

void cSerializeClass::SetLog(bool abX)
{
    gbLog = abX;
}

bool cSerializeClass::GetLog()
{
    return gbLog;
}

//-----------------------------------------------------------------------

void cSerializeClass::PrintMembers(iSerializable* apData)
{
    SetUpData();

    cSerializeMemberFieldIterator classIt = GetMemberFieldIterator(apData);

    Log("Members of class '%s':\n", apData->Serialize_GetTopClass().c_str());
    while(classIt.HasNext())
    {
        cSerializeMemberField *pField = classIt.GetNext();

        Log(" '%s':'%s' type: %d offset: %d\n", pField->msName.c_str(),
            ValueToString(apData, pField->mlOffset, pField->mType),
            pField->mType, pField->mlOffset);
    }
}

//-----------------------------------------------------------------------

bool cSerializeClass::SaveToFile(iSerializable* apData, const tWString &asFile, const tString &asRoot, bool abCompressAndCrc)
{
    SetUpData();

    glTabs = 0;

    pugi::xml_document xmlDoc;

    //Create root
    pugi::xml_node rootElem = xmlDoc.append_child(asRoot.c_str());

    SaveToElement(apData, "", rootElem);

    ///////////////////////////////
    //Normal Save
    if(abCompressAndCrc == false)
    {
        bool bRet = Xml::SaveFile(xmlDoc, asFile);
        if(bRet == false)
        {
            Error("Couldn't save class to '%s'\n", cString::To8Char(asFile).c_str());
        }

        return bRet;
    }
    ///////////////////////////////
    //Compressed Save
    else
    {
        /////////////////////////////
        // Get the data
        tString sData;
        Xml::SaveString(xmlDoc, &sData);

        /////////////////////////////
        // Compress the data
        cBinaryBuffer destBuffer;

        destBuffer.AddCRC_Begin();

        if(destBuffer.CompressAndAdd(&sData[0], sData.size() + 1) == false)
        {
            Error("Unable to compress data for serialized data '%s'!\n", cString::To8Char(asFile).c_str());
            return false;
        }

        destBuffer.AddCRC_End(kSavedDataCRCKey);

        /////////////////////////////
        // Save the file
        if(destBuffer.Save(asFile) == false)
        {
            Error("Unable to save serialized file '%s'!\n", cString::To8Char(asFile).c_str());
            return false;
        }

        return true;
    }
}

//-----------------------------------------------------------------------


void cSerializeClass::SaveToElement(iSerializable* apData, const tString &asName, pugi::xml_node apParent,
                                    bool abIsPointer)
{
    SetUpData();

    if(apData == NULL)
    {
        return;
    }

    //Create element
    pugi::xml_node pClassElem = apParent.append_child(abIsPointer ? "class_ptr" : "class");


    //Set attributes
    Xml::SetAttr(pClassElem, "type", apData->Serialize_GetTopClass().c_str());
    Xml::SetAttr(pClassElem, "name", asName.c_str());

    if(gbLog)
    {
        Log("---Saving class '%s' Begin---\n", apData->Serialize_GetTopClass().c_str());
    }

    cSerializeMemberFieldIterator classIt = GetMemberFieldIterator(apData);

    while(classIt.HasNext())
    {
        cSerializeMemberField *pField = classIt.GetNext();

        if(gbLog)
        {
            Log(" Field : '%s', MainType: %d Type: %d\n", pField->msName.c_str(), pField->mMainType, pField->mType);
        }

        switch(pField->mMainType)
        {
        // VARIABLE /////////////////////////////////
        case eSerializeMainType_Variable:
        {
            SaveVariable(pClassElem, pField, apData);
            break;
        }
        // ARRAY ////////////////////////////////////
        case eSerializeMainType_Array:
        {
            SaveArray(pClassElem, pField, apData);
            break;
        }
        // CONTAINER ////////////////////////////////////
        case eSerializeMainType_Container:
        {
            SaveContainer(pClassElem, pField, apData);
            break;
        }
        }
    }

    if(gbLog)
    {
        Log("---Done saving class '%s' ---\n", apData->Serialize_GetTopClass().c_str());
    }
}

//-----------------------------------------------------------------------

bool cSerializeClass::LoadFromFile(iSerializable* apData, const tWString &asFile, bool abCompressedAndCrc)
{
    SetUpData();

    glTabs = 0;

    //Load document
    pugi::xml_document xmlDoc;

    ///////////////////////////////
    //Normal Load
    if(abCompressedAndCrc == false)
    {
        if(cPlatform::FileExists(asFile) == false)
        {
            Error("Unable to open serialized file '%s' as rb! Invalid filepointer returned!\n", cString::To8Char(asFile).c_str());
            return false;
        }

        tString sErrorDesc;
        if(Xml::LoadFile(xmlDoc, asFile, &sErrorDesc) == false)
        {
            Error("Couldn't load saved class file '%s' from %s!\n",
                  cString::To8Char(asFile).c_str(),
                  sErrorDesc.c_str());
            return false;
        }
    }
    ///////////////////////////////
    //Compressed Load
    else
    {
        ////////////////////////////////
        // Get compressed data
        cBinaryBuffer compBuffer;
        if(compBuffer.Load(asFile) == false)
        {
            Error("Unable to open serialized file '%s'!\n", cString::To8Char(asFile).c_str());
            return false;
        }

        ////////////////////////////////
        // Check CRC
        if(compBuffer.CheckInternalCRC(kSavedDataCRCKey) == false)
        {
            Error("CRC check for serialized file '%s' failed!\n", cString::To8Char(asFile).c_str());
            return false;
        }

        ////////////////////////////////
        // Uncompress data
        cBinaryBuffer textBuffer;
        if(textBuffer.DecompressAndAddFromBuffer(&compBuffer, false) == false)
        {
            Error("Unable to decompress serialized data '%s'!\n", cString::To8Char(asFile).c_str());
            return false;
        }

        ////////////////////
        //Load xml (errors are handled like before: whatever was parsed is used)
        const char *pData = textBuffer.GetDataPointerAtCurrentPos();
        Xml::LoadFromMemory(xmlDoc, pData, pData ? strlen(pData) : 0);
    }


    //Get root
    pugi::xml_node pRootElem = xmlDoc.document_element();
    if(!pRootElem)
    {
        Error("Serialized file '%s' has no root element!\n", cString::To8Char(asFile).c_str());
        return false;
    }

    //Get first, there should only be ONE class at the root.
    pugi::xml_node pClassElem = pRootElem.child("class");
    if(!pClassElem)
    {
        Error("Serialized file '%s' has no class element!\n", cString::To8Char(asFile).c_str());
        return false;
    }

    LoadFromElement(apData, pClassElem);

    return true;
}

//-----------------------------------------------------------------------

void cSerializeClass::LoadFromElement(iSerializable* apData, pugi::xml_node apElement,
                                      bool abIsPointer)
{
    SetUpData();

    if(apData == NULL || !apElement)
    {
        return;
    }

    cSerializeSavedClass *pClass = GetClass(apData->Serialize_GetTopClass());
    if(pClass == NULL)
    {
        return;
    }

    if(gbLog)
    {
        Log("%sBegin class %s\n", GetTabs(), pClass->msName);
        ++glTabs;
    }

    pugi::xml_node pMemberElem = Xml::FirstElem(apElement);
    for(; pMemberElem; pMemberElem = Xml::NextElem(pMemberElem))
    {
        tString sMainType = pMemberElem.name();

        if(gbLog)
        {
            Log("%sMember field type %s\n", GetTabs(), sMainType.c_str());
        }

        if(sMainType == "var")
        {
            LoadVariable(pMemberElem, apData, pClass);
        }
        else if(sMainType == "array")
        {
            LoadArray(pMemberElem, apData, pClass);
        }
        else if(sMainType == "class")
        {
            LoadClass(pMemberElem, apData, pClass);
        }
        else if(sMainType == "class_ptr")
        {
            LoadClassPointer(pMemberElem, apData, pClass);
        }
        else if(sMainType == "container")
        {
            LoadContainer(pMemberElem, apData, pClass);
        }
    }

    if(gbLog)
    {
        --glTabs;
        Log("%sEnd class %s\n", GetTabs(), pClass->msName);
    }
}

//-----------------------------------------------------------------------

cSerializeSavedClass *cSerializeClass::GetClass(const tString &asName)
{
    SetUpData();

    tSerializeSavedClassMapIt it = m_mapSavedClasses.find(asName);
    if(it == m_mapSavedClasses.end())
    {
        Warning("Serialize class '%s' not found!\n", asName.c_str());
        return NULL;
    }

    return &it->second;
}

//-----------------------------------------------------------------------

cSerializeMemberFieldIterator cSerializeClass::GetMemberFieldIterator(iSerializable* apData)
{
    cSerializeSavedClass *pClass = GetClass(apData->Serialize_GetTopClass());

    return cSerializeMemberFieldIterator(pClass);
}

//-----------------------------------------------------------------------

#define ValueTypePointer(aObject, aOffset, aType)    (aType*)(((char*)aObject)+aOffset)
#define ValuePointer(aObject, aOffset)                (((char*)aObject)+aOffset)
#define PointerValue(aPointer,aType)                (*((aType*)aPointer))

const char *cSerializeClass::ValueToString(void* apData, size_t alOffset, eSerializeType aType)
{
    void *pVal = ValuePointer(apData, alOffset);

    switch(aType)
    {
    /////////// BOOL ////////////////////////////////
    case eSerializeType_Bool:
    {
        if(PointerValue(pVal, bool) == 0)
        {
            return "false";
        }
        else
        {
            return "true";
        }
        break;
    }

    /////////// INT 32 ////////////////////////////////
    case eSerializeType_Int32:
    {
        snprintf(msTempCharArray, sizeof(msTempCharArray), "%d", PointerValue(pVal, int));
        return msTempCharArray;
        break;
    }

    /////////// FLOAT 32 ////////////////////////////////
    case eSerializeType_Float32:
    {
        snprintf(msTempCharArray, sizeof(msTempCharArray), "%f", PointerValue(pVal, float));
        return msTempCharArray;
        break;
    }

    /////////// STRING ////////////////////////////////
    case eSerializeType_String:
    {
        tString &sVal = PointerValue(pVal, tString);
        return sVal.c_str();
        break;
    }

    /////////// VECTOR 2L ////////////////////////////////
    case eSerializeType_Vector2l:
    {
        cVector2l &vVec = PointerValue(pVal, cVector2l);
        snprintf(msTempCharArray, sizeof(msTempCharArray), "%d %d", vVec.x, vVec.y);
        return msTempCharArray;
        break;
    }

    /////////// VECTOR 2F ////////////////////////////////
    case eSerializeType_Vector2f:
    {
        cVector2f &vVec = PointerValue(pVal, cVector2f);
        snprintf(msTempCharArray, sizeof(msTempCharArray), "%f %f", vVec.x, vVec.y);
        return msTempCharArray;
        break;
    }

    /////////// VECTOR 3L ////////////////////////////////
    case eSerializeType_Vector3l:
    {
        cVector3l &vVec = PointerValue(pVal, cVector3l);
        snprintf(msTempCharArray, sizeof(msTempCharArray), "%d %d %d", vVec.x, vVec.y, vVec.z);
        return msTempCharArray;
        break;
    }

    /////////// VECTOR 3F ////////////////////////////////
    case eSerializeType_Vector3f:
    {
        cVector3f &vVec = PointerValue(pVal, cVector3f);
        snprintf(msTempCharArray, sizeof(msTempCharArray), "%f %f %f", vVec.x, vVec.y, vVec.z);
        return msTempCharArray;
        break;
    }

    /////////// MATRIX F ////////////////////////////////
    case eSerializeType_Matrixf:
    {
        cMatrixf &Mtx = PointerValue(pVal, cMatrixf);
        snprintf(msTempCharArray, sizeof(msTempCharArray), "%f %f %f %f "
                                                           "%f %f %f %f "
                                                           "%f %f %f %f "
                                                           "%f %f %f %f",
                 Mtx.m[0][0], Mtx.m[0][1], Mtx.m[0][2], Mtx.m[0][3],
                 Mtx.m[1][0], Mtx.m[1][1], Mtx.m[1][2], Mtx.m[1][3],
                 Mtx.m[2][0], Mtx.m[2][1], Mtx.m[2][2], Mtx.m[2][3],
                 Mtx.m[3][0], Mtx.m[3][1], Mtx.m[3][2], Mtx.m[3][3]);
        return msTempCharArray;
        break;
    }

    /////////// COLOR ////////////////////////////////
    case eSerializeType_Color:
    {
        cColor Col = PointerValue(pVal, cColor);
        snprintf(msTempCharArray, sizeof(msTempCharArray), "%f %f %f %f", Col.r, Col.g, Col.b, Col.a);
        return msTempCharArray;
        break;
    }

    /////////// RECT 2L ////////////////////////////////
    case eSerializeType_Rect2l:
    {
        cRect2l &vR = PointerValue(pVal, cRect2l);
        snprintf(msTempCharArray, sizeof(msTempCharArray), "%d %d %d %d", vR.x, vR.y, vR.w, vR.h);
        return msTempCharArray;
        break;
    }

    /////////// RECT 2F ////////////////////////////////
    case eSerializeType_Rect2f:
    {
        cRect2f &vR = PointerValue(pVal, cRect2f);
        snprintf(msTempCharArray, sizeof(msTempCharArray), "%f %f %f %f", vR.x, vR.y, vR.w, vR.h);
        return msTempCharArray;
        break;
    }

    /////////// PLANE F ////////////////////////////////
    case eSerializeType_Planef:
    {
        cPlanef &vP = PointerValue(pVal, cPlanef);
        snprintf(msTempCharArray, sizeof(msTempCharArray), "%f %f %f %f", vP.a, vP.b, vP.c, vP.d);
        return msTempCharArray;
        break;
    }

    /////////// WSTRING ////////////////////////////////
    case eSerializeType_WString:
    {
        msTempString = "";
        tWString &wsString = PointerValue(pVal, tWString);
        for(size_t i = 0; i < wsString.length(); ++i)
        {
            msTempString += cString::ToString((int)wsString[i]) + " ";
        }
        return msTempString.c_str();
        break;
    }

    }


    return "";
}

//-----------------------------------------------------------------------

void cSerializeClass::StringToValue(void* apData, size_t alOffset, eSerializeType aType,
                                    const char *asVal)
{
    if(asVal == NULL)
    {
        asVal = "";
    }

    void *pVal = ValuePointer(apData, alOffset);

    switch(aType)
    {
    /////////// BOOL ////////////////////////////////
    case eSerializeType_Bool:
    {
        PointerValue(pVal, bool) = cString::ToBool(asVal, false);
        break;
    }

    ////////// INT 32 ////////////////////////////////
    case eSerializeType_Int32:
    {
        PointerValue(pVal, int) = cString::ToInt(asVal, 0);
        break;
    }
    ////////// FLOAT 32 ////////////////////////////////
    case eSerializeType_Float32:
    {
        PointerValue(pVal, float) = cString::ToFloat(asVal, 0);
        break;
    }
    ////////// STRING ////////////////////////////////
    case eSerializeType_String:
    {
        PointerValue(pVal, tString) = cString::ToString(asVal, "");
        break;
    }

    /////////// VECTOR 2L ////////////////////////////////
    case eSerializeType_Vector2l:
    {
        tIntVec vVals;
        vVals.reserve(2);
        cString::GetIntVec(asVal, vVals, NULL);

        while(vVals.size() < 2)
        {
            vVals.push_back(0);
        }

        PointerValue(pVal, cVector2l).FromVec(&vVals[0]);

        break;
    }

    /////////// VECTOR 2F ////////////////////////////////
    case eSerializeType_Vector2f:
    {
        tFloatVec vVals;
        vVals.reserve(2);
        cString::GetFloatVec(asVal, vVals, NULL);

        while(vVals.size() < 2)
        {
            vVals.push_back(0.0f);
        }

        PointerValue(pVal, cVector2f).FromVec(&vVals[0]);

        break;
    }

    /////////// VECTOR 3L ////////////////////////////////
    case eSerializeType_Vector3l:
    {
        tIntVec vVals;
        vVals.reserve(3);
        cString::GetIntVec(asVal, vVals, NULL);

        while(vVals.size() < 3)
        {
            vVals.push_back(0);
        }

        PointerValue(pVal, cVector3l).FromVec(&vVals[0]);

        break;
    }

    /////////// VECTOR 3F ////////////////////////////////
    case eSerializeType_Vector3f:
    {
        tFloatVec vVals;
        vVals.reserve(3);
        cString::GetFloatVec(asVal, vVals, NULL);

        while(vVals.size() < 3)
        {
            vVals.push_back(0.0f);
        }

        PointerValue(pVal, cVector3f).FromVec(&vVals[0]);

        break;
    }

    /////////// MATRIX F ////////////////////////////////
    case eSerializeType_Matrixf:
    {
        tFloatVec vVals;
        vVals.reserve(16);
        cString::GetFloatVec(asVal, vVals, NULL);

        while(vVals.size() < 16)
        {
            vVals.push_back(0.0f);
        }

        PointerValue(pVal, cMatrixf).FromVec(&vVals[0]);

        break;
    }

    /////////// COLOR ////////////////////////////////
    case eSerializeType_Color:
    {
        tFloatVec vVals;
        vVals.reserve(4);
        cString::GetFloatVec(asVal, vVals, NULL);

        while(vVals.size() < 4)
        {
            vVals.push_back(0.0f);
        }

        PointerValue(pVal, cColor).FromVec(&vVals[0]);

        break;
    }

    /////////// RECT 2L ////////////////////////////////
    case eSerializeType_Rect2l:
    {
        tIntVec vVals;
        vVals.reserve(4);
        cString::GetIntVec(asVal, vVals, NULL);

        while(vVals.size() < 4)
        {
            vVals.push_back(0);
        }

        PointerValue(pVal, cRect2l).FromVec(&vVals[0]);

        break;
    }

    /////////// RECT 2F ////////////////////////////////
    case eSerializeType_Rect2f:
    {
        tFloatVec vVals;
        vVals.reserve(4);
        cString::GetFloatVec(asVal, vVals, NULL);

        while(vVals.size() < 4)
        {
            vVals.push_back(0.0f);
        }

        PointerValue(pVal, cRect2f).FromVec(&vVals[0]);

        break;
    }

    /////////// PLANE 2F ////////////////////////////////
    case eSerializeType_Planef:
    {
        tFloatVec vVals;
        vVals.reserve(4);
        cString::GetFloatVec(asVal, vVals, NULL);

        while(vVals.size() < 4)
        {
            vVals.push_back(0.0f);
        }

        PointerValue(pVal, cPlanef).FromVec(&vVals[0]);

        break;
    }

    /////////// WSTRING ////////////////////////////////
    case eSerializeType_WString:
    {
        PointerValue(pVal, tWString) = cString::Get16BitFromArray(asVal);
        break;
    }

    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC SERIALIZE CLASS METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cSerializeClass::SaveVariable(pugi::xml_node apElement, cSerializeMemberField *apField,
                                   iSerializable* apData)
{
    //CLASS
    if(apField->mType == eSerializeType_Class)
    {
        void *pClassData = ValuePointer(apData, apField->mlOffset);

        SaveToElement((iSerializable*)pClassData, apField->msName, apElement);
    }
    //CLASS POINTER
    else if(apField->mType == eSerializeType_ClassPointer)
    {
        iSerializable **pFieldData = (iSerializable**)ValuePointer(apData, apField->mlOffset);

        SaveToElement(*pFieldData, apField->msName, apElement, true);
    }
    //NORMAL VAR
    else
    {
        pugi::xml_node pVarElem = apElement.append_child("var");

        Xml::SetAttrInt(pVarElem, "type", (int)(apField->mType));
        Xml::SetAttr(pVarElem, "name", apField->msName.c_str());
        Xml::SetAttr(pVarElem, "val", ValueToString(apData, apField->mlOffset, apField->mType));
    }
}

//-----------------------------------------------------------------------

void cSerializeClass::SaveArray(pugi::xml_node apElement, cSerializeMemberField *apField,
                                iSerializable* apData)
{
    void *pArrayData = ValuePointer(apData, apField->mlOffset);

    pugi::xml_node pArrayElem = apElement.append_child("array");

    Xml::SetAttrInt(pArrayElem, "type", (int)(apField->mType));
    Xml::SetAttr(pArrayElem, "name", apField->msName.c_str());
    Xml::SetAttrInt(pArrayElem, "size", (int)apField->mlArraySize);


    // CLASS ////////////////////////////////////////////
    if(apField->mType == eSerializeType_Class)
    {
        cSerializeSavedClass *pClass = GetClass(((iSerializable*)pArrayData)->Serialize_GetTopClass());
        size_t lClassSize = pClass->mlSize;

        for(size_t i = 0; i < apField->mlArraySize; i++)
        {
            size_t lOffset = lClassSize * i;

            SaveToElement((iSerializable*)ValuePointer(pArrayData, lOffset), "", pArrayElem);
        }
    }
    // CLASS POINTER /////////////////////////////////////
    else if(apField->mType == eSerializeType_ClassPointer)
    {
        iSerializable **pClassDataPtr = (iSerializable **)pArrayData;

        if(*pClassDataPtr == NULL)
        {
            Warning("Array %s is NULL!\n", apField->msName.c_str());
            return;
        }

        Xml::SetAttr(pArrayElem, "class_type", (*pClassDataPtr)->Serialize_GetTopClass().c_str());

        for(size_t i = 0; i < apField->mlArraySize; i++)
        {
            size_t lOffset = sizeof(void*) * i;

            iSerializable **pValue = (iSerializable**)ValuePointer(pArrayData, lOffset);
            SaveToElement(*pValue, "", pArrayElem, true);
        }
    }
    // VARIABLE /////////////////////////////////////////
    else
    {
        for(size_t i = 0; i < apField->mlArraySize; i++)
        {
            size_t lOffset = SizeOfType(apField->mType) * i;

            pugi::xml_node pVarElem = pArrayElem.append_child("var");

            Xml::SetAttr(pVarElem, "val", ValueToString(pArrayData, lOffset, apField->mType));
        }
    }
}

//-----------------------------------------------------------------------

void cSerializeClass::SaveContainer(pugi::xml_node apElement, cSerializeMemberField *apField,
                                    iSerializable* apData)
{
    iContainer* pCont = (iContainer*)ValuePointer(apData, apField->mlOffset);

    iContainerIterator* pContIt = pCont->CreateIteratorPtr();

    pugi::xml_node pArrayElem = apElement.append_child("container");

    Xml::SetAttrInt(pArrayElem, "type", (int)(apField->mType));
    Xml::SetAttr(pArrayElem, "name", apField->msName.c_str());

    // CLASS ////////////////////////////////////////////
    if(apField->mType == eSerializeType_Class)
    {
        Xml::SetAttr(pArrayElem, "class_type", apField->msClassName.c_str());

        while(pContIt->HasNext())
        {
            iSerializable *pData =  (iSerializable*)pContIt->NextPtr();

            SaveToElement(pData, "", pArrayElem);
        }
    }
    // CLASS POINTER /////////////////////////////////////
    else if(apField->mType == eSerializeType_ClassPointer)
    {
        while(pContIt->HasNext())
        {
            iSerializable **pData =  (iSerializable**)pContIt->NextPtr();

            SaveToElement(*pData, "", pArrayElem, true);
        }
    }
    // VARIABLE /////////////////////////////////////////
    else
    {
        while(pContIt->HasNext())
        {
            pugi::xml_node pVarElem = pArrayElem.append_child("var");

            void *pData = const_cast<void *>(pContIt->NextPtr());

            Xml::SetAttr(pVarElem, "val", ValueToString(pData, 0, apField->mType));
        }
    }

    hplDelete(pContIt);
}


//-----------------------------------------------------------------------


void cSerializeClass::LoadVariable(pugi::xml_node apElement, iSerializable* apData,
                                   cSerializeSavedClass *apClass)
{
    tString sName = cString::ToString(Xml::Attr(apElement, "name"), "");
    const char *sVal = Xml::Attr(apElement, "val");

    if(gbLog)
    {
        Log("%s Loading variable: %s val: %s\n", GetTabs(), sName.c_str(), sVal ? sVal : "(null)");
    }

    cSerializeMemberField *pField = GetMemberField(sName, apClass);
    if(pField == NULL)
    {
        return;
    }

    if(pField->mType == eSerializeType_Class || pField->mType == eSerializeType_ClassPointer)
    {
        Warning("LoadVariable called for class/class-pointer field '%s' - ignored\n", sName.c_str());
        return;
    }

    StringToValue(apData, pField->mlOffset, pField->mType, sVal);
}

//-----------------------------------------------------------------------

void cSerializeClass::LoadArray(pugi::xml_node apElement, iSerializable* apData, cSerializeSavedClass *apClass)
{
    tString sName = cString::ToString(Xml::Attr(apElement, "name"), "");
    tString sClassType = cString::ToString(Xml::Attr(apElement, "class_type"), "");

    if(gbLog)
    {
        Log("%s Begin Loading array: '%s' classtype: %s\n", GetTabs(), sName.c_str(), sClassType.c_str());
        ++glTabs;
    }

    cSerializeMemberField *pField = GetMemberField(sName, apClass);
    if(pField == NULL)
    {
        return;
    }

    void *pArrayData = ValuePointer(apData, pField->mlOffset);
    const size_t lArraySize = pField->mlArraySize;

    // CLASS ////////////////////////////////////////////
    if(pField->mType == eSerializeType_Class)
    {
        cSerializeSavedClass *pClass = GetClass(((iSerializable*)pArrayData)->Serialize_GetTopClass());
        if(pClass == NULL)
        {
            return;
        }

        size_t lClassSize = pClass->mlSize;

        size_t lCount = 0;
        pugi::xml_node pVarElem = Xml::FirstElem(apElement);
        for(; pVarElem && lCount < lArraySize; pVarElem = Xml::NextElem(pVarElem), ++lCount)
        {
            size_t lOffset = lCount * lClassSize;

            LoadFromElement( (iSerializable*)ValuePointer(pArrayData, lOffset), pVarElem);
        }
        if(pVarElem)
        {
            Warning("Array '%s' has more elements than declared size %zu - excess ignored\n",
                    sName.c_str(), lArraySize);
        }
    }
    // CLASS POINTER ////////////////////////////////////////////
    else if(pField->mType == eSerializeType_ClassPointer)
    {
        size_t lCount = 0;
        pugi::xml_node pVarElem = Xml::FirstElem(apElement);
        for(; pVarElem && lCount < lArraySize; pVarElem = Xml::NextElem(pVarElem), ++lCount)
        {
            size_t lOffset = sizeof(void*) * lCount;
            iSerializable **pValuePtr = (iSerializable**)ValuePointer(pArrayData, lOffset);

            tString sElemClassType = cString::ToString(Xml::Attr(pVarElem, "type"), "");
            cSerializeSavedClass *pSavedClass = GetClass(sElemClassType);
            if(pSavedClass == NULL || pSavedClass->mpCreateFunc == NULL)
            {
                continue;
            }

            if(gbLog)
            {
                Log("%s Element Class pointer: %s\n", GetTabs(), sElemClassType.c_str());
            }

            //If NULL, then just create else delete and then create-
            //virtual pointers here...yes yes...
            if(*pValuePtr == NULL)
            {
                *pValuePtr = pSavedClass->mpCreateFunc();
            }
            else
            {
                hplDelete(*pValuePtr);
                *pValuePtr = pSavedClass->mpCreateFunc();
            }

            if(*pValuePtr == NULL)
            {
                Warning("mpCreateFunc returned NULL for class '%s'\n", sElemClassType.c_str());
                continue;
            }

            LoadFromElement(*pValuePtr, pVarElem);
        }
        if(pVarElem)
        {
            Warning("Array '%s' has more elements than declared size %zu - excess ignored\n",
                    sName.c_str(), lArraySize);
        }
    }
    // VARIABLE /////////////////////////////////////////
    else
    {
        eSerializeType fieldType = pField->mType;
        size_t lElemSize = SizeOfType(fieldType);
        if(lElemSize == 0)
        {
            Warning("Array '%s' has unknown element type - skipped\n", sName.c_str());
            return;
        }

        size_t lCount = 0;
        pugi::xml_node pVarElem = Xml::FirstElem(apElement);
        for(; pVarElem && lCount < lArraySize; pVarElem = Xml::NextElem(pVarElem), ++lCount)
        {
            const char *sVal = Xml::Attr(pVarElem, "val");

            if(gbLog)
            {
                Log("%s Element variable val '%s'\n", GetTabs(), sVal ? sVal : "(null)");
            }

            StringToValue(pArrayData, lCount * lElemSize, fieldType, sVal);
        }
        if(pVarElem)
        {
            Warning("Array '%s' has more elements than declared size %zu - excess ignored\n",
                    sName.c_str(), lArraySize);
        }
    }

    if(gbLog)
    {
        --glTabs;
        Log("%s End Loading array: %s\n", GetTabs(), sName.c_str());
    }
}

//-----------------------------------------------------------------------

void cSerializeClass::LoadClass(pugi::xml_node apElement, iSerializable* apData, cSerializeSavedClass *apClass)
{
    tString sName = cString::ToString(Xml::Attr(apElement, "name"), "");

    cSerializeMemberField *pField = GetMemberField(sName, apClass);
    if(pField == NULL)
    {
        return;
    }

    iSerializable *pClassData = (iSerializable*)ValuePointer(apData, pField->mlOffset);

    if(gbLog)
    {
        Log("%s Saving class name: '%s' top class: '%s'\n", GetTabs(), sName.c_str(), pClassData->Serialize_GetTopClass().c_str());
    }

    LoadFromElement(pClassData, apElement);
}

//-----------------------------------------------------------------------

void cSerializeClass::LoadClassPointer(pugi::xml_node apElement, iSerializable* apData, cSerializeSavedClass *apClass)
{
    tString sName = cString::ToString(Xml::Attr(apElement, "name"), "");
    tString sType = cString::ToString(Xml::Attr(apElement, "type"), "");
    bool bNull = cString::ToBool(Xml::Attr(apElement, "null"), false);

    cSerializeMemberField *pField = GetMemberField(sName, apClass);
    if(pField == NULL)
    {
        return;
    }

    iSerializable **pClassDataPtr = (iSerializable**)ValuePointer(apData, pField->mlOffset);

    if(gbLog)
    {
        Log("%s Loading classpointer name: '%s' type: '%s' null: %d\n", GetTabs(), sName.c_str(),
            sType.c_str(), bNull ? 1 : 0);
    }

    //TODO: Question is here if previous data should be deleted.
    if(bNull)
    {
        if(*pClassDataPtr)
        {
            hplDelete(*pClassDataPtr);
        }

        *pClassDataPtr = NULL;
        return;
    }

    //If it is NULL create new, else assume it is already created.
    if(*pClassDataPtr == NULL)
    {
        cSerializeSavedClass* pNewClass = GetClass(sType);
        if(pNewClass == NULL || pNewClass->mpCreateFunc == NULL)
        {
            Warning("Cannot create class pointer '%s' of type '%s' - unknown class or no create func\n",
                    sName.c_str(), sType.c_str());
            *pClassDataPtr = NULL;
            return;
        }
        *pClassDataPtr = pNewClass->mpCreateFunc();
        if(*pClassDataPtr == NULL)
        {
            Warning("mpCreateFunc returned NULL for class '%s'\n", sType.c_str());
            return;
        }
    }

    LoadFromElement(*pClassDataPtr, apElement);
}

//-----------------------------------------------------------------------

void cSerializeClass::LoadContainer(pugi::xml_node apElement, iSerializable* apData, cSerializeSavedClass *apClass)
{
    tString sName = cString::ToString(Xml::Attr(apElement, "name"), "");

    cSerializeMemberField *pField = GetMemberField(sName, apClass);
    if(pField == NULL)
    {
        return;
    }

    iContainer *pCont = (iContainer*)ValuePointer(apData, pField->mlOffset);

    if(gbLog)
    {
        Log("%s Begin load container name: '%s' type %d\n", GetTabs(), sName.c_str(), (int)pField->mType);
        ++glTabs;
    }

    // CLASS ////////////////////////////////////////////
    if(pField->mType == eSerializeType_Class)
    {
        tString sClassType = cString::ToString(Xml::Attr(apElement, "class_type"), "");
        pCont->Clear();

        cSerializeSavedClass *pSavedClass = GetClass(sClassType);
        if(pSavedClass == NULL || pSavedClass->mpCreateFunc == NULL)
        {
            Warning("Cannot load container '%s' - unknown class '%s' or no create func\n",
                    sName.c_str(), sClassType.c_str());
            return;
        }

        pugi::xml_node pVarElem = Xml::FirstElem(apElement);
        for(; pVarElem; pVarElem = Xml::NextElem(pVarElem))
        {
            if(gbLog)
            {
                Log("%sCreating element class %s\n", GetTabs(), sClassType.c_str());
            }

            iSerializable *pData = pSavedClass->mpCreateFunc();
            if(pData == NULL)
            {
                Warning("mpCreateFunc returned NULL for class '%s'\n", sClassType.c_str());
                continue;
            }

            LoadFromElement(pData, pVarElem);
            pCont->AddVoidClass(pData);

            hplDelete(pData);
        }
    }
    // CLASS POINTER ////////////////////////////////////////////
    else if(pField->mType == eSerializeType_ClassPointer)
    {
        //This should really be smart pointers.
        //Delete all and clear
        if(gbLog)
        {
            Log("%sClearing container and deleting elements\n", GetTabs());
        }

        iContainerIterator *pContIt = pCont->CreateIteratorPtr();
        while(pContIt->HasNext())
        {
            iSerializable *pContData = (iSerializable*)pContIt->NextPtr();
            hplDelete(pContData);
        }
        hplDelete(pContIt);
        if ( pCont->Size() > 0 )
        {
            pCont->Clear();
        }

        pugi::xml_node pVarElem = Xml::FirstElem(apElement);
        for(; pVarElem; pVarElem = Xml::NextElem(pVarElem))
        {
            tString sClassType = cString::ToString(Xml::Attr(pVarElem, "type"), "");
            cSerializeSavedClass *pSavedClass = GetClass(sClassType);
            if(pSavedClass == NULL || pSavedClass->mpCreateFunc == NULL)
            {
                continue;
            }

            if(gbLog)
            {
                Log("%s Container member class pointer '%s'\n", GetTabs(), sClassType.c_str());
            }

            iSerializable *pData = pSavedClass->mpCreateFunc();
            if(pData == NULL)
            {
                Warning("mpCreateFunc returned NULL for class '%s'\n", sClassType.c_str());
                continue;
            }

            LoadFromElement(pData, pVarElem, true);
            pCont->AddVoidPtr((void**)&pData);
        }
    }
    // VARIABLE /////////////////////////////////////////
    else
    {
        pCont->Clear();

        eSerializeType fieldType = pField->mType;
        size_t lElemSize = SizeOfType(fieldType);
        if(lElemSize == 0)
        {
            Warning("Container '%s' has unknown element type - skipped\n", sName.c_str());
            return;
        }

        if(fieldType == eSerializeType_String || fieldType == eSerializeType_WString)
        {
            Warning("Container '%s' of string type cannot be safely deserialized this way - skipped\n",
                    sName.c_str());
            return;
        }

        pugi::xml_node pVarElem = Xml::FirstElem(apElement);
        for(; pVarElem; pVarElem = Xml::NextElem(pVarElem))
        {
            const char *sVal = Xml::Attr(pVarElem, "val");
            void *pData = hplMalloc(lElemSize);
            if(pData == NULL)
            {
                continue;
            }
            memset(pData, 0, lElemSize);

            if(gbLog)
            {
                Log("%s Element var val '%s' type: %d\n", GetTabs(), sVal ? sVal : "(null)", (int)fieldType);
            }

            StringToValue(pData, 0, fieldType, sVal);
            pCont->AddVoidClass(pData);

            hplFree(pData);
        }
    }

    if(gbLog)
    {
        --glTabs;
        Log("%s End load container name: '%s'\n", GetTabs(), sName.c_str());
    }
}

//-----------------------------------------------------------------------

cSerializeMemberField *cSerializeClass::GetMemberField(const tString &asName, cSerializeSavedClass* apClass)
{
    cSerializeMemberFieldIterator classIt = cSerializeMemberFieldIterator(apClass);

    while(classIt.HasNext())
    {
        cSerializeMemberField* pField = classIt.GetNext();

        if(asName == pField->msName)
        {
            return pField;
        }
    }

    Warning("Couldn't find member field '%s' in class '%s'\n", asName.c_str(), apClass->msName);
    return NULL;
}

//-----------------------------------------------------------------------

size_t cSerializeClass::SizeOfType(eSerializeType aType)
{
    switch(aType)
    {
    case eSerializeType_Bool:
        return sizeof(bool);
    case eSerializeType_Int32:
        return sizeof(int);
    case eSerializeType_Float32:
        return sizeof(float);
    case eSerializeType_String:
        return sizeof(tString);
    case eSerializeType_Vector2l:
        return sizeof(cVector2l);
    case eSerializeType_Vector2f:
        return sizeof(cVector2f);
    case eSerializeType_Vector3l:
        return sizeof(cVector3l);
    case eSerializeType_Vector3f:
        return sizeof(cVector3f);
    case eSerializeType_Matrixf:
        return sizeof(cMatrixf);
    case eSerializeType_Color:
        return sizeof(cColor);
    case eSerializeType_Rect2l:
        return sizeof(cRect2l);
    case eSerializeType_Rect2f:
        return sizeof(cRect2f);
    case eSerializeType_Planef:
        return sizeof(cPlanef);
    case eSerializeType_WString:
        return sizeof(tWString);
    }

    return 0;
}

//-----------------------------------------------------------------------


void cSerializeClass::SetUpData()
{
    if(mbDataSetup)
    {
        return;
    }

    mbDataSetup = true;

    for(int i = 0; i < glSerializeTempClassesNum; i++)
    {
        m_mapSavedClasses.insert(tSerializeSavedClassMap::value_type(
                                     tString(gvSerializeTempClasses[i].msName), gvSerializeTempClasses[i])
                                );
    }
}

//-----------------------------------------------------------------------

void cSerializeClass::FillSaveClassMembersList(tSerializeSavedClassList *apList, cSerializeSavedClass* apClass)
{
    if(apClass == NULL)
    {
        return;
    }
    apList->push_back(apClass);
    FillSaveClassMembersList(apList, GetClass(apClass->msParentName));
}


//-----------------------------------------------------------------------

}
