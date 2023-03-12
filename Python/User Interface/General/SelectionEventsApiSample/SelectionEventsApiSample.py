#Author-Autodesk
#Description-Demo how to use selection events of a command.

import adsk.core, adsk.fusion, adsk.cam, traceback

ui = adsk.core.UserInterface.cast(None)
handlers = []
selectedEdges = []

class MyCommandCreatedHandler(adsk.core.CommandCreatedEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            cmd = args.command
            cmd.isExecutedWhenPreEmpted = False
            inputs = cmd.commandInputs
            
            selectInput = inputs.addSelectionInput('SelectionEventsSample', 'Edges', 'Please select edges')
            selectInput.addSelectionFilter(adsk.core.SelectionCommandInput.Edges)
            selectInput.setSelectionLimits(1)
            
            # Connect to the command related events.
            onExecutePreview = MyCommandExecutePreviewHandler()
            cmd.executePreview.add(onExecutePreview)
            handlers.append(onExecutePreview)        

            onDestroy = MyCommandDestroyHandler()
            cmd.destroy.add(onDestroy)
            handlers.append(onDestroy)  
            
            onPreSelect = MyPreSelectHandler()
            cmd.preSelect.add(onPreSelect)
            handlers.append(onPreSelect)
            
            onPreSelectMouseMove = MyPreSelectMouseMoveHandler()
            cmd.preSelectMouseMove.add(onPreSelectMouseMove)
            handlers.append(onPreSelectMouseMove)

            onPreSelectEnd = MyPreSelectEndHandler()
            cmd.preSelectEnd.add(onPreSelectEnd)
            handlers.append(onPreSelectEnd)

            onSelect = MySelectHandler()
            cmd.select.add(onSelect)
            handlers.append(onSelect) 
            
            onUnSelect = MyUnSelectHandler()
            cmd.unselect.add(onUnSelect)            
            handlers.append(onUnSelect) 
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
                
class MyCommandExecutePreviewHandler(adsk.core.CommandEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            app = adsk.core.Application.get()
            design = adsk.fusion.Design.cast(app.activeProduct)
            if design:
                cggroup = design.rootComponent.customGraphicsGroups.add()
                for i in range(0, len(selectedEdges)):
                    edge = adsk.fusion.BRepEdge.cast(selectedEdges[i])
                    transform = adsk.core.Matrix3D.create()
                    transform.translation = edge.pointOnEdge.asVector()
                    cgtext = cggroup.addText(str(i+1), 'Arial Black', 1, transform)
                    cgtext.color = adsk.fusion.CustomGraphicsSolidColorEffect.create(adsk.core.Color.create(0,255,0,255))
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))                

class MyCommandDestroyHandler(adsk.core.CommandEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            # when the command is done, terminate the script
            # this will release all globals which will remove all event handlers
            adsk.terminate()
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
                
class MyPreSelectHandler(adsk.core.SelectionEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            selectedEdge = adsk.fusion.BRepEdge.cast(args.selection.entity)
            if selectedEdge:
                args.additionalEntities = selectedEdge.tangentiallyConnectedEdges        
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc())) 
                
class MyPreSelectMouseMoveHandler(adsk.core.SelectionEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            app = adsk.core.Application.get()
            design = adsk.fusion.Design.cast(app.activeProduct)
            selectedEdge = adsk.fusion.BRepEdge.cast(args.selection.entity) 
            if design and selectedEdge:
                group = design.rootComponent.customGraphicsGroups.add()
                group.id = str(selectedEdge.tempId)
                cgcurve = group.addCurve(selectedEdge.geometry)
                cgcurve.color = adsk.fusion.CustomGraphicsSolidColorEffect.create(adsk.core.Color.create(255,0,0,255))
                cgcurve.weight = 10      
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
                
class MyPreSelectEndHandler(adsk.core.SelectionEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            app = adsk.core.Application.get()
            design = adsk.fusion.Design.cast(app.activeProduct)
            selectedEdge = adsk.fusion.BRepEdge.cast(args.selection.entity) 
            if design and selectedEdge:
                for group in design.rootComponent.customGraphicsGroups:
                    if group.id == str(selectedEdge.tempId):
                        group.deleteMe()
                        break       
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
                
class MySelectHandler(adsk.core.SelectionEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            selectedEdge = adsk.fusion.BRepEdge.cast(args.selection.entity) 
            if selectedEdge:
                selectedEdges.append(selectedEdge)
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
                
class MyUnSelectHandler(adsk.core.SelectionEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            selectedEdge = adsk.fusion.BRepEdge.cast(args.selection.entity) 
            if selectedEdge:
                selectedEdges.remove(selectedEdge)
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))

def run(context):
    global ui
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        myCmdDef = ui.commandDefinitions.itemById('SelectionEventsSample_Python')
        if myCmdDef is None:
            myCmdDef = ui.commandDefinitions.addButtonDefinition('SelectionEventsSample_Python', 'Selection Events Sample', '', '')
        
        # Connect to the command created event.
        onCommandCreated = MyCommandCreatedHandler()
        myCmdDef.commandCreated.add(onCommandCreated)
        handlers.append(onCommandCreated)
        
        # Execute the command.
        myCmdDef.execute()

        # prevent this module from being terminate when the script returns, because we are waiting for event handlers to fire
        adsk.autoTerminate(False)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))