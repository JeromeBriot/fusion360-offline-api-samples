#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchPoints.h>
#include <Fusion/Sketch/SketchPoint.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/Fusion/AreaProperties.h>


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

 Ptr<Documents> docs = app->documents();
 if (!docs)
  return false;

 // Create a document.
 Ptr<Document> doc = docs->add(DocumentTypes::FusionDesignDocumentType);
 if (!doc)
  return false;

 Ptr<Design> design = app->activeProduct();
 if (!design)
  return false;

 // Get the root component of the active design
 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 // Create sketch
 Ptr<Sketches> sketches = rootComp->sketches();
 if (!sketches)
  return false;

 Ptr<Sketch> sketch = sketches->add(rootComp->xYConstructionPlane());
 if (!sketch)
  return false;

 // Create two sketch circles
 Ptr<SketchCurves> curves = sketch->sketchCurves();
 if (!curves)
  return false;

 Ptr<SketchCircles> circles = curves->sketchCircles();
 if (!circles)
  return false;
 Ptr<Point3D> centerPoint = Point3D::create(0, 0, 0);
 circles->addByCenterRadius(centerPoint, 5.0);
 Ptr<Point3D> centerPointTwo = Point3D::create(10.0, 0, 0);
 circles->addByCenterRadius(centerPointTwo, 5.0);

 // Get the profiles defined by the circle
 Ptr<Profiles> profs = sketch->profiles();
 if (!profs)
  return false;

 Ptr<Profile> prof = profs->item(0);
 Ptr<Profile> profTwo = profs->item(1);

 // Get area properties from a profile
 Ptr<AreaProperties> areaProps = prof->areaProperties(CalculationAccuracy::LowCalculationAccuracy);
 if (!areaProps)
  return false;

 // Get area
 double area = areaProps->area();

 // Get centroid
 Ptr<Point3D> centroid = areaProps->centroid();

 // Get perimeter
 double perimeter = areaProps->perimeter();

 // Get angle of rotation of the principal axes
 double rotationOfPrincipal = areaProps->rotationToPrincipal();

 // Accuracy
 CalculationAccuracy accuracy = areaProps->accuracy();

 // Get area properties from two profiles
 Ptr<ObjectCollection> inputs = ObjectCollection::create();
 if (!inputs)
  return false;
 inputs->add(prof);
 inputs->add(profTwo);
 areaProps = design->areaProperties(inputs);
 if (!areaProps)
  return false;

 // Get principal axes
 Ptr<Vector3D> xAxis;
 Ptr<Vector3D> yAxis;
 areaProps->getPrincipalAxes(xAxis, yAxis);

 // Get moments of inertia
 double ixx = 0, iyy = 0, izz = 0, ixy = 0, iyz = 0, ixz = 0;
 areaProps->getMomentsOfInertia(ixx, iyy, izz, ixy, iyz, ixz);

 // Get centroid moments of inertia
 double centixx = 0, centiyy = 0, centizz = 0, centixy = 0, centiyz = 0, centixz = 0;
 areaProps->getCentroidMomentsOfInertia(centixx, centiyy, centizz, centixy, centiyz, centixz);

 // Get principal moments of inertia
 double i1 = 0, i2 = 0, i3 = 0;
 areaProps->getPrincipalMomentsOfInertia(i1, i2, i3);

 // Get radius of gyration
 double kxx = 0, kyy = 0, kzz = 0;
 areaProps->getRadiusOfGyration(kxx, kyy, kzz);

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