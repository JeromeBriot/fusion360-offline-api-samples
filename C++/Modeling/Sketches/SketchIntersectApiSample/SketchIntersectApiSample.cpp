#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/ObjectCollection.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlanes.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Construction/ConstructionPlaneInput.h>
#include <Fusion/Construction/ConstructionAxes.h>
#include <Fusion/Construction/ConstructionAxis.h>
#include <Fusion/Construction/ConstructionAxisInput.h>
#include <Fusion/Construction/ConstructionPoints.h>
#include <Fusion/Construction/ConstructionPointInput.h>
#include <Fusion/Construction/ConstructionPoint.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchFittedSplines.h>
#include <Fusion/Sketch/SketchFittedSpline.h>
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

 // Create a new sketch on the yz plane.
 Ptr<Sketches> sketches = rootComp->sketches();
 if (!sketches)
  return false;

 Ptr<ConstructionPlane> yzPlane = rootComp->yZConstructionPlane();
 if (!yzPlane)
  return false;

 Ptr<Sketch> yzSketch = sketches->add(yzPlane);
 if (!yzSketch)
  return false;

 // Create an object collection for the points.
 Ptr<ObjectCollection> points = ObjectCollection::create();
 if (!points)
  return false;

 // Define the points the spline with fit through.
 points->add(Point3D::create(-5, 0, 0));
 points->add(Point3D::create(5, 1, 0));
 points->add(Point3D::create(6, 4, 3));
 points->add(Point3D::create(7, 6, 6));
 points->add(Point3D::create(2, 3, 0));
 points->add(Point3D::create(0, 1, 0));

 // Create the spline.
 Ptr<SketchCurves> sketchCurves = yzSketch->sketchCurves();
 if (!sketchCurves)
  return false;

 Ptr<SketchFittedSplines> splines = sketchCurves->sketchFittedSplines();
 if (!splines)
  return false;

 Ptr<SketchFittedSpline>  spline = splines->add(points);
 if (!spline)
  return false;

 // Get sketch lines
 Ptr<SketchLines> sketchLines = sketchCurves->sketchLines();
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
 Ptr<Profiles> profs = yzSketch->profiles();
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

 // Get the body with the extrude
 Ptr<BRepBodies> bodies = ext->bodies();
 if (!bodies)
  return false;

 Ptr<BRepBody> body = bodies->item(0);

 // Get a vertex of the body
 Ptr<BRepVertices> vertices = body->vertices();
 if (!vertices)
  return false;

 Ptr<BRepVertex> vertex = vertices->item(5);

 // Get a face of the vertex
 Ptr<BRepFaces> vertexFaces = vertex->faces();
 if (!vertexFaces)
  return false;
 Ptr<BRepFace> face = vertexFaces->item(0);

 // Get construction axes
 Ptr<ConstructionAxes> constructionAxes = rootComp->constructionAxes();
 if (!constructionAxes)
  return false;

 // Create construction axis input
 Ptr<ConstructionAxisInput> axisInput = constructionAxes->createInput();
 if (!axisInput)
  return false;

 // Create perpendicular construction axis
 axisInput->setByPerpendicularAtPoint(face, vertex);
 Ptr<ConstructionAxis> axis = constructionAxes->add(axisInput);

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
 Ptr<ConstructionPoint> point = constructionPoints->add(pointInput);

 // Create construction plane
 Ptr<ConstructionPlanes> planes = rootComp->constructionPlanes();
 if (!planes)
  return false;

 // Create construction plane input
 Ptr<ConstructionPlaneInput> planeInput = planes->createInput();
 if (!planeInput)
  return false;

 // Add construction plane by offset
 Ptr<ValueInput> offsetValue = ValueInput::createByReal(3.0);
 planeInput->setByOffset(prof, offsetValue);
 Ptr<ConstructionPlane> plane = planes->add(planeInput);

 // Create a new sketch on the xz plane.
 Ptr<ConstructionPlane> xzPlane = rootComp->xZConstructionPlane();
 if (!xzPlane)
  return false;

 Ptr<Sketch> xzSketch = sketches->add(xzPlane);
 if (!xzSketch)
  return false;

 std::vector<Ptr<Base>> entities;
 entities.push_back(body);
 entities.push_back(face);
 entities.push_back(sketchLineOne);
 entities.push_back(vertex);
 entities.push_back(spline);
 entities.push_back(axis);
 entities.push_back(point);
 entities.push_back(plane);

 std::vector<Ptr<SketchEntity>> resVec = xzSketch->intersectWithSketchPlane(entities);
 if (resVec.empty())
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