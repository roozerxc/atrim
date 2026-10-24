#ifndef HPL_XML_PUGI_H
#define HPL_XML_PUGI_H

//////////////////////////////////////////////////////////////////////////
// pugixml helpers
//
// atrim 1.4.6: TinyXML 2.6 was replaced by pugixml. These helpers keep the
// TinyXML behaviour the engine and the game content rely on:
// - A missing attribute is NULL (pugixml returns ""), so cString::ToXXX(..., default)
//   still gets its default.
// - "First/next child ELEMENT" skips text nodes.
// - Files are read as raw bytes (no Latin-1 -> UTF-8 conversion), like TinyXML,
//   so file names and language text keep their exact bytes.
// - Whitespace in attributes is kept as it is (no tab/newline to space conversion).
// - Text can be condensed like TinyXML did (language files depend on it).
// - Output uses 4 space indentation like TinyXML.
//////////////////////////////////////////////////////////////////////////

#include "system/SystemTypes.h"
#include "impl/pugixml/pugixml.hpp"

namespace hpl
{

namespace Xml
{
    //Parse options used everywhere: CDATA, entities, end of line normalisation.
    //Not used: parse_wconv_attribute (TinyXML kept tabs/newlines in attributes), comments, PIs.
    const unsigned int kParseFlags = pugi::parse_cdata | pugi::parse_escapes | pugi::parse_eol;

    //////////////////////////////
    // Reading

    //Attribute value, or NULL if the attribute does not exist (like TiXmlElement::Attribute)
    inline const char* Attr(const pugi::xml_node& aNode, const char* asName)
    {
        pugi::xml_attribute attr = aNode.attribute(asName);
        return attr ? attr.value() : NULL;
    }

    //Attribute value, or "" if the attribute does not exist (for direct assignment to a tString)
    inline const char* AttrOrEmpty(const pugi::xml_node& aNode, const char* asName)
    {
        return aNode.attribute(asName).value();
    }

    //First child element (any name), skipping text
    inline pugi::xml_node FirstElem(const pugi::xml_node& aNode)
    {
        for(pugi::xml_node child = aNode.first_child(); child; child = child.next_sibling())
        {
            if(child.type() == pugi::node_element)
            {
                return child;
            }
        }
        return pugi::xml_node();
    }

    //First child element with name
    inline pugi::xml_node FirstElem(const pugi::xml_node& aNode, const char* asName)
    {
        return aNode.child(asName);
    }

    //Next sibling element (any name), skipping text
    inline pugi::xml_node NextElem(const pugi::xml_node& aNode)
    {
        for(pugi::xml_node sibling = aNode.next_sibling(); sibling; sibling = sibling.next_sibling())
        {
            if(sibling.type() == pugi::node_element)
            {
                return sibling;
            }
        }
        return pugi::xml_node();
    }

    //Next sibling element with name
    inline pugi::xml_node NextElem(const pugi::xml_node& aNode, const char* asName)
    {
        return aNode.next_sibling(asName);
    }

    //Value of the first child if it is text or CDATA (like FirstChild()->ToText()->Value()), else NULL
    inline const char* Text(const pugi::xml_node& aNode)
    {
        pugi::xml_node child = aNode.first_child();
        if(child && (child.type() == pugi::node_pcdata || child.type() == pugi::node_cdata))
        {
            return child.value();
        }
        return NULL;
    }

    //Same as Text(), but "" instead of NULL
    inline const char* TextOrEmpty(const pugi::xml_node& aNode)
    {
        const char* pText = Text(aNode);
        return pText ? pText : "";
    }

    //TinyXML (condenseWhiteSpace = true): leading/trailing white space removed,
    //every run of white space inside becomes a single space.
    tString CondenseWhiteSpace(const char* asText);

    //Text like TinyXML returned it: condensed (CDATA kept as it is), "" if there is no text.
    //Use this for names and file paths; Text() is enough for number arrays.
    inline tString TextCondensed(const pugi::xml_node& aNode)
    {
        pugi::xml_node child = aNode.first_child();
        if(child && child.type() == pugi::node_cdata)
        {
            return child.value();
        }
        return CondenseWhiteSpace(Text(aNode));
    }

    //////////////////////////////
    // Loading

    //Loads a file. On failure the document holds what could be parsed (like TinyXML) and
    //apErrorDesc/apErrorRow/apErrorCol are set if not NULL.
    bool LoadFile(pugi::xml_document& aDoc, const tWString& asPath,
                  tString* apErrorDesc = NULL, int* apErrorRow = NULL, int* apErrorCol = NULL);

    bool LoadFromMemory(pugi::xml_document& aDoc, const char* apData, size_t alSize,
                    tString* apErrorDesc = NULL, int* apErrorRow = NULL, int* apErrorCol = NULL);

    //////////////////////////////
    // Writing

    //Adds a child element at the end
    inline pugi::xml_node AddElem(pugi::xml_node aParent, const char* asName)
    {
        return aParent.append_child(asName);
    }

    //Sets (or adds) an attribute
    inline void SetAttr(pugi::xml_node aNode, const char* asName, const char* asValue)
    {
        pugi::xml_attribute attr = aNode.attribute(asName);
        if(!attr)
        {
            attr = aNode.append_attribute(asName);
        }
        attr.set_value(asValue ? asValue : "");
    }

    void SetAttrInt(pugi::xml_node aNode, const char* asName, int alValue);

    //Same overloads as TiXmlElement::SetAttribute (an int is written with "%d")
    inline void SetAttr(pugi::xml_node aNode, const char* asName, const tString& asValue)
    {
        SetAttr(aNode, asName, asValue.c_str());
    }
    inline void SetAttr(pugi::xml_node aNode, const char* asName, int alValue)
    {
        SetAttrInt(aNode, asName, alValue);
    }

    bool SaveFile(const pugi::xml_document& aDoc, const tWString& asPath);
    void SaveString(const pugi::xml_document& aDoc, tString* apDest);
}

};
#endif // HPL_XML_PUGI_H
