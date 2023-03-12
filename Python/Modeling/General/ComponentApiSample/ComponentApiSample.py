# Author-Autodesk Inc.
# Description-Component related functions

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

        allOccs = rootComp.occurrences
        transform = adsk.core.Matrix3D.create()

        # Create a component under root component
        occ1 = allOccs.addNewComponent(transform)
        subComp1 = occ1.component
        print(subComp1.revisionId)

        # Create a sketch in sub component 1
        sketches1 = subComp1.sketches
        sketch1 = sketches1.add(rootComp.yZConstructionPlane)
        print(subComp1.revisionId)

        # Get sketch lines
        sketchLines = sketch1.sketchCurves.sketchLines

        # Create sketch rectangle
        startPoint = adsk.core.Point3D.create(-8.0, 0, 0)
        endPoint = adsk.core.Point3D.create(8.0, 8.0, 0)
        sketchLines.addTwoPointRectangle(startPoint, endPoint)
        print(subComp1.revisionId)

        # Get the profile of the first sketch
        prof1 = sketch1.profiles.item(0)

        # Create an extrusion input
        extrudes1 = subComp1.features.extrudeFeatures
        extInput1 = extrudes1.createInput(prof1, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        # Define that the extent is a distance extent of 2 cm
        distance1 = adsk.core.ValueInput.createByReal(2.0)
        # Set the distance extent
        extInput1.setDistanceExtent(False, distance1)
        # Set the extrude type to be solid
        extInput1.isSolid = True

        # Create the extrusion
        ext1 = extrudes1.add(extInput1)
        print(subComp1.revisionId)

    except:
        if ui:

            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))