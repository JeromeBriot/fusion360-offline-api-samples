#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Components/Occurrences.h>
#include <Fusion/Components/Occurrence.h>
#include <Core/Geometry/Matrix3D.h>
#include <Fusion/MeshBody/MeshBodies.h>
#include <Fusion/MeshBody/MeshBody.h>
#include <Fusion/MeshBody/MeshBodyList.h>
#include <Fusion/FusionTypeDefs.h>
#include <Core/Materials/MaterialLibraries.h>
#include <Core/Materials/MaterialLibrary.h>
#include <Core/Materials/Materials.h>
#include <Core/Materials/Material.h>
#include <Core/Materials/Appearances.h>
#include <Core/Materials/Appearance.h>



using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;
Ptr<UserInterface> ui;

std::string getDllPath();

extern "C" XI_EXPORT bool run(const char* context)
{
 app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;

 //Create a new document
 Ptr<Documents> docs = app->documents();
 if (!docs)
  return false;

 Ptr<Document> doc = docs->add(DocumentTypes::FusionDesignDocumentType);
 if (!doc)
  return false;

 // Get the root component of the active design
 Ptr<Design> design = app->activeProduct();
 if (!design)
  return false;

 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;
 
 //Create a new component
 Ptr<Occurrences> occs = rootComp->occurrences();
 if (!occs)
  return false;

 Ptr<Matrix3D> transfrom = Matrix3D::create();
 if (!transfrom)
  return false;

 Ptr<Occurrence> occ = occs->addNewComponent(transfrom);
 if (!occ)
  return false;

 //Insert a mesh body in the sub Component
 Ptr<Component> subComp = occ->component();
 if (!subComp)
  return false;

 Ptr<MeshBodies> meshBodies = subComp->meshBodies();
 if (!meshBodies)
  return false;

 std::string stlFilePath = getDllPath() + "/" + "APIMeshFacetColors.stl";

 Ptr<MeshBodyList> meshBodyList = meshBodies->add(stlFilePath, MeshUnits::InchMeshUnit);
 if (!meshBodyList)
  return false;

 Ptr<MeshBody> stlMeshBody = meshBodyList->item(0);
 if (!stlMeshBody)
  return false;

 Ptr<MeshBody> meshBodyProxy = stlMeshBody->createForAssemblyContext(occ);
 if (!meshBodyProxy)
  return false;

 // Get and set mesh body name
 bool isSetNameTrue = stlMeshBody->name("NewName");

 std::string newName = stlMeshBody->name();

 //Get and set mesh body selectable
 bool isSetSelectableTrue = stlMeshBody->isSelectable(false);

 bool isSelectable = stlMeshBody->isSelectable();

 isSetSelectableTrue = stlMeshBody->isSelectable(true);

 isSelectable = stlMeshBody->isSelectable();

 //Get and set Mesh body bulb light and visibility
 bool isSetBulbTrue = stlMeshBody->isLightBulbOn(false);

 bool isLightBulbOn = stlMeshBody->isLightBulbOn();

 bool isVisible = stlMeshBody->isVisible();

 isSetBulbTrue = stlMeshBody->isLightBulbOn(true);

 isLightBulbOn = stlMeshBody->isLightBulbOn();

 isVisible = stlMeshBody->isVisible();

 //Get and set mesh body Opacity
 bool isSetOpacityTrue = subComp->opacity(0.5);

 isSetOpacityTrue = stlMeshBody->opacity(0.6);

 double opacity = stlMeshBody->opacity();

 double visibleOpacity = meshBodyProxy->visibleOpacity();

 //Get and set mesh body material
 Ptr<MaterialLibraries> materialLibraries = app->materialLibraries();
 if (!materialLibraries)
  return false;

 Ptr<MaterialLibrary> materialLibrary = materialLibraries->itemByName("Fusion 360 Material Library");
 if (!materialLibrary)
  return false;

 Ptr<Materials> materials = materialLibrary->materials();
 if (!materials)
  return false;

 Ptr<Material> newMaterial = materials->itemByName("Glass, Bronze Glazing, Low E");
 if (!newMaterial)
  return false;

 bool isSetMaterialTrue = stlMeshBody->material(newMaterial);

 Ptr<Material> material = stlMeshBody->material();
 if (!material)
  return false;

 std::string materialName = material->name();

 //Get and set mesh body appearance
 Ptr<MaterialLibrary> appearanceLibrary = materialLibraries->itemByName("Fusion 360 Appearance Library");
 if (!appearanceLibrary)
  return false;

 Ptr<Appearances> appearances = appearanceLibrary->appearances();
 if (!appearances)
  return false;

 Ptr<Appearance> newAppearance = appearances->itemByName("Glass - Window");
 if (!newAppearance)
  return false;

 bool isSetAppearanceTrue = stlMeshBody->appearance(newAppearance);

 Ptr<Appearance> appearance = stlMeshBody->appearance();
 if (!appearance)
  return false;

 std::string appearanceName = appearance->name();

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
  (LPCSTR)&getDllPath,
  &hModule))
  return "";

 char winTempPath[2048];
 ::GetModuleFileNameA(hModule, winTempPath, 2048);

 std::string strPath = winTempPath;
 size_t stPos = strPath.rfind('\\');
 return strPath.substr(0, stPos);
#else
 Dl_info info;
 dladdr((void*)getDllPath, &info);

 std::string strPath = info.dli_fname;
 int stPos = (int)strPath.rfind('/');
 if (stPos != -1)
  return strPath.substr(0, stPos);
 else
  return "";;
#endif
}