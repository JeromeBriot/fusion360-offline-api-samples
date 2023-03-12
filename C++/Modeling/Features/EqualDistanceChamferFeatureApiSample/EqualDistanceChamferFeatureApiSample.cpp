#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/Selection.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/ValueInput.h>

#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ChamferFeatures.h>
#include <Fusion/Features/ChamferFeatureInput.h>

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
 Ptr<Component> root = design->rootComponent();
 if (!root)
  return false;

 // Have the edge selected and add it to an ObjectCollection.
 Ptr<Selection> selection = ui->selectEntity("Select edge to chamfer", "Edges");
 if (!selection)
  return false;

 Ptr<BRepEdge> edge = selection->entity();
 if (!edge)
  return false;

 Ptr<ObjectCollection> edgeCollection = ObjectCollection::create();
 if (!edgeCollection)
  return false;
 edgeCollection->add(edge);

 Ptr<Features> feats = root->features();
 if (!feats)
  return false;

 Ptr<ChamferFeatures> chamfers = feats->chamferFeatures();
 if (!chamfers)
  return false;

 Ptr<ChamferFeatureInput> chamferInput = chamfers->createInput(edgeCollection, true);
 if (!chamferInput)
  return false;
 chamferInput->setToEqualDistance(ValueInput::createByReal(1.0));

 Ptr<ChamferFeature> chamfer = chamfers->add(chamferInput);
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