#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchPoint.h>


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

 Ptr<SketchCurves> curves = sketch->sketchCurves();
 if (!curves)
  return false;

 // Draw some circles.
 Ptr<SketchCircles> circles = curves->sketchCircles();
 if (!circles)
  return false;

 Ptr<SketchCircle> circle1 = circles->addByCenterRadius(Point3D::create(0, 0, 0), 2);
 if (!circle1)
  return false;

 Ptr<SketchCircle> circle2 = circles->addByCenterRadius(Point3D::create(8, 3, 0), 3);
 if (!circle2)
  return false;

 // Add a circle at the center of one of the existing circles.
 Ptr<SketchCircle> circle3 = circles->addByCenterRadius(circle2->centerSketchPoint(), 4);
 if (!circle3)
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