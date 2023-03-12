# Author-Autodesk Inc.
# Description-Demonstrates how to use Copy Paste Bodies related API

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

        # Get the first sub component
        occs = rootComp.occurrences
        subComp1 = occs.item(0).component

        # Get the second sub component
        subComp2 = occs.item(1).component

        # Get the first body in sub component 1  
        bodyInSubComp1 = subComp1.bRepBodies.item(0)
        
        # Copy/paste body from sub component 1 to sub component 2
        copyPasteBody = subComp2.features.copyPasteBodies.add(bodyInSubComp1)
        
        # Copy/paste bodies from sub component to root component
        sourceBodies = adsk.core.ObjectCollection.create()
        for body in copyPasteBody.sourceBody:
            sourceBodies.add(body)
        for body in subComp2.bRepBodies:
            sourceBodies.add(body)
            
        rootComp.features.copyPasteBodies.add(sourceBodies)
        
        # Dump the information of Copy Paste Body in root component
        for copyPasteBody in rootComp.features.copyPasteBodies:
            copyPasteBodyInfo = 'CopyPasteBody: name - {}'.format(copyPasteBody.name)
            ui.messageBox(copyPasteBodyInfo)
        

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))