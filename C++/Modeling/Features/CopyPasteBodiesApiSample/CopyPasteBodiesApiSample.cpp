#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/Document.h>
#include <Core/Application/ObjectCollection.h>

#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Components/Occurrences.h>
#include <Fusion/Components/Occurrence.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/CopyPasteBodies.h>
#include <Fusion/Features/CopyPasteBody.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>

#include <iostream>



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


 // Get active design
 Ptr<Product> product = app->activeProduct();
 if (!product)
  return false;

 Ptr<Design> design = product;
 if (!design)
  return false;

 // Get root component in this design
 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 // Get the first sub component
 Ptr<Occurrences> occs = rootComp->occurrences();
 if (!occs)
  return false;

 Ptr<Occurrence> firstOcc = occs->item(0);
 if (!firstOcc)
  return false;

 Ptr<Component> subComp1 = firstOcc->component();
 if (!subComp1)
  return false;

 // Get the second sub component
 Ptr<Occurrence> secondOcc = occs->item(1);
 if (!secondOcc)
  return false;

 Ptr<Component> subComp2 = secondOcc->component();
 if (!subComp2)
  return false;

 // Get the first body in sub component 1  
 Ptr<BRepBodies> bodiesInSubComp1 = subComp1->bRepBodies();
 if (!bodiesInSubComp1)
  return false;

 Ptr<BRepBody> bodyInSubComp1 = bodiesInSubComp1->item(0);
 if (!bodyInSubComp1)
  return false;

 // Copy/paste body from sub component 1 to sub component 2
 Ptr<Features> featuresInSubComp2 = subComp2->features();
 if (!featuresInSubComp2)
  return false;

 Ptr<CopyPasteBodies> copyPasteBodiesInSubComp2 = featuresInSubComp2->copyPasteBodies();
 if (!copyPasteBodiesInSubComp2)
  return false;

 Ptr<CopyPasteBody> copyPasteBody = copyPasteBodiesInSubComp2->add(bodyInSubComp1);
 if (!copyPasteBody)
  return false;

 Ptr<ObjectCollection> sourceBodies = copyPasteBody->sourceBody();
 if (!sourceBodies)
  return false;

 // Copy/paste bodies from sub component to root component
 Ptr<ObjectCollection> bodyCollection = ObjectCollection::create();
 if (!bodyCollection)
  return false;

 for (size_t i = 0; i < sourceBodies->count(); ++i)
  bodyCollection->add(sourceBodies->item(i));

 Ptr<BRepBodies> bodiesInSubComp2 = subComp2->bRepBodies();
 if (!bodiesInSubComp2)
  return false;

 for (size_t i = 0; i < bodiesInSubComp2->count(); ++i)
  bodyCollection->add(bodiesInSubComp2->item(i));

 Ptr<Features> featuresInRoot = rootComp->features();
 if (!featuresInRoot)
  return false;

 Ptr<CopyPasteBodies> copyPasteBodiesInRoot = featuresInRoot->copyPasteBodies();
 if (!copyPasteBodiesInRoot)
  return false;

 copyPasteBody = copyPasteBodiesInRoot->add(bodyCollection);
 if (!copyPasteBody)
  return false;

 // Dump the information of Copy Paste Body in root component
 for (size_t i = 0; i < copyPasteBodiesInRoot->count(); ++i)
 {
  copyPasteBody = copyPasteBodiesInRoot->item(i);
  if (!copyPasteBody)
   return false;

  std::string copyPasteBodyInfo = "CopyPasteBody: name - " + copyPasteBody->name();
  ui->messageBox(copyPasteBodyInfo);
 }


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