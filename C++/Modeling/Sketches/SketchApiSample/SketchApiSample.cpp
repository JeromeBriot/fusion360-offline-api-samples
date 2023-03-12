#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/ObjectCollection.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchFittedSplines.h>
#include <Fusion/Sketch/SketchFittedSpline.h>
#include <Fusion/Construction/ConstructionPlane.h>



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

 std::string revisionId1 = yzSketch->revisionId();

 // Get sketch lines
 Ptr<SketchLines> sketchLines = sketchCurves->sketchLines();
 if (!sketchLines)
  return false;

 // Create sketch rectangle
 Ptr<Point3D> startPoint = Point3D::create(0, 0, 0);
 Ptr<Point3D> endPoint = Point3D::create(5.0, 5.0, 0);
 sketchLines->addTwoPointRectangle(startPoint, endPoint);

 std::string revisionId2 = yzSketch->revisionId();

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