# Author-Autodesk Inc.
# Description-Demonstrates creating a new hole feature

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

        # Create an extrusion input
        extrudes = rootComp.features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Define that the extent is a distance extent of 5 cm.
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(False, distance)

        # Create the extrusion.
        ext = extrudes.add(extInput)
        
        # Get the end face of the extrusion
        endFaces = ext.endFaces
        endFace = endFaces.item(0)
        
        # Create a construction plane by offsetting the end face
        planes = rootComp.constructionPlanes
        planeInput = planes.createInput()
        offsetVal = adsk.core.ValueInput.createByString('2 cm')
        planeInput.setByOffset(endFace, offsetVal)
        offsetPlane = planes.add(planeInput)
        
        # Create a sketch on the new construction plane and add four sketch points on it
        offsetSketch = sketches.add(offsetPlane)
        offsetSketchPoints = offsetSketch.sketchPoints
        sPt0 = offsetSketchPoints.add(adsk.core.Point3D.create(1, 0, 0))
        sPt1 = offsetSketchPoints.add(adsk.core.Point3D.create(0, 1, 0))
        sPt2 = offsetSketchPoints.add(adsk.core.Point3D.create(-1, 0, 0))
        sPt3 = offsetSketchPoints.add(adsk.core.Point3D.create(0, -1, 0))
        
        # Add the four sketch points into a collection
        ptColl = adsk.core.ObjectCollection.create()
        ptColl.add(sPt0)
        ptColl.add(sPt1)
        ptColl.add(sPt2)
        ptColl.add(sPt3)
        
        # Create a hole input
        holes = rootComp.features.holeFeatures
        holeInput = holes.createSimpleInput(adsk.core.ValueInput.createByString('2 mm'))
        holeInput.setPositionBySketchPoints(ptColl)
        holeInput.setDistanceExtent(distance)
        
        hole = holes.add(holeInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))