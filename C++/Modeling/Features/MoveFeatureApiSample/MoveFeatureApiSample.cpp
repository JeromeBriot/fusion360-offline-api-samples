#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/Geometry/Matrix3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/MoveFeatures.h>
#include <Fusion/Features/MoveFeature.h>
#include <Fusion/Features/MoveFeatureInput.h>
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
 Ptr<SketchCircles> sketchCircles = sketchCurves->sketchCircles();
 if(!sketchCircles)
  return false;
 Ptr<Point3D> centerPoint = Point3D::create(0, 0, 0);
 if(!centerPoint)
  return false;
 Ptr<SketchCircle> sketchCircle = sketchCircles->addByCenterRadius(centerPoint, 10);
 if(!sketchCircle)
  return false;
 centerPoint = Point3D::create(15, 5, 0);
 if(!centerPoint)
  return false;
 sketchCircle = sketchCircles->addByCenterRadius(centerPoint, 4);
 if(!sketchCircle)
  return false;

 // Create a collection of entities for extrude
 Ptr<ObjectCollection> profiles = ObjectCollection::create();
 if(!profiles)
  return false;
 Ptr<Profiles> sketchProfiles = sketch->profiles();
 if(!sketchProfiles)
  return false;
 Ptr<Profile> sketchProfile = sketchProfiles->item(0);
 if(!sketchProfile)
  return false;
 profiles->add(sketchProfile);
 
 sketchProfile = sketchProfiles->item(1);
 if(!sketchProfile)
  return false;
 profiles->add(sketchProfile);

 // Create two cylinders with ExtrudeFeature using the profiles above.
 Ptr<Features> features = rootComp->features();
 if(!features)
  return false;
 Ptr<ExtrudeFeatures> extrudeFeats = features->extrudeFeatures();
 if(!extrudeFeats)
  return false;
 Ptr<ValueInput> distance = adsk::core::ValueInput::createByReal(2.0);
 if(!distance)
  return false;
 Ptr<ExtrudeFeature> extrudeFeature = extrudeFeats->addSimple(profiles,
                 distance,
                 adsk::fusion::FeatureOperations::NewBodyFeatureOperation);

 // Create a collection of entities for move
 Ptr<BRepBodies> brepBodies = extrudeFeature->bodies();
 if(!brepBodies)
  return false;
 Ptr<BRepBody> brepBody = brepBodies->item(0);
 if(!brepBody)
  return false;
 Ptr<ObjectCollection> entities1 = adsk::core::ObjectCollection::create();
 if(!entities1)
  return false;
 entities1->add(brepBody);

 // Create a transform to do move
 Ptr<Vector3D> vector = adsk::core::Vector3D::create(0.0, 10.0, 0.0);
 if(!vector)
  return false;
 Ptr<Matrix3D> transform = adsk::core::Matrix3D::create();
 if(!transform)
  return false;
 transform->translation(vector);

 // Create a move feature
 Ptr<MoveFeatures> moveFeats = features->moveFeatures();
 if(!moveFeats)
  return false;
 Ptr<MoveFeatureInput> moveFeatureInput = moveFeats->createInput(entities1, transform);
 if(!moveFeatureInput)
  return false;
 Ptr<MoveFeature> moveFeature = moveFeats->add(moveFeatureInput);
 if(!moveFeature)
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