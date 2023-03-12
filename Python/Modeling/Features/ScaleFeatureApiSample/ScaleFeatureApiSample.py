# Author-Autodesk Inc.
# Description-Demonstrates creating a new scale feature

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

        # Get the root component of the active design
        rootComp = design.rootComponent
        
        # Create sketch
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        circle = sketchCircles.addByCenterRadius(centerPoint, 5.0)
        
        # Get the profile defined by the circle
        prof = sketch.profiles.item(0)

        # Create an extrusion input
        extrudes = rootComp.features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Define that the extent is a distance extent of 5 cm
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(False, distance)

        # Create the extrusion
        ext = extrudes.add(extInput)
        
        # Get the body created by the extrusion
        body = ext.bodies.item(0)
        
        # Create a scale input
        inputColl = adsk.core.ObjectCollection.create()
        inputColl.add(body)
        
        basePt = sketch.sketchPoints.item(0)
        scaleFactor = adsk.core.ValueInput.createByReal(2)
        
        scales = rootComp.features.scaleFeatures
        scaleInput = scales.createInput(inputColl, basePt, scaleFactor)
        
        # Set the scale to be non-uniform
        xScale = adsk.core.ValueInput.createByReal(1.5)
        yScale = adsk.core.ValueInput.createByReal(3)
        zScale = adsk.core.ValueInput.createByReal(2)
        scaleInput.setToNonUniform(xScale, yScale, zScale)
        
        scale = scales.add(scaleInput)
        
        # Create another sketch
        sketchVertical = sketches.add(rootComp.yZConstructionPlane)
        sketchCirclesVertical = sketchVertical.sketchCurves.sketchCircles
        centerPointVertical = adsk.core.Point3D.create(0, 10, 0)
        cicleVertical = sketchCirclesVertical.addByCenterRadius(centerPointVertical, 5)
        
        # Create an uniformed input for scale feature input
        inputUniformColl = adsk.core.ObjectCollection.create()
        inputUniformColl.add(sketchVertical)
        
        scaleUniformInput = scales.createInput(inputUniformColl, basePt, scaleFactor)
        
        scaleUniform = scales.add(scaleUniformInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))