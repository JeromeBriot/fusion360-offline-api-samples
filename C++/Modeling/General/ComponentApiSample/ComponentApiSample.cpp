#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Geometry/Matrix3D.h>
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
#include <Fusion/Components/Occurrence.h>
#include <Fusion/Components/Occurrences.h>
#include <Fusion/Components/Component.h>
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

 // Create a sub component under root component
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

 std::string revisionId1 = subComp1->revisionId();

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

 std::string revisionId2 = subComp1->revisionId();


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