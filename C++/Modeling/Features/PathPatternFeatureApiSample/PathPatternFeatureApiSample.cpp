#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Line3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/Path.h>
#include <Fusion/Features/PathPatternFeature.h>
#include <Fusion/Features/PathPatternFeatures.h>
#include <Fusion/Features/PathPatternFeatureInput.h>
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
 Ptr<ConstructionPlane> xz = rootComp->xZConstructionPlane();
 if(!xz)
  return false;
 Ptr<Sketch> sketch = sketches->add(xz);
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
 sketchCircles->addByCenterRadius(centerPoint, 3.0);
 Ptr<SketchLines> sketchLines = sketchCurves->sketchLines();
 if(!sketchLines)
  return false;
 Ptr<Point3D> startPoint = Point3D::create(5, 0, 0);
 if(!sketchLines)
  return false;
 Ptr<Point3D> endPoint = Point3D::create(5, 30, 0);
 if(!endPoint)
  return false;
 Ptr<SketchLine> sketchLine = sketchLines->addByTwoPoints(startPoint, endPoint);
 if(!sketchLine)
  return false;

 // Get the profile defined by the circle.
 Ptr<Profiles> profs = sketch->profiles();
 if(!profs)
  return false;
 Ptr<Profile> prof = profs->item(0);
 if(!prof)
  return false;

 // Create an extrusion input
 Ptr<Features> features = rootComp->features();
 if(!features)
  return false;
 Ptr<ExtrudeFeatures> extrudes = features->extrudeFeatures();
 if(!extrudes)
  return false;
 Ptr<ExtrudeFeatureInput> extInput = extrudes->createInput(prof, FeatureOperations::NewBodyFeatureOperation);
 if(!extInput)
  return false;

 // Define that the extent is a distance extent of 5 cm.
 Ptr<ValueInput> distance = ValueInput::createByReal(5);
 if(!distance)
  return false;
 extInput->setDistanceExtent(false, distance);

 // Create the extrusion.
 Ptr<ExtrudeFeature> ext = extrudes->add(extInput);
 if(!ext)
  return false;

 // Get the body created by extrusion
 Ptr<BRepBodies> bodies = ext->bodies();
 if(!bodies)
  return false;
 Ptr<BRepBody> body = bodies->item(0);
 if(!body)
  return false;

 // Create input entities for path pattern
 Ptr<ObjectCollection> inputEntites = ObjectCollection::create();
 if(!inputEntites)
  return false;
 inputEntites->add(body);

 // Create path for path pattern
 Ptr<Path> path = features->createPath(sketchLine);
 if(!path)
  return false;

 // Quantity and distance
 Ptr<ValueInput> quantity = ValueInput::createByString("3");
 if(!quantity)
  return false;
 Ptr<ValueInput> patternDistance = ValueInput::createByString("8 cm");
 if(!patternDistance)
  return false;

 // Create the input for path pattern
 Ptr<PathPatternFeatures> pathPatterns = features->pathPatternFeatures();
 if(!pathPatterns)
  return false;
 Ptr<PathPatternFeatureInput> pathPatternInput = pathPatterns->createInput(inputEntites, path, 
  quantity, patternDistance, PatternDistanceType::SpacingPatternDistanceType);
 if(!pathPatternInput)
  return false;

 // Create the path pattern
 Ptr<PathPatternFeature> pathFeature = pathPatterns->add(pathPatternInput);
 if(!pathFeature)
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