# Author-Autodesk Inc.
# Description-Demonstrates how to use AreaProperties

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
        
        # Create sketch circle
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCircles.addByCenterRadius(centerPoint, 5.0)        
        centerPointTwo = adsk.core.Point3D.create(15, 0, 0)
        sketchCircles.addByCenterRadius(centerPointTwo, 5.0)
        
        # Get the profiles defined by the circle
        prof = sketch.profiles.item(0)
        profTwo = sketch.profiles.item(1)
        
        # Get area properties from a profile
        areaProps = prof.areaProperties(adsk.fusion.CalculationAccuracy.MediumCalculationAccuracy)
        
        # Get area
        area = areaProps.area
        
        # Get centroid
        centroid = areaProps.centroid
        
        # Get perimeter
        perimeter = areaProps.perimeter
        
        # Get angle of rotation of the principal axes
        rotationOfPrincipal = areaProps.rotationToPrincipal
        
        # Accuracy
        accuracy = areaProps.accuracy
        
        # Get area properties from two profiles
        inputs = adsk.core.ObjectCollection.create()
        inputs.add(prof)
        inputs.add(profTwo)
        areaProps = design.areaProperties(inputs)
        
        # Get principal axes
        (retVal, xAxis, yAxis) = areaProps.getPrincipalAxes()
        
        # Get centroid moments of inertia
        (retVal, centixx, centiyy, centizz, centixy, centiyz, centixz) = areaProps.getCentroidMomentsOfInertia()
        
        # Get principal moments of inertia
        (retVal, i1, i2, i3) = areaProps.getPrincipalMomentsOfInertia()
        
        # Get radius of gyration
        (retVal, kxx, kyy, kzz) = areaProps.getRadiusOfGyration()
        
        # Get moments of inertia
        (retVal, ixx, iyy, izz, ixy, iyz, ixz) = areaProps.getMomentsOfInertia()
 
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))