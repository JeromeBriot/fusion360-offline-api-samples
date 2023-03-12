#Author-Autodesk Inc.
#Description-Demonstrates how to customize marking menu and context menu


import adsk.core, adsk.fusion, adsk.cam, traceback

# global mapping list of event handlers to keep them referenced for the duration of the command
#handlers = {}
handlers = []
cmdDefs = []
entities = []

def run(context):
    ui = None
    handlers.clear()
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        def setLinearMarkingMenu(args):
            try:
                menuArgs = adsk.core.MarkingMenuEventArgs.cast(args)
                cmdDefSelectedEntities = ui.commandDefinitions.itemById('PrintSelectedEntities')
                cmdDef = ui.commandDefinitions.itemById('TestCommand')
                
                commands = []
                commands.append(cmdDefSelectedEntities)
                commands.append(cmdDef)
                    
                linearMenu = menuArgs.linearMarkingMenu                                        
                # linear
                linearMenu.clear()
                linearMenu.controls.addCommand(cmdDef)
                linearMenu.controls.addCommand(cmdDefSelectedEntities)
                linearMenu.controls.addSeparator('LinearSeparator')
                dropdown = linearMenu.controls.addDropDown('Linear Sub Menu', '', 'LinearSubMenu')
                subDropDown = dropdown.controls.addDropDown('sub sub', '', 'Sub Sub')
                for cmd in commands:
                    dropdown.controls.addCommand(cmd)
                    subDropDown.controls.addCommand(cmd)
                
                if args.selectedEntities:
                    sel0 = args.selectedEntities[0]
                    # special command if brep entities selected
                    body = adsk.fusion.BRepBody.cast(sel0)
                    face = adsk.fusion.BRepFace.cast(sel0)
                    edge = adsk.fusion.BRepEdge.cast(sel0)
                    vertex = adsk.fusion.BRepVertex.cast(sel0)
                    if body or face or edge or vertex:
                        cmdDefSpecial = ui.commandDefinitions.itemById('BrepCommand')
                        linearMenu.controls.addCommand(cmdDefSpecial)
                    
                    # special command if sketch entities selected
                    sketch = adsk.fusion.Sketch.cast(sel0)
                    prof = adsk.fusion.Profile.cast(sel0)
                    sketchEntity = adsk.fusion.SketchEntity.cast(sel0)
                    if sketch or sketchEntity or prof:
                        cmdDefSpecial = ui.commandDefinitions.itemById('SketchCommand')
                        linearMenu.controls.addCommand(cmdDefSpecial)        
            except:
                if ui:
                    ui.messageBox('setting linear menu failed: {}').format(traceback.format_exc())

        def setRadialMarkingMenu(args):
            try:
                menuArgs = adsk.core.MarkingMenuEventArgs.cast(args)    
                cmdDefSelectedEntities = ui.commandDefinitions.itemById('PrintSelectedEntities')
                cmdDef = ui.commandDefinitions.itemById('TestCommand')
                    
                radialMenu = menuArgs.radialMarkingMenu            
                # radial
                radialMenu.clear()
                
                subRadial = radialMenu.create("test")
                subRadial.text = "sub"
                
                subsubRadial = subRadial.create('sub sub')
                # sub sub
                subsubRadial.westCommand = cmdDef
                subsubRadial.northCommand = cmdDef
                subsubRadial.southCommand = cmdDefSelectedEntities
                subsubRadial.eastCommand = cmdDef
                
                # sub radial menu
                subRadial.northwestCommand = subsubRadial
                subRadial.southeastCommand = cmdDef
                subRadial.southwestCommand = cmdDef
                subRadial.northeastCommand = cmdDefSelectedEntities
                
                # root radial menu
                radialMenu.eastCommand = cmdDef
                radialMenu.westCommand = cmdDef
                radialMenu.northCommand = cmdDef
                radialMenu.southCommand = cmdDef
                radialMenu.northeastCommand = subRadial
                radialMenu.northwestCommand = cmdDefSelectedEntities
                radialMenu.southeastCommand = cmdDef
                radialMenu.southwestCommand = cmdDef
                
            except:
                if ui:
                    ui.messageBox('setting radial menu failed: {}').format(traceback.format_exc())

        class MyCommandCreatedEventHandler(adsk.core.CommandCreatedEventHandler):
            def __init__(self):
                super().__init__()
            def notify(self, args):
                try:
                    command = args.command                                     
                    onCommandExcute = MyCommandExecuteHandler()
                    handlers.append(onCommandExcute)
                    command.execute.add(onCommandExcute)
                except:
                    ui.messageBox('command created failed: {}').format(traceback.format_exc())
        
        class MyCommandExecuteHandler(adsk.core.CommandEventHandler):
            def __init__(self):
                super().__init__()
            def notify(self, args):
                try:
                    command = args.firingEvent.sender
                    cmdDef = command.parentCommandDefinition
                    if cmdDef:
                        if cmdDef.id == 'PrintSelectedEntities':
                            if entities:
                                entityList = 'selected entities:'
                                for e in entities:
                                    entityList += '\n' + str(e)
                                ui.messageBox(entityList)
                            else:
                                ui.messageBox('No selected entity.')                                    
                        else:
                            ui.messageBox('command {} triggered.'.format(cmdDef.id))
                    else:
                        ui.messageBox('No CommandDefinition')
                except:
                    if ui:
                        ui.messageBox('command executed failed: {}').format(traceback.format_exc())
                        

        class MyMarkingMenuHandler(adsk.core.MarkingMenuEventHandler):
            def __init__(self):
                super().__init__()
            def notify(self, args):
                try:                    
                    setLinearMarkingMenu(args)
                    setRadialMarkingMenu(args)
                    
                    # selected entities
                    global entities
                    entities.clear()
                    entities = args.selectedEntities
                except:
                    if ui:
                        ui.messageBox('Marking Menu Displaying event failed: {}'.format(traceback.format_exc()))
        
        # Add customized handler for marking menu displaying
        onMarkingMenuDisplaying = MyMarkingMenuHandler()                   
        handlers.append(onMarkingMenuDisplaying)                     
        ui.markingMenuDisplaying.add(onMarkingMenuDisplaying)
        
        # Add customized handler for commands creating
        onCommandCreated = MyCommandCreatedEventHandler()        
        handlers.append(onCommandCreated)

        # Create a command to print selected entities
        cmdDefSelectedEntities = ui.commandDefinitions.itemById('PrintSelectedEntities')
        if not cmdDefSelectedEntities:
            cmdDefSelectedEntities = ui.commandDefinitions.addButtonDefinition('PrintSelectedEntities', 'Print Entities', 'Print selected entities.')     
            cmdDefSelectedEntities.commandCreated.add(onCommandCreated) 
            cmdDefs.append(cmdDefSelectedEntities)
            
        # Create a test command
        cmdDef = ui.commandDefinitions.itemById('TestCommand')
        if not cmdDef:
            cmdDef = ui.commandDefinitions.addButtonDefinition('TestCommand', 'Test Command', 'Test Command')            
            cmdDef.commandCreated.add(onCommandCreated)  
            cmdDefs.append(cmdDef)         

        # Create special command for brep entities
        cmdDefBRepSpecial = ui.commandDefinitions.itemById('BrepCommand')
        if not cmdDefBRepSpecial:
            cmdDefBRepSpecial = ui.commandDefinitions.addButtonDefinition('BrepCommand', 'Brep Command', 'This is a command for brep entities.')            
            cmdDefBRepSpecial.commandCreated.add(onCommandCreated)  
            cmdDefs.append(cmdDefBRepSpecial)
            
        # Create special command for sketch entities
        cmdDefSketchSpecial = ui.commandDefinitions.itemById('SketchCommand')
        if not cmdDefSketchSpecial:
            cmdDefSketchSpecial = ui.commandDefinitions.addButtonDefinition('SketchCommand', 'Sketch Command', 'This is a command for sketch entities.')            
            cmdDefSketchSpecial.commandCreated.add(onCommandCreated)  
            cmdDefs.append(cmdDefSketchSpecial)
            
        
        ui.messageBox('Right click to see the customized marking menu.')        
        
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))

def stop(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        for obj in cmdDefs:
            if obj.isValid:
                obj.deleteMe()
            else:
                ui.messageBox(str(obj) + ' is not a valid object')

        handlers.clear()
        
        ui.messageBox('Stop addin')

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))