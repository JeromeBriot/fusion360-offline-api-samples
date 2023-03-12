# Author-Autodesk Inc.
# Description-Demonstrates creating fillets using the various types of fillets

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
        
        # Get fillet features
        fillets = rootComp.features.filletFeatures
        
        # Create constant-radius fillet using the edges of a face of the body.
        edges1 = rootComp.bRepBodies.item(0).faces.item(0).edges
        edgeCollection1 = adsk.core.ObjectCollection.create()
        for edge in edges1:
            edgeCollection1.add(edge);  
        radius1 = adsk.core.ValueInput.createByReal(0.3)
        
        input1 = fillets.createInput()
        input1.isRollingBallCorner = True

        constRadiusInput = input1.edgeSetInputs.addConstantRadiusEdgeSet(edgeCollection1, radius1, True)
        constRadiusInput.continuity = adsk.fusion.SurfaceContinuityTypes.TangentSurfaceContinuityType
        
        fillet1 = fillets.add(input1)
        fillet1.deleteMe()
        
        # Create variable-radius fillet.
        edgeCollection2 = adsk.core.ObjectCollection.create()
        edgeCollection2.add(rootComp.bRepBodies.item(0).faces.item(0).edges.item(0))
        
        input2 = fillets.createInput()
        input2.isRollingBallCorner = False

        startRadius = adsk.core.ValueInput.createByString("1.0 mm")
        endRadius = adsk.core.ValueInput.createByString("5.0 mm")
        varRadiusEdgeSet = input2.edgeSetInputs.addVariableRadiusEdgeSet(edgeCollection2, startRadius, endRadius, True)
        varRadiusEdgeSet.continuity = adsk.fusion.SurfaceContinuityTypes.TangentSurfaceContinuityType       

        positions = [];
        positions.append(adsk.core.ValueInput.createByReal(0.3))
        positions.append(adsk.core.ValueInput.createByReal(0.6))
        radii = [];
        radii.append(adsk.core.ValueInput.createByString("2.0 mm"))
        radii.append(adsk.core.ValueInput.createByString("3.0 mm"))

        varRadiusEdgeSet.setMidRadii(radii, positions)

        fillet2 = fillets.add(input2)   
        fillet2.deleteMe()
        
        # Create chord-length fillet on the edges of a face.
        edges3 = rootComp.bRepBodies.item(0).faces.item(0).edges
        edgeCollection3 = adsk.core.ObjectCollection.create()
        for edge in edges3:
            edgeCollection3.add(edge);  
        chordLength = adsk.core.ValueInput.createByReal(1.0)
        
        input3 = fillets.createInput()
        input3.isRollingBallCorner = True

        chordLengthEdgeSet = input3.edgeSetInputs.addChordLengthEdgeSet(edgeCollection3, chordLength, True)
        chordLengthEdgeSet.continuity = adsk.fusion.SurfaceContinuityTypes.TangentSurfaceContinuityType       
        fillets.add(input3)        
        

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))