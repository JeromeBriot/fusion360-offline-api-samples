#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Core/Geometry/Point3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchArcs.h>
#include <Fusion/Sketch/SketchArc.h>
#include <Fusion/Sketch/SketchTexts.h>
#include <Fusion/Sketch/SketchTextInput.h>
#include <Fusion/Sketch/SketchText.h>


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

 // Create a document and get the Design.
 Ptr<Document> doc = app->documents()->add(DocumentTypes::FusionDesignDocumentType);
 if (!doc)
  return false;

 Ptr<Design> design = app->activeProduct();
 if (!design)
  return false;

 // Get the root component of the active design
 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 // Create a new sketch on the XY construction plane.
 Ptr<Sketch> sk = rootComp->sketches()->add(rootComp->xYConstructionPlane());
 if (!sk)
  return false;

 // Get the SketchTexts collection object. 
 Ptr<SketchTexts> texts = sk->sketchTexts();
 if (!texts)
  return false;

 // Add multi-line text.
 Ptr<SketchTextInput> input = texts->createInput2("This is a long line that is broken automatically.\n\nAnd this is a defined line break.", 0.5);
 if (!input)
  return false;

 bool ret = input->setAsMultiLine(adsk::core::Point3D::create(0, 0, 0),
               adsk::core::Point3D::create(10, 5, 0),
          adsk::core::HorizontalAlignments::LeftHorizontalAlignment,
          adsk::core::VerticalAlignments::TopVerticalAlignment, 0);
 if (!ret)
  return false;

 Ptr<SketchText> text = texts->add(input);
 if (!text)
  return false;

 // Draw an arc to use to create text along a curve.
 Ptr<SketchArc> arc = sk->sketchCurves()->sketchArcs()->addByThreePoints(adsk::core::Point3D::create(-10, 0, 0),
                    adsk::core::Point3D::create(-5, 3, 0),
                    adsk::core::Point3D::create(0, 0, 0));
 if (!arc)
  return false;

 // Create text along the arc.
 input = texts->createInput2("Text Along a Curve", 0.75);
 if (!input)
  return false;

 ret = input->setAsAlongPath(arc, false, adsk::core::HorizontalAlignments::CenterHorizontalAlignment, 0);
 if (!ret)
  return false;

 ret = input->isHorizontalFlip(true);
 if (!ret)
  return false;

 ret = input->isVerticalFlip(true);
 if (!ret)
  return false;

 ret = input->fontName("Comic Sans MS");
 if (!ret)
  return false;

 text = texts->add(input);
 if (!text)
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