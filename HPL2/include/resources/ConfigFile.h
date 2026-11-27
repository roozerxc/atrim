#ifndef HPL_CONFIGFILE_H
#define HPL_CONFIGFILE_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

namespace pugi
{
class xml_document;
}

namespace hpl
{
class cFileSearcher;

class cConfigFile
{
public:
    cConfigFile(const tWString& asFile,  const tWString& asAltPath = _W(""));
    ~cConfigFile();

    /**
     * Loads data from file
     * \return true if loading ws ok, else false
     */
    bool Load();
    /**
     * Saves the data to file
     * \return true if loading ws ok, else false
     */
    bool Save();

    void SetUseCRC(bool abX)
    {
        mbUseCRC = abX;
    }

    void SetFileLocation(const tWString& asFile)
    {
        msFile = asFile;
    }
    const tWString &GetFileLocation()
    {
        return msFile;
    }

    void SetString(const tString& asLevel, const tString& asName, const tString& asVal);
    void SetInt(const tString& asLevel, const tString& asName, int alVal);
    void SetFloat(const tString& asLevel, const tString& asName, float afVal);
    void SetBool(const tString& asLevel, const tString& asName, bool abVal);

    void SetVector2f(const tString& asLevel, const tString& asName, const cVector2f& avVal);
    void SetVector3f(const tString& asLevel, const tString& asName, const cVector3f& avVal);
    void SetVector2l(const tString& asLevel, const tString& asName, const cVector2l& avVal);
    void SetVector3l(const tString& asLevel, const tString& asName, const cVector3l& avVal);


    tString GetString(const tString& asLevel, const tString& asName, const tString& asDefault);
    tWString GetStringW(const tString& asLevel, const tString& asName, const tWString& asDefault);

    int GetInt(const tString& asLevel, const tString& asName, int alDefault);
    float GetFloat(const tString& asLevel, const tString& asName, float afDefault);
    bool GetBool(const tString& asLevel, const tString& asName, bool abDefault);

    cVector2f GetVector2f(const tString& asLevel, const tString& asName, const cVector2f& avDefault);
    cVector3f GetVector3f(const tString& asLevel, const tString& asName, const cVector3f& avDefault);

    cVector2l GetVector2l(const tString& asLevel, const tString& asName, const cVector2l& avDefault);
    cVector3l GetVector3l(const tString& asLevel, const tString& asName, const cVector3l& avDefault);


    cColor GetColor(const tString& asLevel, const tString& asName, const cColor& aDefault);

    /**
     * Copies every attribute of every level (top element) in apSource into this file,
     * overwriting existing values. Levels named asSkipLevel are left out.
     */
    void CopyLevelsFrom(cConfigFile *apSource, const tString& asSkipLevel = "");

    /**
     * Moves the attribute asName from level asFromLevel to level asToLevel (for settings that
     * changed level). If asToLevel already has it, that value is kept. The attribute is removed
     * from asFromLevel either way. Returns true if asFromLevel had the attribute.
     */
    bool MoveAttr(const tString& asFromLevel, const tString& asToLevel, const tString& asName);

    /**
     * Removes the attribute asName from level asLevel (for settings that are no longer used).
     * Returns true if it was there.
     */
    bool RemoveAttr(const tString& asLevel, const tString& asName);

private:
    tWString msFile;
    cFileSearcher *mpFileSearcher;

    pugi::xml_document *mpXmlDoc;

    bool mbUseCRC;

    const char *GetCharArray(tString asLevel, tString asName);
};

};
#endif // HPL_CONFIGFILE_H
