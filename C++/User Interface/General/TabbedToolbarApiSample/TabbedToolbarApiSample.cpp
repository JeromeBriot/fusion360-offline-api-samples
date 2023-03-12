#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/CommandDefinition.h>
#include <Core/UserInterface/CommandDefinitions.h>
#include <Core/UserInterface/ToolbarTab.h>
#include <Core/UserInterface/ToolbarPanels.h>
#include <Core/UserInterface/ToolbarPanel.h>
#include <Core/UserInterface/ToolbarPanelList.h>
#include <Core/UserInterface/CommandControl.h>
#include <Core/UserInterface/ToolbarControls.h>
#include <Core/UserInterface/Workspaces.h>
#include <Core/UserInterface/Workspace.h>
#include <Core/UserInterface/ToolbarTabList.h>
#include <Core/UserInterface/ToolbarTab.h>
#include <Core/UserInterface/ToolbarTabs.h>


using namespace adsk::core;
using namespace std;

Ptr<UserInterface> ui;

extern "C" XI_EXPORT bool run(const char* context)
{
 Ptr<Application> app = Application::get();
 if (!app)
 {
  return false;
 }

 ui = app->userInterface();
 if (!ui)
 {
  return false;
 }

 Ptr<CommandDefinitions> cmdDefs = ui->commandDefinitions();
 if (!cmdDefs)
 {
  return false;
 }

 // For this example, we are adding the already exisiting 'Extrude' command into a new panel.
 // Find the 'Extrude' Command:
 Ptr<CommandDefinition> extrudeCmd = cmdDefs->itemById("Extrude");
 if (!extrudeCmd)
 {
  return false;
 }

 // For a few months, the customer might run either classic UI or tabbed toolbar UI.
 // Find out what is being used :
 bool runningTabbedToolbar = ui->isTabbedToolbarUI();
 if (!runningTabbedToolbar)
 {
  // This is meant to be a tabbed toolbar example.
  // Just return if not enabled:
  return false;
 }

 // Get all of the workspaces:
 Ptr<Workspaces> allWorkspaces = ui->workspaces();
 if (!allWorkspaces)
 {
  // Just return if can't get the workspaces:
  return false;
 }

 // Get the Render workspace :
 Ptr<Workspace> renderWorkspace = allWorkspaces->itemById("FusionRenderEnvironment");
 // Get the Design workspace :
 Ptr<Workspace> designWorkspace = allWorkspaces->itemById("FusionSolidEnvironment");
 if (!renderWorkspace || !designWorkspace)
 {
  // Just return if can't Render or Design workspaces.
  // We want to add tabs to those workspaces:
  return false;
 }

 // Get all the tabs for the Render and Design workspaces:
 Ptr<ToolbarTabs> allRenderTabs = renderWorkspace->toolbarTabs();
 Ptr<ToolbarTabs> allDesignTabs = designWorkspace->toolbarTabs();

 if (!allRenderTabs || !allDesignTabs)
 {
  // Just return if we couldn't get the tabs for the 
  // workspaces as expected:
  return false;
 }

 // Add a new tab to the Render and Design workspaces:
 Ptr<ToolbarTab> newRenderTab = allRenderTabs->add("NewRenderTabHere", "New Render Tab");
 Ptr<ToolbarTab> newDesignTab = allDesignTabs->add("NewDesignTabHere", "New Design Tab");
 if (newRenderTab && newDesignTab)
 {
  // Get all of the toolbar panels for the NewRender and NewDesign tab:
  Ptr<ToolbarPanels> allNewRenderTabPanels = newRenderTab->toolbarPanels();
  Ptr<ToolbarPanels> allNewDesignTabPanels = newDesignTab->toolbarPanels();

  // Activate the Render Workspace before trying to activate the newly added Tab:
  bool activeRenderWorkspace = renderWorkspace->activate();
  if (!activeRenderWorkspace)
  {
   // Just return if we can't activate the workspace.
   return false;
  }

  // Has the new render panel been added already ?
  // You'll get an error if you try to add this more than once to the tab.
  Ptr<ToolbarPanel> brandNewRenderPanel = nullptr;
  brandNewRenderPanel = allNewRenderTabPanels->itemById("bestRenderPanelEverId");
  if (!brandNewRenderPanel)
  {
   // We have not added the panel already. Go ahead and add it.
   brandNewRenderPanel = allNewRenderTabPanels->add("bestRenderPanelEverId", "Best Render Panel");
   if (brandNewRenderPanel)
   {
    // Access the controls that belong to the panel:
    Ptr<ToolbarControls> newRenderPanelControls = brandNewRenderPanel->controls();
    if (!newRenderPanelControls)
    {
     // We can't continue if the panel didn't return controls to use:
     return false;
    }

    // Do we already have this command in the controls ?
    // You'll get an error if you try to add it more than once to the panel:
    Ptr<CommandControl> extrudeCmdControl = newRenderPanelControls->itemById("Extrude");

    if (!extrudeCmdControl)
    {
     // Make sure that the new tab is active before adding the command to the panel controls:
     if (!newRenderTab->isActive())
     {
      newRenderTab->activate();
     }
     // Go ahead and add the command to the panel.
     // The command exists and it has not already been added to the 
     // panel:
     extrudeCmdControl = newRenderPanelControls->addCommand(extrudeCmd);
     extrudeCmdControl->isPromotedByDefault(true);
     extrudeCmdControl->isPromoted(true);
     extrudeCmdControl->isVisible(true);
    }
    else
    {
     // If the command is already added to the Panel, activate the tab if necessary:
     if (!newRenderTab->isActive())
     {
      newRenderTab->activate();
     }
    }
   }
  } // end render panel


  // Activate the Design Workspace before trying to activate the newly added Tab:
  bool activeDesignWorkspace = designWorkspace->activate();
  if (!activeDesignWorkspace)
  {
   // Just return if we can't activate the workspace.
   return false;
  }

  // Has the new design panel been added already?
  // You'll get an error if you try to add this more than once to the tab.
  Ptr<ToolbarPanel> brandNewDesignPanel = nullptr;
  brandNewDesignPanel = allNewDesignTabPanels->itemById("bestDesignPanelEverId");
  if (!brandNewDesignPanel)
  {
   // We have not added the panel already. Go ahead and add it.
   brandNewDesignPanel = allNewDesignTabPanels->add("bestDesignPanelEverId", "Best Design Panel");
   if (brandNewDesignPanel)
   {
    // Access the controls that belong to the panel:
    Ptr<ToolbarControls> newDesignPanelControls = brandNewDesignPanel->controls();
    if (!newDesignPanelControls)
    {
     // We can't continue if the panel didn't return controls to use:
     return false;
    }

    // Do we already have this command in the controls ?
    // You'll get an error if you try to add it more than once to the panel:
    Ptr<CommandControl> extrudeCmdControl = newDesignPanelControls->itemById("Extrude");

    if (!extrudeCmdControl)
    {
     // Make sure that the new tab is active before adding the command to the panel controls:
     if (!newDesignTab->isActive())
     {
      newDesignTab->activate();
     }
     // Go ahead and add the command to the panel.
     // The command exists and it has not already been added to the 
     // panel:
     extrudeCmdControl = newDesignPanelControls->addCommand(extrudeCmd);
     extrudeCmdControl->isPromotedByDefault(true);
     extrudeCmdControl->isPromoted(true);
     extrudeCmdControl->isVisible(true);
    }
    else
    {
     // If the command is already added to the Panel, activate the tab if necessary:
     if (!newDesignTab->isActive())
     {
      newDesignTab->activate();
     }
    }
   }
  } // end design panel
 }

 return true;
}

