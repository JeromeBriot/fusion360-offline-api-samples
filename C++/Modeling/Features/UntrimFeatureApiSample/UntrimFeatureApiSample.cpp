#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/BRep/BRepLoop.h>
#include <Fusion/BRep/BRepLoops.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/UnstitchFeatures.h>
#include <Fusion/Features/UnstitchFeature.h>
#include <Fusion/Features/UntrimFeature.h>
#include <Fusion/Features/UntrimFeatures.h>
#include <Fusion/Features/UntrimFeatureInput.h>
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
 if (areaOfProfile1 < areaOfProfile2) {
  outerProfile = profile2;
 }

 // Create an extrusion based on the outer profile
 Ptr<Features> features = rootComp->features();
 if (!features)
  return false;
 Ptr<ExtrudeFeatures> extrudes = features->extrudeFeatures();
 if (!extrudes)
  return false;
 Ptr<ValueInput> extrudeDistance = ValueInput::createByString("1 cm");
 if (!extrudeDistance)
  return false;
 Ptr<ExtrudeFeature> extrudeFeature = extrudes->addSimple(outerProfile, extrudeDistance, FeatureOperations::NewBodyFeatureOperation);
 if (!extrudeFeature)
  return false;

 // Get the start face of the extrusion
 Ptr<BRepFaces> startFaces = extrudeFeature->startFaces();
 if (startFaces->count() != 1)
  return false;

 // Get the end face of the extrusion
 Ptr<BRepFaces> endFaces = extrudeFeature->endFaces();
 if (endFaces->count() != 1)
  return false;

 // Get the first body
 Ptr<BRepBodies> bodies = rootComp->bRepBodies();
 if (!bodies)
  return false;
 if (bodies->count() != 1)
  return false;
 Ptr<BRepBody> body1 = bodies->item(0);
 if (!body1)
  return false;

 // Create unstitch feature (so there are separate surface bodies).
 Ptr<UnstitchFeatures> unstitchFeatures = features->unstitchFeatures();
 if (!unstitchFeatures)
  return false;
 Ptr<ObjectCollection> unstitchBodies = adsk::core::ObjectCollection::create();
 if (!unstitchBodies)
  return false;
 unstitchBodies->add(body1);
 Ptr<UnstitchFeature> unstitch = unstitchFeatures->add(unstitchBodies);

 // Create untrim feature on start face (remove external loop and extend face)
 std::vector<Ptr<BRepFace>> facesToUntrim;
 Ptr<BRepFace> face = startFaces->item(0);
 if (!face)
  return false;
 facesToUntrim.push_back(face);
 Ptr<BRepLoops> loops = face->loops();
 if (loops->count() != 2)
  return false;

 Ptr<ValueInput> extensionDistance = ValueInput::createByReal(1.0);

 Ptr<UntrimFeatures> untrimFeatures = features->untrimFeatures();
 Ptr<UntrimFeatureInput> untrimFeatureInput = untrimFeatures->createInputFromFaces(facesToUntrim, ExternalLoopsUntrimType, extensionDistance);
 if (!untrimFeatureInput)
  return false;
 Ptr<UntrimFeature> untrimFeature = untrimFeatures->add(untrimFeatureInput);
 if (!untrimFeature)
  return false;

 // Create untrim feature on the end face (remove internal loop from face)
 std::vector<Ptr<BRepLoop>> loopsToRemove;
 face = endFaces->item(0);
 if (!face)
  return false;
 loops = face->loops();
 if (loops->count() != 2)
  return false;
 // Add inner loop
 Ptr<BRepLoop> loop;
 if (loops->item(0)->isOuter()) {
  loop = loops->item(1);
 } else {
  loop = loops->item(0);
 }
 loopsToRemove.push_back(loop);

 untrimFeatureInput = untrimFeatures->createInputFromLoops(loopsToRemove);
 if (!untrimFeatureInput)
  return false;
 untrimFeature = untrimFeatures->add(untrimFeatureInput);
 if (!untrimFeature)
  return false;



 return true;
}


#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
 switch (reason) {
 case DLL_PROCESS_ATTACH:
 case DLL_THREAD_ATTACH:
 case DLL_THREAD_DETACH:
 case DLL_PROCESS_DETACH:
  break;
 }
 return TRUE;
}

#endif // XI_WIN