#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>


using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;
Ptr<UserInterface> ui;

extern "C" XI_EXPORT bool run(const char* context)
{
 app = Application::get();
 if (!app)
  return false;

 Ptr<Document> doc = app->activeDocument();
 if (!doc)
  return false;

 Ptr<Product> product = app->activeProduct();
 if (!product)
  return false;

 Ptr<Design> design = product;
 if (!design)
  return false;

 Ptr<MaterialLibraries> matLibs = app->materialLibraries();
 if (!matLibs)
  return false;

 std::string matLibPath = TestUtils::getDllPath() + "/../../../../APISampleMaterialLibrary3.adsklib";
 Ptr<MaterialLibrary> matLib = matLibs->load(matLibPath);
 if (!matLib)
  return false;

 Ptr<Appearance> appear = matLib->appearances()->item(1);
 if (!appear)
  return false;

 appear->copyTo(design);
 Ptr<Appearance> favAppear = design->appearances()->itemByName(appear->name());

 // Get the root component of the active design
 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 Ptr<BRepBody> body2 = rootComp->bRepBodies()->item(1);
 if (!body2)
  return false;

 body2->appearance(favAppear);


 if (matLib->isNative() == false)
 {
  matLib->unload();
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