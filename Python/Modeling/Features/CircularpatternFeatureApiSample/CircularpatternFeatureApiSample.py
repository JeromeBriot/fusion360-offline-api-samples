# Author-Autodesk Inc.
# Description-Demonstrates creating a new circular pattern feature

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
        centerPoint = adsk.core.Point3D.create(10, 0, 0)
        circle = sketchCircles.addByCenterRadius(centerPoint, 3.0)
        
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
        body = rootComp.bRepBodies.item(0)
        
        # Create input entities for circular pattern
        inputEntites = adsk.core.ObjectCollection.create()
        inputEntites.add(body)
        
        # Get Y axis for circular pattern
        yAxis = rootComp.yConstructionAxis
        
        # Create the input for circular pattern
        circularFeats = rootComp.features.circularPatternFeatures
        circularFeatInput = circularFeats.createInput(inputEntites, yAxis)
        
        # Set the quantity of the elements
        circularFeatInput.quantity = adsk.core.ValueInput.createByReal(5)
        
        # Set the angle of the circular pattern
        circularFeatInput.totalAngle = adsk.core.ValueInput.createByString('180 deg')
        
        # Set symmetry of the circular pattern
        circularFeatInput.isSymmetric = False
        
        # Create the circular pattern
        circularFeat = circularFeats.add(circularFeatInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))