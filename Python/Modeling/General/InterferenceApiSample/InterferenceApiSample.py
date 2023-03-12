# Author-Autodesk Inc.
# Description-Demonstrates Interference APIs

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
        design.designType = adsk.fusion.DesignTypes.DirectDesignType

        # Get the root component of the active design.
        rootComp = design.rootComponent
        features = rootComp.features

        # Create sketch circle on the xz plane.
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCircles.addByCenterRadius(centerPoint, 10)

        # Create a collection of entities for extrude
        entities0 = adsk.core.ObjectCollection.create()
        entities0.add(sketch.profiles.item(0))

        # Create a cylinder with ExtrudeFeature using the profile above.
        extrudeFeats = features.extrudeFeatures
        extrudeFeatureInput = extrudeFeats.createInput(entities0, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extrudeFeatureInput.isSolid = True
        extrudeFeatureInput.setDistanceExtent(False, adsk.core.ValueInput.createByReal(2.5))
        extrudeFeature = extrudeFeats.add(extrudeFeatureInput)
        
        # Create second body to do interference
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(5, 0, 0)
        sketchCircles.addByCenterRadius(centerPoint, 10)

        entities0.clear()
        entities0.add(sketch.profiles.item(0))

        # Create a cylinder with ExtrudeFeature using the profile above.
        extrudeFeatureInput = extrudeFeats.createInput(entities0, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extrudeFeatureInput.isSolid = True
        extrudeFeatureInput.setDistanceExtent(False, adsk.core.ValueInput.createByReal(2.5))
        extrudeFeature = extrudeFeats.add(extrudeFeatureInput)

        # Create a collection of bodies
        bodies = adsk.core.ObjectCollection.create()
        for body in rootComp.bRepBodies:
            bodies.add(body)

        # Create InterferenceInput
        input = design.createInterferenceInput(bodies)

        # Analyze interference
        results = design.analyzeInterference(input)

        # Create bodies
        results.createBodies(True)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))