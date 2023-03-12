# Author-Autodesk Inc.
# Description-Demonstrates how to manage participant During Cut operation

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

        # Create sketch for Cut
        sketchForCut = sketches.add(rootComp.xZConstructionPlane)
        sketchForCutCircles = sketchForCut.sketchCurves.sketchCircles
        sketchForCutCircles.addByCenterRadius(centerPoint, 1.5)

        # Get the profile defined by the circle.
        profForCut = sketchForCut.profiles.item(0)

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
        quantityOne = adsk.core.ValueInput.createByString('1')
        distanceOne = adsk.core.ValueInput.createByString('0 cm')
        quantityTwo = adsk.core.ValueInput.createByString('6')
        distanceTwo = adsk.core.ValueInput.createByString('15 cm')

        # Create the input for rectangular pattern
        rectangularPatterns = rootComp.features.rectangularPatternFeatures
        rectangularPatternInput = rectangularPatterns.createInput(inputEntites, xAxis, quantityOne, distanceOne, adsk.fusion.PatternDistanceType.SpacingPatternDistanceType)

        # Set the data for second direction
        rectangularPatternInput.setDirectionTwo(yAxis, quantityTwo, distanceTwo)

        # Create the rectangular pattern
        rectangularFeature = rectangularPatterns.add(rectangularPatternInput)

        patBodies = rectangularFeature.bodies

        body0 = patBodies.item(0)
        face0 = body0.faces.item(0)

        extCutInput = extrudes.createInput(profForCut, adsk.fusion.FeatureOperations.CutFeatureOperation)

         # Set the extrude input
        distanceForCut = adsk.core.ValueInput.createByString('90 cm')
        extCutInput.setDistanceExtent(False, distanceForCut)

        #set bodies to participate
        extCutInput.participantBodies = [patBodies.item(0), patBodies.item(2), patBodies.item(4)]

        extrudes.add(extCutInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))