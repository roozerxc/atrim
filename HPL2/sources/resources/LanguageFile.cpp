#include "resources/LanguageFile.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/FileSearcher.h"
#include "impl/XmlPugi.h"

#include "resources/Resources.h"
#include "resources/FileSearcher.h"
#include "system/Platform.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLanguageFile::cLanguageFile(cResources *apResources)
{
    mpResources = apResources;
}

//-----------------------------------------------------------------------

cLanguageFile::~cLanguageFile()
{
    STLMapDeleteAll(m_mapCategories);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLanguageFile::AddFromFile(const tWString& asFile, bool abAddResourceDirs, const tWString& asAltPath)
{
    const bool bLog = false;
    pugi::xml_document xmlDoc;

    tWString sPath = asFile;
    if (asAltPath.length() > 0 && cPlatform::FileExists(asAltPath + asFile))
    {
        sPath = asAltPath + asFile;
    }

    if (Xml::LoadFile(xmlDoc, sPath) == false)
    {
        Error("Couldn't load language file '%ls'\n", asFile.c_str());
        return false;
    }

    if(bLog)
    {
        Log("Loading lang file '%ls'\n---------------------\n", asFile.c_str());
    }

    pugi::xml_node rootElem = Xml::FirstElem(xmlDoc);
    if(!rootElem)
    {
        Error("Couldn't load language file '%ls'\n", asFile.c_str());
        return false;
    }

    ///////////////////////////
    //Iterate the resources
    if(abAddResourceDirs)
    {
        pugi::xml_node resourceElem = rootElem.child("RESOURCES");
        if(resourceElem)
        {
            pugi::xml_node dirElem = resourceElem.child("Directory");
            for(; dirElem; dirElem = dirElem.next_sibling("Directory"))
            {
                const char *pPathAttr = Xml::Attr(dirElem, "Path");
                tString sPath = pPathAttr ? pPathAttr : "";
                if(sPath == "")
                {
                    continue;
                }

                bool bAddSubDirs = cString::ToBool(Xml::Attr(dirElem, "AddSubDirs"), true);

                if(sPath[0] == '/' || sPath[0] == '\\')
                {
                    sPath = cString::Sub(sPath, 1);
                }
#ifndef HPL_MINIMAL
                //Log("Adding lang path: '%s' %d\n", sPath.c_str(), bAddSubDirs);
                if (asAltPath.length())
                {
                    mpResources->AddResourceDir(asAltPath + cString::To16Char(sPath), bAddSubDirs);
                }
                mpResources->AddResourceDir(cString::To16Char(sPath), bAddSubDirs);
#endif
            }
        }
        else
        {
            Warning("No resources element found in '%ls'\n", asFile.c_str());
        }
    }


    ///////////////////////////
    //Iterate the categories
    pugi::xml_node catElem = rootElem.child("CATEGORY");
    for(; catElem; catElem = catElem.next_sibling("CATEGORY"))
    {
        tString sCatName = catElem.attribute("Name").value();

        /////////////////////////////////
        //Get the category or create if it does not exist
        cLanguageCategory *pCategory;
        tLanguageCategoryMapIt CatIt = m_mapCategories.find(sCatName);
        if(CatIt == m_mapCategories.end())
        {
            pCategory = hplNew( cLanguageCategory, () );
            m_mapCategories.insert(tLanguageCategoryMap::value_type(sCatName, pCategory));

            if(bLog)
            {
                Log("Creating category '%s'\n", sCatName.c_str());
            }
        }
        else
        {
            pCategory = CatIt->second;

            if(bLog)
            {
                Log("Got existing category '%s'\n", sCatName.c_str());
            }
        }


        ///////////////////////////
        //Iterate the entries
        pugi::xml_node entryElem = catElem.child("Entry");
        for(; entryElem; entryElem = entryElem.next_sibling("Entry"))
        {
            tString sEntryName = entryElem.attribute("Name").value();

            ///////////////////////////////////
            //Check if the entry already exists, if so, just continue.
            if(pCategory->m_mapEntries.find(sEntryName) != pCategory->m_mapEntries.end())
            {
                if(bLog)
                {
                    Log("Entry '%s' already exist!\n", sEntryName.c_str());
                }

                continue;
            }

            ////////////////////////
            //Create entry
            cLanguageEntry *pEntry = hplNew( cLanguageEntry, () );

            if(bLog)
            {
                Log("Creating Entry '%s'\n", sEntryName.c_str());
            }


            ////////////////////////
            //Add Text
            if(entryElem.first_child())
            {
                const char *pText = Xml::Text(entryElem);
                if(pText)
                {
                    //pEntry->msText = pTextNode->Value();
                    //pEntry->msText = cString::ReplaceStringTo(pEntry->msText,"[br]","\n");

                    //TinyXML condensed white space in text (line breaks and indentation in the
                    //lang file become single spaces). Keep doing that, line breaks are [br].
                    //(CDATA was kept as it is.)
                    tString sString = entryElem.first_child().type() == pugi::node_cdata ? tString(pText) : Xml::CondenseWhiteSpace(pText);
                    pEntry->mwsText = _W("");

                    //if(sCatName == "TEST") Log("String: '%s' %d\n",sString.c_str(),sString.size());

                    for(size_t i = 0; i < sString.length(); ++i)
                    {
                        unsigned char c = sString[i];
                        if(c == '[')
                        {
                            bool bFoundCommand = true;
                            tString sCommand = "";
                            int lCount = 1;

                            //Check the length BEFORE reading the char (could read past the end of the string).
                            while(i + lCount < sString.length() && sString[i + lCount] != ']' && lCount < 16)
                            {
                                sCommand += sString[i + lCount];
                                lCount++;
                            }

                            if(sCommand == "br")
                            {
                                pEntry->mwsText += _W('\n');
                            }
                            else if(sCommand.empty() == false && sCommand[0] == 'u')
                            {
                                int lNum = cString::ToInt(sCommand.substr(1).c_str(), 0);
                                pEntry->mwsText += (wchar_t)lNum;
                            }
                            else
                            {
                                bFoundCommand = false;
                            }

                            //Go forward or add [ to string
                            if(bFoundCommand)
                            {
                                i += lCount;
                            }
                            else
                            {
                                pEntry->mwsText += sString[i];
                            }
                        }
                        //Decode UTF-8!
                        //Only 2 byte sequences were handled before, so 3 byte chars
                        //(like the German quotes U+201E U+201C or U+2019) ate the next letter.
                        else if(c >= 128)
                        {
                            int lExtraBytes = 0;
                            int lNum = 0;
                            if((c & 0xE0) == 0xC0)
                            {
                                lExtraBytes = 1;    // 110xxxxx
                                lNum = c & 0x1f;
                            }
                            else if((c & 0xF0) == 0xE0)
                            {
                                lExtraBytes = 2;    // 1110xxxx
                                lNum = c & 0x0f;
                            }
                            else if((c & 0xF8) == 0xF0)
                            {
                                lExtraBytes = 3;    // 11110xxx
                                lNum = c & 0x07;
                            }

                            //Make sure the continuation bytes are really there
                            bool bValid = lExtraBytes > 0 && i + lExtraBytes < sString.length();
                            for(int j = 1; bValid && j <= lExtraBytes; ++j)
                            {
                                unsigned char cNext = sString[i + j];
                                if((cNext & 0xC0) != 0x80)
                                {
                                    bValid = false;
                                }
                                else
                                {
                                    lNum = (lNum << 6) | (cNext & 0x3f);
                                }
                            }

                            if(bValid)
                            {
                                //wchar_t is 16 bit on Windows, chars outside the BMP can not be shown.
                                if(lNum > 0xFFFF)
                                {
                                    lNum = '?';
                                }
                                pEntry->mwsText += (wchar_t)lNum;
                                i += lExtraBytes;
                            }
                            else
                            {
                                //Not valid UTF-8, treat it as a Latin-1 char (old behaviour for stray bytes).
                                pEntry->mwsText += (wchar_t)c;
                            }
                        }
                        else
                        {
                            //if(sCatName == "TEST") Log(" %d: %c | %d\n",i,c,c);
                            pEntry->mwsText += c;
                            //if(sCatName == "TEST") Log(" '%s'\n",cString::To8Char(pEntry->mwsText).c_str());
                        }
                    }

                }
            }
            //////////
            // No text found, add no text.
            else
            {
                pEntry->mwsText = _W("");
            }

            //if(sE == "Motion blur:") Log("After String: '%s'\n",cString::To8Char(pEntry->mwsText).c_str());

            std::pair<tLanguageEntryMap::iterator, bool> ret = pCategory->m_mapEntries.insert(tLanguageEntryMap::value_type(sEntryName, pEntry));
            if(ret.second == false)
            {
                Warning("Language entry '%s' in category '%s' already exists!\n", sEntryName.c_str(), sCatName.c_str());
                hplDelete(pEntry);
            }
        }
    }

    /*{
        FILE *pFile = fopen("raw_lang_text.txt","w+");

        tLanguageCategoryMapIt CatIt = m_mapCategories.begin();
        for(; CatIt != m_mapCategories.end(); ++CatIt)
        {
            cLanguageCategory *pCategory = CatIt->second;
            tLanguageEntryMapIt EntryIt = pCategory->m_mapEntries.begin();
            for(; EntryIt != pCategory->m_mapEntries.end(); ++EntryIt)
            {
                cLanguageEntry *pEntry = EntryIt->second;
                fputs(cString::To8Char(pEntry->mwsText + _W("\n\n")).c_str(), pFile);
            }
        }

        fclose(pFile);
    }*/


    return true;
}

//-----------------------------------------------------------------------

const tWString &cLanguageFile::Translate(const tString& asCat, const tString& asName)
{
    tLanguageCategoryMapIt CatIt = m_mapCategories.find(asCat);
    if(CatIt == m_mapCategories.end())
    {
        Warning("Could not find language file category '%s'\n", asCat.c_str());
        return mwsEmpty;
    }

    cLanguageCategory *pCategory = CatIt->second;
    tLanguageEntryMapIt EntryIt = pCategory->m_mapEntries.find(asName);
    if(EntryIt == pCategory->m_mapEntries.end())
    {
        Warning("Could not find language file entry '%s'\n", asName.c_str());
        return mwsEmpty;
    }

    cLanguageEntry *pEntry = EntryIt->second;

    return pEntry->mwsText;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
}
