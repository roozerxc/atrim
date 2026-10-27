#include "impl/XmlPugi.h"

#include "system/Platform.h"
#include "system/LowLevelSystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace hpl
{

namespace Xml
{

//-----------------------------------------------------------------------

static bool IsWhiteSpace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

tString CondenseWhiteSpace(const char* asText)
{
    tString sOut;
    if(asText == NULL)
    {
        return sOut;
    }

    //Skip leading
    const char* p = asText;
    while(*p && IsWhiteSpace(*p))
    {
        ++p;
    }

    bool bPendingSpace = false;
    for(; *p; ++p)
    {
        if(IsWhiteSpace(*p))
        {
            bPendingSpace = true;
        }
        else
        {
            if(bPendingSpace)
            {
                sOut += ' ';
                bPendingSpace = false;
            }
            sOut += *p;
        }
    }

    return sOut;
}

//-----------------------------------------------------------------------

static pugi::xml_encoding GetEncoding(const char* apData, size_t alSize)
{
    //UTF-16/32 with BOM: let pugixml convert (TinyXML could not read these at all).
    //Everything else is read as raw bytes, like TinyXML did. A Latin-1 declaration
    //would otherwise make pugixml convert the text to UTF-8, which changes file names.
    if(alSize >= 2)
    {
        unsigned char c0 = (unsigned char)apData[0];
        unsigned char c1 = (unsigned char)apData[1];
        if((c0 == 0xFF && c1 == 0xFE) || (c0 == 0xFE && c1 == 0xFF))
        {
            return pugi::encoding_auto;
        }
        if(alSize >= 4 && c0 == 0 && c1 == 0 && (unsigned char)apData[2] == 0xFE && (unsigned char)apData[3] == 0xFF)
        {
            return pugi::encoding_auto;
        }
    }
    return pugi::encoding_utf8;
}

//-----------------------------------------------------------------------

static void SetError(const pugi::xml_parse_result& aResult, const char* apData, size_t alSize,
                     tString* apErrorDesc, int* apErrorRow, int* apErrorCol)
{
    if(apErrorDesc)
    {
        *apErrorDesc = aResult.description();
    }

    //Row and column from the offset (1 based, like TinyXML)
    int lRow = 1;
    int lCol = 1;
    if(apData)
    {
        size_t lEnd = aResult.offset > 0 ? (size_t)aResult.offset : 0;
        if(lEnd > alSize)
        {
            lEnd = alSize;
        }
        for(size_t i = 0; i < lEnd; ++i)
        {
            if(apData[i] == '\n')
            {
                ++lRow;
                lCol = 1;
            }
            else
            {
                ++lCol;
            }
        }
    }

    if(apErrorRow)
    {
        *apErrorRow = lRow;
    }
    if(apErrorCol)
    {
        *apErrorCol = lCol;
    }
}

//-----------------------------------------------------------------------

bool LoadFile(pugi::xml_document& aDoc, const tWString& asPath, tString* apErrorDesc, int* apErrorRow, int* apErrorCol)
{
    aDoc.reset();

    FILE* pFile = cPlatform::OpenFile(asPath, _W("rb"));
    if(pFile == NULL)
    {
        if(apErrorDesc)
        {
            *apErrorDesc = "Failed to open file";
        }
        if(apErrorRow)
        {
            *apErrorRow = 0;
        }
        if(apErrorCol)
        {
            *apErrorCol = 0;
        }
        return false;
    }

    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    if(lSize <= 0)
    {
        fclose(pFile);
        if(apErrorDesc)
        {
            *apErrorDesc = "Error document empty.";
        }
        if(apErrorRow)
        {
            *apErrorRow = 0;
        }
        if(apErrorCol)
        {
            *apErrorCol = 0;
        }
        return false;
    }

    //Read straight into a pugixml owned buffer and parse in place: no copy of the file is kept.
    char* pBuffer = static_cast<char*>(pugi::get_memory_allocation_function()((size_t)lSize));
    if(pBuffer == NULL)
    {
        fclose(pFile);
        if(apErrorDesc)
        {
            *apErrorDesc = "Out of memory";
        }
        return false;
    }

    size_t lRead = fread(pBuffer, 1, (size_t)lSize, pFile);
    fclose(pFile);

    if(lRead == 0)
    {
        pugi::get_memory_deallocation_function()(pBuffer);
        if(apErrorDesc)
        {
            *apErrorDesc = "Failed to read file";
        }
        return false;
    }

    pugi::xml_encoding encoding = GetEncoding(pBuffer, lRead);

    //Row/column for errors have to be counted before the buffer is parsed in place.
    //Only done on error, by parsing a second time from a copy (rare, so no cost normally).
    pugi::xml_parse_result result = aDoc.load_buffer_inplace_own(pBuffer, lRead, kParseFlags, encoding);
    if(result)
    {
        return true;
    }

    if(apErrorDesc || apErrorRow || apErrorCol)
    {
        //Re-read the raw data to get row and column
        FILE* pFile2 = cPlatform::OpenFile(asPath, _W("rb"));
        if(pFile2)
        {
            char* pCopy = static_cast<char*>(malloc(lRead));
            if(pCopy)
            {
                size_t lRead2 = fread(pCopy, 1, lRead, pFile2);
                SetError(result, pCopy, lRead2, apErrorDesc, apErrorRow, apErrorCol);
                free(pCopy);
            }
            else
            {
                SetError(result, NULL, 0, apErrorDesc, apErrorRow, apErrorCol);
            }
            fclose(pFile2);
        }
        else
        {
            SetError(result, NULL, 0, apErrorDesc, apErrorRow, apErrorCol);
        }
    }

    return false;
}

//-----------------------------------------------------------------------

bool LoadFromMemory(pugi::xml_document& aDoc, const char* apData, size_t alSize, tString* apErrorDesc, int* apErrorRow, int* apErrorCol)
{
    aDoc.reset();

    if(apData == NULL || alSize == 0)
    {
        if(apErrorDesc)
        {
            *apErrorDesc = "Error document empty.";
        }
        if(apErrorRow)
        {
            *apErrorRow = 0;
        }
        if(apErrorCol)
        {
            *apErrorCol = 0;
        }
        return false;
    }

    pugi::xml_parse_result result = aDoc.load_buffer(apData, alSize, kParseFlags, GetEncoding(apData, alSize));
    if(result)
    {
        return true;
    }

    SetError(result, apData, alSize, apErrorDesc, apErrorRow, apErrorCol);
    return false;
}

//-----------------------------------------------------------------------

void SetAttrInt(pugi::xml_node aNode, const char* asName, int alValue)
{
    char sBuffer[32];
    sprintf(sBuffer, "%d", alValue);
    SetAttr(aNode, asName, sBuffer);
}

//-----------------------------------------------------------------------

namespace
{
    class cFileWriter : public pugi::xml_writer
    {
    public:
        cFileWriter(FILE* apFile) : mpFile(apFile), mbOk(true) {}

        virtual void write(const void* data, size_t size)
        {
            if(size > 0 && fwrite(data, 1, size, mpFile) != size)
            {
                mbOk = false;
            }
        }

        FILE* mpFile;
        bool mbOk;
    };

    class cStringWriter : public pugi::xml_writer
    {
    public:
        cStringWriter(tString* apDest) : mpDest(apDest) {}

        virtual void write(const void* data, size_t size)
        {
            mpDest->append(static_cast<const char*>(data), size);
        }

        tString* mpDest;
    };

    //4 space indentation and no <?xml?> declaration, like TinyXML wrote it.
    const unsigned int kSaveFlags = pugi::format_indent | pugi::format_no_declaration;
    const char* const kIndent = "    ";
}

bool SaveFile(const pugi::xml_document& aDoc, const tWString& asPath)
{
    if(asPath == _W(""))
    {
        return false;
    }

    //Text mode like before (TinyXML used "w+" too)
    FILE* pFile = cPlatform::OpenFile(asPath, _W("w+"));
    if(pFile == NULL)
    {
        return false;
    }

    cFileWriter writer(pFile);
    aDoc.save(writer, kIndent, kSaveFlags, pugi::encoding_utf8);

    bool bOk = writer.mbOk;
    if(fclose(pFile) != 0)
    {
        bOk = false;
    }

    return bOk;
}

//-----------------------------------------------------------------------

void SaveString(const pugi::xml_document& aDoc, tString* apDest)
{
    apDest->clear();
    cStringWriter writer(apDest);
    aDoc.save(writer, kIndent, kSaveFlags, pugi::encoding_utf8);
}

//-----------------------------------------------------------------------

}

}
