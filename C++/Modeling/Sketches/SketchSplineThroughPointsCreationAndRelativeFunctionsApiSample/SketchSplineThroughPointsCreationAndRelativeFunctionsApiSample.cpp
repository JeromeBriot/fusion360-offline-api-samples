#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Geometry/Point3D.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchFittedSplines.h>
#include <Fusion/Sketch/SketchFittedSpline.h>
#include <Fusion/Sketch/SketchPointList.h>
#include <Fusion/Sketch/SketchPoint.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchArc.h>
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

 // Get the root component.
 Ptr<Component> rootComp = design->rootComponent();
 if(!rootComp)
  return false;

    // Create a new sketch on the xy plane.
 Ptr<Sketches> sketches = rootComp->sketches();
 if(!sketches)
  return false;
 Ptr<ConstructionPlane> xyPlane = rootComp->xYConstructionPlane();
 if(!xyPlane)
  return false;
 Ptr<Sketch> sketch = sketches->add(xyPlane);
 if(!sketch)
  return false;

 // Create an object collection for the points.
 Ptr<ObjectCollection> points = ObjectCollection::create();
 if (!points)
  return false;

 // Define the points the spline with fit through.
 points->add(Point3D::create(0, 0, 0));
 points->add(Point3D::create(5, 1, 0));
 points->add(Point3D::create(6, 4, 3));
 points->add(Point3D::create(7, 6, 6));
 points->add(Point3D::create(2, 3, 0));
 points->add(Point3D::create(0, 1, 0));

 // Create the spline.
 Ptr<SketchCurves> sketchCurves = sketch->sketchCurves();
 if(!sketchCurves)
  return false;
 Ptr<SketchFittedSplines> splines = sketchCurves->sketchFittedSplines();
 if (!splines)
  return false;

 Ptr<SketchFittedSpline> spline = splines->add(points);
 if (!spline)
  return false;

    // Get spline fit points
 Ptr<SketchPointList> fitPoints = spline->fitPoints();
 if (!fitPoints)
  return false;

 // Get the second fit point
 Ptr<SketchPoint> fitPoint = fitPoints->item(1);
 if (!fitPoint)
  return false;
    
 // If there is no the relative tangent handle, activate the tangent handle
 Ptr<SketchLine> line = spline->getTangentHandle(fitPoint);
 if (line)
  return false;
 else
 {
  line = spline->activateTangentHandle(fitPoint);
  if (!line)
   return false;
 }

 // Get the tangent handle
 Ptr<SketchLine> gottenLine = spline->getTangentHandle(fitPoint);
 if (!gottenLine)
  return false;

 // Delete the tangent handle
 bool isSuccess = gottenLine->deleteMe();
 if (!isSuccess)
  return false;

 // Activate the curvature handle
 // If the curvature handle activated. the relative tangentHandle is activated automatically
 Ptr<SketchArc> activatedArc = spline->activateCurvatureHandle(fitPoint);
 if (!activatedArc)
  return false;

 // Get curvature handle and tangent handle
 Ptr<SketchArc> gottenArc = spline->getCurvatureHandle(fitPoint);
 if (!gottenArc)
  return false;

 gottenLine = spline->getTangentHandle(fitPoint);
 if (!gottenLine)
  return false;

 // Delete curvature handle
 bool isDeleted = gottenArc->deleteMe();
 if (!isDeleted)
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