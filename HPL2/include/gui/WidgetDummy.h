#ifndef HPL_WIDGET_DUMMY_H
#define HPL_WIDGET_DUMMY_H

#include "gui/Widget.h"

namespace hpl
{

class cWidgetDummy : public iWidget
{
public:
    cWidgetDummy(cGuiSet *apSet, cGuiSkin *apSkin);
    virtual ~cWidgetDummy();

    void SetEncloseChildren(bool abX);

    void OnChildUpdate(iWidget* apChild);

protected:
    bool mbEncloseChildren;
};

};
#endif // HPL_WIDGET_H
