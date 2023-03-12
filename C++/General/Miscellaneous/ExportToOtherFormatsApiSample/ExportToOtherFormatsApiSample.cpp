#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Geometry/Point3D.h>

#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/FusionTypeDefs.h>
#include <Fusion/Fusion/ExportManager.h>
#include <Fusion/Fusion/ExportOptions.h>
#include <Fusion/Fusion/IGESExportOptions.h>
#include <Fusion/Fusion/FusionArchiveExportOptions.h>
#include <Fusion/Fusion/SATExportOptions.h>
#include <Fusion/Fusion/SMTExportOptions.h>
#include <Fusion/Fusion/STEPExportOptions.h>
#include <Fusion/Fusion/STLExportOptions.h>


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


 Ptr<Product> product = app->activeProduct();
 if (!product)
  return false;

 Ptr<Design> design = product;
 if (!design)
  return false;


 Ptr<ExportManager> exportMgr = design->exportManager();
 if (!exportMgr)
  return false;

 std::string pathName = getDllPath();
 Ptr<IGESExportOptions> igesOptions = exportMgr->createIGESExportOptions(pathName + "/" + "test.igs");
 if (!igesOptions)
  return false;
 bRet = exportMgr->execute(igesOptions);

 Ptr<STEPExportOptions> stepOptions = exportMgr->createSTEPExportOptions(pathName + "/" + "test.step");
 if (!stepOptions)
  return false;
 bRet = exportMgr->execute(stepOptions);

 Ptr<SATExportOptions> satOptions = exportMgr->createSATExportOptions(pathName + "/" + "test.sat");
 if (!satOptions)
  return false;
 bRet = exportMgr->execute(satOptions);

 Ptr<SMTExportOptions> smtOptions = exportMgr->createSMTExportOptions(pathName + "/" + "test.smt");
 if (!smtOptions)
  return false;
 bRet = exportMgr->execute(smtOptions);

 Ptr<FusionArchiveExportOptions> fusionArchiveOptions = exportMgr->createFusionArchiveExportOptions(pathName + "/" + "test.f3d");
 if (!fusionArchiveOptions)
  return false;
 bRet = exportMgr->execute(fusionArchiveOptions);

 Ptr<STLExportOptions> stlOptions = exportMgr->createSTLExportOptions(rootComp, pathName + "/" + "test.stl");
 if (!stlOptions)
  return false;
 stlOptions->isBinaryFormat(true);
 stlOptions->meshRefinement(MeshRefinementHigh);
 bRet = exportMgr->execute(stlOptions);


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