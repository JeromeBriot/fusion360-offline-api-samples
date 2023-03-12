# Author-Autodesk Inc.
# Description-Demonstrates creating a new extrude feature

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
                
        # Get extrude features
        extrudes = rootComp.features.extrudeFeatures

        # Create sketch     
        sketches = rootComp.sketches   
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        circle = sketchCircles.addByCenterRadius(centerPoint, 5.0)
        
        # Get the profile defined by the circle
        prof = sketch.profiles.item(0)
        
        # Create another sketch
        sketchVertical = sketches.add(rootComp.yZConstructionPlane)
        sketchCirclesVertical = sketchVertical.sketchCurves.sketchCircles
        centerPointVertical = adsk.core.Point3D.create(0, 1, 0)
        cicleVertical = sketchCirclesVertical.addByCenterRadius(centerPointVertical, 0.5)    
        
        # Get the profile defined by the vertical circle
        profVertical = sketchVertical.profiles.item(0)
        
        # Extrude Sample 1: A simple way of creating typical extrusions (extrusion that goes from the profile plane the specified distance).
        # Define a distance extent of 5 cm
        distance = adsk.core.ValueInput.createByReal(5)
        extrude1 = extrudes.addSimple(prof, distance, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)        
        # Get the extrusion body
        body1 = extrude1.bodies.item(0)
        body1.name = "simple"

        # Get the state of the extrusion
        health = extrude1.healthState
        if health == adsk.fusion.FeatureHealthStates.WarningFeatureHealthState or health == adsk.fusion.FeatureHealthStates.ErrorFeatureHealthState:
            message = extrude1.errorOrWarningMessage
        
        # Get the state of timeline object
        timeline = design.timeline
        timelineObj = timeline.item(timeline.count - 1);
        health = timelineObj.healthState
        message = timelineObj.errorOrWarningMessage
        
        # Create another sketch
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        circle1 = sketchCircles.addByCenterRadius(centerPoint, 13.0)
        circle2 = sketchCircles.addByCenterRadius(centerPoint, 15.0)
        outerProfile = sketch.profiles.item(1)
        
        # Create taperAngle value inputs
        deg0 = adsk.core.ValueInput.createByString("0 deg")
        deg2 = adsk.core.ValueInput.createByString("2 deg")
        deg5 = adsk.core.ValueInput.createByString("5 deg")
        
        # Create distance value inputs
        mm10 = adsk.core.ValueInput.createByString("10 mm")
        mm100 = adsk.core.ValueInput.createByString("100 mm")
         
        # Extrude Sample 2: Create an extrusion that goes from the profile plane with one side distance extent
        extrudeInput = extrudes.createInput(outerProfile, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        # Create a distance extent definition
        extent_distance = adsk.fusion.DistanceExtentDefinition.create(mm100)        
        extrudeInput.setOneSideExtent(extent_distance, adsk.fusion.ExtentDirections.PositiveExtentDirection)
        # Create the extrusion
        extrude2 = extrudes.add(extrudeInput)
        # Get the body of the extrusion       
        body2 = extrude2.bodies.item(0)
        body2.name = "distance, from profile"
        
        # Extrude Sample 3: Create an extrusion that starts from an entity and goes the specified distance.
        extrudeInput = extrudes.createInput(profVertical, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        # Create a distance extent definition
        extent_distance_2 = adsk.fusion.DistanceExtentDefinition.create(mm10)
        # Create a start extent that starts from a brep face with an offset of 10 mm.
        start_from = adsk.fusion.FromEntityStartDefinition.create(body1.faces.item(0), mm10)
        # taperAngle should be 0 because extrude start face is not a planar face in this case
        extrudeInput.setOneSideExtent(extent_distance_2, adsk.fusion.ExtentDirections.PositiveExtentDirection)        
        extrudeInput.startExtent = start_from
        # Create the extrusion
        extrude3 = extrudes.add(extrudeInput)
        body3 = extrude3.bodies.item(0)
        body3.name = "distance, from entity"
        
        # Edit the distance extent of the extrusion.
        disDef = adsk.fusion.DistanceExtentDefinition.cast(extrude3.extentOne)
        distanceMP = adsk.fusion.ModelParameter.cast(disDef.distance)
        distanceMP.value = 5.0
        
        # Edit the start entity of the extrusion.
        startDef = adsk.fusion.FromEntityStartDefinition.cast(extrude3.startExtent)
        outerFace = body2.faces.item(1)
        extrude3.timelineObject.rollTo(True)
        startDef.entity = outerFace
        design.timeline.moveToEnd()

        # Edit the offset to the start entity in the extrusion.
        startDef = adsk.fusion.FromEntityStartDefinition.cast(extrude3.startExtent)
        offsetMP = adsk.fusion.ModelParameter.cast(startDef.offset)
        offsetMP.value = 1.5
        
        # Extrude Sample 4: Create an extrusion that goes from the profile plane to a specified entity.
        extrudeInput = extrudes.createInput(profVertical, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        # Create a to-entity extent definition
        isChained = True
        extent_toentity = adsk.fusion.ToEntityExtentDefinition.create(body1, isChained)
        # Set the one side extent with the to-entity-extent-definition, and with a taper angle of 0 degree
        extrudeInput.setOneSideExtent(extent_toentity, adsk.fusion.ExtentDirections.PositiveExtentDirection)
        # Create an offset type start definition
        start_offset = adsk.fusion.OffsetStartDefinition.create(mm10)
        # Set the start extent of the extrusion
        extrudeInput.startExtent = start_offset
        # Create the extrusion
        extrude4 = extrudes.add(extrudeInput)
        body4 = extrude4.bodies.item(0)
        body4.name = "to entity, from offset"
        
        # Edit the start offset of the extrusion
        startDef = adsk.fusion.OffsetStartDefinition.cast(extrude4.startExtent)
        offsetMP = adsk.fusion.ModelParameter.cast(startDef.offset)
        offsetMP.value = 0.5

        # Edit the to-entity extent definition of the extrusion
        negative = adsk.core.Vector3D.create(-1,0,0)
        toDef = adsk.fusion.ToEntityExtentDefinition.cast(extrude4.extentOne)
        extrude4.timelineObject.rollTo(True)
        toDef.entity = body2
        toDef.isMinimumSolution = False
        toDef.directionHint = negative
        toDef.isChained = False
        design.timeline.moveToEnd()
      
        # Extrude Sample 5: Create an extrusion that goes through all entities
        extrudeInput = extrudes.createInput(profVertical, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        # Create an extent definition of through-all type.          
        extent_all = adsk.fusion.ThroughAllExtentDefinition.create()
        extrudeInput.setOneSideExtent(extent_all, adsk.fusion.ExtentDirections.NegativeExtentDirection, deg2)
        # Set the extrusion start with an offset
        extrudeInput.startExtent = start_offset
        # Create the extrusion
        extrude5 = extrudes.add(extrudeInput)
        body5 = extrude5.bodies.item(0)
        body5.name = "through-all, from offset"
        
        # Edit the start offset
        startDef = adsk.fusion.OffsetStartDefinition.cast(extrude5.startExtent)
        offsetMP = adsk.fusion.ModelParameter.cast(startDef.offset)
        offsetMP.value = 0.5
        
        # Edit the direction of the extrusion, make it in the same direction as the sketch plane.
        allDef = adsk.fusion.ThroughAllExtentDefinition.cast(extrude5.extentOne)
        extrude5.timelineObject.rollTo(True)
        if allDef.isPositiveDirection:
            allDef.isPositiveDirection = False
        design.timeline.moveToEnd()
        
        # Extrude Sample 6: Create a symmetric extrusion that goes 10 mm from the profile plane with a 5 degree taper angle.
        isFullLength = True
        extrudeInput = extrudes.createInput(profVertical, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extrudeInput.setSymmetricExtent(mm10, isFullLength, deg5)
        # Create the extrusion
        extrude6 = extrudes.add(extrudeInput)
        body6 = extrude6.bodies.item(0)
        body6.name = "symmetric"
        
        # Edit the measurement, distance and taper angle properties of the symmetric extrusion
        symDef = adsk.fusion.SymmetricExtentDefinition.cast(extrude6.extentOne)
        extrude6.timelineObject.rollTo(True)
        symDef.isFullLength = not symDef.isFullLength
        design.timeline.moveToEnd()
        taperAngleMP = adsk.fusion.ModelParameter.cast(symDef.taperAngle)
        taperAngleMP.expression = "6 deg"
        distanceMP = adsk.fusion.ModelParameter.cast(symDef.distance)
        distanceMP.expression = "3 mm"
        # another way to get the symmetric extent definition
        if (extrude6.extentType == adsk.fusion.FeatureExtentTypes.SymmetricFeatureExtentType):
            symDef1 = extrude6.symmetricExtent
            distanceMP1 = symDef1.distance
            distanceMP1.value = 4
        
        # Extrude Sample 7: Create a 2-side extrusion, whose 1st side is 100 mm distance extent, and 2nd side is 10 mm distance extent.
        extrudeInput = extrudes.createInput(profVertical, adsk.fusion.FeatureOperations.CutFeatureOperation)
        extent_distance_2 = adsk.fusion.DistanceExtentDefinition.create(adsk.core.ValueInput.createByString("20cm"))
        extrudeInput.setTwoSidesExtent(extent_distance, extent_distance_2, deg5, deg0)
        extrude7 = extrudes.add(extrudeInput)
        
        # Edit the taper angles of both sides in the extrusion
        angleMP_1 = adsk.fusion.ModelParameter.cast(extrude7.taperAngleOne)
        angleMP_2 = adsk.fusion.ModelParameter.cast(extrude7.taperAngleTwo)
        angleMP_1.expression = "30 deg"
        angleMP_2.expression = "-1 deg"
        # Get the extent definition of both sides
        extent_1 = adsk.fusion.DistanceExtentDefinition.cast(extrude7.extentOne)
        extent_2 = adsk.fusion.DistanceExtentDefinition.cast(extrude7.extentTwo)
        # Edit the distances the extrusion
        distanceMP_1 = adsk.fusion.ModelParameter.cast(extent_1.distance)
        distanceMP_2 = adsk.fusion.ModelParameter.cast(extent_2.distance)
        distanceMP_1.expression = "80 mm"
        distanceMP_2.expression = "25 cm"
        
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))