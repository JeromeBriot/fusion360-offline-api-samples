#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Utils.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Fusion/FusionUnitsManager.h>
#include <sstream>


using namespace adsk::core;
using namespace adsk::fusion;

Ptr<UserInterface> ui;

extern "C" XI_EXPORT bool run(const char* context)
{
 Ptr<Application> app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;

 Ptr<Design> design = app->activeProduct();
 if (!design) {
  ui->messageBox("No active Fusion design", "No Design");
  return false;
 }

 // Prompt the user for a string and validate it's valid.
 bool isValid = false;
 std::string input = "1 in";  // The initial default value.
 double realValue = 0.0;
 while (!isValid) {
  // Get a string from the user.
  bool cancelled = false;
  input = ui->inputBox("Enter a distance", cancelled, "Distance", input);

  // Exit the program if the dialog was cancelled.
  if (cancelled) {
   adsk::terminate();
   return false;
  }

  // Check that a valid length description was entered.
  Ptr<UnitsManager> unitsMgr = design->unitsManager();
  if(!unitsMgr)
   return false;

  realValue = unitsMgr->evaluateExpression(input, unitsMgr->defaultLengthUnits());
  if(app->getLastError())
  {
   // Invalid expression so display an error and set the flag to allow them
   // to enter a value again.
   ui->messageBox(input + " is not a valid length expression.", "Invalid entry", 
    OKButtonType, CriticalIconType);
   isValid = false;
  }
  else
   isValid = true;
 }

 // Use the value for something.
 std::stringstream ss;
 ss << "input: " << input << ", result: " << realValue;
 ui->messageBox(ss.str());

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