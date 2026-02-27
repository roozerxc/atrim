#ifndef HPL_WIDGET_LIST_BOX_BASE_H
#define HPL_WIDGET_LIST_BOX_BASE_H

#include "gui/Widget.h"
#include "gui/WidgetBaseClasses.h"

namespace hpl {

    class cGuiSkinFont;

    class cWidgetSlider;

    enum eListBoxSelectType
    {
        eListBoxSelectType_Select,
        eListBoxSelectType_Toggle,
        eListBoxSelectType_FromLastCursor,
        eListBoxSelectType_FromAnchorPos,
    };

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------

    class iWidgetListBoxBase : public iWidget, public iWidgetItemContainer
    {
    public:
        iWidgetListBoxBase(cGuiSet *apSet, cGuiSkin *apSkin);
        virtual ~iWidgetListBoxBase();

        void SetBackgroundZ(float afZ){mfBackgroundZ = afZ;}
        float GetBackgroundZ(){ return mfBackgroundZ;}

        void SetSelectedItem(int alX,bool abMoveList=false, 
                             bool abGenerateCallback=true,
                             eListBoxSelectType aType=eListBoxSelectType_Toggle,
                             bool abClearPrevious=true);
        int GetSelectedItem();

        void AddIndexToSelection(int alIdx);
        void RemoveIndexFromSelection(int alIdx);

        int GetMultiSelectionNum();
        int GetMultiSelection(int alIdx);

        void SetAllowMultiSelection(bool abX) { mbAllowMultiSelection = abX; }
        bool GetAllowMultiSelection() { return mbAllowMultiSelection; }

        void ClearSelection();

        virtual cVector3f GetItemStartPos();

        void SetCursorPos(int alPos, bool abMoveList, bool abClamp);
        int GetCursorPos() { return mlCursorPos; }
        
        void SetAnchorPos(int alX) { mlAnchorPos = alX; }

        bool GetIsLocked() { return mbLocked; }
        void SetIsLocked(bool abX) { mbLocked = abX; }
                
    protected:
        /////////////////////////
        // Own functions
        virtual void DrawItems(float afTimeStep, cGuiClipRegion* apClipRegion){}

        void SelectRange(int alStart, int alEnd);

        void UpdateProperties();

        void AddItemToSelection(cWidgetItem* apItem);
        void RemoveItemFromSelection(cWidgetItem* apItem);

        bool MoveSlider(iWidget* apWidget, const cGuiMessageData& aData);
        kGuiCallbackDeclarationEnd(MoveSlider);    

        /////////////////////////
        // Implemented functions
        void OnChangeSize();
        void OnLoadGraphics();
        void OnInit();


        void OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion);
        void OnDrawAfterClip(float afTimeStep, cGuiClipRegion* apClipRegion);


        bool OnMouseMove(const cGuiMessageData& aData);
        bool OnMouseDown(const cGuiMessageData& aData);
        bool OnMouseDoubleClick(const cGuiMessageData& aData);
        bool OnMouseUp(const cGuiMessageData& aData);
        bool OnMouseEnter(const cGuiMessageData& aData);
        bool OnMouseLeave(const cGuiMessageData& aData);

        bool OnKeyPress(const cGuiMessageData& aData);

        bool OnUIArrowPress(const cGuiMessageData& aData);
        bool OnUIArrowRelease(const cGuiMessageData& aData);
        bool OnUIButtonPress(const cGuiMessageData& aData);
        bool OnUIButtonRelease(const cGuiMessageData& aData);
        bool OnUIButtonDoublePress(const cGuiMessageData& aData);
        
        /////////////////////////
        // Data
        float mfBackgroundZ;
        float mfSliderWidth;

        int mlFirstItem;
        int mlMaxItems;

        tIntList mlstSelectedItems;

        int mlCursorPos;
        int mlLastCursorPos;
        int mlAnchorPos;
        int mlNumClicksOnSamePos;

        cGuiGfxElement *mpGfxBackground;
        cGuiGfxElement *mpGfxSelection;
        
        cGuiGfxElement *mvGfxBorders[4];
        cGuiGfxElement *mvGfxCorners[4];

        cWidgetSlider *mpSlider;

        bool mbAllowMultiSelection;
        bool mbSelectionUpdated;
        bool mbClearingSelection;

        int mlNumItems;
        int mlNumBlankRows;

        bool mbLocked;
    };

};
#endif // HPL_WIDGET_LIST_BOX_BASE_H
