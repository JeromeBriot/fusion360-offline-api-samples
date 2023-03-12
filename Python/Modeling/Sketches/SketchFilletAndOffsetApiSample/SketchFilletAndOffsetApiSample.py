# Author-Autodesk Inc.
# Description-Demonstrates the creation of a fillet in a sketch and offset a set of curves

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        design = app.activeProduct
        
        # Get the root component of the active design.
        rootComp = design.rootComponent

        # Create a new sketch on the xy plane.
        sketches = rootComp.sketches
        xyPlane = rootComp.xYConstructionPlane
        sketch = sketches.add(xyPlane)

        # Draw two connected lines.
        lines = sketch.sketchCurves.sketchLines
        line1 = lines.addByTwoPoints(adsk.core.Point3D.create(0, 0, 0), adsk.core.Point3D.create(3, 1, 0))
        line2 = lines.addByTwoPoints(line1.endSketchPoint, adsk.core.Point3D.create(1, 4, 0))
        
        # Add a fillet.
        arc = sketch.sketchCurves.sketchArcs.addFillet(line1, line1.endSketchPoint.geometry, line2, line2.startSketchPoint.geometry, 1)

        # Add the geometry to a collection. This uses a utility function that
        # automatically finds the connected curves and returns a collection.
        curves = sketch.findConnectedCurves(line1)
               
        # Create the offset.
        dirPoint = adsk.core.Point3D.create(0, .5, 0)
        offsetCurves = sketch.offset(curves, dirPoint, 0.25)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))