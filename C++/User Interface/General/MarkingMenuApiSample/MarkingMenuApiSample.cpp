#include <Core/Utils.h>
#include <Core/Application/Application.h>
#include <Core/Application/Product.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/CommandCreatedEventHandler.h>
#include <Core/UserInterface/CommandCreatedEvent.h>
#include <Core/UserInterface/CommandCreatedEventArgs.h>
#include <Core/UserInterface/CommandEvent.h>
#include <Core/UserInterface/CommandEventArgs.h>
#include <Core/UserInterface/CommandEventHandler.h>
#include <Core/UserInterface/MarkingMenuEvent.h>
#include <Core/UserInterface/MarkingMenuEventArgs.h>
#include <Core/UserInterface/MarkingMenuEventHandler.h>
#include <Core/UserInterface/RadialMarkingMenu.h>
#include <Core/UserInterface/LinearMarkingMenu.h>
#include <Core/UserInterface/Command.h>
#include <Core/UserInterface/CommandDefinition.h>
#include <Core/UserInterface/CommandDefinitions.h>
#include <Core/UserInterface/ToolbarControls.h>
#include <Core/UserInterface/DropDownControl.h>
#include <Core/UserInterface/Selection.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepVertex.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/SketchEntity.h>
#include <Fusion/Construction/ConstructionPlane.h>

using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;
Ptr<UserInterface> ui;
std::vector<Ptr<Base>> entities;
static void setLinearMarkingMenu(const Ptr<MarkingMenuEventArgs>& eventArgs)
{
 Ptr<CommandDefinitions> definitions = ui->commandDefinitions();
 if (!definitions)
  return;
 Ptr<CommandDefinition> cmdDefSelectedEntities = definitions->itemById("PrintSelectedEntities");
 Ptr<CommandDefinition> cmdDef = definitions->itemById("TestCommand");
 if (!cmdDef || !cmdDefSelectedEntities)
  return;

 Ptr<LinearMarkingMenu> linearMenu = eventArgs->linearMarkingMenu();
 if (!linearMenu)
  return;
 // Clear current linear menu
 linearMenu->clear();

 // Add test command, add print-entity command
 Ptr<ToolbarControls> controls = linearMenu->controls();
 controls->addCommand(cmdDef);
 controls->addCommand(cmdDefSelectedEntities);
 // Add a separator line
 controls->addSeparator("LinearSeparator");
 // Add a sub linear menu
 Ptr<DropDownControl> dropdown = controls->addDropDown("Linear Sub Menu", "", "LinearSubMenu");
 Ptr<ToolbarControls> subControls = dropdown->controls();
 subControls->addCommand(cmdDef);
 subControls->addCommand(cmdDefSelectedEntities);
 // Add a sub-sub linear menu
 if (!subControls)
  return;
 Ptr<DropDownControl> subDropdown = subControls->addDropDown("Linear sub-sub menu", "", "LinearSubSub");
 Ptr<ToolbarControls> subsubControls = subDropdown->controls();
 subsubControls->addCommand(cmdDef);
 subsubControls->addCommand(cmdDefSelectedEntities);

 // Add some special command if selecting BRep/sketch entities.
 std::vector<Ptr<Base>> selectedEntities = eventArgs->selectedEntities();
 if (selectedEntities.size()) {
  Ptr<Base> sel0 = selectedEntities[0];
  // special command if BRep entity selected
  if (sel0->query<BRepFace>() || sel0->query<BRepEdge>() || sel0->query<BRepBody>() || sel0->query<BRepVertex>()) {
   Ptr<CommandDefinition> cmdDefBRepSpecial = definitions->itemById("BrepCommand");
   controls->addCommand(cmdDefBRepSpecial);
  }
  // special command if sketch entity selected
  if (sel0->query<Sketch>() || sel0->query<Profile>() || sel0->query<SketchEntity>()) {
   Ptr<CommandDefinition> cmdDefSketchSpecial = definitions->itemById("SketchCommand");
   controls->addCommand(cmdDefSketchSpecial);
  }
 }
}

