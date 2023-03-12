#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ObjectCollection.h>
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
#include <Fusion/Features/ScaleFeature.h>
#include <Fusion/Features/ScaleFeatures.h>
#include <Fusion/Features/ScaleFeatureInput.h>
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
#include <Fusion/Sketch/SketchPoints.h>

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
 Ptr<SketchCircle> circle = sketchCircles->addByCenterRadius(centerPoint, 5.0);
 if(!circle)
  return false;

 // Get the profile defined by the circle
 Ptr<Profiles> profs = sketch->profiles();
 if(!profs)
  return false;
 Ptr<Profile> prof = profs->item(0);
 if(!prof)
  return false;

 // Create an extrusion input
 Ptr<Features> feats = rootComp->features();
 if(!feats)
  return false;
 Ptr<ExtrudeFeatures> extrudes = feats->extrudeFeatures();
 if(!extrudes)
  return false;
 Ptr<ExtrudeFeatureInput> extInput = extrudes->createInput(prof, FeatureOperations::NewBodyFeatureOperation);
 if(!extInput)
  return false;

 // Define that the extent is a distance extent of 5 cm
 Ptr<ValueInput> distance = ValueInput::createByReal(5);
 if(!distance)
  return false;
 extInput->setDistanceExtent(false, distance);

 // Create the extrusion
 Ptr<ExtrudeFeature> ext = extrudes->add(extInput);
 if(!ext)
  return false;

 // Get the body created by the extrusion
 Ptr<BRepBodies> bodies = ext->bodies();
 if(!bodies)
  return false;
 Ptr<BRepBody> body = bodies->item(0);
 if(!body)
  return false;

 // Create a scale input
 Ptr<ObjectCollection> inputColl = ObjectCollection::create();
 if(!inputColl)
  return false;
 inputColl->add(body);

 Ptr<SketchPoints> sketchPts = sketch->sketchPoints();
 if(!sketchPts)
  return false;
 Ptr<SketchPoint> basePt = sketchPts->item(0);
 if(!basePt)
  return false;
 Ptr<ValueInput> scaleFactor = ValueInput::createByReal(2);
 if(!scaleFactor)
  return false;
 Ptr<ScaleFeatures> scales = feats->scaleFeatures();
 if(!scales)
  return false;
 Ptr<ScaleFeatureInput> scaleInput = scales->createInput(inputColl, basePt, scaleFactor);
 if(!scaleInput)
  return false;

 // Set the scale to be non-uniform
 Ptr<ValueInput> xScale = ValueInput::createByReal(1.5);
 if(!xScale)
  return false;
 Ptr<ValueInput> yScale = ValueInput::createByReal(3);
 if(!yScale)
  return false;
 Ptr<ValueInput> zScale = ValueInput::createByReal(2);
 if(!zScale)
  return false;
 scaleInput->setToNonUniform(xScale, yScale, zScale);

 Ptr<ScaleFeature> scale = scales->add(scaleInput);
 if(!scale)
  return false;

 // Create another sketch
 Ptr<ConstructionPlane> yz = rootComp->yZConstructionPlane();
 if(!yz)
  return false;
 Ptr<Sketch> sketchVertical = sketches->add(yz);
 if(!sketchVertical)
  return false;
 Ptr<SketchCurves> sketchCurvesVertical = sketchVertical->sketchCurves();
 if(!sketchCurvesVertical)
  return false;
 Ptr<SketchCircles> sketchCirclesVertical = sketchCurvesVertical->sketchCircles();
 if(!sketchCirclesVertical)
  return false;
 Ptr<Point3D> centerPointVertical = Point3D::create(0, 10, 0);
 if(!centerPointVertical)
  return false;
 Ptr<SketchCircle> cicleVertical = sketchCirclesVertical->addByCenterRadius(centerPointVertical, 5);
 if(!cicleVertical)
  return false;

 // Create an uniformed input for scale feature input
 Ptr<ObjectCollection> inputUniformColl = ObjectCollection::create();
 if(!inputUniformColl)
  return false;
 inputUniformColl->add(sketchVertical);

 Ptr<ScaleFeatureInput> scaleUniformInput = scales->createInput(inputUniformColl, basePt, scaleFactor);
 if(!scaleUniformInput)
  return false;

 Ptr<ScaleFeature> scaleUniform = scales->add(scaleUniformInput);
 if(!scaleUniform)
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