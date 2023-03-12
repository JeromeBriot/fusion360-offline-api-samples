# Author-Autodesk Inc.
# Description-Demonstrates creating a new Rigid Group

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

        # Create four sub components under root component
        allOccs = rootComp.occurrences

        transform0 = adsk.core.Matrix3D.create()
        vector3d0 = adsk.core.Vector3D.create(10.0, 0.0, 0.0)
        transform0.translation = vector3d0
        subOcc0 = allOccs.addNewComponent(transform0)

        transform1 = adsk.core.Matrix3D.create()
        vector3d1 = adsk.core.Vector3D.create(0.0, 0.0, 12.0)
        transform1.translation = vector3d1
        subOcc1 = allOccs.addNewComponent(transform1)

        transform2 = adsk.core.Matrix3D.create()
        vector3d2 = adsk.core.Vector3D.create(-8.0, 0.0, 0.0)
        transform2.translation = vector3d2
        subOcc2 = allOccs.addNewComponent(transform2)

        transform3 = adsk.core.Matrix3D.create()
        vector3d3 = adsk.core.Vector3D.create(0.0, 0.0, -6.0)
        transform3.translation = vector3d3
        subOcc3 = allOccs.addNewComponent(transform3)

        # Create cylinder 1 in sub component 1
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        subComp0 = subOcc0.component
        sketches0 = subComp0.sketches
        sketch0 = sketches0.add(subComp0.xZConstructionPlane)
        sketchCircles0 = sketch0.sketchCurves.sketchCircles
        sketchCircles0.addByCenterRadius(centerPoint, 0.5)

        profile0 = sketch0.profiles.item(0)
        extrudes0 = subComp0.features.extrudeFeatures
        extInput0 = extrudes0.createInput(profile0, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        distance0 = adsk.core.ValueInput.createByString("50 mm")
        extInput0.setDistanceExtent(False, distance0)
        extInput0.isSolid = False
        extrudes0.add(extInput0)

        # Create cylinder 2 in sub component 2
        subComp1 = subOcc1.component
        sketches1 = subComp1.sketches
        sketch1 = sketches1.add(subComp1.xZConstructionPlane)
        sketchCircles1 = sketch1.sketchCurves.sketchCircles
        sketchCircles1.addByCenterRadius(centerPoint, 0.75)

        profile1 = sketch1.profiles.item(0)
        extrudes1 = subComp1.features.extrudeFeatures
        extInput1 = extrudes1.createInput(profile1, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        distance1 = adsk.core.ValueInput.createByString("75 mm")
        extInput1.setDistanceExtent(False, distance1)
        extInput1.isSolid = False
        extrudes1.add(extInput1)

        # Create cylinder 3 in sub component 3
        subComp2 = subOcc2.component
        sketches2 = subComp2.sketches
        sketch2 = sketches2.add(subComp2.xZConstructionPlane)
        sketchCircles2 = sketch2.sketchCurves.sketchCircles
        sketchCircles2.addByCenterRadius(centerPoint, 1.0)

        profile2 = sketch2.profiles.item(0)
        extrudes2 = subComp2.features.extrudeFeatures
        extInput2 = extrudes2.createInput(profile2, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        distance2 = adsk.core.ValueInput.createByString("100 mm")
        extInput2.setDistanceExtent(False, distance2)
        extInput2.isSolid = False
        extrudes2.add(extInput2)

        # Create cylinder 4 in sub component 4
        subComp3 = subOcc3.component
        sketches3 = subComp3.sketches
        sketch3 = sketches3.add(subComp3.xZConstructionPlane)
        sketchCircles3 = sketch3.sketchCurves.sketchCircles
        sketchCircles3.addByCenterRadius(centerPoint, 1.25)

        profile3 = sketch3.profiles.item(0)
        extrudes3 = subComp3.features.extrudeFeatures
        extInput3 = extrudes3.createInput(profile3, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        distance3 = adsk.core.ValueInput.createByString("125 mm")
        extInput3.setDistanceExtent(False, distance3)
        extInput3.isSolid = False
        extrudes3.add(extInput3)

        # Create object collection
        occs = adsk.core.ObjectCollection.create()
        occs.add(subOcc0)
        occs.add(subOcc1)
        occs.add(subOcc2)
        occs.add(subOcc3)

        # Create a Rigid group
        isIncludeChildren = True
        rigidGroups_ = rootComp.rigidGroups
        rigidGroups_.add(occs, isIncludeChildren)

        camera_ = app.activeViewport.camera
        camera_.isFitView = True
        app.activeViewport.camera = camera_

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))