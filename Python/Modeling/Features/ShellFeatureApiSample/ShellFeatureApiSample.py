# Author-Autodesk Inc.
# Description-Demonstrates creating a new shell feature

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

        # Get the profile from the sketch.
        profile = sketch.profiles.item(0)

        # Create a cylinder with ExtrudeFeature using the profile above.
        extrudeFeats = features.extrudeFeatures
        distance = adsk.core.ValueInput.createByReal(2.5)
        extrudeFeature = extrudeFeats.addSimple(profile, distance, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        # Create a collection of entities for shell
        entities1 = adsk.core.ObjectCollection.create()
        entities1.add(extrudeFeature.endFaces.item(0))

        # Create a shell feature
        shellFeats = features.shellFeatures
        isTangentChain = False
        shellFeatureInput = shellFeats.createInput(entities1, isTangentChain)
        thickness = adsk.core.ValueInput.createByReal(0.5)
        shellFeatureInput.insideThickness = thickness
        shellFeats.add(shellFeatureInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))