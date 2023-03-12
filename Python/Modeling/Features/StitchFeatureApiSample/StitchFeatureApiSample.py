# Author-Autodesk Inc.
# Description-Demonstrates creating a new stitch feature

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
        
        # Create two sketch lines on the xz plane.
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchLines = sketch.sketchCurves.sketchLines
        startPoint = adsk.core.Point3D.create(0, 0, 0)
        endPoint = adsk.core.Point3D.create(1, 0, 0)
        sketchLine = sketchLines.addByTwoPoints(startPoint, endPoint)
        endPoint2 = adsk.core.Point3D.create(0, 1, 0)
        sketchLine2 = sketchLines.addByTwoPoints(startPoint, endPoint2)
        
        # Create surface one with ExtrudeFeature.
        features = rootComp.features
        extrudeFeatures = features.extrudeFeatures
        openProfile = rootComp.createOpenProfile(sketchLine)
        extrudeFeatureInput = extrudeFeatures.createInput(openProfile, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extrudeFeatureInput.isSolid = False
        extrudeFeatureInput.setDistanceExtent(False, adsk.core.ValueInput.createByReal(1.0))
        extrudeFeature = extrudeFeatures.add(extrudeFeatureInput)
        
        # Create surface two with ExtrudeFeature.
        openProfile2 = rootComp.createOpenProfile(sketchLine2)        
        extrudeFeatureInput2 = extrudeFeatures.createInput(openProfile2, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extrudeFeatureInput2.isSolid = False
        extrudeFeatureInput2.setDistanceExtent(False, adsk.core.ValueInput.createByReal(1.0))
        extrudeFeature2 = extrudeFeatures.add(extrudeFeatureInput2)
        
        # Get surface bodies and add them to object collection.
        surface = extrudeFeature.bodies.item(0)
        surface2 = extrudeFeature2.bodies.item(0)
        surfaces = adsk.core.ObjectCollection.create()
        surfaces.add(surface)
        surfaces.add(surface2)
        
        # Define tolerance with 1 cm.
        tolerance = adsk.core.ValueInput.createByReal(1.0)
        
        # Create a stitch input to be able to define the input needed for an stitch.
        stitches = features.stitchFeatures
        stitchInput = stitches.createInput(surfaces, tolerance, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Create a stitch feature.
        stitch = stitches.add(stitchInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))