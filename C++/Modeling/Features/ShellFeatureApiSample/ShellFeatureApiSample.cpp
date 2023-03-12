#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ShellFeatures.h>
#include <Fusion/Features/ShellFeature.h>
#include <Fusion/Features/ShellFeatureInput.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchPoint.h>
#include <Fusion/Sketch/SketchPoints.h>


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

 Ptr<Documents> documents = app->documents();
 if (!documents)
  return false;

 Ptr<Document> doc = documents->add(DocumentTypes::FusionDesignDocumentType);
 if (!doc)
  return false;

 Ptr<Product> product = app->activeProduct();
 if (!product)
  return false;

 Ptr<Design> design = product;
 if (!design)
  return false;

 // Get the root component of the active design
 Ptr<Component> rootComp = design->rootComponent();
 if(!rootComp)
  return false;

 // Create sketch circle on the xz plane.
 Ptr<Sketches> sketches = rootComp->sketches();
 if(!sketches)
  return false;
 Ptr<Sketch> sketch = sketches->add(rootComp->xZConstructionPlane());
 if(!sketch)
  return false;
 Ptr<SketchCurves> sketchCurves = sketch->sketchCurves();
 if(!sketchCurves)
  return false;
 Ptr<SketchCircles> sketchCirles = sketchCurves->sketchCircles();
 if(!sketchCirles)
  return false;
 Ptr<Point3D> centerPoint = Point3D::create(0, 0, 0);
 if(!centerPoint)
  return false;
 Ptr<SketchCircle> sketchCircle = sketchCirles->addByCenterRadius(centerPoint, 10);
 if(!sketchCircle)
  return false;

 // Get the profile from the sketch.
 Ptr<Profiles> sketchProfiles = sketch->profiles();
 if(!sketchProfiles)
  return false;
 Ptr<Profile> profile = sketchProfiles->item(0);
 if(!profile)
  return false;

 // Create a cylinder with ExtrudeFeature using the profile above.
 Ptr<Features> features = rootComp->features();
 if(!features)
  return false;
 Ptr<ExtrudeFeatures> extrudeFeats = features->extrudeFeatures();
 if(!extrudeFeats)
  return false;
 Ptr<ValueInput> distance = adsk::core::ValueInput::createByReal(2.0);
 if(!distance)
  return false;
 Ptr<ExtrudeFeature> extrudeFeature = extrudeFeats->addSimple(profile, distance, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
 if(!extrudeFeature)
  return false;

 // Create a collection of entities for shell
 Ptr<BRepFaces> brepFaces = extrudeFeature->endFaces();
 if(!brepFaces)
  return false;
 Ptr<BRepFace> brepFace = brepFaces->item(0);
 if(!brepFace)
  return false;
 Ptr<ObjectCollection> entities1 = adsk::core::ObjectCollection::create();
 if(!entities1)
  return false;
 entities1->add(brepFace);

 // Create a shell feature
 Ptr<ShellFeatures> shellFeats = features->shellFeatures();
 if(!shellFeats)
  return false;
 bool isTangentChain = false;
 Ptr<ValueInput> thickness = adsk::core::ValueInput::createByReal(0.5);
 if(!thickness)
  return false;
 Ptr<ShellFeatureInput> shellFeatureInput = shellFeats->createInput(entities1, isTangentChain);
 if(!shellFeatureInput)
  return false;
 shellFeatureInput->insideThickness(thickness);
 Ptr<ShellFeature> shellFeature = shellFeats->add(shellFeatureInput);
 if(!shellFeature)
  return false;


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