# Author-Autodesk Inc.
# Description-Demonstrates how to use Cut Paste Bodies related API

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
        
        # Cut/paste body from sub component 1 to sub component 2
        cutPasteBody = subComp2.features.cutPasteBodies.add(bodyInSubComp1)
        
        # Cut/paste bodies from sub component2 to root component
        sourceBodies = adsk.core.ObjectCollection.create()
        for body in subComp2.bRepBodies:
            sourceBodies.add(body)
            
        rootComp.features.cutPasteBodies.add(sourceBodies)
        
        # Dump the information of Cut Paste Body in root component
        for cutPasteBody in rootComp.features.cutPasteBodies:
            cutPasteBodyInfo = 'CutPasteBody: name - {}'.format(cutPasteBody.name)
            ui.messageBox(cutPasteBodyInfo)
        

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))