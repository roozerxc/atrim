#include "system/Platform.h"
#include "system/LowLevelSystem.h"

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

}