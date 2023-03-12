# Author-Autodesk Inc.
# Description-Demonstrates how to work with the tabbed toolbar API

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface

        # For this example, we are adding the already exisiting 'Extrude' command into a new panel:
        cmdDefinitions = ui.commandDefinitions
        anotherExtrudeCmd = cmdDefinitions.itemById('Extrude')
                
        # For a few months, the customer might run either classic UI or tabbed toolbar UI.
        # Find out what is being used:
        runningTabbedToolbar = ui.isTabbedToolbarUI

        if runningTabbedToolbar:
            # Get all workspaces:
            allWorkspaces = ui.workspaces
            # Get the Render workspace:
            renderWorkspace = allWorkspaces.itemById('FusionRenderEnvironment')
            # Get the Design workspace:
            designWorkspace = allWorkspaces.itemById('FusionSolidEnvironment')
            if (renderWorkspace and designWorkspace):
                # Get all the tabs for the Render and Design workspaces:
                allRenderTabs = renderWorkspace.toolbarTabs
                allDesignTabs = designWorkspace.toolbarTabs
                if ((allRenderTabs.count > 0) and (allDesignTabs.count > 0)):
                    # Add a new tab to the Render and Design workspaces:
                    newRenderTab = allRenderTabs.add('NewRenderTabHere', 'New Render Tab')
                    newDesignTab = allDesignTabs.add('NewDesignTabHere', 'New Design Tab')
                    if (newRenderTab and newDesignTab):
                        # Get all of the toolbar panels for the NewRender and NewDesign tab:
                        allNewRenderTabPanels = newRenderTab.toolbarPanels
                        allNewDesignTabPanels = newDesignTab.toolbarPanels

                        # Has the panel been added already?
                        # You'll get an error if you try to add this more than once to the tab.

                        #Activate the Render Workspace before activating the newly added Tab
                        renderWorkspace.activate()
                            
                        brandNewRenderPanel = None
                        brandNewRenderPanel = allNewRenderTabPanels.itemById('bestRenderPanelEverId')
                        if brandNewRenderPanel is None:
                            # We have not added the panel already.  Go ahead and add it.
                            brandNewRenderPanel = allNewRenderTabPanels.add('bestRenderPanelEverId', 'Best Render Panel')

                        if brandNewRenderPanel:
                            # Access the controls that belong to the panel:
                            newPanelControls = brandNewRenderPanel.controls

                            # Do we already have this command in the controls?  
                            # You'll get an error if you try to add it more than once to the panel:
                            extrudeCmdControl =  None
                            extrudeCmdControl = newPanelControls.itemById('Extrude')
                            if extrudeCmdControl is None:                        
                                # Activate the newly added Tab in Render Workspace before adding command to the Panel
                                if renderWorkspace.isActive: 
                                    renderTab = allRenderTabs.itemById('NewRenderTabHere')
                                    if not renderTab.isActive :
                                        activationState = renderTab.activate()
                                        if activationState:
                                            if anotherExtrudeCmd:
                                                # Go ahead and add the command to the panel:
                                                extrudeCmdControl = newPanelControls.addCommand(anotherExtrudeCmd)
                                                if extrudeCmdControl:
                                                    extrudeCmdControl.isVisible = True
                                                    extrudeCmdControl.isPromoted = True
                                                    extrudeCmdControl.isPromotedByDefault = True
                                                    ui.messageBox('Do you see Best Render Panel now?')
                                            
                            else:
                                # If the command is already added to the Panel check if it is visible and display a message
                                if renderWorkspace.isActive: 
                                    renderTab = allRenderTabs.itemById('NewRenderTabHere')
                                    if not renderTab.isActive :
                                        activationState = renderTab.activate()
                                        if activationState :
                                            if brandNewRenderPanel.isVisible:
                                                ui.messageBox('Do you see Best Render Panel now?')     
                                            else:
                                                totalControlsInPanel = newPanelControls.count
                                                if (totalControlsInPanel == 1):
                                                    if extrudeCmdControl.isVisible:
                                                        ui.messageBox('Not visible control')
                        #Activate the Design workspace before activating the newly added Tab
                        designWorkspace.activate()
                                

                        brandNewDesignPanel = None
                        brandNewDesignPanel = allNewDesignTabPanels.itemById('bestDesignPanelEverId')
                        if brandNewDesignPanel is None:
                            # We have not added the panel already.  Go ahead and add it.
                            brandNewDesignPanel = allNewDesignTabPanels.add('bestDesignPanelEverId', 'Best Design Panel')

                        if brandNewDesignPanel:
                            # Access the controls that belong to the panel:
                            newPanelControls = brandNewDesignPanel.controls

                            # Do we already have this command in the controls?  
                            # You'll get an error if you try to add it more than once to the panel:
                            extrudeCmdControl =  None
                            extrudeCmdControl = newPanelControls.itemById('Extrude')
                            if extrudeCmdControl is None:
                            
                            # Activate the newly added Tab in Design workspace before adding Command to the Panel
                                 if designWorkspace.isActive: 
                                    designTab = allDesignTabs.itemById('NewDesignTabHere')
                                    if not designTab.isActive :
                                        activationState = designTab.activate()
                                        if activationState :
                                            if anotherExtrudeCmd:
                                                # Go ahead and add the command to the panel:
                                                extrudeCmdControl = newPanelControls.addCommand(anotherExtrudeCmd)
                                                if extrudeCmdControl:
                                                    extrudeCmdControl.isVisible = True
                                                    extrudeCmdControl.isPromoted = True
                                                    extrudeCmdControl.isPromotedByDefault = True
                                                    ui.messageBox('Do you see Best Design Panel now?')
                                            
                            else:
                                # If the command is already added to the Panel check if it is visible and display a message
                                if designWorkspace.isActive:
                                    designTab = allDesignTabs.itemById('NewDesignTabHere')
                                    if not designTab.isActive :
                                        activationState = designTab.activate()
                                        if activationState :
                                            if brandNewDesignPanel.isVisible:
                                                ui.messageBox('Do you see Best Design Panel now?')     
                                            else:
                                                totalControlsInPanel = newPanelControls.count
                                                if (totalControlsInPanel == 1):
                                                    if extrudeCmdControl.isVisible:
                                                        ui.messageBox('Not visible control')
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
   
