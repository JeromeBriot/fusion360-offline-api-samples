# Author-Autodesk Inc.
# Description-Demonstrates creating a new boundary fill feature

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface


        # Get active design        
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        
        # Get root component in this design
        rootComp = design.rootComponent   
        
        # Prepare tools to create boundary fill
        # Two bodies and one workplane are needed
        tools = adsk.core.ObjectCollection.create()
        tools.add(rootComp.bRepBodies.item(0))
        tools.add(rootComp.bRepBodies.item(1))
        tools.add(rootComp.constructionPlanes.item(0))
        
        # Create input
        boundaryFills = rootComp.features.boundaryFillFeatures
        boundaryFillInput = boundaryFills.createInput(tools, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Boundary fill will be created in sub component
        boundaryFillInput.creationOccurrence = rootComp.occurrences.item(0)
        
        # Specify which cell is kept
        cell = boundaryFillInput.bRepCells.item(0)
        cell.isSelected = True
        
        # Create the boundary fill
        boundaryFills.add(boundaryFillInput)      
        

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))