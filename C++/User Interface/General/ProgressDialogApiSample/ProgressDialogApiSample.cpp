#include <Core/Utils.h>
#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/ProgressDialog.h>
#include <Core/Geometry/Point3D.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchCircles.h>

using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;
Ptr<UserInterface> ui;

extern "C" XI_EXPORT bool run(const char* context)
{
 app = Application::get();
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

 Ptr<ProgressDialog> progressDialog = ui->createProgressDialog();
 if (!progressDialog)
  return false;

 // Set styles of progress dialog.
 progressDialog->cancelButtonText("Cancel");
 progressDialog->isBackgroundTranslucent(false);
 progressDialog->isCancelButtonShown(true);

 // Show dialog
 progressDialog->show("Progress Dialog", "Percentage: %p, Current Value: %v, Total steps: %m", 0, 50, 1);

 // Draw sketches and update status.
 Ptr<Design> design = app->activeProduct();
 if (!design)
  return false;

 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 Ptr<Sketches> sketches = rootComp->sketches();
 if (!sketches)
  return false;

 Ptr<Sketch> sketch = sketches->add(rootComp->xZConstructionPlane());
 if (!sketch)
  return false;

 Ptr<SketchCurves> curves = sketch->sketchCurves();
 if (!curves)
  return false;

 Ptr<SketchCircles> circles = curves->sketchCircles();
 if (!circles)
  return false;

 Ptr<Point3D> centerPt = Point3D::create(0, 0, 0);
 if (!centerPt)
  return centerPt;
 for (int i = 0; i < 50; ++i)
 {
  // If progress dialog is canceled, stop drawing.
  if (progressDialog->wasCancelled())
   break;
  circles->addByCenterRadius(centerPt, i + 1);

  // Update progress value of progress dialog
  progressDialog->progressValue(i + 1);
 }

 // Hide the progress dialog at the end.
 progressDialog->hide();

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