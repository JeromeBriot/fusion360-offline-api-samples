# Author-Autodesk Inc.
# Description-Demonstrates creating a new path pattern feature

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
        sketchCircles.addByCenterRadius(centerPoint, 3.0)
        sketchLines = sketch.sketchCurves.sketchLines
        startPoint = adsk.core.Point3D.create(5, 0, 0)
        endPoint = adsk.core.Point3D.create(5, 30, 0)
        sketchLine = sketchLines.addByTwoPoints(startPoint, endPoint)
        
        # Get the profile defined by the circle.
        prof = sketch.profiles.item(0)

        # Create an extrusion input
        features = rootComp.features
        extrudes = features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Define that the extent is a distance extent of 5 cm.
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(False, distance)

        # Create the extrusion.
        ext = extrudes.add(extInput)
        
        # Get the body created by extrusion
        body = ext.bodies.item(0)
        
        # Create input entities for rectangular pattern
        inputEntites = adsk.core.ObjectCollection.create()
        inputEntites.add(body)
        
        # Create path for path pattern
        path = features.createPath(sketchLine)
        
        # Quantity and distance
        quantity = adsk.core.ValueInput.createByString('3')
        patternDistance = adsk.core.ValueInput.createByString('8 cm')
        
        # Create the input for path pattern
        pathPatterns = features.pathPatternFeatures
        pathPatternInput = pathPatterns.createInput(inputEntites, path, quantity, patternDistance, adsk.fusion.PatternDistanceType.SpacingPatternDistanceType)
        
        # Create the path pattern
        pathFeature = pathPatterns.add(pathPatternInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))