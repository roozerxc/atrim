#ifndef HPL_XML_DOCUMENT_PUGI_H
#define HPL_XML_DOCUMENT_PUGI_H

#include "resources/XmlDocument.h"

namespace pugi
{
class xml_node;
}

namespace hpl
{

//cXmlDocument backend using pugixml (was TinyXML, cXmlDocumentTiny).
class cXmlDocumentPugi : public iXmlDocument
{
public:
    cXmlDocumentPugi(const tString &asName) : iXmlDocument(asName) {}

    void SaveToString(tString *apDestData);
    bool CreateFromString(const tString& asData);

private:
    bool LoadDataFromFile(const tWString& asPath);
    bool SaveDataToFile(const tWString& asPath);

    void LoadFromPugiData(const pugi::xml_node& aSrcElem, cXmlElement *apDestElem);
    void SaveToPugiData(pugi::xml_node& aDestElem, cXmlElement *apSrcElem);
};

};
#endif // HPL_XML_DOCUMENT_PUGI_H
