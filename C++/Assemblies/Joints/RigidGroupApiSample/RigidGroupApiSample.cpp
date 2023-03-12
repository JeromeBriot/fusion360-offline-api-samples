#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Application/Viewport.h>
#include <Core/Application/Camera.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Matrix3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Construction/ConstructionPlanes.h>
#include <Fusion/Construction/ConstructionPlaneInput.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Components/RigidGroups.h>
#include <Fusion/Components/RigidGroup.h>
#include <Fusion/Components/Occurrence.h>
#include <Fusion/Components/Occurrences.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCurves.h>

using namespace adsk::core;
using namespace adsk::fusion;


Ptr<Application> app;
Ptr<UserInterface> ui;

bool CreateCylinder(Ptr<Occurrence> occ, double diameter, double height, bool isSolid)
{
 Ptr<Component> comp = occ->component();
 if(!comp)
  return false;
 Ptr<Sketches> sketches = comp->sketches();
 if(!sketches)
  return false;
 Ptr<ConstructionPlane> xzPlane = comp->xZConstructionPlane();
 if(!xzPlane)
  return false;
 Ptr<Sketch> sketch = sketches->add(xzPlane);
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
 Ptr<SketchCircle> sketchCircle = sketchCircles->addByCenterRadius(centerPoint, diameter);
 if(!sketchCircle)
  return false;

 // Get the profile defined by the circle
 Ptr<Profiles> profs = sketch->profiles();
 if(!profs)
  return false;
 Ptr<Profile> profile = profs->item(0);
 if(!profile)
  return false;

 // Create an extrude input
 Ptr<Features> feats = comp->features();
 if(!feats)
  return false;
 Ptr<ExtrudeFeatures> extrudes = feats->extrudeFeatures();
 if(!extrudes)
  return false;
 Ptr<ExtrudeFeatureInput> extInput = extrudes->createInput(profile, FeatureOperations::NewBodyFeatureOperation);
 if(!extInput)
  return false;

 // Set the extrude input
 Ptr<ValueInput> distance = ValueInput::createByReal(height);
 if(!distance)
  return false;
 extInput->setDistanceExtent(false, distance);
 extInput->isSolid(isSolid);

 // Create the extrude
 Ptr<ExtrudeFeature> extrude = extrudes->add(extInput);
 if(!extrude)
  return false;

 return true;
}

extern "C" XI_EXPORT bool run(const char* context)
{
 app = Application::get();
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

 // Create four sub components under root component
 Ptr<Occurrences> occs = rootComp->occurrences();
 if(!occs)
  return false;

 // sub component 1
 Ptr<Matrix3D> transform0 = adsk::core::Matrix3D::create();
 if(!transform0)
  return false;
 Ptr<Vector3D> vector3d0 = adsk::core::Vector3D::create(10.0, 0.0, 0.0);
 if(!vector3d0)
  return false;
 transform0->translation(vector3d0);
 Ptr<Occurrence> subOcc0 = occs->addNewComponent(transform0);
 if(!subOcc0)
  return false;

 // sub component 2
 Ptr<Matrix3D> transform1 = adsk::core::Matrix3D::create();
 if(!transform1)
  return false;
 Ptr<Vector3D> vector3d1 = adsk::core::Vector3D::create(0.0, 0.0, 12.0);
 if(!vector3d1)
  return false;
 transform1->translation(vector3d1);
 Ptr<Occurrence> subOcc1 = occs->addNewComponent(transform1);
 if(!subOcc1)
  return false;

 // sub component 3
 Ptr<Matrix3D> transform2 = adsk::core::Matrix3D::create();
 if(!transform2)
  return false;
 Ptr<Vector3D> vector3d2 = adsk::core::Vector3D::create(-8.0, 0.0, 0.0);
 if(!vector3d2)
  return false;
 transform2->translation(vector3d2);
 Ptr<Occurrence> subOcc2 = occs->addNewComponent(transform2);
 if(!subOcc2)
  return false;

 // sub component 4
 Ptr<Matrix3D> transform3 = adsk::core::Matrix3D::create();
 if(!transform3)
  return false;
 Ptr<Vector3D> vector3d3 = adsk::core::Vector3D::create(0.0, 0.0, -6.0);
 if(!vector3d3)
  return false;
 transform3->translation(vector3d3);
 Ptr<Occurrence> subOcc3 = occs->addNewComponent(transform3);
 if(!subOcc3)
  return false;

 // Create an object collection of occurrences
 Ptr<ObjectCollection> occGroups = adsk::core::ObjectCollection::create();
 if(!occGroups)
  return false;
 occGroups->add(subOcc0);
 occGroups->add(subOcc1);
 occGroups->add(subOcc2);
 occGroups->add(subOcc3);

 // Create four cylinders in four sub components
 bool result0 = CreateCylinder(subOcc0, 0.5, 5, false);
 if(!result0)
  return false;

 bool result1 = CreateCylinder(subOcc1, 0.75, 7.5, false);
 if(!result1)
  return false;

 bool result2 = CreateCylinder(subOcc2, 1.0, 10, false);
 if(!result2)
  return false;

 bool result3 = CreateCylinder(subOcc3, 1.25, 12.5, false);
 if(!result3)
  return false;

 // Create a RigidGroup
 bool isIncludeChildren = true;
 Ptr<RigidGroups> rigidGroups_ = rootComp->rigidGroups();
 if(!rigidGroups_)
  return false;
 Ptr<RigidGroup> rigidGroup = rigidGroups_->add(occGroups, isIncludeChildren);
 if(!rigidGroup)
  return false;

 // Fit to window
 Ptr<Viewport> viewPort = app->activeViewport();
 if(!viewPort)
  return false;
 Ptr<Camera> cam = viewPort->camera();
 if(!cam)
  return false;
 cam->isFitView(true);
 viewPort->camera(cam);


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