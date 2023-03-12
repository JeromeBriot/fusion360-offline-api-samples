# Author-Autodesk Inc.
# Description-Demonstrates creating a new rectangular pattern feature

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
        
        # Get the profile defined by the circle.
        prof = sketch.profiles.item(0)

        # Create an extrusion input
        extrudes = rootComp.features.extrudeFeatures
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
        
        # Get x and y axes for rectangular pattern
        xAxis = rootComp.xConstructionAxis
        yAxis = rootComp.yConstructionAxis
        
        # Quantity and distance
        quantityOne = adsk.core.ValueInput.createByString('3')
        distanceOne = adsk.core.ValueInput.createByString('8 cm')
        quantityTwo = adsk.core.ValueInput.createByString('3')
        distanceTwo = adsk.core.ValueInput.createByString('8 cm')
        
        # Create the input for rectangular pattern
        rectangularPatterns = rootComp.features.rectangularPatternFeatures
        rectangularPatternInput = rectangularPatterns.createInput(inputEntites, xAxis, quantityOne, distanceOne, adsk.fusion.PatternDistanceType.SpacingPatternDistanceType)
        
        # Set the data for second direction
        rectangularPatternInput.setDirectionTwo(yAxis, quantityTwo, distanceTwo)
        
        # Create the rectangular pattern
        rectangularFeature = rectangularPatterns.add(rectangularPatternInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))