#include <Core/CoreAll.h>
#include <CAM/CAMAll.h>

#ifdef _WINDOWS
    #include <shlwapi.h>
#else
    #include <stdlib.h>
#endif

using namespace adsk::core;
using namespace adsk::cam;

Ptr<UserInterface> ui;

extern "C" XI_EXPORT bool run(const char* context)
{
    Ptr<Application> app = Application::get();
    if (!app)
        return false;

    ui = app->userInterface();
    if (!ui)
        return false;

    Ptr<Document> doc = app->activeDocument();
    if (!doc)
        return false;

    Ptr<Products> products = doc->products();
    if(!products)
        return false;

    Ptr<CAM> camProduct = products->itemByProductType("CAMProductType");
    if (!camProduct)
    {
        ui->messageBox("There are no CAM operations in the active document. This script requires the active document to contain at least one CAM operation.",
                      "No CAM Operations Exist", MessageBoxButtonTypes::OKButtonType, MessageBoxIconTypes::CriticalIconType);
        return false;
    }

    std::string outputFolder = camProduct->temporaryFolder();
    SetupSheetFormats sheetFormat = SetupSheetFormats::HTMLFormat;
    DialogResults dlgResults = ui->messageBox("View setup sheets when done?", "Generate Setup Sheets",
                                              MessageBoxButtonTypes::YesNoButtonType, 
                                              MessageBoxIconTypes::QuestionIconType);
    bool viewResults = dlgResults == DialogResults::DialogNo ? false : true;

    int scenario = 3;
    switch (scenario)
    {
        case 1:
            {
                ui->messageBox("Setup sheets for all operations will be generated.");
                camProduct->generateAllSetupSheets(sheetFormat, outputFolder, viewResults);
            }
            break;
        case 2:
            {
                ui->messageBox("Setup sheets for operations in the first setup will be generated.");
                Ptr<Setups> setups = camProduct->setups();
                if (!setups)
                    return false;

                Ptr<Setup> setup = setups->item(0);
                if (!setup)
                    return false;

                camProduct->generateSetupSheet(setup, sheetFormat, outputFolder, viewResults);
            }
            break;
        case 3:
            {
                ui->messageBox("A setup sheet for the first operation in the first setup will be generated.");
                Ptr<Setups> setups = camProduct->setups();
                if (!setups)
                    return false;

                Ptr<Setup> setup = setups->item(0);
                if (!setup)
                    return false;

                // It is a set of Operations, Folders and Patterns.
                Ptr<ObjectCollection> objects = setup->allOperations();
                if (!objects)
                    return false;

                if (objects->count() == 0)
                {
                    ui->messageBox("There is not any operation in the first setup!");
                    return false;
                }

                Ptr<Base> baseObject = objects->item(0);
                if (!baseObject)
                    return false;

                Operation* pOperation = baseObject->query<Operation>();
                if (!pOperation)
                    return false;

                Ptr<Operation> operation(pOperation, false);
                if (operation->hasToolpath())
                {
                    camProduct->generateSetupSheet(operation, sheetFormat, outputFolder, viewResults);
                }
                else
                {
                    ui->messageBox("This operation has no toolpath.  A valid toolpath must exist in order for a setup sheet to be generated.");
                    return false;
                }
            }
            break;
        default:
            break;
    }

    // Show output folder.
    ui->messageBox("Setup Sheets have been generated in '" + outputFolder + "'.");
#ifdef _WINDOWS
    ShellExecuteA(nullptr, "open", outputFolder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
    std::string command = "open " + outputFolder;
    system(command.c_str());
#endif

    // Active CAM workspace if it is not the active one.
    if (ui->activeWorkspace()->name() != "CAM")
    {
        DialogResults result = ui->messageBox("Activate the CAM Workspace?", "CAM Workspace Activate",
        MessageBoxButtonTypes::YesNoButtonType, MessageBoxIconTypes::QuestionIconType);
        bool activeCAMWorkspace = result == DialogResults::DialogYes ? true : false;
        if (activeCAMWorkspace)
        {
            Ptr<Workspace> camWorkspace = ui->workspaces()->itemById("CAMEnvironment");
            camWorkspace->activate();
        }
    }

 return true;
}

#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
 switch (reason)
 {
 case DLL_PROCESS_ATTACH:
 case DLL_THREAD_ATTACH:
 case DLL_THREAD_DETACH:
 case DLL_PROCESS_DETACH:
  break;
 }
 return TRUE;
}

#endif // XI_WIN