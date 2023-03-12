#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Geometry/Matrix3D.h>
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
#include <Fusion/Sketch/SketchCurve.h>
#include <Fusion/Sketch/SketchPoints.h>
#include <Fusion/Sketch/SketchPoint.h>
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
#include <Fusion/Features/DistanceExtentDefinition.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/Components/Occurrence.h>
#include <Fusion/Components/Occurrences.h>
#include <Fusion/Components/Component.h>



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

 // Create three sub components under root component
 Ptr<Occurrences> occs = rootComp->occurrences();
 if (!occs)
  return false;

 Ptr<Matrix3D> transform = adsk::core::Matrix3D::create();
 if (!transform)
  return false;

 Ptr<Occurrence> subOcc1 = occs->addNewComponent(transform);
 if (!subOcc1)
  return false;

 Ptr<Component> subComp1 = subOcc1->component();
 if (!subComp1)
  return false;

 Ptr<Occurrence> subOcc2 = occs->addNewComponent(transform);
 if (!subOcc2)
  return false;

 Ptr<Component> subComp2 = subOcc2->component();
 if (!subComp2)
  return false;

 Ptr<Occurrence> subOcc3 = occs->addNewComponent(transform);
 if (!subOcc3)
  return false;

 Ptr<Component> subComp3 = subOcc3->component();
 if (!subComp3)
  return false;

 // Create sketch 1 in sub component 1
 Ptr<Sketches> sketches1 = subComp1->sketches();
 if (!sketches1)
  return false;

 Ptr<ConstructionPlane> yzPlane = rootComp->yZConstructionPlane();
 if (!yzPlane)
  return false;

 Ptr<Sketch> sketch1 = sketches1->add(yzPlane);
 if (!sketch1)
  return false;

 // Get sketch curves
 Ptr<SketchCurves> sketchCurves = sketch1->sketchCurves();
 if (!sketchCurves)
  return false;

 // Get sketch lines
 Ptr<SketchLines> sketchLines = sketchCurves->sketchLines();
 if (!sketchLines)
  return false;

 // Create sketch rectangle
 Ptr<Point3D> startPoint = Point3D::create(-8.0, 0, 0);

 Ptr<Point3D> endPoint = Point3D::create(8.0, 8.0, 0);
 sketchLines->addTwoPointRectangle(startPoint, endPoint);

 // Get the profile
 Ptr<Profiles> profs1 = sketch1->profiles();
 if (!profs1)
  return false;

 Ptr<Profile> prof1 = profs1->item(0);

 // Create an extrusion input
 Ptr<Features> feats1 = subComp1->features();
 if (!feats1)
  return false;

 Ptr<ExtrudeFeatures> extrudes1 = feats1->extrudeFeatures();
 if (!extrudes1)
  return false;

 Ptr<ExtrudeFeatureInput> extInput1 = extrudes1->createInput(prof1, FeatureOperations::NewBodyFeatureOperation);

 // Define that the extent is a distance extent of 2 cm
 Ptr<ValueInput> distance1 = ValueInput::createByReal(2.0);
 // Set the distance extent
 extInput1->setDistanceExtent(false, distance1);
 // Set the extrude type to be solid
 extInput1->isSolid(true);

 // Create the extrusion
 Ptr<ExtrudeFeature> ext1 = extrudes1->add(extInput1);
 if (!ext1)
  return false;

 // Create construction plane
 Ptr<ConstructionPlanes> planes = rootComp->constructionPlanes();
 if (!planes)
  return false;

 // Create construction plane input
 Ptr<ConstructionPlaneInput> planeInput = planes->createInput();
 if (!planeInput)
  return false;

 // Add construction plane by offset
 Ptr<ValueInput> offsetValue = ValueInput::createByReal(8.0);
 planeInput->setByOffset(rootComp->yZConstructionPlane(), offsetValue);
 Ptr<ConstructionPlane> plane = planes->add(planeInput);

 // Create sketch 2 in sub component 2
 Ptr<Sketches> sketches2 = subComp2->sketches();
 if (!sketches2)
  return false;

 Ptr<Sketch> sketch2 = sketches2->add(plane);
 if (!sketch2)
  return false;

 // Create an object collection for the points.
 Ptr<ObjectCollection> points = ObjectCollection::create();
 if (!points)
  return false;

 // Define the points the spline with fit through.
 points->add(Point3D::create(0, 8, 0));
 points->add(Point3D::create(5, 6, 0));
 points->add(Point3D::create(-5, 5, 0));

 // Get sketch curves
 Ptr<SketchCurves> sketch2Curves = sketch2->sketchCurves();
 if (!sketch2Curves)
  return false;

 // Create the spline.
 Ptr<SketchFittedSplines> splines = sketch2Curves->sketchFittedSplines();
 if (!splines)
  return false;

 Ptr<SketchFittedSpline>  spline = splines->add(points);
 if (!spline)
  return false;

 // Get sketch lines
 Ptr<SketchLines> sketch2Lines = sketch2Curves->sketchLines();
 if (!sketch2Lines)
  return false;

 // Create sketch rectangle
 Ptr<Point3D> startPoint2 = Point3D::create(-4, 2, 0);

 Ptr<Point3D> endPoint2 = Point3D::create(3, 4, 0);
 sketch2Lines->addTwoPointRectangle(startPoint2, endPoint2);

 // Get the profile
 Ptr<Profiles> profs2 = sketch2->profiles();
 if (!profs2)
  return false;

 Ptr<Profile> prof2 = profs2->item(0);

 // Create an extrusion input
 Ptr<Features> feats2 = subComp2->features();
 if (!feats2)
  return false;

 Ptr<ExtrudeFeatures> extrudes2 = feats2->extrudeFeatures();
 if (!extrudes2)
  return false;

 Ptr<ExtrudeFeatureInput> extInput2 = extrudes2->createInput(prof2, FeatureOperations::NewBodyFeatureOperation);

 // Create distance value input
 Ptr<ValueInput> cm2 = adsk::core::ValueInput::createByString("2 cm");

 // Create taper angle value input
 Ptr<ValueInput> deg10 = adsk::core::ValueInput::createByString("10 deg");

 // Create a distance extent definition
 Ptr<DistanceExtentDefinition> extent_distance_2 = adsk::fusion::DistanceExtentDefinition::create(cm2);
 
 // Set the extrude type to be solid
 extInput2->isSolid(true);

 // Create the extrusion
 Ptr<ExtrudeFeature> ext2 = extrudes2->add(extInput2);
 if (!ext2)
  return false;

 // Get the body with the first extrude
 Ptr<BRepBodies> bodies = ext1->bodies();
 if (!bodies)
  return false;

 Ptr<BRepBody> body = bodies->item(0);
 if (!body)
  return false;
 
 // faces
 std::vector<Ptr<BRepFace>> faceVec;
 for (Ptr<BRepFace> face : body->faces()) {
  faceVec.push_back(face);
 }
 
 // curves
 std::vector<Ptr<Base>> curveVec;
 for (Ptr<SketchCurve> curve : sketch2Curves) {
  curveVec.push_back(curve);
 }

 Ptr<SketchPoints> sketch2Points = sketch2->sketchPoints();
 if (!sketch2Points)
  return false;
 
 for (Ptr<SketchPoint> point : sketch2Points) {
  curveVec.push_back(point);
 }

 // Get the body with the second extrude
 Ptr<BRepBodies> bodies2 = ext2->bodies();
 if (!bodies2)
  return false;

 Ptr<BRepBody> body2 = bodies2->item(0);
 if (!body2)
  return false;
 
 for (Ptr<BRepEdge> edge : body2->edges()) {
  curveVec.push_back(edge);
 }

 curveVec.push_back(rootComp->yConstructionAxis());
 curveVec.push_back(rootComp->originConstructionPoint());

 Ptr<Sketches> sketches3 = subComp3->sketches();
 if (!sketches3)
  return false;

 // Create a sketch in sub component 3
 Ptr<Sketch> skAlongVecProject = sketches3->add(yzPlane);
 if (!skAlongVecProject)
  return false;
 // sketch project to surface (along vector)
 std::vector<Ptr<SketchEntity>> projectedEntities = skAlongVecProject->projectToSurface(faceVec, curveVec, AlongVectorSurfaceProjectType, rootComp->xConstructionAxis());
 
 // Create a sketch in sub component 3
 Ptr<Sketch> skClosestPtProject = sketches3->add(yzPlane);
 projectedEntities.clear();
 // sketch project to surface (closest point)
 projectedEntities = skClosestPtProject->projectToSurface(faceVec, curveVec, ClosestPointSurfaceProjectType);


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