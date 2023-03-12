# Author-Autodesk Inc.
# Description-Demonstrates creating a new unstitch feature

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface

        # Create a document.
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
 
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)

        # Get the root component of the active design.
        rootComp = design.rootComponent
        
        # Create sketch
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchLines = sketch.sketchCurves.sketchLines
        startPoint = adsk.core.Point3D.create(0, 0, 0)
        endPoint = adsk.core.Point3D.create(5, 5, 0)
        sketchLines.addTwoPointRectangle(startPoint, endPoint)
        
        # Get the profile defined by the rectangle.
        prof = sketch.profiles.item(0)

        # Create an extrusion input.
        features = rootComp.features
        extrudes = features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Define that the extent is a distance extent of 5 cm.
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(False, distance)

        # Create the extrusion.
        ext = extrudes.add(extInput)
        
        # Get the body created by extrusion.
        body = ext.bodies.item(0)
        
        # Create unstitch feature.
        unstitchFeatures = features.unstitchFeatures
        entities = adsk.core.ObjectCollection.create()
        entities.add(body)
        unstitch = unstitchFeatures.add(entities)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))