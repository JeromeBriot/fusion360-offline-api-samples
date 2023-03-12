# Author-Autodesk Inc.
# Description-Demonstrates creating a new Sweep feature that uses a guide rail along with a profile

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
        
        # Create sketch for the profile to sweep
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        circle = sketchCircles.addByCenterRadius(centerPoint, 1.0)        
        
        # Get the profile defined by the circle.
        prof = sketch.profiles.item(0)        
        
        # Create a vertical sketch and add a spline (for the sweep path) and a line (for the sweep guide rail)
        sketchVertical = sketches.add(rootComp.yZConstructionPlane)
        sketchSplines = sketchVertical.sketchCurves.sketchFittedSplines
        sketchLines = sketchVertical.sketchCurves.sketchLines
        
        # Create points for the spline definition       
        splineStartPt = adsk.core.Point3D.create(0, 0, 0)
        splineMidPt = adsk.core.Point3D.create(0, 5, 0)
        splineEndPt = adsk.core.Point3D.create(3, 10, 0)
        
        # Create a collection of the points for the input needed to create the spline
        fitPoints = adsk.core.ObjectCollection.create()
        fitPoints.add(splineStartPt)
        fitPoints.add(splineMidPt)
        fitPoints.add(splineEndPt)
        
        # Create the spline
        spline = sketchSplines.add(fitPoints)
        
        # Create points for the line definition       
        lineStartPt = adsk.core.Point3D.create(-2, 0, 0)
        lineEndPt = adsk.core.Point3D.create(-2, 10, 0)
        
        # Create the line
        line = sketchLines.addByTwoPoints(lineStartPt, lineEndPt)
        
        # Create a path for the sweep path and guide rail
        path = rootComp.features.createPath(spline)
        guide = rootComp.features.createPath(line)
        
        # Create a sweep input
        sweeps = rootComp.features.sweepFeatures
        sweepInput = sweeps.createInput(prof, path, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        sweepInput.guideRail = guide
        sweepInput.profileScaling = adsk.fusion.SweepProfileScalingOptions.SweepProfileScaleOption

        # Create the sweep.
        sweep = sweeps.add(sweepInput)
        
        sketchVertical.isVisible = True

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))