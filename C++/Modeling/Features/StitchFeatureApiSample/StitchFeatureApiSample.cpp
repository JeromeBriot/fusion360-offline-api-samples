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
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/StitchFeatures.h>
#include <Fusion/Features/StitchFeatureInput.h>
#include <Fusion/Features/StitchFeature.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCurves.h>
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
 Ptr<SketchLines> sketchLines = sketchCurves->sketchLines();
 if (!sketchLines)
  return false;
 Ptr<Point3D> startPoint = Point3D::create(0, 0, 0);
 Ptr<Point3D> endPoint = Point3D::create(1.0, 0, 0);
 Ptr<SketchLine> sketchLine = sketchLines->addByTwoPoints(startPoint, endPoint);
 Ptr<Point3D> endPoint2 = Point3D::create(0, 1.0, 0);
 Ptr<SketchLine> sketchLine2 = sketchLines->addByTwoPoints(startPoint, endPoint2);

 // Create a open profile.
 Ptr<Profile> openProfile = rootComp->createOpenProfile(sketchLine);
 Ptr<Profile> openProfile2 = rootComp->createOpenProfile(sketchLine2);

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
 Ptr<ExtrudeFeatureInput> extrudeInput2 = extrudes->createInput(openProfile2, FeatureOperations::NewBodyFeatureOperation);
 if (!extrudeInput2)
  return false;
 extrudeInput2->isSolid(false);

 // Define the extent.
 Ptr<ValueInput> distance = ValueInput::createByReal(1.0);
 extrudeInput->setDistanceExtent(false, distance);
 extrudeInput2->setDistanceExtent(false, distance);

 // Create the extrusion.
 Ptr<ExtrudeFeature> extrude = extrudes->add(extrudeInput);
 if (!extrude)
  return false;
 Ptr<ExtrudeFeature> extrude2 = extrudes->add(extrudeInput2);
 if (!extrude2)
  return false;
 Ptr<BRepBodies> bodies = extrude->bodies();
 if (!bodies)
  return false;
 Ptr<BRepBody> body = bodies->item(0);
 Ptr<BRepBodies> bodies2 = extrude2->bodies();
 if (!bodies2)
  return false;
 Ptr<BRepBody> body2 = bodies2->item(0);
 Ptr<ObjectCollection> surfaces = ObjectCollection::create();
 if (!surfaces)
  return false;
 surfaces->add(body);
 surfaces->add(body2);

 // Define tolerance with 1 cm.
 Ptr<ValueInput> tolerance = ValueInput::createByReal(1.0);

 // Create a stitch input to be able to define the input needed for an stitch.
 Ptr<StitchFeatures> stitches = features->stitchFeatures();
 if (!stitches)
  return false;
 Ptr<StitchFeatureInput> stitchInput = stitches->createInput(surfaces, tolerance, FeatureOperations::NewBodyFeatureOperation);

 // Create a stitch feature.
 stitches->add(stitchInput);


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