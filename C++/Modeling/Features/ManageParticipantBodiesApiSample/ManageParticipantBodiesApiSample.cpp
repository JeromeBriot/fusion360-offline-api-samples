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
#include <Fusion/Construction/ConstructionAxis.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/RectangularPatternFeature.h>
#include <Fusion/Features/RectangularPatternFeatures.h>
#include <Fusion/Features/RectangularPatternFeatureInput.h>
#include <Fusion/Features/DistanceExtentDefinition.h>
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
 if (!rootComp)
  return false;

 // Create sketch
 Ptr<Sketches> sketches = rootComp->sketches();
 if (!sketches)
  return false;
 Ptr<ConstructionPlane> xz = rootComp->xZConstructionPlane();
 if (!xz)
  return false;
 Ptr<Sketch> sketch = sketches->add(xz);
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
 Ptr<SketchCircle> circle = sketchCircles->addByCenterRadius(centerPoint, 3.0);
 if (!circle)
  return false;

 // Get the profile defined by the circle.
 Ptr<Profiles> profs = sketch->profiles();
 if (!profs)
  return false;
 Ptr<Profile> prof = profs->item(0);
 if (!prof)
  return false;

 // Create sketch for Cut
 Ptr<Sketch>  sketchForCut = sketches->add(rootComp->xZConstructionPlane());

 Ptr<SketchCurves> sketchForCutCurves = sketchForCut->sketchCurves();
 if (!sketchForCutCurves)
  return false;
 Ptr<SketchCircles>  sketchForCutCircles = sketchForCutCurves->sketchCircles();

 if (!sketchForCutCircles)
  return false;

 sketchForCutCircles->addByCenterRadius(centerPoint, 1.5);

 // Get the profile defined by the circle.
 Ptr<Profiles> profilesForCut = sketchForCut->profiles();
 if (!profilesForCut)
  return false;

 Ptr<Profile> profForCut = profilesForCut->item(0);
 if (!profForCut)
  return false;

 // Define that the extent is a distance extent of 5 cm.
 Ptr<ValueInput> distance = ValueInput::createByReal(5);
 if (!distance)
  return false;

 // Create the extrusion.
 Ptr<Features> feats = rootComp->features();
 if (!feats)
  return false;
 Ptr<ExtrudeFeatures> extrudes = feats->extrudeFeatures();
 if (!extrudes)
  return false;
 Ptr<ExtrudeFeature> ext = extrudes->addSimple(prof, distance, 
              FeatureOperations::NewBodyFeatureOperation);
 if (!ext)
  return false;

 // Get the body created by extrusion
 Ptr<BRepBodies> bodies = ext->bodies();
 if (!bodies)
  return false;
 Ptr<BRepBody> body = bodies->item(0);
 if (!body)
  return false;

 // Create input entities for rectangular pattern
 Ptr<ObjectCollection> inputEntites = ObjectCollection::create();
 if (!inputEntites)
  return false;
 inputEntites->add(body);

 // Get x and y axes for rectangular pattern
 Ptr<ConstructionAxis> xAxis = rootComp->xConstructionAxis();
 if (!xAxis)
  return false;
 Ptr<ConstructionAxis> yAxis = rootComp->yConstructionAxis();
 if (!yAxis)
  return false;

 // Quantity and distance
 Ptr<ValueInput> quantityOne = ValueInput::createByString("0");
 if (!quantityOne)
  return false;
 Ptr<ValueInput> distanceOne = ValueInput::createByString("0 cm");
 if (!distanceOne)
  return false;
 Ptr<ValueInput> quantityTwo = ValueInput::createByString("6");
 if (!quantityTwo)
  return false;
 Ptr<ValueInput> distanceTwo = ValueInput::createByString("15 cm");
 if (!distanceTwo)
  return false;

 // Create the input for rectangular pattern
 Ptr<RectangularPatternFeatures> rectangularPatterns = feats->rectangularPatternFeatures();
 if (!rectangularPatterns)
  return false;
 Ptr<RectangularPatternFeatureInput> rectangularPatternInput = rectangularPatterns->createInput(inputEntites, xAxis,
  quantityOne, distanceOne, PatternDistanceType::SpacingPatternDistanceType);
 if (!rectangularPatternInput)
  return false;

 // Set the data for second direction
 rectangularPatternInput->setDirectionTwo(yAxis, quantityTwo, distanceTwo);

 // Create the rectangular pattern
 Ptr<RectangularPatternFeature> rectangularFeature = rectangularPatterns->add(rectangularPatternInput);
 if (!rectangularFeature)
  return false;

 Ptr<BRepBodies> patBodies = rectangularFeature->bodies();
 if (!patBodies)
  return false;

 Ptr<ExtrudeFeatureInput> extCutInput = extrudes->createInput(profForCut, FeatureOperations::CutFeatureOperation);
 if (!extCutInput)
  return false;

 // Set the extrude input
 Ptr<ValueInput> distanceForCut = ValueInput::createByString("90 cm");
 if (!distanceForCut)
  return false;
 Ptr<DistanceExtentDefinition> distExtent = DistanceExtentDefinition::create(distanceForCut);
 if (!distExtent)
  return false;
 extCutInput->setOneSideExtent(distExtent, ExtentDirections::PositiveExtentDirection);

 //set bodies to participate
 extCutInput->participantBodies({patBodies->item(0), patBodies->item(2), patBodies->item(4)});

 extrudes->add(extCutInput);


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