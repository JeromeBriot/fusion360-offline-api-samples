#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Construction/ConstructionPlanes.h>
#include <Fusion/Construction/ConstructionPlaneInput.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/SplitFaceFeatureInput.h>
#include <Fusion/Features/SplitFaceFeatures.h>
#include <Fusion/Features/SplitFaceFeature.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/Sketches.h>


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

 // Create a sketch that has a rectangle in it
 Ptr<Sketches> sketches = rootComp->sketches();
 if (!sketches)
  return false;

 Ptr<Sketch> sketch = sketches->add(rootComp->xZConstructionPlane());
 if (!sketch)
  return false;

 Ptr<SketchCurves> curves = sketch->sketchCurves();
 if (!curves)
  return false;

 Ptr<SketchLines> sketchLines = curves->sketchLines();
 if (!sketchLines)
  return false;

 Ptr<Point3D> point0 = adsk::core::Point3D::create(0, 0, 0);
 if (!point0)
  return false;
 Ptr<Point3D> point1 = adsk::core::Point3D::create(10, 10, 0);
 if (!point1)
  return false;

 sketchLines->addTwoPointRectangle(point0, point1);

 // Get the profile defined by the circle
 Ptr<Profiles> profs = sketch->profiles();
 if (!profs)
  return false;

 Ptr<Profile> prof = profs->item(0);
 if (!prof)
  return false;

 // Create an extrusion input
 Ptr<Features> feats = rootComp->features();
 if (!feats)
  return false;

 Ptr<ExtrudeFeatures> extrudes = feats->extrudeFeatures();
 if (!extrudes)
  return false;

 Ptr<ExtrudeFeatureInput> extInput = extrudes->createInput(prof, FeatureOperations::NewBodyFeatureOperation);
 if (!extInput)
  return false;

 // Define that the extent is a distance extent of 5 cm
 Ptr<ValueInput> distance = ValueInput::createByReal(5);
 if (!distance)
  return false;

 extInput->setDistanceExtent(true, distance);

 // Create the extrusion
 Ptr<ExtrudeFeature> ext = extrudes->add(extInput);
 if (!ext)
  return false;

 // Get one edge of the extrusion body
 Ptr<BRepFaces> faces = ext->endFaces();
 if (!faces)
  return false;
 Ptr<BRepFace> face = faces->item(0);
 if (!face)
  return false;
 Ptr<BRepEdges> edges = face->edges();
 if (!edges)
  return false;
 Ptr<BRepEdge> edge = edges->item(0);
 if (!edge)
  return false;

 // Create a slant construction plane with an angle of 45 deg on the xZConstructionPlane
 Ptr<ConstructionPlanes> planes = rootComp->constructionPlanes();
 if (!planes)
  return false;

 Ptr<ConstructionPlaneInput> planeInput = planes->createInput();
 if (!planeInput)
  return false;
 planeInput->setByAngle(edge, adsk::core::ValueInput::createByString("45 deg"), rootComp->xZConstructionPlane());
 Ptr<ConstructionPlane> plane = planes->add(planeInput);

 // Create another sketch containing a circle profile on the slant plane
 Ptr<Sketch> toolSketch = sketches->add(plane);
 if (!toolSketch)
  return false;
 Ptr<SketchCurves> toolCurves = toolSketch->sketchCurves();
 if (!toolCurves)
  return false;
 Ptr<SketchCircles> sketchCircles = toolCurves->sketchCircles();
 if (!sketchCircles)
  return false;
 Ptr<SketchCircle> circle = sketchCircles->addByCenterRadius(point0, 5.0);
 if (!circle)
  return false;

 // Get SplitFaceFetures
 Ptr<SplitFaceFeatures> splitFaceFeats = feats->splitFaceFeatures();
 if (!splitFaceFeats)
  return false;

 // Set faces to split
 Ptr<ObjectCollection> facesCol = ObjectCollection::create();
 if (!facesCol)
  return false;
 facesCol->add(face);

 // Create Create a split face feature of surface intersection split type
 Ptr<SplitFaceFeatureInput> splitFaceInput = splitFaceFeats->createInput(facesCol, circle, true);
 if (!splitFaceInput)
  return false;
 Ptr<SplitFaceFeature> split = splitFaceFeats->add(splitFaceInput);
 if (!split)
  return false;
 split->deleteMe();

 // Create another split face feature of closest point split type   
 splitFaceInput->setClosestPointSplitType();
 split = splitFaceFeats->add(splitFaceInput);
 if (!split)
  return false;
 split->deleteMe();

 // Create another split face feature of along vector split type
 Ptr<BRepEdge> dirEdge = edges->item(1);
 if (!dirEdge)
  return false;
 splitFaceInput->setAlongVectorSplitType(dirEdge);
 splitFaceFeats->add(splitFaceInput);

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