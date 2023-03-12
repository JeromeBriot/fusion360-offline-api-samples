#include <Core/Utils.h>
#include <Core/Application/Application.h>
#include <Core/Application/Product.h>
#include <Core/Application/CustomEvents.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/CommandDefinitions.h>
#include <Core/UserInterface/CommandDefinition.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Fusion/ModelParameters.h>
#include <Fusion/Fusion/ModelParameter.h>

#include <thread> 
#include <sstream>
#include <chrono>
#include <random>

using namespace adsk::core;
using namespace adsk::fusion;

const std::string myCustomEvent = "MyCustomEventId1";

Ptr<Application> app;
Ptr<UserInterface> ui;
Ptr<CustomEvent> customEvent;
bool stopFlag;
std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(1000,10000);

class ThreadEventHandler : public CustomEventHandler
{
public:
 void notify(const Ptr<CustomEventArgs>& eventArgs) override
 {
  if (eventArgs)
  {
   // Make sure a command isn't running before changes are made.
   if (ui->activeCommand() != "SelectCommand")
   {
    Ptr<CommandDefinitions> cmdDefs = ui->commandDefinitions();
    cmdDefs->itemById("SelectCommand")->execute();
   }

   Ptr<Design> design = app->activeProduct();
   if (!design)
    return;

   Ptr<Component> rootComp = design->rootComponent();
   if (!rootComp)
    return;

   Ptr<ModelParameters> params = rootComp->modelParameters();
   if (!params)
    return;

   Ptr<ModelParameter> param = params->itemByName("Length");
   if (!param)
    return;

   // Get the value that was passed in from other thread and set the paraemter value.
   std::string info = eventArgs->additionalInfo();
   param->value(std::stod(info));
  }
 }
} onCustomEvent_;

void myThreadRun()
{
 while (!stopFlag) {
  double randVal = distribution(generator);
  std::string additionalInfo = std::to_string(randVal/1000.0);
  app->fireCustomEvent(myCustomEvent, additionalInfo);

  std::this_thread::sleep_for(std::chrono::seconds(2));
 }
}

extern "C" XI_EXPORT bool run(const char* context)
{
 app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;

 customEvent = app->registerCustomEvent(myCustomEvent);
 if (!customEvent)
  return false;
 customEvent->add(&onCustomEvent_);

 stopFlag = false;
 std::thread myThread(myThreadRun);
 myThread.detach();

 return true;
}

extern "C" XI_EXPORT bool stop(const char* context)
{
 if (ui)
 {
  customEvent->remove(&onCustomEvent_);
  stopFlag = true;
  app->unregisterCustomEvent(myCustomEvent);
  ui->messageBox("Stop addin");
  ui = nullptr;
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