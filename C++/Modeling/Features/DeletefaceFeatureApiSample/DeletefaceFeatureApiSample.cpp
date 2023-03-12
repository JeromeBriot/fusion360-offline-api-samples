#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/SurfaceDeleteFaceFeatures.h>
#include <Fusion/Features/SurfaceDeleteFaceFeature.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCircles.h>
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

 // Create sketch
 Ptr<Sketches> sketches = rootComp->sketches();
 if (!sketches)
  return false;

 Ptr<Sketch> sketch = sketches->add(rootComp->xZConstructionPlane());
 if (!sketch)
  return false;

 Ptr<SketchCurves> curves = sketch->sketchCurves();
 if (!curves)
  return false;

 Ptr<SketchCircles> sketchCircles = curves->sketchCircles();
 if (!sketchCircles)
  return false;

 Ptr<Point3D> centerPoint = adsk::core::Point3D::create(0, 0, 0);
 if (!centerPoint)
  return false;
 sketchCircles->addByCenterRadius(centerPoint, 5.0);

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

 extInput->setDistanceExtent(false, distance);

 // Create the extrusion
 Ptr<ExtrudeFeature> ext = extrudes->add(extInput);
 if (!ext)
  return false;

 // Get end faces
 Ptr<BRepFaces> endFacesObj = ext->endFaces();
 if (!endFacesObj)
  return false;
 Ptr<BRepFace> endFace = endFacesObj->item(0);
 if (!endFace)
  return false;

 // Create SurfaceDeleteFaceFeature
 Ptr<SurfaceDeleteFaceFeatures> surfaceDeleteFaceFeaturesObj = feats->surfaceDeleteFaceFeatures();
 if (!surfaceDeleteFaceFeaturesObj)
  return false;
 Ptr<SurfaceDeleteFaceFeature> surfaceDeleteFaceFeature = surfaceDeleteFaceFeaturesObj->add(endFace);
 if (!surfaceDeleteFaceFeature)
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