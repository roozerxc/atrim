#ifndef HPL_WIDGET_MULTI_PROPERTY_LIST_BOX_H
#define HPL_WIDGET_MULTI_PROPERTY_LIST_BOX_H

#include "gui/WidgetListBoxBase.h"

namespace hpl
{

class cWidgetLabel;

//-------------------------------------------------------------------

class cSubList
{
public:
    cSubList(const tString& asName, const int alIndex, eFontAlign aAlign) : msName(asName), mlIndex(alIndex), mAlign(aAlign) {}

    tString msName;
    int mlIndex;
    eFontAlign mAlign;
};

//-------------------------------------------------------------------

typedef std::vector<cSubList> tSubListVec;
typedef tSubListVec::iterator tSubListVecIt;

typedef std::vector<cWidgetLabel*> tLabelVec;
typedef tLabelVec::iterator tLabelVecIt;

//-------------------------------------------------------------------

class cWidgetMultiPropertyListBox : public iWidgetListBoxBase
{
public:
    cWidgetMultiPropertyListBox(cGuiSet* apSet, cGuiSkin* apSkin);
    virtual ~cWidgetMultiPropertyListBox();

    void AddColumn(const tString& asName, const int alIndex, eFontAlign aAlign=eFontAlign_Left );

    void SetColumnWidth(const int alIdx, float afWidth);

    cVector3f GetItemStartPos();

protected:
    ////////////////////////
    // Own functions
    void UpdateColumns();

    ////////////////////////
    // Implemented functions
    void DrawItems(float afTimeStep, cGuiClipRegion* apClipRegion);

    void OnLoadGraphics();

    //////////////////////////////
    // Data

    tSubListVec mvSubLists;
    tLabelVec    mvColumns;

    float mfIconSize;
};
};

#endif // HPL_WIDGET_MULTI_PROPERTY_LIST_BOX_H
