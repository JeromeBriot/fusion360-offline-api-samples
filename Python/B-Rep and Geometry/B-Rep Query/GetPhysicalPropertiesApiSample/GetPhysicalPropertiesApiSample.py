#Author-Autodesk Inc.
#Description-Demonstrates getting physical properties

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

        # Get the root component of the active design.
        rootComp = design.rootComponent
                
        # Create sub occurrence
        occurrences = rootComp.occurrences
        subOcc = occurrences.addNewComponent(adsk.core.Matrix3D.create())
        
        # Get features from sub component
        subComponent = subOcc.component
        features = subComponent.features

        # Create sketch circle on the xz plane.
        sketches = subComponent.sketches
        sketch = sketches.add(subComponent.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCircles.addByCenterRadius(centerPoint, 10)

        # Create a collection of entities for extrude
        entities0 = adsk.core.ObjectCollection.create()
        entities0.add(sketch.profiles.item(0))

        # Create a cylinder with ExtrudeFeature using the profile above.
        extrudeFeats = features.extrudeFeatures
        extrudeFeatureInput = extrudeFeats.createInput(entities0, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extrudeFeatureInput.isSolid = True
        extrudeFeatureInput.setDistanceExtent(False, adsk.core.ValueInput.createByReal(2.5))
        extrudeFeature = extrudeFeats.add(extrudeFeatureInput)
        
        # Get physical properties from body
        body = extrudeFeature.bodies[0]
        physicalProperties = body.physicalProperties
        
        # Get physical properties from occurrence
        physicalProperties = subOcc.physicalProperties
        
        # Get physical properties from occurrence (low accuracy)
        physicalProperties = subOcc.getPhysicalProperties(adsk.fusion.CalculationAccuracy.LowCalculationAccuracy);
        
     # Get physical properties from occurrence (medium accuracy)
        physicalProperties = subOcc.getPhysicalProperties(adsk.fusion.CalculationAccuracy.MediumCalculationAccuracy);

     # Get physical properties from occurrence (high accuracy)
        physicalProperties = subOcc.getPhysicalProperties(adsk.fusion.CalculationAccuracy.HighCalculationAccuracy);     

        # Get physical properties from occurrence (very high accuracy)
        physicalProperties = subOcc.getPhysicalProperties(adsk.fusion.CalculationAccuracy.VeryHighCalculationAccuracy);
        
        # Get physical properties from component
        physicalProperties = subComponent.physicalProperties
        
        # Get physical properties from component (low accuracy)
        physicalProperties = subComponent.getPhysicalProperties(adsk.fusion.CalculationAccuracy.LowCalculationAccuracy);

     # Get physical properties from component (medium accuracy)
        physicalProperties = subComponent.getPhysicalProperties(adsk.fusion.CalculationAccuracy.MediumCalculationAccuracy);
        
     # Get physical properties from component (high accuracy)
        physicalProperties = subComponent.getPhysicalProperties(adsk.fusion.CalculationAccuracy.HighCalculationAccuracy);
        
        # Get physical properties from component (very high accuracy)
        physicalProperties = subComponent.getPhysicalProperties(adsk.fusion.CalculationAccuracy.VeryHighCalculationAccuracy);   
        
        # Get data from physical properties
        area = physicalProperties.area
        density = physicalProperties.density
        mass = physicalProperties.mass
        volume = physicalProperties.volume
        
        # Get accuracy from physical properties
        accuracy = physicalProperties.accuracy

        # Get center of mass from physical properties
        cog = physicalProperties.centerOfMass

        # Get principal axes from physical properties
        (retVal, xAxis0, yAxis0, zAxis0) = physicalProperties.getPrincipalAxes()   
        
        # Get the moments of inertia about the principal axes. Unit for returned values is kg/cm^2.
        (retVal,i1,i2,i3) = physicalProperties.getPrincipalMomentsOfInertia()

        # Get the radius of gyration about the principal axes. Unit for returned values is cm.
        (retVal, kx, ky, kz) = physicalProperties.getRadiusOfGyration()

        # Get the rotation from the world coordinate system of the target to the principal coordinate system.
        (retVal, rx, ry, rz) = physicalProperties.getRotationToPrincipal()

        # Get the moment of inertia about the world coordinate system.
        (retVal, xx, yy, zz, xy, yz, xz) = physicalProperties.getXYZMomentsOfInertia()
        
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))