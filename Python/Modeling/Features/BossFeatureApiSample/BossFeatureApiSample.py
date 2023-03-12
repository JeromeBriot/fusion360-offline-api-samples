# Author-Autodesk Inc.
# Description-Demonstrates creating a new boss feature

import adsk.core, adsk.fusion, traceback, math

# Get the Application object.
app = adsk.core.Application.get()

def createBoss():
    ui = None
    # Get various top-level Fusion objects.
    ui = app.userInterface
    root = app.activeProduct.rootComponent

    # Select sketch point for boss location        
    selection = ui.selectEntity("Select Sketch Point", "SketchPoints")
    sp = selection.entity
    
    #Create the BossFeatureInput object
    bosses = root.features.bossFeatures
    input = bosses.createInput()
    input.setPositionBySketchPoints(sp)

    draftAng = adsk.core.ValueInput.createByReal(math.pi/90) #draft angle (2 deg)
    holeDraftAng = adsk.core.ValueInput.createByReal(math.pi/180) #draft angle (1 deg)
    
    #set top side inputs
    top = input.createSideInput()
    top.setCountersink(
        adsk.core.ValueInput.createByReal(1.3),
        adsk.core.ValueInput.createByReal(0.5),
        adsk.core.ValueInput.createByReal(0.9),
        adsk.core.ValueInput.createByReal(.3))
    top.draftAngle = draftAng
    top.holeDraftAngle = holeDraftAng
    top.holeMajorDraftAngle = draftAng
    top.rootRadius = adsk.core.ValueInput.createByReal(0.1)
    top.tipRadius = adsk.core.ValueInput.createByReal(0.05)    

    #set alignment properties
    top.alignmentType = adsk.fusion.BossAlignmentTypes.BossAlignStepIn
    top.alignmentDiameter = adsk.core.ValueInput.createByReal(0.9)
    top.alignmentDepth = adsk.core.ValueInput.createByReal(0.1)
    top.alignmentRootRadius = top.tipRadius 
    top.alignmentDraftAngle = draftAng

    #set top rib properties
    top.ribCount = adsk.core.ValueInput.createByReal(4)
    top.ribType = adsk.fusion.BossRibShapeTypes.BossRibShapeChamfer
    top.ribLength = adsk.core.ValueInput.createByReal(1.3)
    top.ribThickness = adsk.core.ValueInput.createByReal(0.2)
    top.ribOffset = adsk.core.ValueInput.createByReal(0.7)
    top.ribCutSize = adsk.core.ValueInput.createByReal(0.5)
    top.ribChamferAngle = adsk.core.ValueInput.createByReal(math.pi/3)
    top.ribOuterDraftAngle = adsk.core.ValueInput.createByReal(math.pi/18)
    top.ribDraftAngle = draftAng
    top.ribTipRadius =  adsk.core.ValueInput.createByReal(0.05)
    top.ribRootRadius = adsk.core.ValueInput.createByReal(0.05)

    #set individual rib extent type for the selected point
    top.setRibExtent(sp,[
        adsk.fusion.BossRibExtentTypes.RibSizeByLength,
        adsk.fusion.BossRibExtentTypes.RibSizeToNext,
        adsk.fusion.BossRibExtentTypes.RibSuppressed,
        adsk.fusion.BossRibExtentTypes.RibSizeByLength
    ])
    
    #set bottom side inputs
    bottom = input.createSideInput()
    bottom.setCounterbore(
        top.alignmentDiameter,
        adsk.core.ValueInput.createByReal(.3),
        adsk.core.ValueInput.createByReal(.5),
        adsk.core.ValueInput.createByReal(.1))

    bottom.tipRadius = top.alignmentRootRadius    
    bottom.holeMajorRootRadius = adsk.core.ValueInput.createByReal(0.05)
    bottom.holeExtentType = adsk.fusion.BossHoleExtentTypes.BossBlindFull
    bottom.holeDepth = adsk.core.ValueInput.createByReal(.3)
    bottom.holeEndRadius = adsk.core.ValueInput.createByReal(.05)
    bottom.draftAngle = draftAng
    bottom.holeDraftAngle = holeDraftAng
    bottom.rootRadius = adsk.core.ValueInput.createByReal(0.1)

    #set bottom rib properties
    bottom.ribCount = adsk.core.ValueInput.createByReal(3)
    bottom.ribType = adsk.fusion.BossRibShapeTypes.BossRibShapeFillet
    bottom.ribOffset = adsk.core.ValueInput.createByReal(0.7)
    bottom.ribLength = adsk.core.ValueInput.createByReal(0.8)
    bottom.ribCutSize = adsk.core.ValueInput.createByReal(0.5)
    bottom.ribThickness = top.ribThickness
    bottom.ribDraftAngle = draftAng
    bottom.ribOuterDraftAngle = top.ribOuterDraftAngle
    bottom.ribTipRadius =  adsk.core.ValueInput.createByReal(0.05)
    bottom.ribRootRadius = adsk.core.ValueInput.createByReal(0.05)

    input.side1 = top
    input.side2 = bottom

    # Create the boss.        
    bosses.add(input)
    
def run(context):
    try:
        createBoss()
    except:
        app.log('AddIn Start Failed:\n{}'.format(traceback.format_exc()))

def stop(context):
    try:
        pass
    except:
        app.log('AddIn Stop Failed: {}'.format(traceback.format_exc()))