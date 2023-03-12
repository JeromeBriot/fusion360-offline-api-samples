# Author-Autodesk Inc.
# Description-Demonstrates creating a new deleteFace feature

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

        # Get the root component of the active design
        rootComp = design.rootComponent

        # Create sketch
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCircles.addByCenterRadius(centerPoint, 5.0)

        # Get the profile defined by the circle
        prof = sketch.profiles.item(0)

        # Create an extrusion input
        extrudes = rootComp.features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        # Define that the extent is a distance extent of 5 cm
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(False, distance)

        # Create the extrusion
        extrudeFeature = extrudes.add(extInput)

        # Get end faces
        endFacesObj = extrudeFeature.endFaces
        endFace = endFacesObj.item(0)

        # Create SurfaceDeleteFaceFeature
        surfaceDeleteFaceFeas = rootComp.features.surfaceDeleteFaceFeatures
        surfaceDeleteFaceFeas.add(endFace)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))