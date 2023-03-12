#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Matrix3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Fusion/PhysicalProperties.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Components/Occurrence.h>
#include <Fusion/Components/Occurrences.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>
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

 // Create sub occurrence
 Ptr<Occurrences> occurrences = rootComp->occurrences();
 if (!occurrences)
  return false;
 Ptr<Occurrence> subOcc = occurrences->addNewComponent(Matrix3D::create());
 if (!subOcc)
  return false;

 // Get features from sub component
 Ptr<Component> subComponent = subOcc->component();
 if (!subComponent)
  return false;
 Ptr<Features> features = subComponent->features();
 if (!features)
  return false;

 // Create sketch circle on the xz plane.
 Ptr<Sketches> sketches = subComponent->sketches();
 if(!sketches)
  return false;
 Ptr<Sketch> sketch = sketches->add(subComponent->xZConstructionPlane());
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
 Ptr<ExtrudeFeatures> extrudeFeats = features->extrudeFeatures();
 if(!extrudeFeats)
  return false;
 Ptr<ExtrudeFeatureInput> extrudeFeatureInput = extrudeFeats->createInput(entities0, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
 if(!extrudeFeatureInput)
  return false;
 Ptr<ValueInput> distance = adsk::core::ValueInput::createByReal(2.0);
 if(!distance)
  return false;
 extrudeFeatureInput->isSolid(true);
 extrudeFeatureInput->setDistanceExtent(false, distance);
 Ptr<ExtrudeFeature> extrudeFeature = extrudeFeats->add(extrudeFeatureInput);
 if(!extrudeFeature)
  return false;

 // Get physical properties from body
 Ptr<BRepBodies> bodies = extrudeFeature->bodies();
 if(!bodies)
  return false;
 Ptr<BRepBody> body = bodies->item(0);
 Ptr<PhysicalProperties> physicalProperties = body->physicalProperties();

 // Get physical properties from occurrence
 physicalProperties = subOcc->physicalProperties();

 // Get physical properties from occurrence (low accuracy)
 physicalProperties = subOcc->getPhysicalProperties(LowCalculationAccuracy);

 // Get physical properties from occurrence (medium accuracy)
 physicalProperties = subOcc->getPhysicalProperties(MediumCalculationAccuracy);

 // Get physical properties from occurrence (high accuracy)
 physicalProperties = subOcc->getPhysicalProperties(HighCalculationAccuracy);

 // Get physical properties from occurrence (very high accuracy)
 physicalProperties = subOcc->getPhysicalProperties(VeryHighCalculationAccuracy);

 // Get physical properties from component
 physicalProperties = subComponent->physicalProperties();

 // Get physical properties from component (low accuracy)
 physicalProperties = subComponent->getPhysicalProperties(CalculationAccuracy::LowCalculationAccuracy);

 // Get physical properties from component (medium accuracy)
 physicalProperties = subComponent->getPhysicalProperties(CalculationAccuracy::MediumCalculationAccuracy);

 // Get physical properties from component (high accuracy)
 physicalProperties = subComponent->getPhysicalProperties(CalculationAccuracy::HighCalculationAccuracy);

 // Get physical properties from component (very high accuracy)
 physicalProperties = subComponent->getPhysicalProperties(CalculationAccuracy::VeryHighCalculationAccuracy);
 if (!physicalProperties)
  return false;

 // Get data from physical properties
 double area = physicalProperties->area();
 double density = physicalProperties->density();
 double mass = physicalProperties->mass();
 double volume = physicalProperties->volume();

 //Get accuracy from physical properties
 CalculationAccuracy accuracy = physicalProperties->accuracy();

 //Get center of mass from physical properties
 Ptr<Point3D> cog = physicalProperties->centerOfMass();

 //Get principal axes from physical properties
 Ptr<Vector3D> xAxis;
 Ptr<Vector3D> yAxis;
 Ptr<Vector3D> zAxis;
 physicalProperties->getPrincipalAxes(xAxis, yAxis, zAxis);

 //Get the moments of inertia about the principal axes. Unit for returned values is kg/cm^2.
 double i1 = 0, i2 = 0, i3 = 0;
 physicalProperties->getPrincipalMomentsOfInertia(i1, i2, i3);

 //Get the radius of gyration about the principal axes. Unit for returned values is cm.
 double kx = 0, ky = 0, kz = 0;
 physicalProperties->getRadiusOfGyration(kx, ky, kz);

 //Get the rotation from the world coordinate system of the target to the principal coordinate system.
 double rx = 0, ry = 0, rz = 0;
 physicalProperties->getRotationToPrincipal(rx, ry, rz);

 //Get the moment of inertia about the world coordinate system.
 double xx = 0, yy = 0, zz = 0, xy = 0, yz = 0, xz = 0;
 physicalProperties->getXYZMomentsOfInertia(xx, yy, zz, xy, yz, xz);


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