static void setRadialMarkingMenu(const Ptr<MarkingMenuEventArgs>& eventArgs)
{
 Ptr<CommandDefinitions> definitions = ui->commandDefinitions();
 if (!definitions)
  return;
 Ptr<CommandDefinition> cmdDefSelectedEntities = definitions->itemById("PrintSelectedEntities");
 Ptr<CommandDefinition> cmdDef = definitions->itemById("TestCommand");
 if (!cmdDef || !cmdDefSelectedEntities)
  return;

 Ptr<RadialMarkingMenu> radialMenu = eventArgs->radialMarkingMenu();
 if (!radialMenu)
  return;

 // Clear current radial menu
 radialMenu->clear();

 // Create sub radial menu
 Ptr<RadialMarkingMenu> subRadial = radialMenu->create("test");
 subRadial->text("sub");

 // Create sub-sub radial menu
 Ptr<RadialMarkingMenu> subsubRadial = subRadial->create("sub sub");

 // sub-sub radial menu layout
 subsubRadial->westCommand(cmdDef);
 subsubRadial->northCommand(cmdDef);
 subsubRadial->southCommand(cmdDefSelectedEntities);
 subsubRadial->eastCommand(cmdDef);

 // sub radial menu layout
 subRadial->northwestCommand(subsubRadial);
 subRadial->southeastCommand(cmdDef);
 subRadial->southwestCommand(cmdDef);
 subRadial->northeastCommand(cmdDefSelectedEntities);

 // root radial menu layout
 radialMenu->westCommand(cmdDef);
 radialMenu->northCommand(cmdDef);
 radialMenu->southCommand(cmdDef);
 radialMenu->eastCommand(cmdDef);
 radialMenu->northeastCommand(subRadial);
 radialMenu->northwestCommand(cmdDefSelectedEntities);
 radialMenu->southeastCommand(cmdDef);
 radialMenu->southwestCommand(cmdDef);
}

class MyMarkingMenuHandler : public adsk::core::MarkingMenuEventHandler
{
public:
 void notify(const Ptr<MarkingMenuEventArgs>& eventArgs) override
 {
  setLinearMarkingMenu(eventArgs);
  setRadialMarkingMenu(eventArgs);

  entities.clear();
  entities = eventArgs->selectedEntities();
 }
} onMarkingMenuDisplaying_;

class CommandExecutedHandler : public adsk::core::CommandEventHandler
{
public:
 void notify(const Ptr<CommandEventArgs>& eventArgs) override
 {
  Ptr<Event> firingEvent = eventArgs->firingEvent();
  if (!firingEvent)
   return;

  Ptr<Command> command = firingEvent->sender();
  if (!command)
   return;

  Ptr<CommandDefinition> parentDefinition = command->parentCommandDefinition();
  if (!parentDefinition)
   return;

  Ptr<MarkingMenuEvent> markingMenuEvent = ui->markingMenuDisplaying();
  if (!markingMenuEvent)
   return;

  if (parentDefinition->id() == "PrintSelectedEntities") {
   if (!entities.empty()) {
    std::string msg = "selected entities:";
    for (Ptr<Base> obj : entities) {
     msg += '\n' + obj->objectType();
    }
    ui->messageBox(msg);
   }
   else {
    ui->messageBox("No selected entity.");
   }
  }
  else {
   ui->messageBox("command " + parentDefinition->id() + " triggered");
  }
 }
};

class CommandCreatedHandler : public adsk::core::CommandCreatedEventHandler
{
public:
 void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
 {
  if (eventArgs)
  {
   Ptr<Command> command = eventArgs->command();
   if (!command)
    return;

   Ptr<CommandEvent> exec = command->execute();
   if (!exec)
    return;
   exec->add(&onCommandExecuted_);
  }
 }
private:
 CommandExecutedHandler onCommandExecuted_;
} onCommandCreated_;


