#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Line3D.h>
#include <Core/Application/ObjectCollection.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchPoint.h>
#include <Fusion/Sketch/SketchArcs.h>
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

 // Get the root component of the active design.
 Ptr<Component> rootComp = design->rootComponent();
 if(!rootComp)
  return false;

 // Create a new sketch on the xy plane.
 Ptr<Sketches> sketches = rootComp->sketches();
 if (!sketches)
  return false;
 Ptr<ConstructionPlane> xyPlane = rootComp->xYConstructionPlane();
 if (!xyPlane)
  return false;
 Ptr<Sketch> sketch = sketches->add(xyPlane);
 if (!sketch)
  return false;

 // Draw two connected lines.
 Ptr<SketchCurves> curves = sketch->sketchCurves();
 if (!curves)
  return false;
 Ptr<SketchLines> lines = curves->sketchLines();
 if (!lines)
  return false;
 Ptr<SketchLine> line1 = lines->addByTwoPoints(adsk::core::Point3D::create(0, 0, 0), adsk::core::Point3D::create(3, 1, 0));
 if (!line1)
  return false;
 Ptr<SketchLine> line2 = lines->addByTwoPoints(line1->endSketchPoint(), adsk::core::Point3D::create(1, 4, 0));
 if (!line2)
  return false;

 // Add a fillet.
 Ptr<SketchArcs> arcs = curves->sketchArcs();
 if (!arcs)
  return false;
 Ptr<SketchArc> arc = arcs->addFillet(line1, line1->endSketchPoint()->geometry(), line2, line2->startSketchPoint()->geometry(), 1);
 if (!arc)
  return false;

 // Add the geometry to a collection. This uses a utility function that
 // automatically finds the connected curves and returns a collection.
 Ptr<ObjectCollection> connectedCurves = sketch->findConnectedCurves(line1);
 if (!connectedCurves)
  return false;

 // Create the offset.
 Ptr<Point3D> dirPoint = adsk::core::Point3D::create(0, 0.5, 0);
 if (!dirPoint)
  return false;
 Ptr<ObjectCollection> offsetCurves = sketch->offset(connectedCurves, dirPoint, 0.25);
 if (!offsetCurves)
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