#ifndef HPL_GUI_POP_UP_FILE_PICKER_H
#define HPL_GUI_POP_UP_FILE_PICKER_H

#include "gui/WidgetBaseClasses.h"
#include "gui/GuiPopUp.h"

namespace hpl
{

class cGuiSet;
class cGuiSkin;

class cWidgetWindow;
class cWidgetMultiPropertyListBox;
class cWidgetButton;
class cWidgetTextBox;
class cWidgetComboBox;

//---------------------------------------------------------------

//---------------------------------------------------------------

class cGuiPopUpFilePicker : public iGuiPopUp, public iFileBrowser
{
public:
    cGuiPopUpFilePicker(cGuiSet* apSet, cGuiSkin* apSkin, eFilePickerType aeType, const tWString &asStartPath, bool abShowHiddenFiles,
                        void *apCallbackObject, tGuiCallbackFunc apCallback, const tWString& asStartFilename=_W(""));
    virtual ~cGuiPopUpFilePicker();

    void Init();

    void SetSaveFileDest( tWString &asFileName );
    void SetLoadFileListDest( tWStringVec &asFileList );

protected:
    /////////////////////////////
    // Own functions
    bool Button_Pressed(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(Button_Pressed);

    bool MessageBoxReturnCallback( iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(MessageBoxReturnCallback);

    bool Directory_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(Directory_OnSelectionChange);

    bool FileList_OnSelectionDblClick(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(FileList_OnSelectionDblClick);

    bool FileList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(FileList_OnSelectionChange);

    bool FilterList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(FilterList_OnSelectionChange);


    void BuildSaveFileName();
    void BuildLoadFileList();

    void OnAddFilter();
    void OnNavigate();
    void PopulateFileList();

    void ClosePopUp(bool abPickedFile);




    //////////////////////////////
    // Data
    tWString msStartFilename;

    tWString* mpSaveFileDest;
    tWStringVec* mpLoadFileListDest;

    eFilePickerType mPickerType;

    cGuiGfxElement* mvGfxFileTypeIcons[6];
    cGuiGfxElement* mpGfxUpButton;

    cWidgetComboBox* mpCurrentDirectory;
    cWidgetMultiPropertyListBox* mpFileList;
    cWidgetTextBox* mpCurrentFileName;
    cWidgetComboBox* mpFilterList;
    cWidgetButton* mvButtons[3];

    void* mpCallbackObject;
    tGuiCallbackFunc mpCallback;
};
};

#endif // HPL_GUI_POP_UP_FILE_PICKER_H
