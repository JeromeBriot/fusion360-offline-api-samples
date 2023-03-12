#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Construction/ConstructionPoint.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Core/Geometry/Point3D.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Core/Application/ValueInput.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/BRep/BRepVertices.h>
#include <Fusion/BRep/BRepVertex.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/BRep/BRepFace.h>
#include <Core/Application/MeasureManager.h>
#include <Core/Application/MeasureResults.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/Geometry/OrientedBoundingBox3D.h>


using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;
Ptr<UserInterface> ui;

extern "C" XI_EXPORT bool run(const char* context)
{
 app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;

 // Create a new document
 Ptr<Documents> docs = app->documents();
 if (!docs)
  return false;

 Ptr<Document> doc = docs->add(DocumentTypes::FusionDesignDocumentType);
 if (!doc)
  return false;

 Ptr<Design> design = app->activeProduct();
 if (!design)
  return false;

 //Get the root component of active design
 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 //Create a new sketch on the yz plane
 Ptr<Sketches> sketches = rootComp->sketches();
 if (!sketches)
  return false;

 Ptr<ConstructionPlane> yzPlane = rootComp->yZConstructionPlane();
 if (!yzPlane)
  return false;

 Ptr<Sketch> sketch1 = sketches->add(yzPlane);
 if (!sketch1)
  return false;

 //Create sketch lines
 Ptr<SketchCurves> sketchCurves = sketch1->sketchCurves();
 if (!sketchCurves)
  return false;

 Ptr<SketchLines> sketchLines = sketchCurves->sketchLines();
 if (!sketchLines)
  return false;

 //Create sketch rectangle
 Ptr<Point3D> point1 = Point3D::create(0.0, 0.0, 0.0);
 Ptr<Point3D> point2 = Point3D::create(5.0, 5.0, 0.0);
 sketchLines->addTwoPointRectangle(point1, point2);

 //Get the profile
 Ptr<Profiles> profiles = sketch1->profiles();
 if (!profiles)
  return false;

 Ptr<Profile> profile = profiles->item(0);
 if (!profile)
  return false;

 //Create an extrusion input
 Ptr<Features> features = rootComp->features();
 if (!features)
  return false;

 Ptr<ExtrudeFeatures> extrudes = features->extrudeFeatures();
 if (!extrudes)
  return false;

 Ptr<ExtrudeFeatureInput> extrudeInput = extrudes->createInput(profile, FeatureOperations::NewBodyFeatureOperation);

 //Set extrude distance is 6 cm
 Ptr<ValueInput> distance = ValueInput::createByReal(6.0);
 //Set the distance extent
 extrudeInput->setDistanceExtent(false, distance);
 //Set the extrude type to be solid
 extrudeInput->isSolid(true);

 //Create the extrude
 Ptr<ExtrudeFeature> extrude = extrudes->add(extrudeInput);
 if (!extrude)
  return false;

 //Get the extrude brepbody
 Ptr<BRepBodies> brepBodies = extrude->bodies();
 if (!brepBodies)
  return false;

 Ptr<BRepBody> brepBody = brepBodies->item(0);
 if (!brepBody)
  return false;

 //Get one sketch line
 Ptr<SketchLine> sketchLine = sketchLines->item(0);
 if (!sketchLine)
  return false;

 //Get one brepEdge
 Ptr<BRepEdges> brepEdges = brepBody->edges();
 if (!brepEdges)
  return false;

 Ptr<BRepEdge> brepEdge = brepEdges->item(0);
 if (!brepEdge)
  return false;

 //Get measure manager
 Ptr<MeasureManager> measureMgr = app->measureManager();
 if (!measureMgr)
  return false;

 //Measure minimum distance
 Ptr<MeasureResults> results = measureMgr->measureMinimumDistance(sketchLine, brepEdge);
 if (!results)
  return false;

 double minIdstance = results->value();

 Ptr<Point3D> position1 = results->positionOne();
 if (!position1)
  return false;

 Ptr<Point3D> position2 = results->positionTwo();
 if (!position2)
  return false;

 //Get root component origin point
 Ptr<ConstructionPoint> constructionPoint = rootComp->originConstructionPoint();
 if (!constructionPoint)
  return false;

 //Get a brep Vertex
 Ptr<BRepVertices> brepVertices = brepBody->vertices();
 if (!brepVertices)
  return false;

 Ptr<BRepVertex> brepVertex = brepVertices->item(0);
 if (!brepVertex)
  return false;

 //Measure angle for three point
 results = measureMgr->measureAngle(constructionPoint, point2, brepVertex);
 if (!results)
  return false;

 double angle = results->value();

 position1 = results->positionOne();
 if (!position1)
  return false;

 position2 = results->positionTwo();
 if (!position2)
  return false;

 Ptr<Point3D> position3 = results->positionThree();
 if (!position3)
  return false;

 //Get a brep face
 Ptr<BRepFaces> brepFaces = brepBody->faces();
 if (!brepFaces)
  return false;

 Ptr<BRepFace> brepFace = brepFaces->item(1);
 if (!brepFace)
  return false;

 //Measure angle for two objects
 results = measureMgr->measureAngle(brepFace, brepEdge);
 if (!results)
  return false;

 angle = results->value();

 position1 = results->positionOne();
 if (!position1)
  return false;

 position2 = results->positionTwo();
 if (!position2)
  return false;

 position3 = results->positionThree();
 if (!position3)
  return false;

 //Get brep body's oriented bounding box
 Ptr<Vector3D> vector1 = Vector3D::create(0.0, 0.0, 1.0);
 Ptr<Vector3D> vector2 = Vector3D::create(0.0, 1.0, 0.0);

 Ptr<OrientedBoundingBox3D> boundingBox = measureMgr->getOrientedBoundingBox(brepBody, vector1, vector2);
 if (!boundingBox)
  return false;

 //Get the bounding box length, width and height
 double length = boundingBox->length();

 double width = boundingBox->width();

 double height = boundingBox->height();

 //Get the bounding box width direction
 Ptr<Vector3D> widthDir = boundingBox->widthDirection();
 if (!widthDir)
  return false;

 //Get the bounding box length direction
 Ptr<Vector3D> lengthDir = boundingBox->lengthDirection();
 if (!lengthDir)
  return false;

 //Get the bounding box height direction
 Ptr<Vector3D> heightDir = boundingBox->heightDirection();
 if (!heightDir)
  return false;

 //Get the bounding contain a point or not
 Ptr<Point3D> pointA = Point3D::create(-2.0, 0.5, 2.0);
 bool isContain = boundingBox->contains(pointA);

 Ptr<Point3D> pointB = Point3D::create(0.0, 0.0, 0.0);
 isContain = boundingBox->contains(pointB);

 //Copy the bounding box
 Ptr<OrientedBoundingBox3D> boudingBoxCopy = boundingBox->copy();
 if (!boudingBoxCopy)
  return false;

 //Set bounding box height
 boudingBoxCopy->height(10.0);
 double newHeight = boudingBoxCopy->height();

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