extern "C" XI_EXPORT bool run(const char* context)
{
 app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;

 // Add customized handler for marking menu displaying
 Ptr<MarkingMenuEvent> markingMenuEvent = ui->markingMenuDisplaying();
 if (!markingMenuEvent)
  return false;
 markingMenuEvent->add(&onMarkingMenuDisplaying_);

 // Add customized handler for marking menu displaying
 // Create a command to print selected entities
 Ptr<CommandDefinitions> definitions = ui->commandDefinitions();
 if (!definitions)
  return false;
 Ptr<CommandDefinition> cmdDefSelectedEntities = definitions->itemById("PrintSelectedEntities");
 if (!cmdDefSelectedEntities) {
  cmdDefSelectedEntities = definitions->addButtonDefinition("PrintSelectedEntities", "Print Entities", "Print selected entities.");
  if (!cmdDefSelectedEntities)
   return false;
  Ptr<CommandCreatedEvent> printCmdCreatedEvent = cmdDefSelectedEntities->commandCreated();
  if (!printCmdCreatedEvent)
   return false;
  printCmdCreatedEvent->add(&onCommandCreated_);
 }

 // Create a test command
 Ptr<CommandDefinition> cmdDef = definitions->itemById("TestCommand");
 if (!cmdDef) {
  cmdDef = definitions->addButtonDefinition("TestCommand", "Test Command", "Test Command");
  if (!cmdDef)
   return false;
  Ptr<CommandCreatedEvent> testCmdCreatedEvent = cmdDef->commandCreated();
  if (!testCmdCreatedEvent)
   return false;
  testCmdCreatedEvent->add(&onCommandCreated_);
 }

 // Create special command for BRep entities
 Ptr<CommandDefinition> cmdDefBRepSpecial = definitions->itemById("BrepCommand");
 if (!cmdDefBRepSpecial) {
  cmdDefBRepSpecial = definitions->addButtonDefinition("BrepCommand", "Brep Command", "This is a command for BRep entities.");
  if (!cmdDefBRepSpecial)
   return false;
  Ptr<CommandCreatedEvent> brepCmdCreatedEvent = cmdDefBRepSpecial->commandCreated();
  if (!brepCmdCreatedEvent)
   return false;
  brepCmdCreatedEvent->add(&onCommandCreated_);
 }

 // Create special command for sketch entities
 Ptr<CommandDefinition> cmdDefSketchSpecial = definitions->itemById("SketchCommand");
 if (!cmdDefSketchSpecial) {
  cmdDefSketchSpecial = definitions->addButtonDefinition("SketchCommand", "Sketch Command", "This is a command for sketch entities.");
  if (!cmdDefSketchSpecial)
   return false;
  Ptr<CommandCreatedEvent> sketchCmdCreatedEvent = cmdDefSketchSpecial->commandCreated();
  if (!sketchCmdCreatedEvent)
   return false;
  sketchCmdCreatedEvent->add(&onCommandCreated_);
 }

 // prevent this module from being terminate when the script returns, because we are waiting for event handlers to fire
 adsk::autoTerminate(false);

 ui->messageBox("Right click to see the customized marking menu.");

 return true;
}

extern "C" XI_EXPORT bool stop(const char* context)
{
 if (ui)
 {
  Ptr<CommandDefinitions> definitions = ui->commandDefinitions();
  if (!definitions)
   return false;

  Ptr<CommandDefinition> cmdDefSelectedEntities = definitions->itemById("PrintSelectedEntities");
  if (cmdDefSelectedEntities)
   cmdDefSelectedEntities->deleteMe();

  Ptr<CommandDefinition> cmdDef = definitions->itemById("TestCommand");
  if (cmdDef)
   cmdDef->deleteMe();

  Ptr<CommandDefinition> cmdDefBRepSpecial = definitions->itemById("BrepCommand");
  if (cmdDefBRepSpecial)
   cmdDefBRepSpecial->deleteMe();

  Ptr<CommandDefinition> cmdDefSketchSpecial = definitions->itemById("SketchCommand");
  if (cmdDefSketchSpecial)
   cmdDefSketchSpecial->deleteMe();

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