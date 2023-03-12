#Author-Autodesk Inc.
#Description-Demonstrates creating a new remove feature

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
        
        # Get remove features
        features = rootComp.features
        removeFeatures = features.removeFeatures

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
        
        # Get created body
        body = extrudeFeature.bodies[0]
        
        # Create remove feature
        removeFeat = removeFeatures.add(body)
        
        # Create an occurrence
        occs = rootComp.occurrences
        occ = occs.addNewComponent(adsk.core.Matrix3D.create())
        
        # Create remove feature
        removeFeat2 = removeFeatures.add(occ)
        
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))