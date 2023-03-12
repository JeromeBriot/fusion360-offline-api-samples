#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Application/Viewport.h>
#include <Core/Application/Camera.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Components/JointOrigins.h>
#include <Fusion/Components/JointOriginInput.h>
#include <Fusion/Components/JointOrigin.h>
#include <Fusion/Components/JointGeometry.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/SketchLines.h>
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
 Ptr<SketchLines> sketchLines = sketchCurves->sketchLines();
 if(!sketchLines)
  return false;
 sketchLines->addTwoPointRectangle(adsk::core::Point3D::create(0, 0, 0), adsk::core::Point3D::create(10, 10, 0));

 // Get the profile defined by the circle
 Ptr<Profiles> profs = sketch->profiles();
 if(!profs)
  return false;
 Ptr<Profile> profile = profs->item(0);
 if(!profile)
  return false;

 // Create an extrude input and make sure it's in a new component
 Ptr<Features> feats = rootComp->features();
 if(!feats)
  return false;
 Ptr<ExtrudeFeatures> extrudes = feats->extrudeFeatures();
 if(!extrudes)
  return false;
 Ptr<ExtrudeFeatureInput> extInput = extrudes->createInput(profile, FeatureOperations::NewComponentFeatureOperation);
 if(!extInput)
  return false;

 // Set the extrude input
 Ptr<ValueInput> distance = ValueInput::createByString("50 mm");
 if(!distance)
  return false;
 extInput->setDistanceExtent(false, distance);
 extInput->isSolid(true);

 // Create the extrude
 Ptr<ExtrudeFeature> ext = extrudes->add(extInput);
 if(!ext)
  return false;

 Ptr<BRepFaces> faces = ext->faces();
 if(!faces)
  return false;

 // Create the joint geometry
 Ptr<JointGeometry> jointGeometry = JointGeometry::createByBetweenTwoPlanes(faces->item(0), faces->item(1), faces->item(0), nullptr, JointKeyPointTypes::CenterKeyPoint);
 if(!jointGeometry)
  return false;

 // Create JointOrigininput
 Ptr<JointOrigins> jointOrigs = rootComp->jointOrigins();
 if(!jointOrigs)
  return false;
 Ptr<JointOriginInput> jointOriginInput = jointOrigs->createInput(jointGeometry);
 if(!jointOriginInput)
  return false;

 // Create the JointOrigin
 Ptr<JointOrigin> jointOrigin = jointOrigs->add(jointOriginInput);
 if(!jointOrigin)
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