# Author-Autodesk Inc.
# Description-Demonstrates creating construction axis in various ways

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
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCircles.addByCenterRadius(centerPoint, 5.0)
        
        sketchLines = sketch.sketchCurves.sketchLines
        sketchLines.addTwoPointRectangle(adsk.core.Point3D.create(6, 6, 0), adsk.core.Point3D.create(-6, -6, 0))
        
        # Get the profile defined by the circle
        prof = sketch.profiles.item(1)

        # Create an extrusion input
        extrudes = rootComp.features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Define that the extent is a distance extent of 5 cm
        distance = adsk.core.ValueInput.createByReal(5)
        # Set the distance extent to be symmetric
        extInput.setDistanceExtent(True, distance)
        # Set the extrude to be a solid one
        extInput.isSolid = True

        # Create the extrusion
        ext = extrudes.add(extInput)
        
        # Get the body created by the extrusion
        body = ext.bodies.item(0)
        
        axes = rootComp.constructionAxes
        axisInput = axes.createInput()
        
        # Add by line
        if design.designType == adsk.fusion.DesignTypes.DirectDesignType:
            axisInput.setByLine(adsk.core.InfiniteLine3D.create(adsk.core.Point3D.create(0), adsk.core.Vector3D.create(1, 0, 0)))
            axes.add(axisInput)
        
        # Prepare reference data
        circularFace = None
        for face in body.faces:
            geom = face.geometry
            if geom.surfaceType == adsk.core.SurfaceTypes.CylinderSurfaceType:
                circularFace = face
                break
        
        linearEdge = None
        for edge in body.edges:
            edgeGeom = edge.geometry
            if edgeGeom.curveType == adsk.core.Curve3DTypes.Line3DCurveType:
                linearEdge = edge
                break
        
        faceOne = linearEdge.faces.item(0)
        faceTwo = linearEdge.faces.item(1)
        vertexOne = faceOne.vertices.item(0)
        vertexTwo = faceOne.vertices.item(1)
        
        # Add by circularFace
        axisInput.setByCircularFace(circularFace)
        axes.add(axisInput)
        
        # Add by perpendicular at point
        axisInput.setByPerpendicularAtPoint(faceOne, vertexOne)
        axes.add(axisInput)
        
        # Add by two planes
        axisInput.setByTwoPlanes(faceOne, faceTwo)
        axes.add(axisInput)
        
        # Add by two points
        axisInput.setByTwoPoints(vertexOne, vertexTwo)
        axes.add(axisInput)
        
        # Add by edge
        axisInput.setByEdge(linearEdge)
        axes.add(axisInput)
        
        # Add by normal to face at point
        axisInput.setByNormalToFaceAtPoint(faceTwo, vertexOne)
        axis = axes.add(axisInput)
        
        # Get health state of the axis
        health = axis.healthState
        if health == adsk.fusion.FeatureHealthStates.WarningFeatureHealthState or health == adsk.fusion.FeatureHealthStates.ErrorFeatureHealthState:
            message = axis.errorOrWarningMessage
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))