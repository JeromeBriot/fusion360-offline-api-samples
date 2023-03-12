# Author-Autodesk Inc.
# Description-Demonstrates creating a new extend feature

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface

        # Create a document
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        
        design = app.activeProduct

        # Get the root component of the active design.
        rootComp = design.rootComponent
        
        # Create sketch line on the xz plane.
        sketches = rootComp.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)
        sketchLines = sketch.sketchCurves.sketchLines
        startPoint = adsk.core.Point3D.create(0, 0, 0)
        endPoint = adsk.core.Point3D.create(1, 0, 0)
        sketchLine = sketchLines.addByTwoPoints(startPoint, endPoint)
        
        # Create a surface with ExtrudeFeature.
        features = rootComp.features
        extrudeFeatures = features.extrudeFeatures
        openProfile = rootComp.createOpenProfile(sketchLine)
        extrudeFeatureInput = extrudeFeatures.createInput(openProfile, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extrudeFeatureInput.isSolid = False
        extrudeFeatureInput.setDistanceExtent(False, adsk.core.ValueInput.createByReal(1.0))
        
        # Get the surface body.
        extrudeFeature = extrudeFeatures.add(extrudeFeatureInput)
        surface = extrudeFeature.bodies.item(0)
        
        # Get an edge from surface, and add it to object collection.
        edges = surface.edges
        edge = edges.item(0)
        inputEdges = adsk.core.ObjectCollection.create()
        inputEdges.add(edge)
        
        # Define a distance to extend with 1 cm.
        distance = adsk.core.ValueInput.createByReal(1.0)
        
        # Create an extend input to be able to define the input needed for an extend.
        extendFeatures = features.extendFeatures
        extendFeatureInput = extendFeatures.createInput(inputEdges, distance, adsk.fusion.SurfaceExtendTypes.NaturalSurfaceExtendType)
        extendFeatureInput.extendAlignment = adsk.fusion.SurfaceExtendAlignment.FreeEdges
        
        # Create an extend feature.
        extendFeature = extendFeatures.add(extendFeatureInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))