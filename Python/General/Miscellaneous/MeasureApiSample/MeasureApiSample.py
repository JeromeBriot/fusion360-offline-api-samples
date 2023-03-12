#Author-Autodesk Inc.
#Description-Measure related functions


import adsk.core, adsk.fusion, adsk.cam, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        # Create a new document
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        
        # Get the root component of the active design
        rootComp = design.rootComponent
        
        # Create a sketch
        sketches = rootComp.sketches
        sketch1 = sketches.add(rootComp.yZConstructionPlane)
        
        # Create sketch lines
        sketchLines = sketch1.sketchCurves.sketchLines
        
        # Create some 3D points
        point1 = adsk.core.Point3D.create(0.0, 0.0, 0.0)
        point2 = adsk.core.Point3D.create(5.0, 5.0, 0.0)
        
        # Create sketch rectangle
        sketchLines.addTwoPointRectangle(point1, point2)
        
        # Get the profile
        profile = sketch1.profiles.item(0)
        
        # Create an extusion input
        extrudes = rootComp.features.extrudeFeatures
        extrudeInput = extrudes.createInput(profile, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)     
        # Set extrude distance is 6 cm
        distance = adsk.core.ValueInput.createByReal(6.0)
        # Set the distance extent
        extrudeInput.setDistanceExtent(False, distance)
        # Set the extrude type is solid
        extrudeInput.isSolid = True
        
        # Create the extrude
        extrude = extrudes.add(extrudeInput)
        
        # Get the extrude brepbody
        brepBody = extrude.bodies.item(0)
          
        # Measure minimum distance
        print("*** Measure minimum distance.")
        sketchLine = sketchLines.item(0)
        brepEdge = brepBody.edges.item(0)
        measureResult = app.measureManager.measureMinimumDistance(sketchLine, brepEdge)
        minDistance = measureResult.value
        print('Minimum distance value is ' + str(minDistance))
        
        position1 = measureResult.positionOne
        position2 = measureResult.positionTwo
        print('positionOne is ('
              + str(position1.x) 
              + ',' 
              + str(position1.y) 
              + ',' 
              + str(position1.z)
              + ')'
             )
        print('positionTwo is ('
              + str(position2.x) 
              + ',' 
              + str(position2.y) 
              + ',' 
              + str(position2.z) 
              + ')'
             ) 
        
        # Measure angle for three point
        print("*** Measure angle for three point.")
        constructionPoint = rootComp.originConstructionPoint
        vertex = brepBody.vertices.item(0)
        measureResult = app.measureManager.measureAngle(constructionPoint, point2, vertex)
        angle = measureResult.value
        print('Angle value is ' + str(angle))
        
        position1 = measureResult.positionOne
        position2 = measureResult.positionTwo
        position3 = measureResult.positionThree
        print('positionOne is ('
              + str(position1.x) 
              + ',' 
              + str(position1.y) 
              + ',' 
              + str(position1.z)
              + ')'
             )
        print('positionTwo is ('
              + str(position2.x) 
              + ',' 
              + str(position2.y) 
              + ',' 
              + str(position2.z) 
              + ')'
             )       
        print('positionThree is ('
              + str(position3.x) 
              + ',' 
              + str(position3.y) 
              + ',' 
              + str(position3.z) 
              + ')'
             )  
        
        # Measure angle for two object
        print("*** Measure angle for two objects.")
        brepFace = brepBody.faces.item(1)
        brepEdge = brepBody.edges.item(0)
        measureResult = app.measureManager.measureAngle(brepFace, brepEdge)
        angle = measureResult.value
        print('Angle value is ' + str(angle))
        
        # Get oriented bounding box
        print("*** Get oriented bounding box.")
        vector1 = adsk.core.Vector3D.create(0.0, 0.0, 1.0)
        vector2 = adsk.core.Vector3D.create(0.0, 1.0, 0.0)
        boundingBox = app.measureManager.getOrientedBoundingBox(brepBody, vector1, vector2)
        
        # Get the bounding box length, width and height
        length = boundingBox.length
        print("Bounding box length is " + str(length))
        width = boundingBox.width
        print("Bounding box width is " + str(width))
        height = boundingBox.height
        print("Bounding box height is " + str(height))
        
        # Get the bounding box width direction 
        widthDirection = boundingBox.widthDirection
        heightDirection = boundingBox.heightDirection
        lengthDirection = boundingBox.lengthDirection
        print("Bounding box width direction is " 
              + "("
              + str(widthDirection.x)
              + ", "
              + str(widthDirection.y)
              + ", "
              + str(widthDirection.z)
              + ")"
             )
        
        # Get the bounding contain a point or not
        pointA= adsk.core.Point3D.create(-2.0, 0.5, 2.0)
        isContain = boundingBox.contains(pointA)
        print("Dose the bounding box contain pointA? " + str(isContain))
        
        pointB = adsk.core.Point3D.create(0.0, 0.0, 0.0)
        isContain = boundingBox.contains(pointB)
        print("Dose the bounding box contain pointB? " + str(isContain))
        
        # Copy the bounding box
        boundingBoxCopy = boundingBox.copy()
        length = boundingBoxCopy.length
        print("The copied Bounding box length is " + str(length))
             
        # Set bounding box height
        boundingBoxCopy.height = 10.0
        length = boundingBoxCopy.height
        print("The new set length of the copied bounding box is " + str(length))
        
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))