#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Geometry/Point3D.h>

#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/ThreadFeatures.h>
#include <Fusion/Features/ThreadFeature.h>
#include <Fusion/Features/ThreadFeatureInput.h>
#include <Fusion/Features/ThreadDataQuery.h>
#include <Fusion/Features/ThreadInfo.h>
#include <Fusion/FusionTypeDefs.h>

#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/BRep/BRepFace.h>


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
 
 // create a new document
 Ptr<Documents> docs = app->documents();
 if (!docs)
  return false;

 Ptr<Document> doc = docs->add(FusionDesignDocumentType);
 if (!doc)
  return false;

 // get the design
 Ptr<Product> product = app->activeProduct();
 if (!product)
  return false;

 Ptr<Design> design = product;
 if (!design)
  return false;

 // get the root component of the active design.
 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 // create a new sketch on the xy plane.
 Ptr<Sketches> sketches = rootComp->sketches();
 if (!sketches)
  return false;

 Ptr<ConstructionPlane> xyPlane = rootComp->xYConstructionPlane();
 if (!xyPlane)
  return false;

 Ptr<Sketch> sketch = sketches->add(xyPlane);
 if (!sketch)
  return false;

 // create a sketch circle
 Ptr<SketchCurves> sketchCurves = sketch->sketchCurves();
 if (!sketchCurves)
  return false;

 Ptr<SketchCircles> sketchCircles = sketchCurves->sketchCircles();
 if (!sketchCircles)
  return false;

 Ptr<SketchCircle> sketchCircle = sketchCircles->addByCenterRadius(Point3D::create(0, 0, 0), 3.0);
 if (!sketchCircle)
  return false;

 Ptr<Profiles> profiles = sketch->profiles();
 if (!profiles)
  return false;

 // get the profile defined by the circle.
 Ptr<Profile> profile = profiles->item(0);
 if (!profile)
  return false;

 // Create an extrusion input
 Ptr<Features> features = rootComp->features();
 if (!features)
  return false;

 Ptr<ExtrudeFeatures> extrudes = features->extrudeFeatures();
 if (!extrudes)
  return false;

 Ptr<ExtrudeFeatureInput> extrudeInput = extrudes->createInput(profile, NewBodyFeatureOperation);
 if (!extrudeInput)
  return false;

 // Define that the extent is a distance extent of 7 cm.
 bool bRet = extrudeInput->setDistanceExtent(false, ValueInput::createByReal(7));
 if (!bRet)
  return false;

 // Create the extrusion.
 Ptr<ExtrudeFeature> extrude = extrudes->add(extrudeInput);
 if (!extrude)
  return false;

 // define all of the thread information.
 Ptr<ThreadFeatures> threadFeatures = features->threadFeatures();
 if (!threadFeatures)
  return false;

 // query the thread table to get the thread information
 Ptr<ThreadDataQuery> threadDataQuery = threadFeatures->threadDataQuery();
 if (!threadDataQuery)
  return false;
 std::vector<std::string> threadTypes = threadDataQuery->allThreadTypes();
 if (threadTypes.size() == 0)
  return false;
 std::string threadType = threadTypes[0];

 std::vector<std::string> allsizes = threadDataQuery->allSizes(threadType);
 if (allsizes.size() == 0)
  return false;
 std::string threadSize = allsizes[0];

 std::vector<std::string> allDesignations = threadDataQuery->allDesignations(threadType, threadSize);
 if (allDesignations.size() == 0)
  return false;
 std::string threadDesignation = allDesignations[0];

 std::vector<std::string> allClasses = threadDataQuery->allClasses(false, threadType, threadDesignation);
 if (allClasses.size() == 0)
  return false;
 std::string threadClass = allClasses[0];

 // create the threadInfo according to the query result
 Ptr<ThreadInfo> threadInfo = threadFeatures->createThreadInfo(false, threadType, threadDesignation, threadClass);
 if (!threadInfo)
  return false;

 // get the face the thread will be applied to
 Ptr<BRepFaces> sideFaces = extrude->sideFaces();
 if (!sideFaces)
  return false;

 Ptr<BRepFace> sideface = sideFaces->item(0);
 if (!sideface)
  return false;
 Ptr<ObjectCollection> faces = ObjectCollection::create();
 if (!faces)
  return false;
 faces->add(sideface);

 // define the thread input with the lenght 3.5 cm
 Ptr<ThreadFeatureInput> threadInput = threadFeatures->createInput(faces, threadInfo);
 if (!threadInput)
  return false;

 threadInput->isFullLength(false);
 threadInput->threadLength(ValueInput::createByReal(3.5));

 // create the final thread
 Ptr<ThreadFeature> thread = threadFeatures->add(threadInput);
 if (!thread)
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