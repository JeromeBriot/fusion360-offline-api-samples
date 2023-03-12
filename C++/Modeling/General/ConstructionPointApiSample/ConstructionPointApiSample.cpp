#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Construction/ConstructionAxes.h>
#include <Fusion/Construction/ConstructionAxis.h>
#include <Fusion/Construction/ConstructionAxisInput.h>
#include <Fusion/Construction/ConstructionPoints.h>
#include <Fusion/Construction/ConstructionPointInput.h>
#include <Fusion/Construction/ConstructionPoint.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchPoints.h>
#include <Fusion/Sketch/SketchPoint.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepVertices.h>
#include <Fusion/BRep/BRepVertex.h>


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

 // Get sketch lines
 Ptr<SketchCurves> curves = sketch->sketchCurves();
 if (!curves)
  return false;
 Ptr<SketchLines> sketchLines = curves->sketchLines();
 if (!sketchLines)
  return false;

 // Create sketch rectangle
 Ptr<Point3D> startPoint = Point3D::create(0, 0, 0);
 Ptr<Point3D> endPoint = Point3D::create(5.0, 5.0, 0);
 sketchLines->addTwoPointRectangle(startPoint, endPoint);

 // Get two sketch lines
 Ptr<SketchLine> sketchLineOne = sketchLines->item(0);
 Ptr<SketchLine> sketchLineTwo = sketchLines->item(1);

 // Get the profile
 Ptr<Profiles> profs = sketch->profiles();
 if (!profs)
  return false;
 Ptr<Profile> prof = profs->item(0);

 // Create an extrusion input
 Ptr<Features> feats = rootComp->features();
 if (!feats)
  return false;
 Ptr<ExtrudeFeatures> extrudes = feats->extrudeFeatures();
 if (!extrudes)
  return false;
 Ptr<ExtrudeFeatureInput> extInput = extrudes->createInput(prof, FeatureOperations::NewBodyFeatureOperation);

 // Define that the extent is a distance extent of 5 cm
 Ptr<ValueInput> distance = ValueInput::createByReal(5.0);
 // Set the distance extent
 extInput->setDistanceExtent(false, distance);
 // Set the extrude type to be solid
 extInput->isSolid(true);

 // Create the extrusion
 Ptr<ExtrudeFeature> ext = extrudes->add(extInput);
 if (!ext)
  return false;

 // Create a sketch circle
 Ptr<SketchCircles> sketchCircles = curves->sketchCircles();
 if (!sketchCircles)
  return false;
 Ptr<Point3D> centerPoint = Point3D::create(8.0, 8.0, 0);
 Ptr<SketchCircle> sketchCircle = sketchCircles->addByCenterRadius(centerPoint, 3.0);

 // Get the body with the extrude
 Ptr<BRepBodies> bodies = ext->bodies();
 if (!bodies)
  return false;
 Ptr<BRepBody> body = bodies->item(0);

 // Get a vertex of the body
 Ptr<BRepVertices> vertices = body->vertices();
 if (!vertices)
  return false;
 Ptr<BRepVertex> vertex = vertices->item(0);

 // Get three intersect faces
 Ptr<BRepFaces> vertexFaces = vertex->faces();
 if (!vertexFaces)
  return false;
 Ptr<BRepFace> faceOne = vertexFaces->item(0);
 Ptr<BRepFace> faceTwo = vertexFaces->item(1);
 Ptr<BRepFace> faceThree = vertexFaces->item(2);

 // Create perpendicular construction axis
 Ptr<ConstructionAxes> axes = rootComp->constructionAxes();
 if (!axes)
  return false;
 Ptr<ConstructionAxisInput> axisInput = axes->createInput();
 if (!axisInput)
  return false;
 axisInput->setByPerpendicularAtPoint(faceOne, vertex);
 Ptr<ConstructionAxis> axis = axes->add(axisInput);

 // Get construction points
 Ptr<ConstructionPoints> constructionPoints = rootComp->constructionPoints();
 if (!constructionPoints)
  return false;

 // Create construction point input
 Ptr<ConstructionPointInput> pointInput = constructionPoints->createInput();
 if (!pointInput)
  return false;

 // Create construction point by two points
 pointInput->setByTwoEdges(sketchLineOne, sketchLineTwo);
 constructionPoints->add(pointInput);

 // Create construction point by three planes
 pointInput->setByThreePlanes(faceOne, faceTwo, faceThree);
 constructionPoints->add(pointInput);

 // Create construction point by edge and plane
 pointInput->setByEdgePlane(axis, faceOne);
 constructionPoints->add(pointInput);

 // Create construction point by center
 pointInput->setByCenter(sketchCircle);
 constructionPoints->add(pointInput);

 // Create construction point by point
 pointInput->setByPoint(vertex);
 Ptr<ConstructionPoint> point = constructionPoints->add(pointInput);

 // Get health state of a construction point
 adsk::fusion::FeatureHealthStates health = point->healthState();
 if (health == adsk::fusion::FeatureHealthStates::ErrorFeatureHealthState
  || health == adsk::fusion::FeatureHealthStates::WarningFeatureHealthState) {
  std::string msg = point->errorOrWarningMessage();
 }

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