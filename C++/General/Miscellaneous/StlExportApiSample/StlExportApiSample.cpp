#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/Document.h>

#include <Fusion/Fusion/Design.h>
#include <Fusion/Fusion/ExportManager.h>
#include <Fusion/Fusion/ExportOptions.h>
#include <Fusion/Fusion/STLExportOptions.h>
#include <Fusion/Components/Components.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Components/OccurrenceList.h>
#include <Fusion/Components/Occurrence.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>


using namespace adsk::core;
using namespace adsk::fusion;

Ptr<UserInterface> ui;

std::string getDllPath();

extern "C" XI_EXPORT bool run(const char* context)
{
 Ptr<Application> app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;


 // get active design
 Ptr<Product> product = app->activeProduct();
 if (!product)
  return false;

 Ptr<Design> design = product;
 if (!design)
  return false;

 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 // create a single exportManager instance
 Ptr<ExportManager> exportMgr = design->exportManager();
 if (!exportMgr)
  return false;

 // export the root component to printer utility
 Ptr<STLExportOptions> stlRootOptions = exportMgr->createSTLExportOptions(rootComp);
 if (!stlRootOptions)
  return false;

 // get all available print utilities
 std::vector<std::string> printUtils = stlRootOptions->availablePrintUtilities();

 // export the root component to the print utility, instead of a specified file
 size_t count = printUtils.size();
 for(size_t i = 0; i < count; i++)
 {
  std::string printUtil = printUtils[i];
  stlRootOptions->sendToPrintUtility(true);
  stlRootOptions->printUtility(printUtil);

  exportMgr->execute(stlRootOptions);
 }

 // export the occurrence one by one in the root component to a specified file
 Ptr<OccurrenceList> occurs = rootComp->allOccurrences();
 if (!occurs)
  return false;
 size_t occrCount = occurs->count();
 for (size_t j = 0; j < occrCount; j++ )
 {
  Ptr<Occurrence> occur = occurs->item(j);
  if (!occur) continue;

  Ptr<Component> comp = occur->component();
  if (!comp) continue;

  std::string compName = comp->name();
  std::string fileName = getDllPath() + "/" + compName;

  // create stl exportOptions
  Ptr<STLExportOptions> stlExportOptions = exportMgr->createSTLExportOptions(occur, fileName);
  stlExportOptions->sendToPrintUtility(false);

  exportMgr->execute(stlExportOptions);
 }

 // export the body one by one in the design to a specified file
 Ptr<BRepBodies> bRepBodies = rootComp->bRepBodies();
 if (!bRepBodies)
  return false;
 size_t bodyCount = bRepBodies->count();
 for ( size_t k = 0; k < bodyCount; k++)
 {
  Ptr<BRepBody> body = bRepBodies->item(k);
  if (!body) continue;

  Ptr<Component> comp = body->parentComponent();
  if (!comp) continue;

  std::string compName = comp->name();
  std::string bodyName = body->name();
  std::string fileName = getDllPath() + "/" + compName + "-" + bodyName;

  // create stl exportOptions
  Ptr<STLExportOptions> stlExportOptions = exportMgr->createSTLExportOptions(body, fileName);
  if (!stlExportOptions) continue;

  stlExportOptions->sendToPrintUtility(false);

  exportMgr->execute(stlExportOptions);
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

std::string getDllPath()
{
#if defined(_WINDOWS) || defined(_WIN32) || defined(_WIN64)
 HMODULE hModule = NULL;
 if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
  (LPCSTR) &getDllPath, 
  &hModule))
  return "";

 char winTempPath[2048];
 ::GetModuleFileNameA (hModule, winTempPath, 2048);

 std::string strPath = winTempPath;
 size_t stPos = strPath.rfind('\\');
 return strPath.substr(0, stPos);
#else
 Dl_info info;
 dladdr((void*) getDllPath, &info);

 std::string strPath = info.dli_fname;
 int stPos = (int)strPath.rfind('/');
 if(stPos != -1)
  return strPath.substr(0, stPos);
 else
  return "";;
#endif
}