# Author-Autodesk Inc.
# Description-Demonstrates creating a new patch feature

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

        # Get the root component of the active design.
        rootComp = design.rootComponent
        
        # Create sketch
        sketchesObj = rootComp.sketches
        sketch = sketchesObj.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCircles.addByCenterRadius(centerPoint, 3.0)
        sketchCircles.addByCenterRadius(centerPoint, 10.0)

        # Get the profiles defined by the circles.
        profile1 = sketch.profiles.item(0)
        profile2 = sketch.profiles.item(1)
        
        # Get the inner and outer profile
        areaPropertiesOfProfile1 = profile1.areaProperties()
        areaPropertiesOfProfile2 = profile2.areaProperties()
        areaOfProfile1 = areaPropertiesOfProfile1.area
        areaOfProfile2 = areaPropertiesOfProfile2.area

        outerProfile = profile1
        if areaOfProfile1 < areaOfProfile2:
            outerProfile = profile2

        # Create a extrusion based on the outer profile
        extrudes = rootComp.features.extrudeFeatures
        extrudeDistance = adsk.core.ValueInput.createByString("1 cm")
        extrudeFeature = extrudes.addSimple(outerProfile, extrudeDistance, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        # Get BrepEdge from inner loop on the end face of the extrusion
        extrudeEndFace = extrudeFeature.endFaces.item(0)
        brepLoops = extrudeEndFace.loops
        innerLoop = brepLoops.item(0)
        if innerLoop.isOuter:
            innerLoop = brepLoops.item(1)
        brepEdges = innerLoop.edges
        brepEdge = brepEdges.item(0)

        # Create the patch feature
        patches = rootComp.features.patchFeatures
        patchInput = patches.createInput(brepEdge, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        patches.add(patchInput)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))