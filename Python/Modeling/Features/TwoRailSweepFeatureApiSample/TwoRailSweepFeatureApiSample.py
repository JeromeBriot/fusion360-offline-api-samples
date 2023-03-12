# Author-Autodesk Inc.
# Description-Demonstrates creating new two rail sweep feature

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
        sketch = sketches.add(rootComp.xYConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        circle = sketchCircles.addByCenterRadius(centerPoint, 3.0)
        
        # Get the profile defined by the circle.
        prof = sketch.profiles.item(0)
        
        # Create a vertical sketch and add two lines on it
        sketchVertical = sketches.add(rootComp.xZConstructionPlane)
        sketchLines = sketchVertical.sketchCurves.sketchLines
        startPtOne = adsk.core.Point3D.create(0, 0, 0)
        endPtOne = adsk.core.Point3D.create(0, 10, 0)
        startPtTwo = adsk.core.Point3D.create(3, 0, 0)
        endPtTwo = adsk.core.Point3D.create(6, 10, 0)
        line1 = sketchLines.addByTwoPoints(startPtOne, endPtOne)
        line2 = sketchLines.addByTwoPoints(startPtTwo, endPtTwo)
        
        # Create a path
        path = rootComp.features.createPath(line1)
        
        # Create guid rail path
        guidRailPath = rootComp.features.createPath(line2)
        
        # Create a sweep input
        sweeps = rootComp.features.sweepFeatures
        sweepInput = sweeps.createInput(prof, path, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        sweepInput.guideRail = guidRailPath
        sweepInput.isDirectionFlipped = False
        sweepInput.profileScaling = adsk.fusion.SweepProfileScalingOptions.SweepProfileScaleOption

        # Create the sweep.
        sweeps.add(sweepInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))