# Author-Autodesk Inc.
# Description-Demonstrates creating a new sweep feature

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
        circle = sketchCircles.addByCenterRadius(centerPoint, 3.0)
        
        # Get the profile defined by the circle.
        prof = sketch.profiles.item(0)
        
        # Create a vertical sketch and add two lines on it
        sketchVertical = sketches.add(rootComp.yZConstructionPlane)
        sketchLines = sketchVertical.sketchCurves.sketchLines
        startPt = adsk.core.Point3D.create(0, 0, 0)
        midPt = adsk.core.Point3D.create(0, 3, 0)
        endPt = adsk.core.Point3D.create(2, 6, 0)
        line1 = sketchLines.addByTwoPoints(startPt, midPt)
        line2 = sketchLines.addByTwoPoints(midPt, endPt)
        
        # Merge the two lines
        line1.endSketchPoint.merge(line2.startSketchPoint)
        
        # Create a path and let it find connected curves automatically
        path = rootComp.features.createPath(line1)

        # Create a sweep input
        sweeps = rootComp.features.sweepFeatures
        sweepInput = sweeps.createInput(prof, path, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        sweepInput.taperAngle = adsk.core.ValueInput.createByString('5 deg')
        sweepInput.twistAngle = adsk.core.ValueInput.createByString('10 deg')

        # Create the sweep.
        sweep = sweeps.add(sweepInput)
        
        # Get taperAngel and twistAngle from sweep feature
        taperAngle = sweep.taperAngle
        twistAngle = sweep.twistAngle
        print('taper angle: {}'.format(taperAngle.expression))
        print('twist angle: {}'.format(twistAngle.expression))
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))