# Author-Autodesk Inc.
# Description-Demonstrates creating construction point by different ways

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

        # Get the root component of the active design
        rootComp = design.rootComponent

        # Create sketch
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        
        # Get sketch lines
        sketchLines = sketch.sketchCurves.sketchLines
        
        # Create sketch rectangle
        startPoint = adsk.core.Point3D.create(0, 0, 0)
        endPoint = adsk.core.Point3D.create(5.0, 5.0, 0)
        sketchLines.addTwoPointRectangle(startPoint, endPoint)
        
        # Get two sketch lines
        sketchLineOne = sketchLines.item(0)
        sketchLineTwo = sketchLines.item(1)
        
        # Get the profile
        prof = sketch.profiles.item(0)
        
        # Create an extrusion input
        extrudes = rootComp.features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Define that the extent is a distance extent of 5 cm
        distance = adsk.core.ValueInput.createByReal(5.0)
        # Set the distance extent
        extInput.setDistanceExtent(False, distance)
        # Set the extrude type to be solid
        extInput.isSolid = True
        
        # Create the extrusion
        ext = extrudes.add(extInput)
        
        # Create a sketch circle
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(8, 8, 0)
        sketchCircle = sketchCircles.addByCenterRadius(centerPoint, 3.0)
        
        # Get the body with the extrude
        body = ext.bodies.item(0)
        
        # Get a vertex of the body
        vertex = body.vertices.item(0)
        
        # Get three intersect faces
        faceOne = vertex.faces.item(0)
        faceTwo = vertex.faces.item(1)
        faceThree = vertex.faces.item(2)
        
        # Create perpendicular construction axis
        axes = rootComp.constructionAxes
        axisInput = axes.createInput()
        axisInput.setByPerpendicularAtPoint(faceOne, vertex)
        axis = axes.add(axisInput)
        
        # Get construction points
        constructionPoints = rootComp.constructionPoints
        
        # Create construction point input
        pointInput = constructionPoints.createInput()
        
        # Create construction point by two points
        pointInput.setByTwoEdges(sketchLineOne, sketchLineTwo)
        constructionPoints.add(pointInput)

        # Create construction point by three planes
        pointInput.setByThreePlanes(faceOne, faceTwo, faceThree)
        constructionPoints.add(pointInput)
        
        # Create construction point by edge and plane
        pointInput.setByEdgePlane(axis, faceOne)
        constructionPoints.add(pointInput)
        
        # Create construction point by center
        pointInput.setByCenter(sketchCircle)
        constructionPoints.add(pointInput)
        
        # Create construction point by point
        pointInput.setByPoint(vertex)
        point = constructionPoints.add(pointInput)
        
        # Get the health state of the construction point
        health = point.healthState
        if health == adsk.fusion.FeatureHealthStates.ErrorFeatureHealthState or health == adsk.fusion.FeatureHealthStates.WarningFeatureHealthState:
            message = point.errorOrWarningMessage

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))