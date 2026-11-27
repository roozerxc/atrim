#include "system/Platform.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"

//---------------------------------------------------------------

namespace hpl
{

//---------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// Predicate for sorting video mode lists

bool VideoComp::operator() (const cVideoMode& aVM1, const cVideoMode& aVM2) const
{
    if(aVM1.mvScreenSize.x != aVM2.mvScreenSize.x)
    {
        return aVM1.mvScreenSize.x < aVM2.mvScreenSize.x;
    }
    if(aVM1.mvScreenSize.y != aVM2.mvScreenSize.y)
    {
        return aVM1.mvScreenSize.y < aVM2.mvScreenSize.y;
    }
    return false;
}

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

void cPlatform::CreateMessageBox( eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, ...)
{
    va_list ap;

    if (fmt == NULL)
    {
        return;
    }
    va_start(ap, fmt);
    CreateMessageBoxBase(eType, asCaption, fmt, ap);
    va_end(ap);
}

void cPlatform::CreateMessageBox( const wchar_t* asCaption, const wchar_t *fmt, ...)
{
    va_list ap;
    if (fmt == NULL)
    {
        return;
    }
    va_start(ap, fmt);
    CreateMessageBoxBase( eMsgBoxType_Default, asCaption, fmt, ap );
    va_end(ap);
}

void cPlatform::CreateMessageBox( const wchar_t* asCaption, const wchar_t *fmt, va_list ap)
{
    CreateMessageBoxBase(eMsgBoxType_Default, asCaption, fmt, ap);
}

void cPlatform::CreateMessageBox( eMsgBoxType eType, const wchar_t* asCaption, const wchar_t *fmt, va_list ap)
{
    CreateMessageBoxBase(eType, asCaption, fmt, ap);
}

//---------------------------------------------------------------

tWString cPlatform::GetScreenshotFileName(const tWString& asDir, const tWString& asExt)
{
    if(asDir.empty() == false && FolderExists(asDir) == false)
    {
        CreateFolder(asDir);
    }

    //cDate::month is tm_mon, which is 0-11
    cDate date = GetDate();

    tWString sBaseName = asDir.empty() ? _W("") : cString::AddSlashAtEndW(asDir);
    sBaseName += _W("Screenshot_");
    sBaseName += cString::ToStringW(date.year, 4);
    sBaseName += cString::ToStringW(date.month + 1, 2);
    sBaseName += cString::ToStringW(date.month_day, 2);
    sBaseName += _W("_");
    sBaseName += cString::ToStringW(date.hours, 2);
    sBaseName += cString::ToStringW(date.minutes, 2);
    sBaseName += cString::ToStringW(date.seconds, 2);

    tWString sExt = _W(".") + asExt;

    tWString sFileName = sBaseName + sExt;
    for(int lCount = 2; FileExists(sFileName); ++lCount)
    {
        sFileName = sBaseName + _W("_") + cString::ToStringW(lCount) + sExt;
    }

    return sFileName;
}

//---------------------------------------------------------------

}