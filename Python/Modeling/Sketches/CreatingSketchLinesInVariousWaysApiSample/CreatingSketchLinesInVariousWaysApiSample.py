# Author-Autodesk Inc.
# Description-Demonstrates several ways to create sketch lines, including as the result of creating a rectangle

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try: 
        app = adsk.core.Application.get()
        ui = app.userInterface

        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        design = app.activeProduct

        # Get the root component of the active design.
        rootComp = design.rootComponent

        # Create a new sketch on the xy plane.
        sketches = rootComp.sketches;
        xyPlane = rootComp.xYConstructionPlane
        sketch = sketches.add(xyPlane)

        # Draw two connected lines.
        lines = sketch.sketchCurves.sketchLines;
        line1 = lines.addByTwoPoints(adsk.core.Point3D.create(0, 0, 0), adsk.core.Point3D.create(3, 1, 0))
        line2 = lines.addByTwoPoints(line1.endSketchPoint, adsk.core.Point3D.create(1, 4, 0))

        # Draw a rectangle by two points.
        recLines = lines.addTwoPointRectangle(adsk.core.Point3D.create(4, 0, 0), adsk.core.Point3D.create(7, 2, 0))

        # Use the returned lines to add some constraints.
        sketch.geometricConstraints.addHorizontal(recLines.item(0))
        sketch.geometricConstraints.addHorizontal(recLines.item(2))
        sketch.geometricConstraints.addVertical(recLines.item(1))
        sketch.geometricConstraints.addVertical(recLines.item(3))
        sketch.sketchDimensions.addDistanceDimension(recLines.item(0).startSketchPoint, recLines.item(0).endSketchPoint,
                                                     adsk.fusion.DimensionOrientations.HorizontalDimensionOrientation,
                                                     adsk.core.Point3D.create(5.5, -1, 0));

        # Draw a rectangle by three points.
        recLines = lines.addThreePointRectangle(adsk.core.Point3D.create(8, 0, 0), adsk.core.Point3D.create(11, 1, 0), adsk.core.Point3D.create(9, 3, 0))

        # Draw a rectangle by a center point.
        recLines = lines.addCenterPointRectangle(adsk.core.Point3D.create(14, 3, 0), adsk.core.Point3D.create(16, 4, 0))
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))