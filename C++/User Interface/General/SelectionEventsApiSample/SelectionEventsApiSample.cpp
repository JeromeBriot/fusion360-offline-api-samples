#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <CAM/CAMAll.h>

#include <algorithm>

using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;

Ptr<Application> app;
Ptr<UserInterface> ui;
std::vector<Ptr<BRepEdge> > selectedEdges;

class MyCommandExecutePreviewHandler : public CommandEventHandler
{
public:
 void notify(const Ptr<CommandEventArgs>& eventArgs) override
 {
  Ptr<Design> design = app->activeProduct();
  if (!design)
   return;
  Ptr<Component> root = design->rootComponent();
  if (!root)
   return;
  Ptr<CustomGraphicsGroups> cggroups = root->customGraphicsGroups();
  if (!cggroups)
   return;
  Ptr<CustomGraphicsGroup> cggroup = cggroups->add();
  if (!cggroup)
   return;
  for (size_t i = 0; i < selectedEdges.size(); ++i)
  {
   Ptr<BRepEdge> edge = selectedEdges[i];
   if (!edge)
    continue;
   Ptr<Point3D> ptOnEdge = edge->pointOnEdge();
   if (!ptOnEdge)
    return;
   Ptr<Matrix3D> transform = Matrix3D::create();
   if (!transform)
    return;
   transform->translation(ptOnEdge->asVector());
   Ptr<CustomGraphicsText> cgtext = cggroup->addText(std::to_string(i+1), "Arial Black", 1, transform);
   if (!cgtext)
    return;
   Ptr<Color> color = Color::create(0, 255, 0, 255);
   if (!color)
    return;
   Ptr<CustomGraphicsSolidColorEffect> cgcolor = CustomGraphicsSolidColorEffect::create(color);
   if (!cgcolor)
    return;
   cgtext->color(cgcolor);
  }
 }
};

class MyCommandDestroyHandler : public CommandEventHandler
{
public:
 void notify(const Ptr<CommandEventArgs>& eventArgs) override
 {
  adsk::terminate();
 }
};

class MyPreSelectHandler : public SelectionEventHandler
{
public:
 void notify(const Ptr<SelectionEventArgs>& eventArgs) override
 {
  if (!eventArgs)
   return;
  Ptr<Selection> selection = eventArgs->selection();
  if (!selection)
   return;
  Ptr<BRepEdge> edge = selection->entity();
  if (!edge)
   return;
  eventArgs->additionalEntities(edge->tangentiallyConnectedEdges());
 }
};

class MyPreSelectMouseMoveHandler : public SelectionEventHandler
{
public:
 void notify(const Ptr<SelectionEventArgs>& eventArgs) override
 {
  if (!eventArgs)
   return;
  Ptr<Selection> selection = eventArgs->selection();
  if (!selection)
   return;
  Ptr<BRepEdge> edge = selection->entity();
  if (!edge)
   return;
  Ptr<Curve3D> geom = edge->geometry();
  if (!geom)
   return;
  Ptr<Design> design = app->activeProduct();
  if (!design)
   return;
  Ptr<Component> root = design->rootComponent();
  if (!root)
   return;
  Ptr<CustomGraphicsGroups> cggroups = root->customGraphicsGroups();
  if (!cggroups)
   return;
  Ptr<CustomGraphicsGroup> cggroup = cggroups->add();
  if (!cggroup)
   return;
  cggroup->id(std::to_string(edge->tempId()));
  Ptr<CustomGraphicsCurve> cgcurve = cggroup->addCurve(geom);
  if (!cgcurve)
   return;
  cgcurve->weight(10);
  Ptr<Color> color = Color::create(255, 0, 0, 255);
  if (!color)
   return;
  Ptr<CustomGraphicsSolidColorEffect> cgcolor = CustomGraphicsSolidColorEffect::create(color);
  if (!cgcolor)
   return;
  cgcurve->color(cgcolor);
 }
};

class MyPreSelectEndHandler : public SelectionEventHandler
{
public:
 void notify(const Ptr<SelectionEventArgs>& eventArgs) override
 {
  if (!eventArgs)
   return;
  Ptr<Selection> selection = eventArgs->selection();
  if (!selection)
   return;
  Ptr<BRepEdge> edge = selection->entity();
  if (!edge)
   return;
  Ptr<Design> design = app->activeProduct();
  if (!design)
   return;
  Ptr<Component> root = design->rootComponent();
  if (!root)
   return;
  Ptr<CustomGraphicsGroups> cggroups = root->customGraphicsGroups();
  if (!cggroups)
   return;
  std::string edgeId = std::to_string(edge->tempId());
  for (size_t i = 0; i < cggroups->count(); ++i)
  {
   Ptr<CustomGraphicsGroup> group = cggroups->item(i);
   if (group && (group->id() == edgeId))
   {
    group->deleteMe();
    break;
   }
  }
 }
};

