# Author-Autodesk Inc.
# Description-Demonstrates creating a new loft feature

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
        rootComp = design.rootComponent

        # Create profile 1
        sketchesObj = rootComp.sketches
        sketch0 = sketchesObj.add(rootComp.xZConstructionPlane)
        sketchCirclesObj0 = sketch0.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCirclesObj0.addByCenterRadius(centerPoint, 5.0)
        profile0 = sketch0.profiles.item(0)

        # Create profile 2
        ctorPlanes = rootComp.constructionPlanes
        ctorPlaneInput1 = ctorPlanes.createInput()
        offset = adsk.core.ValueInput.createByString("10 cm")
        ctorPlaneInput1.setByOffset(rootComp.xZConstructionPlane, offset)
        ctorPlane1 = ctorPlanes.add(ctorPlaneInput1)
        sketch1 = sketchesObj.add(ctorPlane1)
        sketchCirclesObj1 = sketch1.sketchCurves.sketchCircles
        sketchCirclesObj1.addByCenterRadius(centerPoint, 2.0)
        profile1 = sketch1.profiles.item(0)

        # Create profile 3
        ctorPlaneInput2 = ctorPlanes.createInput()
        ctorPlaneInput2.setByOffset(ctorPlane1, offset)
        ctorPlane2 = ctorPlanes.add(ctorPlaneInput2)
        sketch2 = sketchesObj.add(ctorPlane2)
        sketchCirclesObj2 = sketch2.sketchCurves.sketchCircles
        sketchCirclesObj2.addByCenterRadius(centerPoint, 10.0)
        profile2 = sketch2.profiles.item(0)

        # Create loft feature input
        loftFeats = rootComp.features.loftFeatures
        loftInput = loftFeats.createInput(adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        loftSectionsObj = loftInput.loftSections
        loftSectionsObj.add(profile0)
        loftSectionsObj.add(profile1)
        loftSectionsObj.add(profile2)
        loftInput.isSolid = False
        loftInput.isClosed = False
        loftInput.isTangentEdgesMerged = True

        # Create loft feature
        loftFeats.add(loftInput)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))