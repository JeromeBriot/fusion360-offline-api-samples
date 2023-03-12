# Author-Autodesk Inc.
# Description-Creates an equal distance chamfer on the selected edge

import adsk.core, adsk.fusion, traceback

def run(context):
    try:
        # Get various top-level objects.
        app = adsk.core.Application.get()
        ui = app.userInterface
        design: adsk.fusion.Design = app.activeProduct
        root = design.rootComponent
        
        # Have the edge selected and add it to an ObjectCollection.        
        selection = ui.selectEntity('Select edge to chamfer', 'Edges')
        edge: adsk.fusion.BRepEdge = selection.entity
        edgeCollection = adsk.core.ObjectCollection.create()
        edgeCollection.add(edge)
        
        # Create the ChamferInput object.
        chamfers = root.features.chamferFeatures
        input = chamfers.createInput2() 
        offset = adsk.core.ValueInput.createByReal(1)   
        input.chamferEdgeSets.addEqualDistanceChamferEdgeSet(edgeCollection, offset, True)
        
        # Create the chamfer.
        chamfer = chamfers.add(input) 
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))