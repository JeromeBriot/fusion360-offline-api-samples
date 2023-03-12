# Author-Autodesk Inc.
# Description-Demonstrates creating a joint with planar joint motion

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

        # Create sketch in root component
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchPts = sketch.sketchPoints
        point = adsk.core.Point3D.create(1, 0, 1)
        sketchPt = sketchPts.add(point)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        circle = sketchCircles.addByCenterRadius(centerPoint, 5.0)

        # Get the profile defined by the circle
        prof = sketch.profiles.item(0)

        # Create an extrusion input and make sure it's in a new component
        extrudes = rootComp.features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewComponentFeatureOperation)

        # Set the extrusion input
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(True, distance)
        extInput.isSolid = True

        # Create the extrusion
        ext = extrudes.add(extInput)

        # Get the end face of the created extrusion body
        endFace = ext.endFaces.item(0)

        # Get the occurrence of the new component
        occ = rootComp.occurrences.item(0)

        # Create a new sketch in the occurrence
        sketchInOcc = sketches.add(endFace, occ)

        # Get the sketch curve projected to the sketch
        curve = sketchInOcc.sketchCurves.item(0)

        # Create the first joint geometry with the sketch curve
        geo0 = adsk.fusion.JointGeometry.createByCurve(curve, adsk.fusion.JointKeyPointTypes.CenterKeyPoint)

        # Create the second joint geometry with sketch point
        geo1 = adsk.fusion.JointGeometry.createByPoint(sketchPt)

        # Create joint input
        joints = rootComp.joints
        jointInput = joints.createInput(geo0, geo1)

        # Set the joint input
        jointInput.setAsPlanarJointMotion(adsk.fusion.JointDirections.YAxisJointDirection)

        # Create the joint
        joint = joints.add(jointInput)
  
        planarMotion = joint.jointMotion
        limits = planarMotion.rotationLimits
        limits.isRestValueEnabled = True
        limits.restValue = 1.0
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))