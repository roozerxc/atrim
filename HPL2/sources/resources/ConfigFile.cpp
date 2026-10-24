#include "resources/ConfigFile.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/FileSearcher.h"
#include "impl/XmlPugi.h"

#include "resources/BinaryBuffer.h"
#include "system/Platform.h"

#include <stdio.h>
#include <string.h>

namespace hpl
{

#define kCRCKey (0x16AF2C1D)

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cConfigFile::cConfigFile(const tWString& asFile, const tWString& asAltPath)
{
    if (asAltPath.length() && cPlatform::FileExists(asAltPath + asFile))
    {
        msFile = asAltPath + asFile;
    }
    else
    {
        msFile = asFile;
    }
    mpXmlDoc = hplNew( pugi::xml_document, () );
    //mpFileSearcher = apFileSearcher;

    mbUseCRC = false;
}

cConfigFile::~cConfigFile()
{
    hplDelete(mpXmlDoc);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cConfigFile::Load()
{
    ////////////////////////////////////
    // CRC Loading
    if(mbUseCRC)
    {
        /////////////////////////////////
        // Load
        cBinaryBuffer buff;
        if(buff.Load(msFile) == false)
        {
            Error("Unable to open binary config file '%s'!\n", cString::To8Char(msFile).c_str());
            return false;
        }

        /////////////////////////////////
        // CRC
        if(buff.CheckInternalCRC(kCRCKey) == false)
        {
            Error("CRC check for binary config file '%s' failed!\n", cString::To8Char(msFile).c_str());
            return false;
        }

        /////////////////////////////////
        // Get XML
        const char *pData = buff.GetDataPointerAtCurrentPos();
        if(Xml::LoadFromMemory(*mpXmlDoc, pData, pData ? strlen(pData) : 0) == false)
        {
            Error("Parsing binary config file '%s' failed!\n", cString::To8Char(msFile).c_str());
            return false;
        }

        return true;
    }
    ////////////////////////////////////
    // Normal load
    else
    {
        if(cPlatform::FileExists(msFile) == false)
        {
            Error("Unable to open config file '%s'! Invalid filepointer returned!\n", cString::To8Char(msFile).c_str());
            return false;
        }

        return Xml::LoadFile(*mpXmlDoc, msFile);
    }
}

//-----------------------------------------------------------------------

bool cConfigFile::Save()
{
    ////////////////////////////////////
    // CRC Loading
    if(mbUseCRC)
    {
        /////////////////////////////
        // Get the data
        tString sData;
        Xml::SaveString(*mpXmlDoc, &sData);

        /////////////////////////////
        // Compress the data
        cBinaryBuffer buff;

        buff.AddCRC_Begin();

        buff.AddCharArray(sData.c_str(), sData.size() + 1);

        buff.AddCRC_End(kCRCKey);

        /////////////////////////////
        // Save the file
        if(buff.Save(msFile) == false)
        {
            Error("Unable to save bin config file '%s'!\n", cString::To8Char(msFile).c_str());
            return false;
        }

        return true;
    }
    ////////////////////////////////////
    // Normal load
    else
    {
#ifdef _WIN32
        tWString sFile = cString::ReplaceCharToW(msFile, _W("/"), _W("\\"));
#else
        tWString sFile = msFile;
#endif
        if(Xml::SaveFile(*mpXmlDoc, sFile) == false)
        {
            Error("Unable to save config file '%s'! Invalid filepointer returned!\n", cString::To8Char(msFile).c_str());
            return false;
        }

        return true;
    }

}

//-----------------------------------------------------------------------

void cConfigFile::CopyLevelsFrom(cConfigFile *apSource, const tString& asSkipLevel)
{
    if(apSource == NULL || apSource->mpXmlDoc == NULL)
    {
        return;
    }

    pugi::xml_node srcLevel = Xml::FirstElem(*apSource->mpXmlDoc);
    for(; srcLevel; srcLevel = Xml::NextElem(srcLevel))
    {
        tString sLevel = srcLevel.name();
        if(sLevel == asSkipLevel)
        {
            continue;
        }

        for(pugi::xml_attribute attr = srcLevel.first_attribute(); attr; attr = attr.next_attribute())
        {
            SetString(sLevel, attr.name(), attr.value());
        }
    }
}

//-----------------------------------------------------------------------

void cConfigFile::SetString(const tString& asLevel, const tString& asName, const tString& asVal)
{
    pugi::xml_node levelElem = mpXmlDoc->child(asLevel.c_str());

    if(!levelElem)
    {
        levelElem = mpXmlDoc->append_child(asLevel.c_str());
    }

    Xml::SetAttr(levelElem, asName.c_str(), asVal.c_str());
}
//-----------------------------------------------------------------------

void cConfigFile::SetInt(const tString& asLevel, const tString& asName, int alVal)
{
    char sBuffer[40];
    snprintf(sBuffer, sizeof(sBuffer), "%d", alVal);

    SetString(asLevel, asName, sBuffer);
}

//-----------------------------------------------------------------------

void cConfigFile::SetFloat(const tString& asLevel, const tString& asName, float afVal)
{
    char sBuffer[40];
    snprintf(sBuffer, sizeof(sBuffer), "%f", afVal);

    SetString(asLevel, asName, sBuffer);
}

//-----------------------------------------------------------------------

void cConfigFile::SetBool(const tString& asLevel, const tString& asName, bool abVal)
{
    SetString(asLevel, asName, abVal ? "true" : "false");
}

void cConfigFile::SetVector2f(const tString& asLevel, const tString& asName, const cVector2f& avVal)
{
    tString sVal =    cString::ToString(avVal.x) + " " +
                      cString::ToString(avVal.y);
    SetString(asLevel, asName, sVal.c_str());
}
void cConfigFile::SetVector3f(const tString& asLevel, const tString& asName, const cVector3f& avVal)
{
    tString sVal =    cString::ToString(avVal.x) + " " +
                      cString::ToString(avVal.y) + " " +
                      cString::ToString(avVal.z);
    SetString(asLevel, asName, sVal.c_str());
}
void cConfigFile::SetVector2l(const tString& asLevel, const tString& asName, const cVector2l& avVal)
{
    tString sVal =    cString::ToString(avVal.x) + " " +
                      cString::ToString(avVal.y);
    SetString(asLevel, asName, sVal.c_str());
}
void cConfigFile::SetVector3l(const tString& asLevel, const tString& asName, const cVector3l& avVal)
{
    tString sVal =    cString::ToString(avVal.x) + " " +
                      cString::ToString(avVal.y) + " " +
                      cString::ToString(avVal.z);
    SetString(asLevel, asName, sVal.c_str());
}

//-----------------------------------------------------------------------

tString cConfigFile::GetString(const tString& asLevel, const tString& asName, const tString& asDefault)
{
    const char *sVal = GetCharArray(asLevel, asName);
    if(sVal == NULL)
    {
        return asDefault;
    }

    return sVal;
}

tWString cConfigFile::GetStringW(const tString& asLevel, const tString& asName, const tWString& asDefault)
{
    tString str = GetString(asLevel, asName, cString::To8Char(asDefault));

    return cString::To16Char(str);
}

int cConfigFile::GetInt(const tString& asLevel, const tString& asName, int alDefault)
{
    const char *sVal = GetCharArray(asLevel, asName);
    if(sVal == NULL)
    {
        return alDefault;
    }

    return cString::ToInt(sVal, alDefault);
}

float cConfigFile::GetFloat(const tString& asLevel, const tString& asName, float afDefault)
{
    const char *sVal = GetCharArray(asLevel, asName);
    if(sVal == NULL)
    {
        return afDefault;
    }

    return cString::ToFloat(sVal, afDefault);
}

bool cConfigFile::GetBool(const tString& asLevel, const tString& asName, bool abDefault)
{
    const char *sVal = GetCharArray(asLevel, asName);
    if(sVal == NULL)
    {
        return abDefault;
    }

    return cString::ToBool(sVal, abDefault);
}

//-----------------------------------------------------------------------

cVector2f cConfigFile::GetVector2f(const tString& asLevel, const tString& asName, const cVector2f& avDefault)
{
    return cString::ToVector2f(GetCharArray(asLevel, asName), avDefault);
}

cVector3f cConfigFile::GetVector3f(const tString& asLevel, const tString& asName, const cVector3f& avDefault)
{
    return cString::ToVector3f(GetCharArray(asLevel, asName), avDefault);
}

//-----------------------------------------------------------------------

cVector2l cConfigFile::GetVector2l(const tString& asLevel, const tString& asName, const cVector2l& avDefault)
{
    return cString::ToVector2l(GetCharArray(asLevel, asName), avDefault);
}

cVector3l cConfigFile::GetVector3l(const tString& asLevel, const tString& asName, const cVector3l& avDefault)
{
    return cString::ToVector3l(GetCharArray(asLevel, asName), avDefault);
}

//-----------------------------------------------------------------------


cColor cConfigFile::GetColor(const tString& asLevel, const tString& asName, const cColor& aDefault)
{
    return cString::ToColor(GetCharArray(asLevel, asName), aDefault);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

const char *cConfigFile::GetCharArray(tString asLevel, tString asName)
{
    pugi::xml_node levelElem = mpXmlDoc->child(asLevel.c_str());
    if(!levelElem)
    {
        return NULL;
    }

    return Xml::Attr(levelElem, asName.c_str());
}

//-----------------------------------------------------------------------
}
