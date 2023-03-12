# Author-Autodesk Inc.
# Description-Demonstrates creating a new silhouette split feature

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
        extInput.setDistanceExtent(True, distance)

        # Create the extrusion
        ext = extrudes.add(extInput)

        # Get the body created by the extrusion
        body = ext.bodies.item(0)
        
        # Create SilhouetteSplitFeatureInput
        silhouetteSplitFeats = rootComp.features.silhouetteSplitFeatures
        silhouetteSplitInput = silhouetteSplitFeats.createInput(rootComp.xZConstructionPlane, body, adsk.fusion.SilhouetteSplitOperations.SilhouetteSplitFacesOnlyOperation)
        
        # Create silhouette split feature
        silhouetteSplitFeats.add(silhouetteSplitInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))