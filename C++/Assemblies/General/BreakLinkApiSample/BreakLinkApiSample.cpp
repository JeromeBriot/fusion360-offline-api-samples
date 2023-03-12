#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Occurrence.h>
#include <Fusion/Components/Occurrences.h>
#include <Fusion/Components/Component.h>


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
 if (!design)
  return false;

 // Get the root component of the active design
 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 // Get the occurrences from the root component.
 Ptr<Occurrences> occurrences = rootComp->occurrences();
 if (!occurrences)
  return false;

 // Build an array of occurrences because breaking a link will cause the
 // collection to be modified and causes a problem iterating over the colection.
 std::vector<Ptr<Occurrence>> occs;
 for (int i = 0; i < occurrences->count(); ++i)
 {
  if (Ptr<Occurrence> occ = occurrences->item(i))
   occs.push_back(occ);
 }

 // Iterate through the top-level occurrences to see if any of them are external references.
 for (Ptr<Occurrence> occ: occs)
 {
  if (occ->isReferencedComponent())
  {
   occ->breakLink();
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