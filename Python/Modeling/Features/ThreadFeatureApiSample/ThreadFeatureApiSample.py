# Author-Autodesk Inc.
# Description-Demonstrates creating a new thread feature

import adsk.core, adsk.fusion, traceback


def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
  
        # get the design
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        
        # get the root component of the active design.
        rootComp = design.rootComponent
        
        # create a new sketch on the xy plane.
        sketch = rootComp.sketches.add(rootComp.xYConstructionPlane)
        
        # create a sketch circle
        sketchCircles = sketch.sketchCurves.sketchCircles
        sketchCircle = sketchCircles.addByCenterRadius(adsk.core.Point3D.create(0, 0, 0), 3)
        
        # get the profile defined by the circle.
        prof = sketch.profiles.item(0)
        
        # create extrude input
        extrudes = rootComp.features.extrudeFeatures
        extrudeInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # define that the extent is a distance extent of 7 cm
        extrudeInput.setDistanceExtent(False, adsk.core.ValueInput.createByReal(7))
        
        # extrude the circle to create a cylinder
        extrude = extrudes.add(extrudeInput)
        
        # define all of the thread information.
        threadFeatures = rootComp.features.threadFeatures
        
        # query the thread table to get the thread information
        threadDataQuery = threadFeatures.threadDataQuery
        threadTypes = threadDataQuery.allThreadTypes
        threadType = threadTypes[0]
        
        allsizes = threadDataQuery.allSizes(threadType)
        threadSize = allsizes[0]
        
        allDesignations = threadDataQuery.allDesignations(threadType, threadSize)
        threadDesignation = allDesignations[0]
        
        allClasses = threadDataQuery.allClasses(False, threadType, threadDesignation)
        threadClass = allClasses[0]
        
        # create the threadInfo according to the query result
        threadInfo = threadFeatures.createThreadInfo(False, threadType, threadDesignation, threadClass)
        
        # get the face the thread will be applied to
        sideface = extrude.sideFaces.item(0)
        faces = adsk.core.ObjectCollection.create()
        faces.add(sideface)
        
        # define the thread input with the lenght 3.5 cm
        threadInput = threadFeatures.createInput(faces, threadInfo)
        threadInput.isFullLength = False
        threadInput.threadLength = adsk.core.ValueInput.createByReal(3.5)
        
        # create the final thread
        thread = threadFeatures.add(threadInput)
        
        
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))