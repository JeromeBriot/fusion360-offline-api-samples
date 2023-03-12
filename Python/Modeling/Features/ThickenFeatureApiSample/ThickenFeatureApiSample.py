# Author-Autodesk Inc.
# Description-Demonstrates creating a new thiken feature

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface

        # Create a document
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        
        design = app.activeProduct

        # Get the root component of the active design.
        rootComp = design.rootComponent
        
        # Create sketch
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCircle = sketchCircles.addByCenterRadius(centerPoint, 3.0)
        
        # Create surface
        openProfile = rootComp.createOpenProfile(sketchCircle)
        features = rootComp.features
        extrudes = features.extrudeFeatures
        extrudeInput = extrudes.createInput(openProfile, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extrudeInput.isSolid = False
        distance = adsk.core.ValueInput.createByReal(3.0)
        extrudeInput.setDistanceExtent(False, distance)
        extrude = extrudes.add(extrudeInput)
        
        # Create thiken feature
        thickenFeatures = features.thickenFeatures
        inputSurfaces = adsk.core.ObjectCollection.create()
        bodies = extrude.bodies
        for body in bodies :
            inputSurfaces.add(body)
        thickness = adsk.core.ValueInput.createByReal(1.0)
        thickenInput = thickenFeatures.createInput(inputSurfaces, thickness, False,  adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        thickenFeatures.add(thickenInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))