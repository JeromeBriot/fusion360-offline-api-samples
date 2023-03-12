# Author-Autodesk Inc.
# Description-Demonstrates creating a new untrim feature

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
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(0, 0, 0)
        sketchCircle1 = sketchCircles.addByCenterRadius(centerPoint, 3.0)
        sketchCircle2 = sketchCircles.addByCenterRadius(centerPoint, 10.0)
        
        # Get the profiles defined by the circles
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
        extrudeFeatures = rootComp.features.extrudeFeatures
        extrudeDistance = adsk.core.ValueInput.createByString("1 cm")
        extrudeFeature = extrudeFeatures.addSimple(outerProfile, extrudeDistance, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
 
        # Get start and end faces of the extrusion
        extrudeStartFaces = extrudeFeature.startFaces
        extrudeEndFaces = extrudeFeature.endFaces

        # Create unstitch feature (so there are separate surface bodies)
        unstitchFeatures = rootComp.features.unstitchFeatures
        body = extrudeFeature.bodies.item(0)
        entities = adsk.core.ObjectCollection.create()
        entities.add(body)
        unstitchFeature = unstitchFeatures.add(entities)

        # Create untrim feature on start face (remove external loop and extend face)
        facesToUntrim = [];
        facesToUntrim.append(extrudeStartFaces.item(0))
        untrimFeatures = rootComp.features.untrimFeatures
        extensionDistance = adsk.core.ValueInput.createByReal(1.0)
        untrimFeatureInput = untrimFeatures.createInputFromFaces(facesToUntrim, adsk.fusion.UntrimLoopTypes.ExternalLoopsUntrimType, extensionDistance)
        untrimFeature = untrimFeatures.add(untrimFeatureInput)
 
        # Create untrim feature on the end face (remove internal loop from face)
        brepLoops = extrudeEndFaces.item(0).loops
        innerLoop = brepLoops.item(0)
        if innerLoop.isOuter:
            innerLoop = brepLoops.item(1)
        loopsToRemove = []
        loopsToRemove.append(innerLoop)
        untrimFeatureInput = untrimFeatures.createInputFromLoops(loopsToRemove);
        untrimFeature = untrimFeatures.add(untrimFeatureInput)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))