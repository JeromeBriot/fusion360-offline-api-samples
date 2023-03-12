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
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/ExtendFeatures.h>
#include <Fusion/Features/ExtendFeatureInput.h>
#include <Fusion/Features/ExtendFeature.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>


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

 // Create sketch circle on the xz plane.
 Ptr<Sketches> sketches = rootComp->sketches();
 if(!sketches)
  return false;
 Ptr<Sketch> sketch = sketches->add(rootComp->xZConstructionPlane());
 if(!sketch)
  return false;
 Ptr<SketchCurves> sketchCurves = sketch->sketchCurves();
 if(!sketchCurves)
  return false;
 Ptr<SketchLines> sketchLines = sketchCurves->sketchLines();
 if (!sketchLines)
  return false;
 Ptr<Point3D> startPoint = Point3D::create(0, 0, 0);
 Ptr<Point3D> endPoint = Point3D::create(1.0, 0, 0);
 Ptr<SketchLine> sketchLine = sketchLines->addByTwoPoints(startPoint, endPoint);

 // Create a open profile.
 Ptr<Profile> openProfile = rootComp->createOpenProfile(sketchLine);

 // Create an extrusion input.
 Ptr<Features> features = rootComp->features();
 if (!features)
  return false;
 Ptr<ExtrudeFeatures> extrudes = features->extrudeFeatures();
 if (!extrudes)
  return false;
 Ptr<ExtrudeFeatureInput> extrudeInput = extrudes->createInput(openProfile, FeatureOperations::NewBodyFeatureOperation);
 if (!extrudeInput)
  return false;
 extrudeInput->isSolid(false);

 // Define the extent.
 Ptr<ValueInput> distance = ValueInput::createByReal(1.0);
 extrudeInput->setDistanceExtent(false, distance);

 // Create the extrusion.
 Ptr<ExtrudeFeature> extrude = extrudes->add(extrudeInput);
 if (!extrude)
  return false;
 Ptr<BRepBodies> bodies = extrude->bodies();
 if (!bodies)
  return false;
 Ptr<BRepBody> body = bodies->item(0);

 // Get an edge from surface, and add it to object collection.
 Ptr<BRepEdges> edges = body->edges();
 if (!edges)
  return false;
 Ptr<BRepEdge> edge = edges->item(0);
 Ptr<ObjectCollection> inputEdges = ObjectCollection::create();
 if (!inputEdges)
  return inputEdges;
 inputEdges->add(edge);

 // Define a distance to extend with 1 cm.
 distance = ValueInput::createByReal(1.0);

    // Create an extend input to be able to define the input needed for an extend.
 Ptr<ExtendFeatures> extends = features->extendFeatures();
 if (!extends)
  return false;
 Ptr<ExtendFeatureInput> extendInput = extends->createInput(inputEdges, distance, SurfaceExtendTypes::NaturalSurfaceExtendType);
 extendInput->extendAlignment(SurfaceExtendAlignment::FreeEdges);

 // Create an extend feature.
 extends->add(extendInput);


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