# Author-Autodesk Inc.
# Description-Creates a constant radius fillet on the selected edge

import adsk.core, adsk.fusion, traceback


def createFillet():
    ui = None
    try:
        # Get the Application object.
        app = adsk.core.Application.get()

        # Get various top-level Fusion objects.
        ui  = app.userInterface
        design = app.activeProduct
        root = design.rootComponent

        # Have the edge selected and add it to an ObjectCollection.        
        selection = ui.selectEntity("Select edge to fillet", "Edges")
        edge = selection.entity
        edgeCollection = adsk.core.ObjectCollection.create()
        edgeCollection.add(edge)
        
        # Create the FilletInput object.
        fillets = root.features.filletFeatures
        filletInput = fillets.createInput()      
        filletInput.addConstantRadiusEdgeSet(edgeCollection, adsk.core.ValueInput.createByString('.25 in'), True)

        # Create the fillet.        
        fillet = fillets.add(filletInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
    

def main():
    createFillet()


main()