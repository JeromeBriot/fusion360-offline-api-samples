#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/BRep/BRepLoops.h>
#include <Fusion/BRep/BRepLoop.h>
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/PatchFeatures.h>
#include <Fusion/Features/PatchFeatureInput.h>
#include <Fusion/Features/PatchFeature.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Fusion/AreaProperties.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCurves.h>


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

 // Create a document
 Ptr<Documents> docs = app->documents();
 if (!docs)
  return false;
 Ptr<Document> doc = docs->add(adsk::core::DocumentTypes::FusionDesignDocumentType);
 if (!doc)
  return false;

 Ptr<Product> activeProd = app->activeProduct();
 if (!activeProd)
  return false;

 Ptr<Design> design = activeProd;
 if (!design)
  return false;

 // Get the root component of the active design
 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 // Create sketch.
 Ptr<Sketches> sketches = rootComp->sketches();
 if (!sketches)
  return false;
 Ptr<ConstructionPlane> xzPlane = rootComp->xZConstructionPlane();
 if (!xzPlane)
  return false;
 Ptr<Sketch> sketch = sketches->add(xzPlane);
 if (!sketch)
  return false;
 Ptr<SketchCurves> sketchCurves = sketch->sketchCurves();
 if (!sketchCurves)
  return false;
 Ptr<SketchCircles> sketchCircles = sketchCurves->sketchCircles();
 if (!sketchCircles)
  return false;
 Ptr<Point3D> centerPoint = Point3D::create(0, 0, 0);
 if (!centerPoint)
  return false;
 Ptr<SketchCircle> circle1 = sketchCircles->addByCenterRadius(centerPoint, 3.0);
 if (!circle1)
  return false;
 Ptr<SketchCircle> circle2 = sketchCircles->addByCenterRadius(centerPoint, 10.0);
 if (!circle2)
  return false;

 // Get the profiles defined by the circles
 Ptr<Profiles> profs = sketch->profiles();
 if (!profs)
  return false;
 Ptr<Profile> profile1 = profs->item(0);
 if (!profile1)
  return false;
 Ptr<Profile> profile2 = profs->item(1);
 if (!profile2)
  return false;

 // Get the inner and outer profile
 Ptr<AreaProperties> areaPropertiesOfProfile1 = profile1->areaProperties();
 if (!areaPropertiesOfProfile1)
  return false;
 Ptr<AreaProperties> areaPropertiesOfProfile2 = profile2->areaProperties();
 if (!areaPropertiesOfProfile2)
  return false;
 double areaOfProfile1 = areaPropertiesOfProfile1->area();
 double areaOfProfile2 = areaPropertiesOfProfile2->area();
 Ptr<Profile> outerProfile = profile1;
 if (areaOfProfile1 < areaOfProfile2)
 {
  outerProfile = profile2;
 }

 // Create an extrusion based on the outer profile
 Ptr<Features> feats = rootComp->features();
 if (!feats)
  return false;
 Ptr<ExtrudeFeatures> extrudes = feats->extrudeFeatures();
 if (!extrudes)
  return false;
 Ptr<ValueInput> extrudeDistance = ValueInput::createByString("1 cm");
 if (!extrudeDistance)
  return false;
 Ptr<ExtrudeFeature> extrudeFeature = extrudes->addSimple(outerProfile, extrudeDistance, FeatureOperations::NewBodyFeatureOperation);
 if (!extrudeFeature)
  return false;

 // Get BrepEdge from inner loop on the end face of the extrusion
 Ptr<BRepFaces> endFacesObj = extrudeFeature->endFaces();
 if (!endFacesObj)
  return false;
 Ptr<BRepFace> endFace = endFacesObj->item(0);
 if (!endFace)
  return false;
 Ptr<BRepLoops> brepLoops = endFace->loops();
 if (!brepLoops)
  return false;
 Ptr<BRepLoop> innerLoop = brepLoops->item(0);
 if (!innerLoop)
  return false;
 if (innerLoop->isOuter())
 {
  innerLoop = brepLoops->item(1);
 }
 if (!innerLoop)
  return false;
 Ptr<BRepEdges> brepEdges = innerLoop->edges();
 if (!brepEdges)
  return false;
 Ptr<BRepEdge> brepEdge = brepEdges->item(0);
 if (!brepEdge)
  return false;

 // Create the patch feature
 Ptr<PatchFeatures> patches = feats->patchFeatures();
 if (!patches)
  return false;
 Ptr<PatchFeatureInput> patchInput = patches->createInput(brepEdge, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
 if (!patchInput)
  return false;
 Ptr<PatchFeature> patchFeature = patches->add(patchInput);
 if (!patchFeature)
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