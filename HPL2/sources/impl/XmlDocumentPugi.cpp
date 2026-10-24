#include "impl/XmlDocumentPugi.h"

#include "system/LowLevelSystem.h"
#include "system/Platform.h"
#include "system/String.h"

#include "impl/XmlPugi.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cXmlDocumentPugi::SaveToString(tString *apDestData)
{
    pugi::xml_document xmlDoc;
    pugi::xml_node rootElem = xmlDoc.append_child(GetValue().c_str());
    SaveToPugiData(rootElem, this);

    Xml::SaveString(xmlDoc, apDestData);
}

//-----------------------------------------------------------------------

bool cXmlDocumentPugi::CreateFromString(const tString& asData)
{
    pugi::xml_document xmlDoc;

    tString sErrorDesc;
    int lErrorRow = 0, lErrorCol = 0;
    if(Xml::LoadFromMemory(xmlDoc, asData.c_str(), asData.size(), &sErrorDesc, &lErrorRow, &lErrorCol) == false)
    {
        SaveErrorInfo(sErrorDesc, lErrorRow, lErrorCol);
        return false;
    }

    DestroyChildren();

    pugi::xml_node rootElem = Xml::FirstElem(xmlDoc);
    if(rootElem)
    {
        LoadFromPugiData(rootElem, this);
    }

    return true;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cXmlDocumentPugi::LoadDataFromFile(const tWString& asPath)
{
    //The pugixml document only lives during the copy into the cXmlElement tree,
    //and is parsed in place (no second copy of the file in memory).
    pugi::xml_document xmlDoc;

    tString sErrorDesc;
    int lErrorRow = 0, lErrorCol = 0;
    if(Xml::LoadFile(xmlDoc, asPath, &sErrorDesc, &lErrorRow, &lErrorCol) == false)
    {
        SaveErrorInfo(sErrorDesc, lErrorRow, lErrorCol);
        return false;
    }

    DestroyChildren();

    pugi::xml_node rootElem = Xml::FirstElem(xmlDoc);
    if(rootElem)
    {
        LoadFromPugiData(rootElem, this);
    }

    return true;
}

//-----------------------------------------------------------------------

bool cXmlDocumentPugi::SaveDataToFile(const tWString& asPath)
{
    pugi::xml_document xmlDoc;
    pugi::xml_node rootElem = xmlDoc.append_child(GetValue().c_str());
    SaveToPugiData(rootElem, this);

    return Xml::SaveFile(xmlDoc, asPath);
}

//-----------------------------------------------------------------------

void cXmlDocumentPugi::LoadFromPugiData(const pugi::xml_node& aSrcElem, cXmlElement *apDestElem)
{
    /////////////////////////////
    //Load the attributes
    apDestElem->SetValue(aSrcElem.name());

    for(pugi::xml_attribute attr = aSrcElem.first_attribute(); attr; attr = attr.next_attribute())
    {
        apDestElem->SetAttribute(attr.name(), attr.value());
    }

    /////////////////////////////
    //Load the elements
    for(pugi::xml_node childElem = Xml::FirstElem(aSrcElem); childElem; childElem = Xml::NextElem(childElem))
    {
        cXmlElement *pDestChild = apDestElem->CreateChildElement();

        LoadFromPugiData(childElem, pDestChild);
    }
}

//-----------------------------------------------------------------------

void cXmlDocumentPugi::SaveToPugiData(pugi::xml_node& aDestElem, cXmlElement *apSrcElem)
{
    /////////////////////////////
    //Save the attributes
    aDestElem.set_name(apSrcElem->GetValue().c_str());

    tAttributeMap *pAttributeMap = apSrcElem->GetAttributeMap();
    tAttributeMapIt attrIt = pAttributeMap->begin();
    for(; attrIt != pAttributeMap->end(); ++attrIt)
    {
        aDestElem.append_attribute(attrIt->first.c_str()).set_value(attrIt->second.c_str());
    }

    /////////////////////////////
    //Save the elements
    cXmlNodeListIterator it = apSrcElem->GetChildIterator();
    while(it.HasNext())
    {
        cXmlElement *pChild = it.Next()->ToElement();

        pugi::xml_node childElem = aDestElem.append_child(pChild->GetValue().c_str());

        SaveToPugiData(childElem, pChild);
    }
}

//-----------------------------------------------------------------------
}
