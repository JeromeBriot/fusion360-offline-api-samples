#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Components/Joint.h>
#include <Fusion/Components/JointGeometry.h>
#include <Fusion/Components/JointInput.h>
#include <Fusion/Components/JointLimits.h>
#include <Fusion/Components/Joints.h>
#include <Fusion/Components/BallJointMotion.h>
#include <Fusion/Components/Occurrence.h>
#include <Fusion/Components/Occurrences.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCurves.h>
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

 // Create sketch in root component
 Ptr<Sketches> sketches = rootComp->sketches();
 if(!sketches)
  return false;
 Ptr<ConstructionPlane> xz = rootComp->xZConstructionPlane();
 if(!xz)
  return false;
 Ptr<Sketch> sketch = sketches->add(xz);
 if(!sketch)
  return false;
 Ptr<SketchPoints> sketchPts = sketch->sketchPoints();
 if(!sketchPts)
  return false;
 Ptr<Point3D> point = Point3D::create(1, 0, 1);
 if(!point)
  return false;
 Ptr<SketchPoint> sketchPt = sketchPts->add(point);
 if(!sketchPt)
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

 // Create an extrusion input and make sure it's in a new component
 Ptr<Features> feats = rootComp->features();
 if(!feats)
  return false;
 Ptr<ExtrudeFeatures> extrudes = feats->extrudeFeatures();
 if(!extrudes)
  return false;
 Ptr<ExtrudeFeatureInput> extInput = extrudes->createInput(prof, FeatureOperations::NewComponentFeatureOperation);
 if(!extInput)
  return false;

 // Set the extrusion input
 Ptr<ValueInput> distance = ValueInput::createByReal(5);
 if(!distance)
  return false;
 extInput->setDistanceExtent(true, distance);
 extInput->isSolid(true);

 // Create the extrusion
 Ptr<ExtrudeFeature> ext = extrudes->add(extInput);
 if(!ext)
  return false;

 // Get the end face of the created extrusion body
 Ptr<BRepFaces> endFaces = ext->endFaces();
 if(!endFaces)
  return false;
 Ptr<BRepFace> endFace = endFaces->item(0);
 if(!endFace)
  return false;

 // Get the occurrence of the new component
 Ptr<Occurrences> occs = rootComp->occurrences();
 if(!occs)
  return false;
 Ptr<Occurrence> occ = occs->item(0);
 if(!occ)
  return false;

 // Create a new sketch in the occurrence
 Ptr<Sketch> sketchInOcc = sketches->add(endFace, occ);
 if(!sketchInOcc)
  return false;

 // Get the sketch curve projected to the sketch
 Ptr<SketchCurves> sketchCurvesInOcc = sketchInOcc->sketchCurves();
 if(!sketchCurvesInOcc)
  return false;
 Ptr<SketchCurve> curve = sketchCurvesInOcc->item(0);
 if(!curve)
  return false;

 // Create the first joint geometry with the sketch curve
 Ptr<JointGeometry> geo0 = JointGeometry::createByCurve(curve, JointKeyPointTypes::CenterKeyPoint);
 if(!geo0)
  return false;

 // Create the second joint geometry with sketch point
 Ptr<JointGeometry> geo1 = JointGeometry::createByPoint(sketchPt);
 if(!geo1)
  return false;

 // Create joint input
 Ptr<Joints> joints = rootComp->joints();
 if(!joints)
  return false;
 Ptr<JointInput> jointInput = joints->createInput(geo0, geo1);
 if(!jointInput)
  return false;

 // Set the joint input
 jointInput->setAsBallJointMotion(JointDirections::ZAxisJointDirection, JointDirections::XAxisJointDirection);

 // Create the joint
 Ptr<Joint> joint = joints->add(jointInput);
 if(!joint)
  return false;
  
 Ptr<BallJointMotion> ballMotion = joint->jointMotion();
 if(!ballMotion)
  return false;
 Ptr<JointLimits> limits = ballMotion->pitchLimits();
 if(!limits)
  return false;
 limits->isRestValueEnabled(true);
 limits->restValue(1.0);

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