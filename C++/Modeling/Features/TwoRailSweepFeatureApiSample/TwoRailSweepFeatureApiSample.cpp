#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Line3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/Path.h>
#include <Fusion/Features/SweepFeature.h>
#include <Fusion/Features/SweepFeatures.h>
#include <Fusion/Features/SweepFeatureInput.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchLines.h>
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
 Ptr<ConstructionPlane> xy = rootComp->xYConstructionPlane();
 if(!xy)
  return false;
 Ptr<Sketch> sketch = sketches->add(xy);
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

 // Create a vertical sketch and add two lines on it
 Ptr<ConstructionPlane> xz = rootComp->xZConstructionPlane();
 if(!xz)
  return false;
 Ptr<Sketch> sketchVertical = sketches->add(xz);
 if(!sketchVertical)
  return false;
 Ptr<SketchCurves> sketchCurvesVertical = sketchVertical->sketchCurves();
 if(!sketchCurvesVertical)
  return false;
 Ptr<SketchLines> sketchLines = sketchCurvesVertical->sketchLines();
 if(!sketchLines)
  return false;
 Ptr<Point3D> startPtOne = Point3D::create(0, 0, 0);
 if(!startPtOne)
  return false;
 Ptr<Point3D> endPtOne = Point3D::create(0, 10, 0);
 if(!endPtOne)
  return false;
 Ptr<Point3D> startPtTwo = Point3D::create(3, 0, 0);
 if (!startPtTwo)
  return false;
 Ptr<Point3D> endPtTwo = Point3D::create(6, 10, 0);
 if(!endPtTwo)
  return false;
 Ptr<SketchLine> line1 = sketchLines->addByTwoPoints(startPtOne, endPtOne);
 if(!line1)
  return false;
 Ptr<SketchLine> line2 = sketchLines->addByTwoPoints(startPtTwo, endPtTwo);
 if(!line2)
  return false;

 // Create a path
 Ptr<Features> feats = rootComp->features();
 if(!feats)
  return false;
 Ptr<Path> path = feats->createPath(line1);
 if(!path)
  return false;

 // Create guid rail path
 Ptr<Path> guidRailPath = feats->createPath(line2);
 if (!guidRailPath)
  return false;

 // Create a sweep input
 Ptr<SweepFeatures> sweeps = feats->sweepFeatures();
 if(!sweeps)
  return false;
 Ptr<SweepFeatureInput> sweepInput = sweeps->createInput(prof, path, FeatureOperations::NewBodyFeatureOperation);
 if(!sweepInput)
  return false;
 sweepInput->guideRail(guidRailPath);
 sweepInput->isDirectionFlipped(false);
 sweepInput->profileScaling(SweepProfileScalingOptions::SweepProfileScaleOption);

 // Create the sweep.
 Ptr<SweepFeature> sweep = sweeps->add(sweepInput);
 if(!sweep)
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