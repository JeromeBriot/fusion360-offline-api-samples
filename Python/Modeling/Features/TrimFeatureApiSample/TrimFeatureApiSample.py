# Author-Autodesk Inc.
# Description-Demonstrates creating a new trim feature

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

        # Get the root component of the active design.
        rootComp = design.rootComponent
        
        # Create sketch
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCircle = sketchCircles.addByCenterRadius(centerPoint, 3.0)
        
        # Create a open profile for extrusion.
        openProfile = rootComp.createOpenProfile(sketchCircle)
        
        # Create an extrusion input.
        features = rootComp.features
        extrudes = features.extrudeFeatures
        extrudeInput = extrudes.createInput(openProfile, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extrudeInput.isSolid = False
        
        # Define the extent with a distance extent of 3 cm.
        distance = adsk.core.ValueInput.createByReal(3.0)
        extrudeInput.setDistanceExtent(False, distance)
        
        # Create the extrusion.
        extrude = extrudes.add(extrudeInput)
        
        # Get the body created by extrusion
        body = extrude.bodies[0]
        
        # Create sketch 2.
        sketch2 = sketches.add(rootComp.xYConstructionPlane);
        sketchLines = sketch2.sketchCurves.sketchLines;
        startPoint = adsk.core.Point3D.create(-5, 0, 0);
        endPoint = adsk.core.Point3D.create(5, 0, 0);
        sketchLine = sketchLines.addByTwoPoints(startPoint, endPoint);
        openProfile2 = rootComp.createOpenProfile(sketchLine);
        
        # Create a open profile for extrusion.
        extrudeInput2 = extrudes.createInput(openProfile2, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extrudeInput2.isSolid = False
        
        # Define the extent
        distance2 = adsk.core.ValueInput.createByReal(5.0)
        extrudeInput2.setDistanceExtent(False, distance2)
        
        # Create the extrusion.
        extrudes.add(extrudeInput2)
        
        # Create trim feature
        trims = features.trimFeatures
        trimInput = trims.createInput(body)
        cells = trimInput.bRepCells
        cells[0].isSelected = True
        trims.add(trimInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))