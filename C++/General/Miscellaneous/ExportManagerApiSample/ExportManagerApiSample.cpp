#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/Document.h>

#include <Fusion/Fusion/Design.h>
#include <Fusion/Fusion/ExportManager.h>
#include <Fusion/Fusion/ExportOptions.h>
#include <Fusion/Fusion/IGESExportOptions.h>
#include <Fusion/Fusion/FusionArchiveExportOptions.h>
#include <Fusion/Fusion/SATExportOptions.h>
#include <Fusion/Fusion/SMTExportOptions.h>
#include <Fusion/Fusion/STEPExportOptions.h>
#include <Fusion/Components/Components.h>
#include <Fusion/Components/Component.h>


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

 // get all components in this design
 Ptr<Components> comps = design->allComponents();
 if (!comps)
  return false;

 // create a single exportManager instance
 Ptr<ExportManager> exportMgr = design->exportManager();
 if (!exportMgr)
  return false;

 // export the component one by one with a specified format
 size_t count = comps->count();
 for (size_t index = 0; index < count; ++index)
 {
  Ptr<Component> comp = comps->item(index);
  std::string name = comp->name();
  std::string fileName = getDllPath() + "/" + name;

  // export the component with IGS format
  Ptr<IGESExportOptions> igesOptions = exportMgr->createIGESExportOptions(fileName, comp);
  if (!igesOptions) continue;
  exportMgr->execute(igesOptions);

  // export the component with SAT format
  Ptr<SATExportOptions> satOptions = exportMgr->createSATExportOptions(fileName, comp);
  if (!satOptions) continue;
  exportMgr->execute(satOptions);

  // export the component with SMT format
  Ptr<SMTExportOptions> smtOptions = exportMgr->createSMTExportOptions(fileName, comp);
  if (!smtOptions) continue;
  exportMgr->execute(smtOptions);

  // export the component with STP format
  Ptr<STEPExportOptions> stepOptions = exportMgr->createSTEPExportOptions(fileName, comp);
  if (!stepOptions) continue;
  exportMgr->execute(stepOptions);

  // export the component with F3D format
  Ptr<FusionArchiveExportOptions> archOptions = exportMgr->createFusionArchiveExportOptions(fileName, comp);
  if (!stepOptions) continue;
  exportMgr->execute(archOptions);
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