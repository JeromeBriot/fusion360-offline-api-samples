# Author-Autodesk Inc.
# Description-Demonstrates creating a new split face feature

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
         # Create a document.
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
 
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)

        # Get the root component of the active design
        rootComp = design.rootComponent
        
        # Create a sketch that has a rectangle in it
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchLines = sketch.sketchCurves.sketchLines
        point0 = adsk.core.Point3D.create(0, 0, 0)
        point1 = adsk.core.Point3D.create(10, 10, 0)
        sketchLines.addTwoPointRectangle(point0, point1)
        
        # Get the profile defined by the rectangle
        prof = sketch.profiles.item(0)

        # Create an extrusion input
        extrudes = rootComp.features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Define that the extent is a distance extent of 5 cm
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(True, distance)

        # Create the extrusion
        ext = extrudes.add(extInput)

        # Get one edge of the extrusion body
        face = ext.endFaces.item(0)
        edge = face.edges.item(0)
        
        # Create a slant construction plane with an angle of 45 deg on the xZConstructionPlane
        planeInput = rootComp.constructionPlanes.createInput()
        planeInput.setByAngle(edge, adsk.core.ValueInput.createByString('45 deg'), rootComp.xZConstructionPlane)
        plane = rootComp.constructionPlanes.add(planeInput)
        
        # Create another sketch containing a circle profile on the slant plane
        toolSketch = sketches.add(plane)
        sketchCircles = toolSketch.sketchCurves.sketchCircles
        circle = sketchCircles.addByCenterRadius(point0, 3)
        
        # Get SplitFaceFetures
        splitFaceFeats = rootComp.features.splitFaceFeatures
        
        # Set faces to split
        facesCol = adsk.core.ObjectCollection.create()
        facesCol.add(face)
        
        # Create a split face feature of surface intersection split type
        splitFaceInput = splitFaceFeats.createInput(facesCol, circle, True)
        split = splitFaceFeats.add(splitFaceInput)
        split.deleteMe()
        
        # Create another split face feature of closest point split type   
        splitFaceInput.setClosestPointSplitType()
        split = splitFaceFeats.add(splitFaceInput)
        split.deleteMe()
        
        # Create another split face feature of along vector split type   
        splitFaceInput.setAlongVectorSplitType(face.edges.item(1))
        splitFaceFeats.add(splitFaceInput)
        
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))