#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/FolderDialog.h>
#include <Core/Application/Viewport.h>
#include <Core/Utils.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Fusion/Parameter.h>
#include <Fusion/Fusion/ParameterList.h>
#include <iostream>
#include <iomanip> 
#include <sstream>

using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;
Ptr<UserInterface> ui;

// Function that pads the input number with leading zeros to get to the specified length.
std::string pad(int number, int length)
{
 std::stringstream ss;
 ss << std::setw(length) << std::setfill('0') << number;
 return ss.str();
}

extern "C" XI_EXPORT bool run(const char* context)
{
 app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;

 Ptr<Design> des = app->activeProduct();

 std::string paramName = "Length";
 double startValue = 10.0;
 double endValue = 15.0;
 double increment = 0.1;

 // Get the output folder.
 Ptr<FolderDialog> fd = ui->createFolderDialog();
 fd->title("Specify Output Folder");
 if (fd->showDialog() != adsk::core::DialogResults::DialogOK)
  return false;

 std::string resultFolder = fd->folder() + "/";

 Ptr<Parameter> param = des->allParameters()->itemByName(paramName);
 if (!param)
 {
  ui->messageBox("The parameter \"" + paramName + "\" must exist.");
  return false;
 }

 double currentValue = startValue;
 param->value(currentValue);

 int cnt = 0;
 while (param->value() < endValue)
 {
  param->value(currentValue);
  adsk::doEvents();
  currentValue += increment;
  std::string filename = resultFolder + "frame" + pad(cnt, 4);
  app->activeViewport()->saveAsImageFile(filename, 0, 0);
  cnt += 1;
 }

 ui->messageBox("Finished.");

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