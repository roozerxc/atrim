#ifndef HPL_XML_DOCUMENT_TINY_H
#define HPL_XML_DOCUMENT_TINY_H

#include "resources/XmlDocument.h"

class TiXmlDocument;
class TiXmlElement;

namespace hpl
{

class cXmlDocumentTiny : public iXmlDocument
{
public:
    cXmlDocumentTiny(const tString &asName) : iXmlDocument(asName) {}

    void SaveToString(tString *apDestData);
    bool CreateFromString(const tString& asData);

private:
    bool LoadDataFromFile(const tWString& asPath);
    bool SaveDataToFile(const tWString& asPath);

    void LoadFromTinyXMLData(TiXmlElement* apTinyElem, cXmlElement *apDestElem);
    void SaveToTinyXMLData(TiXmlElement* apTinyElem, cXmlElement *apSrcElem);

    bool CreateTinyXMLFromFile(TiXmlDocument* pDoc,const tWString& asPath);
    bool SaveTinyXMLToFile(TiXmlDocument* pDoc,const tWString& asPath);
};

};
#endif // HPL_XML_DOCUMENT_TINY_H
