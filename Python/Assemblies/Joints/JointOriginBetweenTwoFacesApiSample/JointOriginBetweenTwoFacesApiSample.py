# Author-Autodesk Inc.
# Description-Demonstrates creating a new Joint Origin between two planes

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

        # Create a sub component under root component
        allOccs = rootComp.occurrences
        transform = adsk.core.Matrix3D.create()
        subOcc = allOccs.addNewComponent(transform)

        # Create an extrude in the sub component
        subComp = subOcc.component
        sketches = subComp.sketches
        sketch = sketches.add(subComp.xZConstructionPlane)
        sketchLines = sketch.sketchCurves.sketchLines
        sketchLines.addTwoPointRectangle(adsk.core.Point3D.create(0, 0, 0), adsk.core.Point3D.create(10, 10, 0))

        profile = sketch.profiles.item(0)

        extrudes = subComp.features.extrudeFeatures
        extInput = extrudes.createInput(profile, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        distance = adsk.core.ValueInput.createByString("50 mm")
        extInput.setDistanceExtent(False, distance)
        extInput.isSolid = True
        extrude = extrudes.add(extInput)

        faces = extrude.faces

        # Create the joint geometry
        jointGeometry = adsk.fusion.JointGeometry.createByBetweenTwoPlanes(faces.item(0), faces.item(1), faces.item(0), None, adsk.fusion.JointKeyPointTypes.CenterKeyPoint)

        # Create the JointOriginInput
        jointOrigins = subComp.jointOrigins
        jointOriginInput = jointOrigins.createInput(jointGeometry)

        # Create the JointOrigin
        jointOrigins.add(jointOriginInput)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))