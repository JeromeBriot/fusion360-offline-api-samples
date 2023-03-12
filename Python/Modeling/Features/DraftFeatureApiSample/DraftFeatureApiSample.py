# Author-Autodesk Inc.
# Description-Demonstrates creating a new draft feature

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

        # Get the first face of box
        face1 = rootComp.bRepBodies.item(0).faces.item(0)
        
        # Get all faces which connect to the first face
        connectedFaces = [];
        for edge in face1.edges:
            for face in edge.faces:
                if face1 != face:
                    connectedFaces.append(face)
        
        # Create draft feature
        drafts = rootComp.features.draftFeatures
        draftFeatInput = drafts.createInput(connectedFaces, face1, True)
        draftFeatInput.isDirectionFlipped = False
        angle = adsk.core.ValueInput.createByString("10 deg")
        draftFeatInput.setSingleAngle(True, angle)
        drafts.add(draftFeatInput)       
        
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))