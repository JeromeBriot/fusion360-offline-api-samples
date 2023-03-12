# Author-Autodesk Inc.
# Description-B-Rep (Boundary Representation) body related functions

import adsk.core, adsk.fusion, traceback


def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface

        # Create a document.
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)

        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)

        # Get the root component of the active design
        rootComp = design.rootComponent

        # Create a sketch
        sketches = rootComp.sketches
        sketch1 = sketches.add(rootComp.yZConstructionPlane)

        # Get sketch lines
        sketchLines = sketch1.sketchCurves.sketchLines

        # Create sketch rectangle
        startPoint = adsk.core.Point3D.create(0, 0, 0)
        endPoint = adsk.core.Point3D.create(5.0, 5.0, 0)
        sketchLines.addTwoPointRectangle(startPoint, endPoint)

        # Get the profile
        prof = sketch1.profiles.item(0)

        # Create an extrusion input
        extrudes = rootComp.features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        # Define that the extent is a distance extent of 5 cm
        distance = adsk.core.ValueInput.createByReal(5.0)
        # Set the distance extent
        extInput.setDistanceExtent(False, distance)
        # Set the extrude type to be solid
        extInput.isSolid = True

        # Create the extrusion
        ext = extrudes.add(extInput)

        # Get the body with the extrude
        brepBody = ext.bodies.item(0)
        
        # Get the original revision id of the BRep Body
        print(brepBody.revisionId)
        
        # Set the light bulb besides the body node in the browser to off
        brepBody.isLightBulbOn = False
        
        # Get the revision id of the BRep Body after having the body's light bulb off
        print(brepBody.revisionId)
        
        # Verify if the light bulb is on or off
        print(brepBody.isLightBulbOn)
        
        # Verify if the body is visible or not
        print(brepBody.isVisible)

    except:
        if ui:

            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))