# Author-Autodesk Inc.
# Description-Demonstrates analyzing the interference between components

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface

        # Create a new document.
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
 
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        
        # Set the design type to DirectDesignType (for non-parametric modelling)        
        design.designType = adsk.fusion.DesignTypes.DirectDesignType

        # Get the root component of the active design
        rootComp = design.rootComponent
        
        # Create the first component - containing a box
        occurrences = rootComp.occurrences
        matrix = adsk.core.Matrix3D.create()
        firstComponentOccurrence = occurrences.addNewComponent(matrix)                
        
        # Create sketch
        sketches = firstComponentOccurrence.component.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)        
        
        # Create a rectangle
        sketchLines = sketch.sketchCurves.sketchLines
        startPoint = adsk.core.Point3D.create(0, 0, 0)
        endPoint = adsk.core.Point3D.create(5, 5, 0)
        sketchLines.addTwoPointRectangle(startPoint, endPoint)
        
        # Get the profile defined by the rectangle
        prof = sketch.profiles.item(0)
        
        # Create an extrusion input for the profile.
        features = firstComponentOccurrence.component.features
        extrudes = features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Define that the extent of the extrusion is a distance extent of 5 cm.
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(False, distance)
        
        # Create the extrusion.
        ext = extrudes.add(extInput)

        # Create the second component - containing a box that overlaps the box in the first component
        secondComponentOccurrence = occurrences.addNewComponent(matrix)                
        
        # Create sketch
        sketches = secondComponentOccurrence.component.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)        
        
        # Create a rectangle
        sketchLines = sketch.sketchCurves.sketchLines
        startPoint = adsk.core.Point3D.create(3, 3, 0)
        endPoint = adsk.core.Point3D.create(8, 8, 0)
        sketchLines.addTwoPointRectangle(startPoint, endPoint)
        # Get the profile defined by the rectangle
        prof = sketch.profiles.item(0)
        
        # Create an extrusion input for the profile.
        features = secondComponentOccurrence.component.features
        extrudes = features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Define that the extent of the extrusion is a distance extent of 5 cm.
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(False, distance)
        
        # Create the extrusion.
        ext = extrudes.add(extInput)
        
        # Create the third component - containing a cylinder that overlaps the box in the second component
        thirdComponentOccurrence = occurrences.addNewComponent(matrix)                
        
        # Create sketch
        sketches = thirdComponentOccurrence.component.sketches
        sketch = sketches.add(rootComp.xZConstructionPlane)        
        
        # Create a circle
        sketchCircles = sketch.sketchCurves.sketchCircles
        centerPoint = adsk.core.Point3D.create(8, 8, 0)
        sketchCircles.addByCenterRadius(centerPoint, 2)
        
        # Get the profile defined by the circle
        prof = sketch.profiles.item(0)
        
        # Create an extrusion input for the profile.
        features = thirdComponentOccurrence.component.features
        extrudes = features.extrudeFeatures
        extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        
        # Define that the extent of the extrusion is a distance extent of 5 cm.
        distance = adsk.core.ValueInput.createByReal(5)
        extInput.setDistanceExtent(False, distance)
        
        # Create the extrusion.
        ext = extrudes.add(extInput)    
        
        # Create a collection of the components to check for interference
        inputOccurrences = adsk.core.ObjectCollection.create()
        inputOccurrences.add(firstComponentOccurrence)
        inputOccurrences.add(secondComponentOccurrence)
        inputOccurrences.add(thirdComponentOccurrence)
        
        # Create the interferenceInput object and run the analysis.
        interferenceInput = design.createInterferenceInput(inputOccurrences)
        interferenceInput.areCoincidentFacesIncluded = False
        results = design.analyzeInterference(interferenceInput)    
        
        # Create bodies for every intersection.  This is not supported in Parametric designs.
        interferenceBodies = results.createBodies(True)
        
        # Activate the Intersections component created by Fusion that stores the interference bodies
        resultsOccurrence = occurrences.item(occurrences.count-1)
        resultsOccurrence.activate()
        
        # Fit the view        
        viewport = app.activeViewport
        viewport.fit()
        
        # Report the results
        bod = 0        
        for res in results:
            comp1Name = res.entityOne.parentComponent.name
            comp2Name = res.entityTwo.parentComponent.name
            bodyVolume = str(round(res.interferenceBody.volume, 2))
            interferenceBodies.item(bod).name = 'Interference between ' + comp1Name + ' & ' + comp2Name
            ui.messageBox('There is interference between ' + comp1Name + ' and ' + comp2Name + ' with a volume of ' + bodyVolume + ' cubic centimeters')
            bod += 1
        
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))