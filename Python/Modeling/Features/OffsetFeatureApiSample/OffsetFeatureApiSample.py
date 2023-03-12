# Author-Autodesk Inc.
# Description-Demonstrates creating a new offset feature

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
        
        # Create input entities for offset feature
        inputEntities = adsk.core.ObjectCollection.create()
        inputEntities.add(body)
        
        # Distance for offset feature
        distance = adsk.core.ValueInput.createByString('1 cm')
        
        # Create an input for offset feature
        offsetFeatures = features.offsetFeatures
        offsetInput = offsetFeatures.createInput(inputEntities, distance, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Create the offset feature
        offsetFeatures.add(offsetInput);
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))