# When the addin stops we need to clean up the ui
def stop(context):
    app = adsk.core.Application.get()
    ui = app.userInterface

    try:
        # Get all the toolbar panels
        allToolbarPanels = ui.allToolbarPanels

        # See if our design panel still exists
        brandNewDesignPanel = allToolbarPanels.itemById('bestDesignPanelEverId')
        if brandNewDesignPanel:
            # Remove the controls we added to our panel
            for control in brandNewDesignPanel.controls:
                if control.isValid:
                    control.deleteMe()

            # Remove our panel
            brandNewDesignPanel.deleteMe()

        # See if our render panel still exists
        brandNewRenderPanel = allToolbarPanels.itemById('bestRenderPanelEverId')
        if brandNewRenderPanel:
            # Remove the controls we added to our panel
            for control in brandNewRenderPanel.controls:
                if control.isValid:
                    control.deleteMe()

            # Remove our panel
            brandNewRenderPanel.deleteMe()

        # Get all of the toolbar tabs
        allToolbarTabs = ui.allToolbarTabs

        # See if our render tab still exists
        newRenderTab = allToolbarTabs.itemById('NewRenderTabHere')
        if brandNewDesignPanel:
            if newRenderTab.isValid:
                newRenderTab.deleteMe()

        # See if our design tab still exists
        newDesignTab = allToolbarTabs.itemById('NewDesignTabHere')
        if brandNewDesignPanel:
            if newDesignTab.isValid:
                newDesignTab.deleteMe()
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))