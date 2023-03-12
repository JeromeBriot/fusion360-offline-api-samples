//////////////////////////////////////////////////////////////////////////
// For this sample script to run, the active Fusion document must contain
// at least one CAM operation.
//////////////////////////////////////////////////////////////////////////

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

    // Check if the document has a CAMProductType. It will return if there are no CAM operations in it.
    Ptr<CAM> camProduct = products->itemByProductType("CAMProductType");
    if (!camProduct)
    {
        ui->messageBox("There are no CAM operations in the active document. This script requires the active document to contain at least one CAM operation.",
                       "No CAM Operations Exist", MessageBoxButtonTypes::OKButtonType, MessageBoxIconTypes::CriticalIconType);
        return false;
    }

    std::string outputFolder = camProduct->temporaryFolder();
    std::string programName = "101";

    // Set the post configuration to use based on Operation Type of the first Setup
    Ptr<Setups> setups = camProduct->setups();
    if (!setups)
        return false;

    Ptr<Setup> setup = setups->item(0);
    if (!setup)
        return false;

    OperationTypes opType = setup->operationType();
    std::string postConfig = camProduct->genericPostFolder();
    switch (opType)
    {
        case adsk::cam::MillingOperation:
            postConfig += "/fanuc.cps";
            break;
        case adsk::cam::TurningOperation:
            postConfig += "/fanuc turning.cps";
            break;
        case adsk::cam::JetOperation:
            postConfig += "/omax.cps";
            break;
        default:
            postConfig += "/fanuc.cps";
            break;
    }

    DialogResults dlgResults = ui->messageBox("View nc file when done?", "Post",
                                              MessageBoxButtonTypes::YesNoButtonType, 
                                              MessageBoxIconTypes::QuestionIconType);
    bool viewResults = dlgResults == DialogResults::DialogNo ? false : true;

 // Specify the NC file output units.
 PostOutputUnitOptions units = adsk::cam::PostOutputUnitOptions::DocumentUnitsOutput;
 //PostOutputUnitOptions units = adsk::cam::PostOutputUnitOptions::MillimetersOutput;
 //PostOutputUnitOptions units = adsk::cam::PostOutputUnitOptions::InchesOutput;

 // Create the postInput object.
 Ptr<PostProcessInput> pstInput = PostProcessInput::create(programName, postConfig, outputFolder, units);
 pstInput->isOpenInEditor(viewResults);

 // Create a named value to use in setting a property of the post.
 Ptr<NamedValues> pstProperties = adsk::core::NamedValues::create();
 Ptr<ValueInput> disableSequenceNumbers = adsk::core::ValueInput::createByBoolean(false);
 pstProperties->add("showSequenceNumbers", disableSequenceNumbers);
 pstInput->postProperties(pstProperties);

 // Set the value of scenario to 1, 2 or 3 to post all, post the first setup, or post only the first operation of the first setup.
 int scenario = 3;
 switch (scenario)
 {
  case 1:
   ui->messageBox("All toolpaths will be posted.");
   camProduct->postProcessAll(pstInput);
   break;
  case 2:
   ui->messageBox("Toolpaths in the first setup will be posted.");
   camProduct->postProcess(setup, pstInput);
   break;
  case 3:
   ui->messageBox("The first Toolpath in the first setup will be posted.");
   // It is a set of Operations, Folders and Patterns.
   Ptr<ObjectCollection> allOps = setup->allOperations();
   if (!allOps)
    return false;

   if (allOps->count() == 0)
   {
    ui->messageBox("There is not any operation in the first setup!");
    return false;
   }

   // Get the first operation.
   Ptr<Operation> operation = allOps->item(0);
   if (!operation)
    return false;

   if (operation->hasToolpath())
   {
    camProduct->postProcess(operation, pstInput);
   }
   else
   {
    ui->messageBox("This operation has no toolpath to post.");
    return false;
   }
   break;
 }

 // Show output folder.
 ui->messageBox("Post processing is complete. The results have been written to:\n" + outputFolder + "/" + programName + ".nc");

#ifdef _WINDOWS
 ShellExecuteA(nullptr, "open", outputFolder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
 std::string command = "open " + outputFolder;
 system(command.c_str());
#endif

    // Activate CAM workspace if it is not the active one.
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