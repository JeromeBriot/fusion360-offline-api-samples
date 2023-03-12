# Author-Autodesk Inc.
# Description-Demonstrates creating a new As-Built Joint

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

        # Create two new components under root component
        allOccs = rootComp.occurrences
        transform = adsk.core.Matrix3D.create()
        subOcc0 = allOccs.addNewComponent(transform)

        # Create sketch 1 in sub component 1
        subComp0 = subOcc0.component
        sketches0 = subComp0.sketches
        sketch0 = sketches0.add(subComp0.xZConstructionPlane)
        sketchCircles0 = sketch0.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCircles0.addByCenterRadius(centerPoint, 2.5)

        # Get the profile defined by the circle
        prof0 = sketch0.profiles.item(0)

        # Create an extrude input and make sure it's in the new component
        extrudes0 = subComp0.features.extrudeFeatures
        extInput0 = extrudes0.createInput(prof0, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        # Set the extrude input
        distance0 = adsk.core.ValueInput.createByString("5 mm")
        extInput0.setDistanceExtent(False, distance0)
        extInput0.isSolid = True

        # Create the extrude
        extrude0 = extrudes0.add(extInput0)

        # Get the end face of the created extrude
        endFaceOfExtrude0 = extrude0.endFaces.item(0)

        # Create a construction plane for extrude 2
        constructionPlanes_ = subComp0.constructionPlanes
        constructionPlaneInput = constructionPlanes_.createInput()
        constructionPlaneInput.setByOffset(endFaceOfExtrude0, adsk.core.ValueInput.createByString("20 mm"))
        constructionPlane = constructionPlanes_.add(constructionPlaneInput)
        constructionPlaneProxy = constructionPlane.createForAssemblyContext(subOcc0)

        # Create sketch 2 in sub component 2
        subOcc1 = allOccs.addNewComponent(transform)
        subComp1 = subOcc1.component
        sketches1 = subComp1.sketches
        sketch1 = sketches1.add(constructionPlaneProxy)
        sketchCircles1 = sketch1.sketchCurves.sketchCircles
        sketchCircles1.addByCenterRadius(centerPoint, 0.5)

        # Get the profile defined by the circle
        prof1 = sketch1.profiles.item(0)

        # Create an extrude input and make sure it's in the new component
        extrudes1 = subComp1.features.extrudeFeatures
        extInput1 = extrudes1.createInput(prof1, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        # Set the extrude input
        distance1 = adsk.core.ValueInput.createByString("50 mm")
        extInput1.setDistanceExtent(False, distance1)
        extInput1.isSolid = False

        # Create the extrude
        extrudes1.add(extInput1)

        # Create the AsBuiltJointInput
        asBuiltJoints_ = rootComp.asBuiltJoints
        asBuiltJointInput = asBuiltJoints_.createInput(subOcc0, subOcc1, None)

        # Create the AsBuiltJoint
        asBuiltJoints_.add(asBuiltJointInput)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))