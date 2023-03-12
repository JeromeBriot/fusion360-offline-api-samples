# Author-Autodesk Inc.
# Description-Demonstrates creating a new move feature

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
        features = rootComp.features

        # Create sketch circle on the xz plane.
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCircles.addByCenterRadius(centerPoint, 10)
        centerPoint = adsk.core.Point3D.create(15, 5, 0)
        sketchCircles.addByCenterRadius(centerPoint, 4)

        # Create a collection of entities for extrude
        profiles = adsk.core.ObjectCollection.create()
        profiles.add(sketch.profiles.item(0))
        profiles.add(sketch.profiles.item(1))

        # Create a cylinder with ExtrudeFeature using the profile above.
        extrudeFeats = features.extrudeFeatures
        extrudeFeature = extrudeFeats.addSimple(profiles, 
                                                adsk.core.ValueInput.createByReal(2.0),
                                                adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        # Create a collection of entities for move
        bodies = adsk.core.ObjectCollection.create()
        bodies.add(extrudeFeature.bodies.item(0))

        # Create a transform to do move
        vector = adsk.core.Vector3D.create(0.0, 10.0, 0.0)
        transform = adsk.core.Matrix3D.create()
        transform.translation = vector

        # Create a move feature
        moveFeats = features.moveFeatures
        moveFeatureInput = moveFeats.createInput(bodies, transform)
        moveFeats.add(moveFeatureInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))