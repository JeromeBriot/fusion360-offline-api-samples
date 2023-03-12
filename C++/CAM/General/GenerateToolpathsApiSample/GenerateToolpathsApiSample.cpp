#include <Core/CoreAll.h>
#include <CAM/CAMAll.h>

#include <chrono>

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

    // set the value of scenario to 1, 2 or 3 to generate all, the first setup, or the first operation of the first setup.
    int scenario = 1;
    Ptr<GenerateToolpathFuture> future;
    std::string message;
    switch (scenario)
    {
        case 1:
            {
                future = camProduct->generateAllToolpaths(false);
                message = "The toolpaths for all operations in the document have been generated.";
            }
            break;
        case 2:
            {
                Ptr<Setups> setups = camProduct->setups();
                if (!setups)
                    return false;

                Ptr<Setup> setup = setups->item(0);
                if (!setup)
                    return false;

                future = camProduct->generateToolpath(setup);
                message = "The toolpaths for the operations of the first setup in the document have been generated.";
            }
            break;
        case 3:
            {
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
                    ui->messageBox("There are not any operations in the first setup!");
                    return false;
                }

                Ptr<Operation> operation = objects->item(0);
                future = camProduct->generateToolpath(operation);
                message = "The toolpath for the first operation of the first setup in the document have been generated.";
            }
            break;
        default:
            break;
    }

    if (!future)
        return false;

    int operationNum = future->numberOfOperations();
    Ptr<ProgressDialog> progressDlg = ui->createProgressDialog();
    progressDlg->isCancelButtonShown(false);
    progressDlg->isBackgroundTranslucent(false);
    progressDlg->show("Toolpath Generation Progress", "Generating Toolpaths", 0, 10);

    // Since toolpaths are calculated in parallel, loop the progress bar while the toolpaths are being generated but none are yet complete.
    while (future->isGenerationCompleted() == false)
    {
        int n = 0;
        std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
        while (future->numberOfCompleted() == 0)
        {
            std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now(); 
            std::chrono::duration<double> elapsed_seconds = currentTime - startTime;
            if (elapsed_seconds.count() > .125)
            {
                startTime = std::chrono::system_clock::now();
                n += 1;
                progressDlg->progressValue(n);
                adsk::doEvents();
            }
            if (n > 10)
                n = 0;
        }

        // Set the progress bar value to the number of completed toolpaths.
        progressDlg->progressValue(future->numberOfCompleted());

        // Set the progress bar max to the number of operations to be completed.
        progressDlg->maximumValue(operationNum);

        // set the message for the progress dialog to track the progress value and the total number of operations to be completed.
        progressDlg->message("Generating %v of %m toolpaths");
        adsk::doEvents();
    }

    progressDlg->hide();

    ui->messageBox(message);

    // Active CAM workspace if it is not the active one.
    if (ui->activeWorkspace()->name() != "CAM")
    {
        DialogResults result = ui->messageBox("Activate the CAM Workspace?", "CAM Workspace Activate",
                                              MessageBoxButtonTypes::YesNoButtonType, 
                                              MessageBoxIconTypes::QuestionIconType);
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