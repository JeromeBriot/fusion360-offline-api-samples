#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/Features/BRepCells.h>
#include <Fusion/Features/BRepCell.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/ThickenFeatures.h>
#include <Fusion/Features/ThickenFeatureInput.h>
#include <Fusion/Features/ThickenFeature.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>


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

 // Get the root component of the active design.
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
 Ptr<SketchCircle> sketchCircle = sketchCirles->addByCenterRadius(centerPoint, 3.0);
 if(!sketchCircle)
  return false;

 // Create a open profile for extrusion.
 Ptr<Profile> openProfile = rootComp->createOpenProfile(sketchCircle);

 // Create an extrusion input.
 Ptr<Features> features = rootComp->features();
 if (!features)
  return false;
 Ptr<ExtrudeFeatures> extrudes = features->extrudeFeatures();
 if (!extrudes)
  return false;
 Ptr<ExtrudeFeatureInput> extrudeInput = extrudes->createInput(openProfile, FeatureOperations::NewBodyFeatureOperation);
 if (!extrudeInput)
  return false;
 extrudeInput->isSolid(false);

 // Define the extent with a distance extent of 3 cm.
 Ptr<ValueInput> distance = ValueInput::createByReal(3.0);
 if (!distance)
  return false;
 extrudeInput->setDistanceExtent(false, distance);

 // Create the extrusion.
 Ptr<ExtrudeFeature> extrude = extrudes->add(extrudeInput);
 if (!extrude)
  return false;

 // Get the body created by extrusion.
 Ptr<BRepBodies> bodies = extrude->bodies();
 if (!bodies)
  return false;

 // Create thicken feature.
 Ptr<ThickenFeatures> thickens = features->thickenFeatures();
 if (!thickens)
  return false;
 Ptr<ObjectCollection> inputSurfaces = ObjectCollection::create();
 if (!inputSurfaces)
  return false;
 for (Ptr<BRepBody> body : bodies)
 {
  inputSurfaces->add(body);
 }
 Ptr<ValueInput> thickness = ValueInput::createByReal(1.0);
 Ptr<ThickenFeatureInput> thickenInput = thickens->createInput(inputSurfaces, thickness, false, FeatureOperations::NewBodyFeatureOperation);
 thickens->add(thickenInput);

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