extern "C" XI_EXPORT bool stop(const char* context)
{
 if (ui)
 {
  Ptr<ToolbarPanelList> allToolbarPanels = ui->allToolbarPanels();
  Ptr<ToolbarPanel> brandNewDesignPanel = allToolbarPanels->itemById("bestDesignPanelEverId");
  if (brandNewDesignPanel)
  {
   Ptr<ToolbarControls> controls = brandNewDesignPanel->controls();
   for (size_t i = 0; i < controls->count(); ++i)
   {
    Ptr<ToolbarControl> control = controls->item(i);
    if (control->isValid())
     control->deleteMe();
   }
   brandNewDesignPanel->deleteMe();
  }

  Ptr<ToolbarPanel> brandNewRenderPanel = allToolbarPanels->itemById("bestRenderPanelEverId");
  if (brandNewRenderPanel)
  {
   Ptr<ToolbarControls> controls = brandNewRenderPanel->controls();
   for (size_t i = 0; i < controls->count(); ++i)
   {
    Ptr<ToolbarControl> control = controls->item(i);
    if (control->isValid())
     control->deleteMe();
   }
   brandNewRenderPanel->deleteMe();
  }

  Ptr<ToolbarTabList> allToolbarTabs = ui->allToolbarTabs();

  Ptr<ToolbarTab> newRenderTab = allToolbarTabs->itemById("NewRenderTabHere");
  if (newRenderTab && newRenderTab->isValid())
   newRenderTab->deleteMe();

  Ptr<ToolbarTab> newDesignTab = allToolbarTabs->itemById("NewDesignTabHere");
  if (newDesignTab && newDesignTab->isValid())
   newDesignTab->deleteMe();

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