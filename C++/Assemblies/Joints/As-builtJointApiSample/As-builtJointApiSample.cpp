#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Application/Viewport.h>
#include <Core/Application/Camera.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Matrix3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/Construction/ConstructionPlanes.h>
#include <Fusion/Construction/ConstructionPlaneInput.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Components/AsBuiltJoints.h>
#include <Fusion/Components/AsBuiltJointInput.h>
#include <Fusion/Components/AsBuiltJoint.h>
#include <Fusion/Components/Occurrence.h>
#include <Fusion/Components/Occurrences.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
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

 // Create sub component 1 under root component
 Ptr<Occurrences> occs = rootComp->occurrences();
 if(!occs)
  return false;
 Ptr<Matrix3D> transform = adsk::core::Matrix3D::create();
 if(!transform)
  return false;
 Ptr<Occurrence> subOcc0 = occs->addNewComponent(transform);
 if(!subOcc0)
  return false;

 // Create sketch 1 in sub component 1
 Ptr<Component> subComp0 = subOcc0->component();
 if(!subComp0)
  return false;
 Ptr<Sketches> sketches0 = subComp0->sketches();
 if(!sketches0)
  return false;
 Ptr<ConstructionPlane> xzPlane = rootComp->xZConstructionPlane();
 if(!xzPlane)
  return false;
 Ptr<Sketch> sketch0 = sketches0->add(xzPlane);
 if(!sketch0)
  return false;
 Ptr<SketchCurves> sketchCurves0 = sketch0->sketchCurves();
 if(!sketchCurves0)
  return false;
 Ptr<SketchCircles> sketchCircles0 = sketchCurves0->sketchCircles();
 if(!sketchCircles0)
  return false;
 Ptr<Point3D> centerPoint = Point3D::create(0, 0, 0);
 if(!centerPoint)
  return false;
 Ptr<SketchCircle> sketchCircle0 = sketchCircles0->addByCenterRadius(centerPoint, 2.5);
 if(!sketchCircle0)
  return false;

 // Get the profile defined by the circle
 Ptr<Profiles> profs0 = sketch0->profiles();
 if(!profs0)
  return false;
 Ptr<Profile> profile0 = profs0->item(0);
 if(!profile0)
  return false;

 // Create an extrude input
 Ptr<Features> feats0 = subComp0->features();
 if(!feats0)
  return false;
 Ptr<ExtrudeFeatures> extrudes0 = feats0->extrudeFeatures();
 if(!extrudes0)
  return false;
 Ptr<ExtrudeFeatureInput> extInput0 = extrudes0->createInput(profile0, FeatureOperations::NewBodyFeatureOperation);
 if(!extInput0)
  return false;

 // Set the extrude input
 Ptr<ValueInput> distance0 = ValueInput::createByString("5 mm");
 if(!distance0)
  return false;
 extInput0->setDistanceExtent(false, distance0);
 extInput0->isSolid(true);

 // Create the extrude
 Ptr<ExtrudeFeature> ext = extrudes0->add(extInput0);
 if(!ext)
  return false;

 // Get the end face of the created extrude
 Ptr<BRepFaces> endFaces = ext->endFaces();
 if(!endFaces)
  return false;
 Ptr<BRepFace> endFace = endFaces->item(0);
 if(!endFace)
  return false;

 // Create a construction plane for extrude 2
 Ptr<ConstructionPlanes> ctorPlanes = subComp0->constructionPlanes();
 if(!ctorPlanes)
  return false;
 Ptr<ConstructionPlaneInput> ctorPlaneInput = ctorPlanes->createInput();
 if(!ctorPlaneInput)
  return false;
 ctorPlaneInput->setByOffset(endFace, adsk::core::ValueInput::createByString("20 mm"));
 Ptr<ConstructionPlane> ctorPlane = ctorPlanes->add(ctorPlaneInput);
 if(!ctorPlane)
  return false;
 Ptr<ConstructionPlane> ctorPlaneProxy = ctorPlane->createForAssemblyContext(subOcc0);
 if(!ctorPlaneProxy)
  return false;

 // Create sub component 2 under root component
 Ptr<Occurrence> subOcc1 = occs->addNewComponent(transform);
 if(!subOcc1)
  return false;

 // Create sketch 2 in sub component 2
 Ptr<Component> subComp1 = subOcc1->component();
 if(!subComp1)
  return false;
 Ptr<Sketches> sketches1 = subComp1->sketches();
 if(!sketches1)
  return false;
 Ptr<Sketch> sketch1 = sketches1->add(ctorPlaneProxy);
 if(!sketch1)
  return false;
 Ptr<SketchCurves> sketchCurves1 = sketch1->sketchCurves();
 if(!sketchCurves1)
  return false;
 Ptr<SketchCircles> sketchCircles1 = sketchCurves1->sketchCircles();
 if(!sketchCircles1)
  return false;
 Ptr<SketchCircle> sketchCircle1 = sketchCircles1->addByCenterRadius(centerPoint, 0.5);
 if(!sketchCircle1)
  return false;

 // Get the profile defined by the circle
 Ptr<Profiles> profs1 = sketch1->profiles();
 if(!profs1)
  return false;
 Ptr<Profile> profile1 = profs1->item(0);
 if(!profile1)
  return false;

 // Create an extrude input
 Ptr<Features> feats1 = subComp1->features();
 if(!feats1)
  return false;
 Ptr<ExtrudeFeatures> extrudes1 = feats1->extrudeFeatures();
 if(!extrudes1)
  return false;
 Ptr<ExtrudeFeatureInput> extInput1 = extrudes1->createInput(profile1, FeatureOperations::NewBodyFeatureOperation);
 if(!extInput1)
  return false;

 // Set the extrude input
 Ptr<ValueInput> distance1 = ValueInput::createByString("50 mm");
 if(!distance1)
  return false;
 extInput1->setDistanceExtent(false, distance1);
 extInput1->isSolid(false);

 // Create the extrude
 Ptr<ExtrudeFeature> extrude1 = extrudes1->add(extInput1);
 if(!extrude1)
  return false;

 // Create the AsBuiltJoint
 Ptr<AsBuiltJoints> asBuiltJoints_ = rootComp->asBuiltJoints();
 if(!asBuiltJoints_)
  return false;
 Ptr<AsBuiltJointInput> asBuiltJointInput = asBuiltJoints_->createInput(subOcc0, subOcc1, nullptr);
 if(!asBuiltJointInput)
  return false;
 Ptr<AsBuiltJoint> asBuiltJoint = asBuiltJoints_->add(asBuiltJointInput);
 if(!asBuiltJoint)
  return false;

 // Fit to window
 Ptr<Viewport> viewPort = app->activeViewport();
 if(!viewPort)
  return false;
 Ptr<Camera> cam = viewPort->camera();
 if(!cam)
  return false;
 cam->isFitView(true);
 viewPort->camera(cam);


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