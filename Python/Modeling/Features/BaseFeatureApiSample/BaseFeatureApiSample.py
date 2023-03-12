# Author-Autodesk Inc.
# Description-Creates a new base feature

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try: 
        app = adsk.core.Application.get()
        ui = app.userInterface

        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        design = app.activeProduct
        design.designType = adsk.fusion.DesignTypes.ParametricDesignType

        # Get the root component of the active design.
        rootComp = design.rootComponent

        # Create a base feature
        baseFeats = rootComp.features.baseFeatures
        baseFeat = baseFeats.add()
        
        baseFeat.startEdit()
        
        # Create construction plane in base feature
        planes = rootComp.constructionPlanes
        planeInput = planes.createInput()
        planeInput.targetBaseOrFormFeature = baseFeat
        planeInput.setByOffset(rootComp.xYConstructionPlane, adsk.core.ValueInput.createByReal(1))
        plane = planes.add(planeInput)
        
        # Create sketch in base feature
        sketches = rootComp.sketches
        sketch = sketches.addToBaseOrFormFeature(plane, baseFeat, True)

        # Draw a circle.
        circles = sketch.sketchCurves.sketchCircles
        circles.addByCenterRadius(adsk.core.Point3D.create(0, 0, 0), 2)

        # Get the profile defined by the circle.
        prof = sketch.profiles.item(0)

        # Create an extrusion input to be able to define the input needed for an extrusion
        # while specifying the profile and that a new component is to be created
        extrudes = rootComp.features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        # Define that the extent is a distance extent of 5 cm.
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(False, distance)
        extInput.baseFeature = baseFeat

        # Create the extrusion.
        ext = extrudes.add(extInput)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))