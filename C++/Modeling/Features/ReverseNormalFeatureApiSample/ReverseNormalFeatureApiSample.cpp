#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlanes.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Construction/ConstructionPlaneInput.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/ReverseNormalFeature.h>
#include <Fusion/Features/ReverseNormalFeatures.h>
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

 // Create a collection of entities for extrude
 Ptr<ObjectCollection> entities0 = ObjectCollection::create();
 if(!entities0)
  return false;
 Ptr<Profiles> sketchProfiles = sketch->profiles();
 if(!sketchProfiles)
  return false;
 Ptr<Profile> sketchProfile = sketchProfiles->item(0);
 if(!sketchProfile)
  return false;
 entities0->add(sketchProfile);

 // Create a cylinder with ExtrudeFeature using the profile above.
 Ptr<Features> features = rootComp->features();
 if(!features)
  return false;
 Ptr<ExtrudeFeatures> extrudeFeats = features->extrudeFeatures();
 if(!extrudeFeats)
  return false;
 Ptr<ExtrudeFeatureInput> extrudeFeatureInput = extrudeFeats->createInput(entities0, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
 if(!extrudeFeatureInput)
  return false;
 Ptr<ValueInput> distance = adsk::core::ValueInput::createByReal(2.0);
 if(!distance)
  return false;
 extrudeFeatureInput->isSolid(false);
 extrudeFeatureInput->setDistanceExtent(false, distance);
 Ptr<ExtrudeFeature> extrudeFeature = extrudeFeats->add(extrudeFeatureInput);
 if(!extrudeFeature)
  return false;

 // Create a collection of source bodies
 Ptr<BRepBodies> brepBodies = extrudeFeature->bodies();
 if(!brepBodies)
  return false;
 Ptr<BRepBody> surface = brepBodies->item(0);
 if(!surface)
  return false;
 Ptr<ObjectCollection> bodies = adsk::core::ObjectCollection::create();
 if(!bodies)
  return false;
 bodies->add(surface);

 // Create a replace feature
 Ptr<ReverseNormalFeatures> reverseFeats = features->reverseNormalFeatures();
 if(!reverseFeats)
  return false;
 
 Ptr<ReverseNormalFeature> reverseFeat = reverseFeats->add(bodies);
 if(!reverseFeat)
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