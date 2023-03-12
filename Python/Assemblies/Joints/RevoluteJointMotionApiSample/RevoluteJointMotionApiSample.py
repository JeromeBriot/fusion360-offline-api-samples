#Author-Autodesk Inc.
#Description-Demonstrates creating a joint with revolute joint motion

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
        
        # Create sketch in root component
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint0 = adsk.core.Point3D.create(0, 0, 0)
        circle0 = sketchCircles.addByCenterRadius(centerPoint0, 5.0)
        centerPoint1 = adsk.core.Point3D.create(10, 10, 0)
        circle1 = sketchCircles.addByCenterRadius(centerPoint1, 5.0)
        
        # Get the profile defined by the circle
        prof0 = sketch.profiles.item(0)
        prof1 = sketch.profiles.item(1)

        # Create an extrusion input and make sure it's in a new component
        extrudes = rootComp.features.extrudeFeatures
        extInput = extrudes.createInput(prof0, adsk.fusion.FeatureOperations.NewComponentFeatureOperation)
        
        # Set the extrusion input
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(True, distance)
        extInput.isSolid = True

        # Create the extrusion
        ext = extrudes.add(extInput)
        
        # Get the side face of the created extrusion body
        sideFace = ext.sideFaces.item(0)
        
        # Create the first joint geometry with the side face
        geo0 = adsk.fusion.JointGeometry.createByNonPlanarFace(sideFace, adsk.fusion.JointKeyPointTypes.StartKeyPoint)
        
        # Create the second joint geometry with prof1
        geo1 = adsk.fusion.JointGeometry.createByProfile(prof1, circle1, adsk.fusion.JointKeyPointTypes.CenterKeyPoint)
        
        # Create joint input
        joints = rootComp.joints
        jointInput = joints.createInput(geo0, geo1)
        
        # Set the joint input
        angle = adsk.core.ValueInput.createByString('90 deg')
        jointInput.angle = angle
        offset = adsk.core.ValueInput.createByString('1 cm')
        jointInput.offset = offset
        jointInput.isFlipped = True
        jointInput.setAsRevoluteJointMotion(adsk.fusion.JointDirections.YAxisJointDirection)
        
        # Create the joint
        joint = joints.add(jointInput)
  
        revoluteMotion = joint.jointMotion
        limits = revoluteMotion.rotationLimits
        limits.isMinimumValueEnabled = True
        limits.minimumValue = 3.14 / 3
        limits.isMaximumValueEnabled = True
        limits.maximumValue = 3.14 / 3 * 2
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))