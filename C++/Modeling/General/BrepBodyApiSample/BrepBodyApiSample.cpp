#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Core/Geometry/Point3D.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>


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

 // Create the spline.
 Ptr<SketchCurves> sketchCurves = yzSketch->sketchCurves();
 if (!sketchCurves)
  return false;

 // Get sketch lines
 Ptr<SketchLines> sketchLines = sketchCurves->sketchLines();
 if (!sketchLines)
  return false;

 // Create sketch rectangle
 Ptr<Point3D> startPoint = Point3D::create(0, 0, 0);
 Ptr<Point3D> endPoint = Point3D::create(5.0, 5.0, 0);
 sketchLines->addTwoPointRectangle(startPoint, endPoint);

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

 Ptr<BRepBody> brepBody = bodies->item(0);

 std::string oldRevisionId = brepBody->revisionId();
 bool isVisibleFalse = brepBody->isLightBulbOn(false);
 std::string newRevisionId = brepBody->revisionId();


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