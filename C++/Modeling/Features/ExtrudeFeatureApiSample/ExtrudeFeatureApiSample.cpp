#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Line3D.h>
#include <Core/Geometry/Vector3D.h>
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
#include <Fusion/Features/ExtentDefinition.h>
#include <Fusion/Features/DistanceExtentDefinition.h>
#include <Fusion/Features/FromEntityStartDefinition.h>
#include <Fusion/Features/OffsetStartDefinition.h>
#include <Fusion/Features/SymmetricExtentDefinition.h>
#include <Fusion/Features/ThroughAllExtentDefinition.h>
#include <Fusion/Features/ToEntityExtentDefinition.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Fusion/ModelParameter.h>
#include <Fusion/Fusion/Timeline.h>
#include <Fusion/Fusion/TimelineObject.h>
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

 // Get extrude features
 Ptr<Features> feats = rootComp->features();
 if (!feats)
  return false;
 Ptr<ExtrudeFeatures> extrudes = feats->extrudeFeatures();
 if (!extrudes)
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
 Ptr<Point3D> centerPointVertical = Point3D::create(0, 1, 0);
 if(!centerPointVertical)
  return false;
 Ptr<SketchCircle> cicleVertical = sketchCirclesVertical->addByCenterRadius(centerPointVertical, 0.5);
 if(!cicleVertical)
  return false;

 // Get the profile defined by the vertical circle
 Ptr<Profiles> profsVertical = sketchVertical->profiles();
 if(!profsVertical)
  return false;
 Ptr<Profile> profVertical = profsVertical->item(0);
 if(!profVertical)
  return false;

 // Extrude Sample 1: A simple way of creating typical extrusions (extrusion that goes from the profile plane the specified distance).
 // Define that the extent is a distance extent of 5 cm
 Ptr<ValueInput> distance = ValueInput::createByReal(5);
 if (!distance)
  return false;
 Ptr<ExtrudeFeature> extrude1 = extrudes->addSimple(prof, distance, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
 if (!extrude1)
  return false;
 // Get the body created by the extrusion
 Ptr<BRepBodies> bodies = extrude1->bodies();
 if (!bodies)
  return false;
 Ptr<BRepBody> body1 = bodies->item(0);
 if (!body1)
  return false;
 body1->name("simple"); 

 // Get the state of the extrusion
 adsk::fusion::FeatureHealthStates health = extrude1->healthState();
 if(adsk::fusion::FeatureHealthStates::ErrorFeatureHealthState == health 
  || adsk::fusion::FeatureHealthStates::WarningFeatureHealthState == health){ 
  std::string strWarningMsgForExtrusion = extrude1->errorOrWarningMessage();
 }
 // Get the state of a timeline object
 Ptr<Timeline> timeline = design->timeline();
 Ptr<TimelineObject> timelineObj = timeline->item(timeline->count() - 1);
 health = timelineObj->healthState();
 std::string strMsg = timelineObj->errorOrWarningMessage();

 // Create another sketch
 sketch = sketches->add(rootComp->xZConstructionPlane());
 sketchCurves = sketch->sketchCurves();
 sketchCircles = sketchCurves->sketchCircles();
 Ptr<SketchCircle> circle1 = sketchCircles->addByCenterRadius(centerPoint, 13.0);
 Ptr<SketchCircle> circle2 = sketchCircles->addByCenterRadius(centerPoint, 15.0);
 Ptr<Profiles> profiles = sketch->profiles();
 Ptr<Profile> outerProfile = profiles->item(1);

 // Create taper angle value inputs
 Ptr<ValueInput> deg0 = adsk::core::ValueInput::createByString("0 deg");
 Ptr<ValueInput> deg2 = adsk::core::ValueInput::createByString("2 deg");
 Ptr<ValueInput> deg5 = adsk::core::ValueInput::createByString("5 deg");
 // Create distance value inputs
 Ptr<ValueInput> mm10 = adsk::core::ValueInput::createByString("10 mm");
 Ptr<ValueInput> mm100 = adsk::core::ValueInput::createByString("100 mm");
 
 // Extrude Sample 2: Create an extrusion that goes from the profile plane with one side distance extent
 Ptr<ExtrudeFeatureInput> extrudeInput = extrudes->createInput(outerProfile, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
 // Create a distance extent definition
 Ptr<DistanceExtentDefinition> extent_distance = adsk::fusion::DistanceExtentDefinition::create(mm100);
 // Create the extrusion
 Ptr<ExtrudeFeature> extrude2 = extrudes->add(extrudeInput);
 // Get the body of the extrusion
 bodies = extrude2->bodies();
 Ptr<BRepBody> body2 = bodies->item(0);
 body2->name("distance, from profile");

 // Extrude Sample 3: Create an extrusion that starts from an entity and goes the specified distance.
 extrudeInput = extrudes->createInput(profVertical, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
 // Create a distance extent definition
 Ptr<DistanceExtentDefinition> extent_distance_2 = adsk::fusion::DistanceExtentDefinition::create(mm10);
 // Create a start extent that starts from a BRep face with an offset of 10 mm.
 Ptr<BRepFaces> faces = body1->faces();
 Ptr<BRepFace> face = faces->item(0);
 Ptr<FromEntityStartDefinition> start_from = adsk::fusion::FromEntityStartDefinition::create(face, mm10);
 // Create the extrusion
 Ptr<ExtrudeFeature> extrude3 = extrudes->add(extrudeInput);
 // Get the body of the extrusion
 bodies = extrude3->bodies();
 Ptr<BRepBody> body3 = bodies->item(0);
 body3->name("distance, from entity");
 
 // taperAngle is nullptr, because extrude start face is not a planar face in this case
 Ptr<ModelParameter> taperAngle = extrude3->taperAngleOne();

 // Edit the distance extent of the extrusion.
 Ptr<ExtentDefinition> extent1 = extrude3->extentOne();
 Ptr<DistanceExtentDefinition> disDef = extent1->cast<DistanceExtentDefinition>();
 
 Ptr<ModelParameter> distanceMP = disDef->distance();

 // Edit the start entity of the extrusion.
 Ptr<FromEntityStartDefinition> startDef_from = extrude3->startExtent();

 Ptr<BRepFaces> faces2 = body2->faces();
 Ptr<BRepFace> outerFace = faces2->item(0);

 Ptr<TimelineObject> extrudeTimelineObj = extrude3->timelineObject();

 // Edit the offset to the start entity in the extrusion.
 startDef_from = extrude3->startExtent();
 Ptr<ModelParameter> offsetMP = startDef_from->offset();

 // Extrude Sample 4: Create an extrusion that goes from the profile plane to a specified entity.
 extrudeInput = extrudes->createInput(profVertical, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
 // Create a to-entity extent definition
 bool isChained = true;
 Ptr<ToEntityExtentDefinition> extent_toentity = adsk::fusion::ToEntityExtentDefinition::create(body1, isChained);
 // Set the one side extent with the to-entity-extent-definition, and with a taper angle of 0 degree
 // Create an offset type start definition
 Ptr<OffsetStartDefinition> start_offset = adsk::fusion::OffsetStartDefinition::create(mm10);
 // Create the extrusion
 Ptr<ExtrudeFeature> extrude4 = extrudes->add(extrudeInput);
 // Get the body of the extrusion
 bodies = extrude4->bodies();
 Ptr<BRepBody> body4 = bodies->item(0);
 body4->name("to entity, from offset");

 // Edit the start offset of the extrusion
 Ptr<OffsetStartDefinition> startDef_offset = extrude4->startExtent();
 offsetMP = startDef_offset->offset();

 // Edit the to-entity extent definition of the extrusion
 Ptr<Vector3D> negative = adsk::core::Vector3D::create(-1, 0, 0);
 Ptr<ToEntityExtentDefinition> toDef = extrude4->extentOne();
 extrudeTimelineObj = extrude4->timelineObject();

 // Extrude Sample 5: Create an extrusion that goes through all entities
 extrudeInput = extrudes->createInput(profVertical, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
 // Create an extent definition of through-all type. 
 Ptr<ThroughAllExtentDefinition> extent_all = adsk::fusion::ThroughAllExtentDefinition::create();
 // Set the extrusion start with an offset
 Ptr<ExtrudeFeature> extrude5 = extrudes->add(extrudeInput);
 // Get the body of the extrusion
 bodies = extrude5->bodies();
 Ptr<BRepBody> body5 = bodies->item(0);
 body5->name("through-all, from offset");

 // Edit the start offset
 startDef_offset = extrude5->startExtent();
 offsetMP = startDef_offset->offset();

 // Edit the direction of the extrusion, make it in the same direction as the sketch plane.
 Ptr<ThroughAllExtentDefinition> allDef = extrude5->extentOne();
 extrudeTimelineObj = extrude5->timelineObject();
 if (allDef->isPositiveDirection()) {
 }

 // Extrude Sample 6: Create a symmetric extrusion that goes 10 mm from the profile plane with a 5 degree taper angle.
 bool isFullLength = true;
 extrudeInput = extrudes->createInput(profVertical, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
 Ptr<ExtrudeFeature> extrude6 = extrudes->add(extrudeInput);
 // Get the body of the extrusion
 bodies = extrude6->bodies();
 Ptr<BRepBody> body6 = bodies->item(0);
 body6->name("symmetric");

 // Edit the measurement, distance and taper angle properties of the symmetric extrusion
 Ptr<SymmetricExtentDefinition> symDef = extrude6->extentOne();
 extrudeTimelineObj = extrude6->timelineObject();
 Ptr<ModelParameter> taperAngleMP = symDef->taperAngle();
 distanceMP = symDef->distance();
 // another way to get the symmetric extent definition
 if (extrude6->extentType() == adsk::fusion::FeatureExtentTypes::SymmetricFeatureExtentType) {
  Ptr<SymmetricExtentDefinition> symDef1 = extrude6->symmetricExtent();
  Ptr<ModelParameter> distanceMP1 = symDef1->distance();
 }

 // Extrude Sample 7: Create a 2-side extrusion, whose 1st side is 100 mm distance extent, and 2nd side is 10 mm distance extent.
 extrudeInput = extrudes->createInput(profVertical, adsk::fusion::FeatureOperations::CutFeatureOperation);
 Ptr<DistanceExtentDefinition> extent_distance_20cm = adsk::fusion::DistanceExtentDefinition::create(adsk::core::ValueInput::createByString("20cm"));

 Ptr<ExtrudeFeature> extrude7 = extrudes->add(extrudeInput);

 // Edit the taper angles of both sides in the extrusion
 Ptr<ModelParameter> angleMP_1 = extrude7->taperAngleOne();
 Ptr<ModelParameter> angleMP_2 = extrude7->taperAngleTwo();
 // Get the extent definition of both sides
 Ptr<DistanceExtentDefinition> extent_1 = extrude7->extentOne();
 Ptr<DistanceExtentDefinition> extent_2 = extrude7->extentTwo();
 // Edit the distances the extrusion
 Ptr<ModelParameter> distanceMP_1 = extent_1->distance();
 Ptr<ModelParameter> distanceMP_2 = extent_2->distance();


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