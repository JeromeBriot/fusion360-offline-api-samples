#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Product.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlanes.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Construction/ConstructionPlaneInput.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/LoftFeatures.h>
#include <Fusion/Features/LoftFeature.h>
#include <Fusion/Features/LoftFeatureInput.h>
#include <Fusion/Features/LoftSections.h>
#include <Fusion/Features/LoftSection.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/Sketches.h>


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

 Ptr<Documents> docs = app->documents();
 if (!docs)
  return false;

 // Create a document.
 Ptr<Document> doc = docs->add(DocumentTypes::FusionDesignDocumentType);
 if (!doc)
  return false;

 Ptr<Design> design = app->activeProduct();
 if (!design)
  return false;

 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 // Create profile 1
 Ptr<Sketches> sketchesObj = rootComp->sketches();
 if (!sketchesObj)
  return false;

 Ptr<Sketch> sketch0 = sketchesObj->add(rootComp->xZConstructionPlane());
 if (!sketch0)
  return false;

 Ptr<SketchCurves> sketchCurvesObj0 = sketch0->sketchCurves();
 if (!sketchCurvesObj0)
  return false;

 Ptr<SketchCircles> sketchCirclesObj0 = sketchCurvesObj0->sketchCircles();
 if (!sketchCirclesObj0)
  return false;

 Ptr<Point3D> centerPoint = adsk::core::Point3D::create(0, 0, 0);
 if (!centerPoint)
  return false;
 sketchCirclesObj0->addByCenterRadius(centerPoint, 5.0);

 Ptr<Profiles> profilesObj0 = sketch0->profiles();
 if (!profilesObj0)
  return false;

 Ptr<Profile> profile0 = profilesObj0->item(0);
 if (!profile0)
  return false;

 // Create profile 2
 Ptr<ConstructionPlanes> ctorPlanes = rootComp->constructionPlanes();
 if (!ctorPlanes)
  return false;

 Ptr<ConstructionPlaneInput> ctorPlaneInput1 = ctorPlanes->createInput();
 if (!ctorPlaneInput1)
  return false;

 Ptr<ValueInput> offset = adsk::core::ValueInput::createByString("10 cm");
 if (!offset)
  return false;

 bool ret = ctorPlaneInput1->setByOffset(rootComp->xZConstructionPlane(), offset);
 if (!ret)
  return false;

 Ptr<ConstructionPlane> ctorPlane1 = ctorPlanes->add(ctorPlaneInput1);
 if (!ctorPlane1)
  return false;

 Ptr<Sketch> sketch1 = sketchesObj->add(ctorPlane1);
 if (!sketch1)
  return false;

 Ptr<SketchCurves> sketchCurvesObj1 = sketch1->sketchCurves();
 if (!sketchCurvesObj1)
  return false;

 Ptr<SketchCircles> sketchCirclesObj1 = sketchCurvesObj1->sketchCircles();
 if (!sketchCirclesObj1)
  return false;
 sketchCirclesObj1->addByCenterRadius(centerPoint, 2.0);

 Ptr<Profiles> profilesObj1 = sketch1->profiles();
 if (!profilesObj1)
  return false;

 Ptr<Profile> profile1 = profilesObj1->item(0);
 if (!profile1)
  return false;

 // Create profile 3
 Ptr<ConstructionPlaneInput> ctorPlaneInput2 = ctorPlanes->createInput();
 if (!ctorPlaneInput2)
  return false;

 ret = ctorPlaneInput2->setByOffset(ctorPlane1, offset);
 if (!ret)
  return false;

 Ptr<ConstructionPlane> ctorPlane2 = ctorPlanes->add(ctorPlaneInput2);
 if (!ctorPlane2)
  return false;

 Ptr<Sketch> sketch2 = sketchesObj->add(ctorPlane2);
 if (!sketch2)
  return false;

 Ptr<SketchCurves> sketchCurvesObj2 = sketch2->sketchCurves();
 if (!sketchCurvesObj2)
  return false;

 Ptr<SketchCircles> sketchCirclesObj2 = sketchCurvesObj2->sketchCircles();
 if (!sketchCirclesObj2)
  return false;
 sketchCirclesObj2->addByCenterRadius(centerPoint, 10.0);

 Ptr<Profiles> profilesObj2 = sketch2->profiles();
 if (!profilesObj2)
  return false;

 Ptr<Profile> profile2 = profilesObj2->item(0);
 if (!profile2)
  return false;

 // Create loft feature input
 Ptr<Features> feats = rootComp->features();
 if (!feats)
  return false;

 Ptr<LoftFeatures> loftFeats = feats->loftFeatures();
 if (!loftFeats)
  return false;

 Ptr<LoftFeatureInput> loftInput = loftFeats->createInput(adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
 if (!loftInput)
  return false;

 Ptr<LoftSections> loftSectionsObj = loftInput->loftSections();
 if (!loftSectionsObj)
  return false;

 Ptr<LoftSection> loftSection0 = loftSectionsObj->add(profile0);
 if (!loftSection0)
  return false;

 Ptr<LoftSection> loftSection1 = loftSectionsObj->add(profile1);
 if (!loftSection1)
  return false;

 Ptr<LoftSection> loftSection2 = loftSectionsObj->add(profile2);
 if (!loftSection2)
  return false;

 // Set solid, closed and tangent edges merged options
 loftInput->isSolid(false);
 loftInput->isClosed(false);
 loftInput->isTangentEdgesMerged(true);

 // Create loft feature
 Ptr<LoftFeature> loftFeature = loftFeats->add(loftInput);
 if (!loftFeature)
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