class MySelectHandler : public SelectionEventHandler
{
public:
 void notify(const Ptr<SelectionEventArgs>& eventArgs) override
 {
  if (!eventArgs)
   return;
  Ptr<Selection> selection = eventArgs->selection();
  if (!selection)
   return;
  Ptr<BRepEdge> edge = selection->entity();
  if (!edge)
   return;
  selectedEdges.emplace_back(edge);
 }
};

class MyUnSelectHandler : public SelectionEventHandler
{
public:
 void notify(const Ptr<SelectionEventArgs>& eventArgs) override
 {
  if (!eventArgs)
   return;
  Ptr<Selection> selection = eventArgs->selection();
  if (!selection)
   return;
  Ptr<BRepEdge> edge = selection->entity();
  if (!edge)
   return;
  for (std::vector<Ptr<BRepEdge> >::iterator it = selectedEdges.begin(); it != selectedEdges.end(); ++it)
  {
   if ((*it) && ((*it)->tempId() == edge->tempId()))
   {
    selectedEdges.erase(it);
    break;
   }
  }
 }
};

class MyCommandCreatedHandler : public CommandCreatedEventHandler
{
public:
 void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
 {
  if (!eventArgs)
   return;
  Ptr<Command> cmd = eventArgs->command();
  if (!cmd)
   return;
  cmd->isExecutedWhenPreEmpted(false);
  Ptr<CommandInputs> inputs = cmd->commandInputs();
  if (!inputs)
   return;
  Ptr<SelectionCommandInput> selectInput = inputs->addSelectionInput("SelectionEventsSample", "Edges", "Please select edges");
  if (!selectInput)
   return;
  selectInput->addSelectionFilter(SelectionCommandInput::Edges);
  selectInput->setSelectionLimits(1);
  Ptr<CommandEvent> executePreview = cmd->executePreview();
  if (!executePreview)
   return;
  executePreview->add(&m_cmdPreviewHandler);
  Ptr<CommandEvent> destroy = cmd->destroy();
  if (!destroy)
   return;
  destroy->add(&m_cmdDestroyHandler);
  Ptr<SelectionEvent> preSelect = cmd->preSelect();
  if (!preSelect)
   return;
  preSelect->add(&m_preSelectHandler);
  Ptr<SelectionEvent> preSelectMouseMove = cmd->preSelectMouseMove();
  if (!preSelectMouseMove)
   return;
  preSelectMouseMove->add(&m_preSelectMouseMoveHandler);
  Ptr<SelectionEvent> preSelectEnd = cmd->preSelectEnd();
  if (!preSelectEnd)
   return;
  preSelectEnd->add(&m_preSelectEndHandler);
  Ptr<SelectionEvent> select = cmd->select();
  if (!select)
   return;
  select->add(&m_selectHandler);
  Ptr<SelectionEvent> unselect = cmd->unselect();
  if (!unselect)
   return;
  unselect->add(&m_unSelectHandler);
 }

private:
 MyCommandExecutePreviewHandler m_cmdPreviewHandler;
 MyCommandDestroyHandler m_cmdDestroyHandler;
 MyPreSelectHandler m_preSelectHandler;
 MyPreSelectMouseMoveHandler m_preSelectMouseMoveHandler;
 MyPreSelectEndHandler m_preSelectEndHandler;
 MySelectHandler m_selectHandler;
 MyUnSelectHandler m_unSelectHandler;
} myCmdCreatedHandler;

extern "C" XI_EXPORT bool run(const char* context)
{
 app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;

 Ptr<CommandDefinitions> cmdDefs = ui->commandDefinitions();
 if (!cmdDefs)
  return false;

 Ptr<CommandDefinition> myCmdDef = cmdDefs->itemById("SelectionEventsSample_Cpp");
 if (!myCmdDef)
  myCmdDef = cmdDefs->addButtonDefinition("SelectionEventsSample_Cpp", "Selection Events Sample", "", "");
 if (!myCmdDef)
  return false;

 Ptr<CommandCreatedEvent> createdEvent = myCmdDef->commandCreated();
 if (!createdEvent)
  return false;
 createdEvent->add(&myCmdCreatedHandler);

 // Execute the command definition.
 myCmdDef->execute();

 // Prevent this module from being terminated when the script returns, because we are waiting for event handlers to fire.
 adsk::autoTerminate(false);

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