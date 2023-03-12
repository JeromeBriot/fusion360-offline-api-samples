#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Construction/ConstructionPlanes.h>
#include <Fusion/Construction/ConstructionPlaneInput.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/HoleFeature.h>
#include <Fusion/Features/HoleFeatures.h>
#include <Fusion/Features/HoleFeatureInput.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCurves.h>
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

 // Create sketch
 Ptr<Sketches> sketches = rootComp->sketches();
 if(!sketches)
  return false;
 Ptr<ConstructionPlane> xz = rootComp->xZConstructionPlane();
 if(!xz)
  return false;
 Ptr<Sketch> sketch = sketches->add(xz);
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
 Ptr<SketchCircle> circle = sketchCircles->addByCenterRadius(centerPoint, 3.0);
 if(!circle)
  return false;

 // Get the profile defined by the circle.
 Ptr<Profiles> profs = sketch->profiles();
 if(!profs)
  return false;
 Ptr<Profile> prof = profs->item(0);
 if(!prof)
  return false;

 // Create an extrusion input
 Ptr<Features> feats = rootComp->features();
 if(!feats)
  return false;
 Ptr<ExtrudeFeatures> extrudes = feats->extrudeFeatures();
 if(!extrudes)
  return false;
 Ptr<ExtrudeFeatureInput> extInput = extrudes->createInput(prof, FeatureOperations::NewBodyFeatureOperation);
 if(!extInput)
  return false;

 // Define that the extent is a distance extent of 5 cm.
 Ptr<ValueInput> distance = ValueInput::createByReal(5);
 if(!distance)
  return false;
 extInput->setDistanceExtent(false, distance);

 // Create the extrusion.
 Ptr<ExtrudeFeature> ext = extrudes->add(extInput);
 if(!ext)
  return false;

 // Get the end face of the extrusion
 Ptr<BRepFaces> endFaces = ext->endFaces();
 if(!endFaces)
  return false;
 Ptr<BRepFace> endFace = endFaces->item(0);
 if(!endFace)
  return false;

 // Create a construction plane by offsetting the end face
 Ptr<ConstructionPlanes> planes = rootComp->constructionPlanes();
 if(!planes)
  return false;
 Ptr<ConstructionPlaneInput> planeInput = planes->createInput();
 if(!planeInput)
  return false;
 Ptr<ValueInput> offsetVal = ValueInput::createByString("2 cm");
 if(!offsetVal)
  return false;
 planeInput->setByOffset(endFace, offsetVal);
 Ptr<ConstructionPlane> offsetPlane = planes->add(planeInput);
 if(!offsetPlane)
  return false;

 // Create a sketch on the new construction plane and add four sketch points on it
 Ptr<Sketch> offsetSketch = sketches->add(offsetPlane);
 if(!offsetSketch)
  return false;
 Ptr<SketchPoints> offsetSketchPoints = offsetSketch->sketchPoints();
 if(!offsetSketchPoints)
  return false;
 Ptr<Point3D> p0 = Point3D::create(1, 0, 0);
 if(!p0)
  return false;
 Ptr<SketchPoint> sPt0 = offsetSketchPoints->add(p0);
 if(!sPt0)
  return false;
 Ptr<Point3D> p1 = Point3D::create(0, 1, 0);
 if(!p1)
  return false;
 Ptr<SketchPoint> sPt1 = offsetSketchPoints->add(p1);
 if(!sPt1)
  return false;
 Ptr<Point3D> p2 = Point3D::create(-1, 0, 0);
 if(!p2)
  return false;
 Ptr<SketchPoint> sPt2 = offsetSketchPoints->add(p2);
 if(!sPt2)
  return false;
 Ptr<Point3D> p3 = Point3D::create(0, -1, 0);
 if(!p3)
  return false;
 Ptr<SketchPoint> sPt3 = offsetSketchPoints->add(p3);
 if(!sPt3)
  return false;

 // Add the four sketch points into a collection
 Ptr<ObjectCollection> ptColl = ObjectCollection::create();
 if(!ptColl)
  return false;
 ptColl->add(sPt0);
 ptColl->add(sPt1);
 ptColl->add(sPt2);
 ptColl->add(sPt3);

 // Create a hole input
 Ptr<HoleFeatures> holes = feats->holeFeatures();
 if(!holes)
  return false;
 Ptr<HoleFeatureInput> holeInput = holes->createSimpleInput(ValueInput::createByString("2 mm"));
 if(!holeInput)
  return false;
 holeInput->setPositionBySketchPoints(ptColl);
 holeInput->setDistanceExtent(distance);

 Ptr<HoleFeature> hole = holes->add(holeInput);
 if(!hole)
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