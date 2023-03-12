#Author-Autodesk Inc.
#Description-Demo custom graphics examples

import adsk.core, adsk.fusion, adsk.cam, traceback
import math

# Globals
_app = adsk.core.Application.cast(None)
_ui = adsk.core.UserInterface.cast(None)
_des = adsk.fusion.Design.cast(None)
_cgGroups = adsk.fusion.CustomGraphicsGroups.cast(None)
_numTeeth = 5
_handlers = []
appearancesMap = {}
coordNumber = 0
stripNumber = 0
_pointSetImage = './resources/16x16.png'
_thickness = 0.5 * 2.54
_anchorPt = adsk.core.Point3D.cast(None)
#_scaleFactor is used to limit the size of pixel-scaled model however large the actual graphics model.
_scaleFactor = 10

_commandId = 'CustomGraphicsSample_Python'
_colorEffect_solid_id = 'SolidColorEfect'
_colorEffect_basicMaterial_id = 'BasicMaterialColorEffect'
_colorEffect_appearance_id = 'AppearanceColorEffect'
_colorEffect_vertex_id = 'VertexColorEffect'

# Global Command inputs
_customGraphicsObj = adsk.core.DropDownCommandInput.cast(None)
_colorEffects = adsk.core.DropDownCommandInput.cast(None)
_red = adsk.core.IntegerSliderCommandInput.cast(None)
_green = adsk.core.IntegerSliderCommandInput.cast(None)
_blue = adsk.core.IntegerSliderCommandInput.cast(None)
_opacity = adsk.core.SliderCommandInput.cast(None)
_glossiness = adsk.core.SliderCommandInput.cast(None)
#_text = adsk.core.StringValueCommandInput.cast(None)
_selection = adsk.core.SelectionCommandInput.cast(None)
_transform = adsk.core.DistanceValueCommandInput.cast(None)
_materialLibList = adsk.core.DropDownCommandInput.cast(None)
_appearanceList = adsk.core.DropDownCommandInput.cast(None)
_appearanceFilter = adsk.core.StringValueCommandInput.cast(None)
_coordTable = adsk.core.TableCommandInput.cast(None)
_add = adsk.core.BoolValueCommandInput.cast(None)
_addStrip = adsk.core.BoolValueCommandInput.cast(None)
_delete = adsk.core.BoolValueCommandInput.cast(None)
_isLineStrip = adsk.core.BoolValueCommandInput.cast(None)
_lineStylePattern = adsk.core.DropDownCommandInput.cast(None)
_lineStyleWeight = adsk.core.IntegerSliderCommandInput.cast(None)
_lineStyleScale = adsk.core.IntegerSliderCommandInput.cast(None)

_viewPlacementGroup = adsk.core.GroupCommandInput.cast(None)
_viewCorner = adsk.core.ButtonRowCommandInput.cast(None)
_viewScaleGroup = adsk.core.GroupCommandInput.cast(None)
_pixelScale = adsk.core.FloatSliderCommandInput.cast(None)
_billboardingGroup = adsk.core.GroupCommandInput.cast(None)
_billboardingStyle = adsk.core.ButtonRowCommandInput.cast(None)

def run(context):
    try:
        global _app, _ui, _des, _cgGroups
        _app = adsk.core.Application.get()
        _ui  = _app.userInterface        
        doc = _app.activeDocument
        prods = doc.products
        _des = prods.itemByProductType('DesignProductType')
        if not _des:
            raise Exception('Failed to get fusion design.')

        # get the entry for custom graphics
        activeProd = _app.activeProduct
        cam = adsk.cam.CAM.cast(activeProd)
        if cam:              
            _cgGroups = cam.customGraphicsGroups
        else:
            _cgGroups = _des.rootComponent.customGraphicsGroups

        cmdDef = _ui.commandDefinitions.itemById(_commandId)
        if not cmdDef:
            # Create a command definition.
            cmdDef = _ui.commandDefinitions.addButtonDefinition(_commandId, 'CustomGraphicsSample', 'Custom Graphics Sample') 
        
        # Connect to the command created event.
        onCommandCreated = MyCommandCreatedHandler()
        cmdDef.commandCreated.add(onCommandCreated)
        _handlers.append(onCommandCreated)
        
        # Execute the command.
        cmdDef.execute()

        # prevent this module from being terminate when the script returns, because we are waiting for event handlers to fire
        adsk.autoTerminate(False)
    except:
        if _ui:
            _ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
            
class MyCommandDestroyHandler(adsk.core.CommandEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
#            eventArgs = adsk.core.CommandEventArgs.cast(args)

            # when the command is done, terminate the script
            # this will release all globals which will remove all event handlers
            adsk.terminate()
        except:
            if _ui:
                _ui.messageBox('Failed in MyCommandDestroyHandler:\n{}'.format(traceback.format_exc()))     

def addRow(tableInput):
    global coordNumber
    tableChildInputs = tableInput.commandInputs
    xValueInput = tableChildInputs.addValueInput(_coordTable.id + '_x{}'.format(coordNumber), 'Value', 'cm', adsk.core.ValueInput.createByReal(coordNumber))
    yValueInput = tableChildInputs.addValueInput(_coordTable.id + '_y{}'.format(coordNumber), 'Value', 'cm', adsk.core.ValueInput.createByReal(coordNumber))
    zValueInput = tableChildInputs.addValueInput(_coordTable.id + '_z{}'.format(coordNumber), 'Value', 'cm', adsk.core.ValueInput.createByReal(coordNumber))
    
    row = tableInput.rowCount
    tableInput.addCommandInput(xValueInput, row, 0)
    tableInput.addCommandInput(yValueInput, row, 1)
    tableInput.addCommandInput(zValueInput, row, 2)
    
    coordNumber = coordNumber + 1
    
def addLineStrip(tableInput):
    global stripNumber
    tableChildInputs = tableInput.commandInputs
    strInput = tableChildInputs.addStringValueInput(_coordTable.id + '_strip{}'.format(stripNumber), 'Line Strip', '-- Line Strip --')
    strInput.isReadOnly = True
    
    row = tableInput.rowCount
    tableInput.addCommandInput(strInput, row, 0, 0, 2)
    
    stripNumber = stripNumber + 1                
    
def replaceItems(cmdInput, newItems):
    try:
        cmdInput.listItems.clear()
        itemNone = cmdInput.listItems.add('None', True, '')
        itemNone.isSelected = True
        if len(newItems) > 0:
            for item in newItems:
                cmdInput.listItems.add(item, False, '')
            cmdInput.listItems[1].isSelected = True
            cmdInput.listItems[0].deleteMe()   
    except:
        if _ui:
            _ui.messageBox('Failed in replaceItems:\n{}'.format(traceback.format_exc()))        
        
def getAppearancesFromLib(libName, filterExp):
    try:
        global appearancesMap
        appearanceList = None
        if libName in appearancesMap:
            appearanceList = appearancesMap[libName]
        else:
            materialLib = _app.materialLibraries.itemByName(libName)
            appearances = materialLib.appearances
            appearanceNames = []
            for appearance in appearances:
                appearanceNames.append(appearance.name)
            appearancesMap[libName] = appearanceNames
            appearanceList = appearanceNames

        if filterExp and len(filterExp) > 0:
            filteredList = []
            for appearanceName in appearanceList:
                if appearanceName.lower().find(filterExp.lower()) >= 0:
                    filteredList.append(appearanceName)
            return filteredList
        else:
            return appearanceList
    except:
        if _ui:
            _ui.messageBox('Failed in getAppearancesFromLib:\n{}'.format(traceback.format_exc()))        

def hasAppearances(lib):
    if lib and lib.appearances.count > 0:
        return True
    return False
    
def getMaterialLibNames(libFilter):
    materialLibs = _app.materialLibraries
    libNames = []
    for materialLib in materialLibs:
        if (not libFilter) or libFilter(materialLib):
            libNames.append(materialLib.name)
    return libNames   
             
def getAppearance(libName, appearanceName):
    try:
        if not appearanceName or appearanceName == 'None':
            return
        appearance = _des.appearances.itemByName(appearanceName)
        if appearance:
            return appearance
        
        matLib = _app.materialLibraries.itemByName(libName)      
        if matLib:
            appearance = matLib.appearances.itemByName(appearanceName)

        return appearance            
    except:
        if _ui:
            _ui.messageBox('Failed in getAppearance:\n{}'.format(traceback.format_exc()))        
    
# Event handler for the commandCreated event.
class MyCommandCreatedHandler(adsk.core.CommandCreatedEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            eventArgs = adsk.core.CommandCreatedEventArgs.cast(args)
            
            # Verify that a Fusion design is active.
            if not _des:
                _ui.messageBox('A Fusion design must be active when invoking this command.')
                return()

            cmd = eventArgs.command
            cmd.isExecutedWhenPreEmpted = False
            inputs = cmd.commandInputs            
            
            global _customGraphicsObj, _selection, _coordTable, _add, _delete
            global _colorEffects,_glossiness, _opacity, _transform, _isLineStrip, _addStrip, _lineStyleWeight, _lineStyleScale, _lineStylePattern
            global _red,_green,_blue, _appearanceList, _materialLibList, _appearanceFilter
            
            # menu for different kinds of custom graphics
            _customGraphicsObj = inputs.addDropDownCommandInput(_commandId + '_cgObj', 'Custom Graphics Object', adsk.core.DropDownStyles.TextListDropDownStyle)
            _customGraphicsObj.listItems.add('Mesh', True)     
            _customGraphicsObj.listItems.add('Lines', False) 
            _customGraphicsObj.listItems.add('PointSet', False)
            _customGraphicsObj.listItems.add('Curve', False)
            _customGraphicsObj.listItems.add('BRep', False)
            #_customGraphicsObj.listItems.add('Text', False)       
            _customGraphicsObj.listItems.add('Lines - Custom', False)
            _customGraphicsObj.listItems.add('PointSet - Custom', False) 
            
            # coordinates table used by 'Lines - Custom' and 'PointSet - Custom'
            _coordTable = inputs.addTableCommandInput(_commandId + '_table', 'Coordinates Table', 3, '1:1:1')    
            _coordTable.maximumVisibleRows = 10
            addRow(_coordTable)          
            _add = inputs.addBoolValueInput(_coordTable.id + '_add', 'Add', False, '', True)
            _coordTable.addToolbarCommandInput(_add)
            _addStrip = inputs.addBoolValueInput(_coordTable.id + '_addStrip', 'AddStrip', False, '', True)
            _coordTable.addToolbarCommandInput(_addStrip)
            _delete = inputs.addBoolValueInput(_coordTable.id + '_delete', 'Delete', False, '', True)
            _coordTable.addToolbarCommandInput(_delete)
            _coordTable.isVisible = False
            _add.isVisible = False
            _delete.isVisible = False
            _addStrip.isVisible = False
            
            # specific for 'Lines - Custom'
            _isLineStrip = inputs.addBoolValueInput(_commandId + '_isLineStrip', 'Use LineStrip', True, '', True)
            _isLineStrip.isVisible = False
            
            # color effects for custom graphics Mesh/BRep
            _colorEffects = inputs.addDropDownCommandInput(_commandId + '_colorEffects', 'Color Effect', adsk.core.DropDownStyles.TextListDropDownStyle)
            _colorEffects.listItems.add(_colorEffect_solid_id, True) 
            _colorEffects.listItems.add(_colorEffect_basicMaterial_id, False) 
            _colorEffects.listItems.add(_colorEffect_appearance_id, False)     
            _colorEffects.listItems.add(_colorEffect_vertex_id, False)   
            
            # RGB for solid colors
            _red = inputs.addIntegerSliderCommandInput(_commandId + '_red', 'Red', 0, 255, False)
            _red.valueOne = 255
            _green = inputs.addIntegerSliderCommandInput(_commandId + '_green', 'Green', 0, 255, False)
            _green.valueOne = 0
            _blue = inputs.addIntegerSliderCommandInput(_commandId + '_blue', 'Blue', 0, 255, False)   
            _blue.valueOne = 0
            
            # specific for basic material color effect
            _glossiness = inputs.addFloatSliderCommandInput(_commandId + '_glossiness', 'Glossiness', '', 0.0, 128.0, False)
            _glossiness.valueOne = 128.0
            _glossiness.isVisible = False            
            _opacity = inputs.addFloatSliderCommandInput(_commandId + '_opacity', 'Opacity', '', 0.0, 1.0, False)
            _opacity.valueOne = 1.0
            _opacity.isVisible = False

            # for appearance color effect
            _materialLibList = inputs.addDropDownCommandInput(_commandId + '_materialLib', 'Material Library', adsk.core.DropDownStyles.TextListDropDownStyle)
            listItems = _materialLibList.listItems
            materialLibNames = getMaterialLibNames(hasAppearances)
            for materialName in materialLibNames:
                listItems.add(materialName, False, '')
            listItems[0].isSelected = True
            _materialLibList.isVisible = False
            _appearanceList = inputs.addDropDownCommandInput(_commandId + '_appearanceList', 'Appearance', adsk.core.DropDownStyles.TextListDropDownStyle)
            appearances = getAppearancesFromLib(materialLibNames[0], '')
            listItems = _appearanceList.listItems
            for appearanceName in appearances:
                listItems.add(appearanceName, False, '')
            listItems[0].isSelected = True
            _appearanceList.isVisible = False
            _appearanceFilter = inputs.addStringValueInput(_commandId + '_appearanceFilter', 'Filter', '')
            _appearanceFilter.isVisible = False

            # selection input for custom graphics BRep/Curve
            _selection = inputs.addSelectionInput(_commandId + '_sel', 'Selection', '')
            _selection.setSelectionLimits(0, 1)
            _selection.isVisible = False
            _selection.isEnabled = False
            
            # for custom graphics text
            #_text = inputs.addStringValueInput(_commandId + '_text', 'Text', 'This is a text.')
            #_text.isVisible = False
            
            # transform for all custom graphics entity
            _transform = inputs.addDistanceValueCommandInput(_commandId + '_transform', 'Transform', adsk.core.ValueInput.createByReal(0))
            _transform.setManipulator( adsk.core.Point3D.create(0,0,0), adsk.core.Vector3D.create(1,0,0))
            
            # menu for different kinds of line sytles
            _lineStylePattern = inputs.addDropDownCommandInput(_commandId + '_LSPattern', 'Line Style Pattern', adsk.core.DropDownStyles.TextListDropDownStyle)
            _lineStylePattern.listItems.add('Solid Line', True)     
            _lineStylePattern.listItems.add('Center Line', False) 
            _lineStylePattern.listItems.add('Dashed Line', False)
            _lineStylePattern.listItems.add('Dot Line', False)
            _lineStylePattern.listItems.add('Phantom Line', False)
            _lineStylePattern.listItems.add('Tracks Line', False)       
            _lineStylePattern.listItems.add('ZigZag Line', False)
            _lineStylePattern.isVisible = False
            
            # for line sytle weight
            _lineStyleWeight = inputs.addIntegerSliderCommandInput(_commandId + '_LSWeight', 'Line Style Weight', 1, 20, False)
            _lineStyleWeight.valueOne = 1
            _lineStyleWeight.isVisible = False
            
            # for line style scale
            _lineStyleScale = inputs.addIntegerSliderCommandInput(_commandId + '_LSScale', 'Line Style Scale', 1, 100, False)
            _lineStyleScale.valueOne = 10
            _lineStyleScale.isVisible = False
            
            global _viewPlacementGroup, _viewCorner, _viewScaleGroup, _pixelScale, _billboardingGroup, _billboardingStyle            
            # for view placement attribute
            _viewPlacementGroup = inputs.addGroupCommandInput(_commandId + '_VPGroup', 'View Placement')
            _viewPlacementGroup.isEnabledCheckBoxDisplayed = True
            _viewPlacementGroup.isEnabledCheckBoxChecked = False
            _viewCorner = _viewPlacementGroup.children.addButtonRowCommandInput(_commandId + '_viewCorner', 'corner', False)
            _viewCorner.listItems.add('Upper Left', False, './resources/upperLeft')
            _viewCorner.listItems.add('Upper Right', False, './resources/upperRight')
            _viewCorner.listItems.add('Lower Left', False, './resources/lowerLeft')
            _viewCorner.listItems.add('Lower Right', False, './resources/lowerRight')     
            
            # for view scale attribute
            _viewScaleGroup = inputs.addGroupCommandInput(_commandId + '_VSGroup', 'View Scale')
            _viewScaleGroup.isEnabledCheckBoxDisplayed = True
            _viewScaleGroup.isEnabledCheckBoxChecked = False
            _pixelScale = _viewScaleGroup.children.addFloatSliderCommandInput(_commandId + '_pixelScale', 'pixel scale', '', 0.5, 5, False)
            _pixelScale.valueOne = 1
            _pixelScale.setText('Smaller', 'Larger')
            
            # for billboarding attribute
            _billboardingGroup = inputs.addGroupCommandInput(_commandId + '_BBGroup', 'Billboarding')
            _billboardingGroup.isEnabledCheckBoxDisplayed = True
            _billboardingGroup.isEnabledCheckBoxChecked = False        
            _billboardingStyle = _billboardingGroup.children.addButtonRowCommandInput(_commandId + '_billboardingStyle', 'style', False)
            _billboardingStyle.listItems.add('Screen', False, './resources/One')
            _billboardingStyle.listItems.add('Axis', False, './resources/Two')
            _billboardingStyle.listItems.add('Right Reading', False, './resources/Three')          
            
            # Connect to the command related events.
            onExecute = MyCommandExecuteHandler()
            cmd.execute.add(onExecute)
            _handlers.append(onExecute)        
            
            onExecutePreview = MyCommandExecuteHandler()
            cmd.executePreview.add(onExecutePreview)
            _handlers.append(onExecutePreview)  
            
            onInputChanged = MyCommandInputChangedHandler()
            cmd.inputChanged.add(onInputChanged)
            _handlers.append(onInputChanged)     

            onDestroy = MyCommandDestroyHandler()
            cmd.destroy.add(onDestroy)
            _handlers.append(onDestroy)
        except:
            if _ui:
                _ui.messageBox('Failed in MyCommandCreatedHandler:\n{}'.format(traceback.format_exc()))

def applyColorEffect(cgEnt):
    try:
        colorEffect = None
        if _colorEffects.selectedItem.name == _colorEffect_solid_id:
            colorEffect = adsk.fusion.CustomGraphicsSolidColorEffect.create(adsk.core.Color.create(int(_red.valueOne),int(_green.valueOne),int(_blue.valueOne),255))
        elif _colorEffects.selectedItem.name == _colorEffect_basicMaterial_id:
            diffuseColor = adsk.core.Color.create(0,255,0,255)                      
            ambientColor = adsk.core.Color.create(255,0,0,255)            
            specularColor = adsk.core.Color.create(0,0,255,255)  
            emissiveColor = adsk.core.Color.create(0,0,0,255) 
            colorEffect = adsk.fusion.CustomGraphicsBasicMaterialColorEffect.create(diffuseColor, ambientColor, specularColor, emissiveColor, float(_glossiness.valueOne), float(_opacity.valueOne))
        elif _colorEffects.selectedItem.name == _colorEffect_appearance_id:
            appearance = getAppearance(_materialLibList.selectedItem.name, _appearanceList.selectedItem.name)
            if appearance:
                if not _des.appearances.itemByName(appearance.name):
                    appearance = _des.appearances.addByCopy(appearance, appearance.name)
                colorEffect = adsk.fusion.CustomGraphicsAppearanceColorEffect.create(appearance)
        elif _colorEffects.selectedItem.name == _colorEffect_vertex_id:
            colorEffect = adsk.fusion.CustomGraphicsVertexColorEffect.create()
        if colorEffect:
            cgEnt.color = colorEffect
    except:
        if _ui:
            _ui.messageBox('Failed in applyColorEffect:\n{}'.format(traceback.format_exc()))

def getCoordinatesFromTable(tableInput):
    try:
        vecCoord = []
        vecStripLen = []
        stripLen = 0
        if _coordTable:
            for i in range(0, _coordTable.rowCount):
                xValueInput = adsk.core.ValueCommandInput.cast(_coordTable.getInputAtPosition(i,0))
                if xValueInput:
                    stripLen = stripLen + 1
                    yValueInput = adsk.core.ValueCommandInput.cast(_coordTable.getInputAtPosition(i,1))
                    zValueInput = adsk.core.ValueCommandInput.cast(_coordTable.getInputAtPosition(i,2))
                    vecCoord.extend([xValueInput.value, yValueInput.value, zValueInput.value])    
                else:
                    vecStripLen.append(stripLen)
                    stripLen = 0
            vecStripLen.append(stripLen)
        return vecCoord, vecStripLen
    except:
        if _ui:
            _ui.messageBox('Failed in getCoordinatesFromTable:\n{}'.format(traceback.format_exc()))    
    
# Event handler for the execute event.
class MyCommandExecuteHandler(adsk.core.CommandEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            # get selection entity first since it's fragile and any creation/edit operations will clear the selection.
            selEntity = None
            if _selection.selectionCount > 0:
                selEntity = _selection.selection(0).entity
                                    
            if _customGraphicsObj:
                cgGroup = adsk.fusion.CustomGraphicsGroup.cast(_cgGroups.add())     
                
                global _anchorPt, _scaleFactor
                if not _anchorPt:
                    _anchorPt = adsk.core.Point3D.create(0,0,0)
                cgEnt = None
                if _customGraphicsObj.selectedItem.name == 'Mesh':
                    cgEnt = drawMesh(cgGroup) 
                    _anchorPt.setWithArray([0,0,_thickness/2])
                elif _customGraphicsObj.selectedItem.name == 'Lines':
                    cgEnt = drawLines(cgGroup)                
                    _anchorPt.setWithArray([0,0,_thickness/2])
                elif _customGraphicsObj.selectedItem.name == 'PointSet':
                    cgEnt = drawPointSet(cgGroup)
                elif _customGraphicsObj.selectedItem.name == 'BRep':                    
                    if selEntity:
                        body = adsk.fusion.BRepBody.cast(selEntity)
                        cgEnt = cgGroup.addBRepBody(body)
                elif _customGraphicsObj.selectedItem.name == 'Curve':
                    if selEntity:
                        skCurve = adsk.fusion.SketchCurve.cast(selEntity)
                        sk = skCurve.parentSketch
                        curve = skCurve.geometry
                        curve.transformBy(sk.transform)
                        cgEnt = cgGroup.addCurve(curve)
                        cgEnt.weight = float(_lineStyleWeight.valueOne)                        
                #elif _customGraphicsObj.selectedItem.name == 'Text':
                #    if _text.value:
                #        cgEnt = cgGroup.addText(_text.value, 'None', 10, adsk.core.Point3D.create(0,0,0))
                elif _customGraphicsObj.selectedItem.name == 'PointSet - Custom':
                    vecCoords, vecStripLen = getCoordinatesFromTable(_coordTable)
                    coords = adsk.fusion.CustomGraphicsCoordinates.create(vecCoords)
                    cgEnt = cgGroup.addPointSet(coords, [], adsk.fusion.CustomGraphicsPointTypes.UserDefinedCustomGraphicsPointType, _pointSetImage)
                elif _customGraphicsObj.selectedItem.name == 'Lines - Custom':
                    vecCoords, vecStripLength = getCoordinatesFromTable(_coordTable)
                    coords = adsk.fusion.CustomGraphicsCoordinates.create(vecCoords)
                    isLineStrip = _isLineStrip.value
                    if coords.coordinateCount < 1:
                        return
                    cgEnt = cgGroup.addLines(coords, [], isLineStrip, vecStripLength)
                
                # add attributes to the custom graphics entity
                if cgEnt:
                    # transform
                    transMat = adsk.core.Matrix3D.create()
                    origin = adsk.core.Point3D.create(float(_transform.value),0,0)
                    transMat.setWithCoordinateSystem(origin, adsk.core.Vector3D.create(1,0,0), adsk.core.Vector3D.create(0,1,0), adsk.core.Vector3D.create(0,0,1))
                    cgEnt.transform = transMat   
                    # color effect
                    if not adsk.fusion.CustomGraphicsPointSet.cast(cgEnt):
                        applyColorEffect(cgEnt)                    
                    # calculate _scaleFactor and _anchorPt for viewPlacement, viewScale and billboarding attributes based on the bounding box of custom graphics entity
                    maxPt = cgEnt.boundingBox.maxPoint
                    minPt = cgEnt.boundingBox.minPoint       
                    _scaleFactor = 100 / minPt.distanceTo(maxPt)
                    _anchorPt.setWithArray([(minPt.x + maxPt.x) / 2, (minPt.y + maxPt.y) / 2, (minPt.z + maxPt.z) / 2])
                    # view placement
                    if _viewPlacementGroup and _viewPlacementGroup.isVisible and _viewPlacementGroup.isEnabledCheckBoxChecked and _viewCorner and _viewCorner.selectedItem:                       
                        viewPt = adsk.core.Point2D.create(100,100)
                        # upper left by default
                        corner = adsk.fusion.ViewCorners.upperLeftViewCorner                          
                        if _viewCorner.selectedItem.name == 'Upper Right':
                            corner = adsk.fusion.ViewCorners.upperRightViewCorner
                        elif _viewCorner.selectedItem.name == 'Lower Left':
                            corner = adsk.fusion.ViewCorners.lowerLeftViewCorner
                        elif _viewCorner.selectedItem.name == 'Lower Right':
                            corner = adsk.fusion.ViewCorners.lowerRightViewCorner
                        attr = adsk.fusion.CustomGraphicsViewPlacement.create(_anchorPt, corner, viewPt)
                        cgEnt.viewPlacement = attr
                    # view scale
                    if _viewScaleGroup and _viewScaleGroup.isVisible and _viewScaleGroup.isEnabledCheckBoxChecked and _pixelScale:
                        attr = adsk.fusion.CustomGraphicsViewScale.create(_scaleFactor * _pixelScale.valueOne, _anchorPt)
                        cgEnt.viewScale = attr
                    # billboarding
                    if _billboardingGroup and _billboardingGroup.isVisible and _billboardingGroup.isEnabledCheckBoxChecked and _billboardingStyle and _billboardingStyle.selectedItem:
                        # screen style by default
                        bbStyle = adsk.fusion.CustomGraphicsBillBoardStyles.ScreenBillBoardStyle
                        if _billboardingStyle.selectedItem.name == 'Axis':
                            bbStyle = adsk.fusion.CustomGraphicsBillBoardStyles.AxialBillBoardStyle
                        elif _billboardingStyle.selectedItem.name == 'Right Reading':
                            bbStyle = adsk.fusion.CustomGraphicsBillBoardStyles.RightReadingBillBoardStyle
                        attr = adsk.fusion.CustomGraphicsBillBoard.create(_anchorPt)
                        attr.axis = adsk.core.Vector3D.create(0,1,0)
                        attr.billBoardStyle = bbStyle
                        cgEnt.billBoarding = attr
                            
        except:
            if _ui:
                _ui.messageBox('Failed in MyCommandExecuteHandler:\n{}'.format(traceback.format_exc()))

def changeCGObjVisibility(strObjName):
    try:       
        _colorEffects.listItems.clear()
        _colorEffects.listItems.add(_colorEffect_solid_id, True) 
        _colorEffects.listItems.add(_colorEffect_basicMaterial_id, False) 
        _colorEffects.listItems.add(_colorEffect_appearance_id, False)   
        _colorEffects.isVisible = False
        _selection.clearSelection()
        _selection.clearSelectionFilter()
        _selection.setSelectionLimits(0, 0)
        _selection.isVisible = False
        _selection.isEnabled = False        
        #_text.isVisible = False
        _coordTable.isVisible = False
        _isLineStrip.isVisible = False
        _lineStylePattern.isVisible = False
        _lineStyleWeight.isVisible = False
        _lineStyleScale.isVisible = False
        changeColorEffectVisibility(None)
        
        _viewPlacementGroup.isVisible = False
        _viewScaleGroup.isVisible = False
        _billboardingGroup.isVisible = False
        
        if strObjName == 'Mesh':
            _colorEffects.isVisible = True
            _colorEffects.listItems.add(_colorEffect_vertex_id, False) 
            changeColorEffectVisibility(_colorEffect_solid_id)
            _viewPlacementGroup.isVisible = True
            _viewScaleGroup.isVisible = True
            _billboardingGroup.isVisible = True
        elif strObjName == 'Lines':               
            changeColorEffectVisibility(_colorEffect_solid_id)
            _lineStylePattern.isVisible = True
            _lineStyleWeight.isVisible = True
            if _lineStylePattern.selectedItem.name != 'Solid Line':
                _lineStyleScale.isVisible = True
            _viewPlacementGroup.isVisible = True
            _viewScaleGroup.isVisible = True
            _billboardingGroup.isVisible = True
        elif strObjName == 'Curve':
            _selection.isVisible = True
            _selection.isEnabled = True
            _selection.tooltip = 'select a curve'
            _selection.commandPrompt = 'select a curve'
            _selection.addSelectionFilter('SketchCurves')
            _selection.setSelectionLimits(1,1)
            changeColorEffectVisibility(_colorEffect_solid_id)
            _lineStyleWeight.isVisible = True
            _viewPlacementGroup.isVisible = True
            _viewScaleGroup.isVisible = True
            _billboardingGroup.isVisible = True
        elif strObjName == 'BRep':
            _selection.isVisible = True
            _selection.isEnabled = True
            _selection.tooltip = 'select a body'
            _selection.commandPrompt = 'select a body'
            _selection.addSelectionFilter('Bodies')  
            _selection.setSelectionLimits(1,1)
            _colorEffects.isVisible = True
            changeColorEffectVisibility(_colorEffect_solid_id)
            _viewPlacementGroup.isVisible = True
            _viewScaleGroup.isVisible = True
            _billboardingGroup.isVisible = True
        #elif strObjName == 'Text':
        #    _text.isVisible = True
        #    changeColorEffectVisibility(_colorEffect_solid_id)   
        elif strObjName == 'PointSet - Custom':   
            _coordTable.isVisible = True 
            _addStrip.isEnabled = False
        elif strObjName == 'Lines - Custom': 
            _coordTable.isVisible = True
            _isLineStrip.isVisible = True
            _addStrip.isEnabled = True
            changeColorEffectVisibility(_colorEffect_solid_id)
            _lineStylePattern.isVisible = True
            _lineStyleWeight.isVisible = True
            if _lineStylePattern.selectedItem.name != 'Solid Line':
                _lineStyleScale.isVisible = True
    except:
        if _ui:
            _ui.messageBox('Failed in changeCGObjVisibility:\n{}'.format(traceback.format_exc()))
                 
def changeColorEffectVisibility(strColorEffectName):
    try:
        _red.isVisible = False
        _green.isVisible = False
        _blue.isVisible = False
        _opacity.isVisible = False
        _glossiness.isVisible = False
        _appearanceList.isVisible = False
        _materialLibList.isVisible = False
        _appearanceFilter.isVisible = False
        
        if strColorEffectName == _colorEffect_solid_id: 
            _red.isVisible = True
            _green.isVisible = True
            _blue.isVisible = True           
        elif strColorEffectName == _colorEffect_basicMaterial_id:        
            _opacity.isVisible = True
            _glossiness.isVisible = True
        elif strColorEffectName == _colorEffect_appearance_id:
            _appearanceList.isVisible = True
            _materialLibList.isVisible = True
            _appearanceFilter.isVisible = True
            
    except:
        if _ui:
            _ui.messageBox('Failed in changeColorEffectVisibility:\n{}'.format(traceback.format_exc()))

def changeLineStyleInputsVisibility(patternName):
    try:
        if patternName == 'Solid Line':
            _lineStyleScale.isVisible = False
        else:
            _lineStyleScale.isVisible = True
        
    except:
        if _ui:
            _ui.messageBox('Failed in changeLineStyleInputsVisibility:\n{}'.format(traceback.format_exc()))
    
# Event handler for the inputChanged event.
class MyCommandInputChangedHandler(adsk.core.InputChangedEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            eventArgs = adsk.core.InputChangedEventArgs.cast(args)
            changedInput = eventArgs.input
            if changedInput.id == _commandId + '_cgObj':
                changeCGObjVisibility(_customGraphicsObj.selectedItem.name)
            elif changedInput.id == _commandId + '_colorEffects':
                changeColorEffectVisibility(_colorEffects.selectedItem.name)  
            elif changedInput.id == _commandId + '_appearanceFilter' or changedInput.id == _commandId + '_materialLib':
                appearances = getAppearancesFromLib(_materialLibList.selectedItem.name, _appearanceFilter.value)
                replaceItems(_appearanceList, appearances)
            elif changedInput.id == _coordTable.id + '_add':
                addRow(_coordTable)
            elif changedInput.id == _coordTable.id + '_addStrip':
                addLineStrip(_coordTable)
            elif changedInput.id == _coordTable.id + '_delete':
               if _coordTable.selectedRow == -1:
                   _ui.messageBox('Select one row to delete')
               else:
                   _coordTable.deleteRow(_coordTable.selectedRow)
            elif changedInput.id == _commandId + '_LSPattern':
                changeLineStyleInputsVisibility(_lineStylePattern.selectedItem.name)
        except:
            if _ui:
                _ui.messageBox('Failed in MyCommandInputChangedHandler:\n{}'.format(traceback.format_exc()))
def rotate2D(rad, vec):
    try:
        x = vec[0]
        y = vec[1]
        return (x*math.cos(rad)-y*math.sin(rad), x*math.sin(rad)+y*math.cos(rad))
    except:
        if _ui:
            _ui.messageBox('Failed in rotate2D:\n{}'.format(traceback.format_exc()))    

def calculateCoordinates(numTeeth):
    try:         
        # holeDia < rootDia < pitchDia < outsideDia     
        holeDia = 0.5 * 2.54
        diametralPitch = 2 / 2.54
        pitchDia = numTeeth / diametralPitch
        if (diametralPitch < (20 *(math.pi/180))-0.000001):
            dedendum = 1.157 / diametralPitch
        else:
            circularPitch = math.pi / diametralPitch
            if circularPitch >= 20:
                dedendum = 1.25 / diametralPitch
            else:
                dedendum = (1.2 / diametralPitch) + (.002 * 2.54)                
        rootDia = pitchDia - (2 * dedendum)        
        outsideDia = (numTeeth + 2) / diametralPitch
        
        rPts0 = []    # 2 * numTeeth for root
        hPts0 = []    # 2 * numTeeth for hole
        pPts0 = []    # 2 * numTeeth for pitch
        oPts0 = []    # 1 * numTeeth for outside
        rPts1 = []    # 2 * numTeeth for root with thickness
        hPts1 = []    # 2 * numTeeth for hole with thickness
        pPts1 = []    # 2 * numTeeth for pitch with thickness
        oPts1 = []    # 1 * numTeeth for outside with thickness
        
        vecRootRadi = [rootDia/2.0, 0]
        vecHoleRadi = [holeDia/2.0, 0]
        vecPitchRadi = [pitchDia/2.0, 0]
        vecOutRadi = [outsideDia/2.0, 0]
        unitRadian = math.pi / numTeeth
        
        vecCoords = []    
        vecColors = []
        for i in range(0, 2 * numTeeth):            
            x, y = rotate2D(unitRadian * (i - 0.5), vecRootRadi)
            rPts0.append(int(len(vecCoords) / 3))
            rPts1.append(int(len(vecCoords) / 3) + 1)
            vecCoords.extend([x, y, 0, x, y, _thickness])
            vecColors.extend([255,0,255,128, 255,0,255,128])
    
        for i in range(0, 2 * numTeeth):            
            x, y = rotate2D(unitRadian * (i - 0.5), vecHoleRadi)
            hPts0.append(int(len(vecCoords) / 3))
            hPts1.append(int(len(vecCoords) / 3) + 1)   
            vecCoords.extend([x, y, 0, x, y, _thickness])
            vecColors.extend([255,0,0,128,  255,0,0,128])
            
        for i in range(0, 2 * numTeeth):            
            x, y = rotate2D(unitRadian * (i - 0.5), vecPitchRadi)
            pPts0.append(int(len(vecCoords) / 3))
            pPts1.append(int(len(vecCoords) / 3) + 1)   
            vecCoords.extend([x, y, 0, x, y, _thickness])
            vecColors.extend([0,0,255,128, 0,0,255,128])
    
        for i in range(0, numTeeth):            
            x, y = rotate2D(unitRadian * i * 2 , vecOutRadi)
            oPts0.append(int(len(vecCoords) / 3))
            oPts1.append(int(len(vecCoords) / 3) + 1)   
            vecCoords.extend([x, y, 0, x, y, _thickness])
            vecColors.extend([0,255,255,128, 0,255,255,128])
    
        return (rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1, vecCoords, vecColors)
    except:
        if _ui:
            _ui.messageBox('Failed in calculateCoordinates:\n{}'.format(traceback.format_exc()))

def calculateStripLen(numTeeth):
    try:
        vecStripLen = []
        for i in range(0, numTeeth):
            vecStripLen.append(6)
        for i in range(0, 2 * numTeeth):
            vecStripLen.append(21)  
        for i in range(0, numTeeth):
            vecStripLen.append(24)
        for i in range(0, 2 * numTeeth):
            vecStripLen.append(6)            
        return vecStripLen
    except Exception as error:
        _ui.messageBox("calculateTriangles Failed : " + str(error)) 
            
def calculateTriangles(numTeeth, rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1):
    try: 
        vertexIndexList = []    
        # triangles between teeth
        for i in range(0, numTeeth):
            idx0 = (2*i+1) % (2*numTeeth)
            idx1 = (2*i+2) % (2*numTeeth)
            rPtA0 = rPts0[idx0]
            rPtB0 = rPts0[idx1]         
            rPtA1 = rPts1[idx0]
            rPtB1 = rPts1[idx1]  
            vertexIndexList.extend([rPtA0,rPtB0,rPtB1, rPtB1,rPtA1,rPtA0])
        # triangles on surface0
        for i in range(0, numTeeth):
            rPtA = rPts0[i*2]
            rPtB = rPts0[i*2 + 1]
            rPtC = rPts0[(i*2 + 2)%(2*numTeeth)]        
            hPtA = hPts0[i*2]
            hPtB = hPts0[i*2 + 1]  
            hPtC = hPts0[(i*2 + 2)%(2*numTeeth)]        
            pPtA = pPts0[i*2]
            pPtB = pPts0[i*2 + 1]               
            oPt = oPts0[i]                                  
            vertexIndexList.extend([hPtB,hPtC,rPtC, rPtC,rPtB,hPtB])
            vertexIndexList.extend([rPtA,rPtB,pPtB, pPtB,pPtA,rPtA])  
            vertexIndexList.extend([hPtA,hPtB,rPtB, rPtB,rPtA,hPtA])           
            vertexIndexList.extend([pPtA,pPtB,oPt])              
        # triangles on surface1
        for i in range(0, numTeeth):
            rPtA = rPts1[i*2]
            rPtB = rPts1[i*2 + 1]
            rPtC = rPts1[(i*2 + 2)%(2*numTeeth)]
            hPtA = hPts1[i*2]
            hPtB = hPts1[i*2 + 1]  
            hPtC = hPts1[(i*2 + 2)%(2*numTeeth)]
            pPtA = pPts1[i*2]
            pPtB = pPts1[i*2 + 1]         
            oPt = oPts1[i]             
            vertexIndexList.extend([hPtC,hPtB,rPtB, rPtB,rPtC,hPtC])
            vertexIndexList.extend([rPtB,rPtA,pPtA, pPtA,pPtB,rPtB]) 
            vertexIndexList.extend([hPtB,hPtA,rPtA, rPtA,rPtB,hPtB])          
            vertexIndexList.extend([pPtB,pPtA,oPt])         
        # triangles on teeth
        for i in range(0, numTeeth):
            rPtA0 = rPts0[i*2]
            rPtB0 = rPts0[i*2 + 1]  
            pPtA0 = pPts0[i*2]
            pPtB0 = pPts0[i*2 + 1]    
            rPtA1 = rPts1[i*2]
            rPtB1 = rPts1[i*2 + 1]  
            pPtA1 = pPts1[i*2]
            pPtB1 = pPts1[i*2 + 1] 
            oPt0 = oPts0[i]
            oPt1 = oPts1[i]        
            # triangles on one tooth
            vertexIndexList.extend([rPtA1, rPtA0, pPtA0, pPtA0, pPtA1, rPtA1])
            vertexIndexList.extend([pPtA1, pPtA0, oPt0, oPt0, oPt1, pPtA1])
            vertexIndexList.extend([rPtB0, rPtB1, pPtB1, pPtB1, pPtB0, rPtB0])    
            vertexIndexList.extend([pPtB0, pPtB1, oPt1, oPt1, oPt0, pPtB0])         
        # triangles on inner face
        for i in range(0, 2*numTeeth):
            hPtA0 = hPts0[i]
            hPtB0 = hPts0[(i + 1)%(2*numTeeth)] 
            hPtA1 = hPts1[i]
            hPtB1 = hPts1[(i + 1)%(2*numTeeth)] 
            vertexIndexList.extend([hPtA1,hPtB1,hPtB0, hPtB0,hPtA0,hPtA1])                
            
        return vertexIndexList
    except Exception as error:
        _ui.messageBox("calculateTriangles Failed : " + str(error)) 
        return None
# Builds a custom graphics mesh.
def drawMesh(cgGroup):
    try:        
        # Calculate mesh coordinates
        rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1, vecCoords, vecColors = calculateCoordinates(_numTeeth)        
        coordinates = adsk.fusion.CustomGraphicsCoordinates.create(vecCoords)
        coordinates.colors = vecColors

        # Calculate mesh triangles
        vertexIndexList = calculateTriangles(_numTeeth, rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1)        
        # Add Custom Graphics mesh
        normalVectors = []
        normalIndexList = []
        cgMesh = cgGroup.addMesh(coordinates, vertexIndexList, normalVectors, normalIndexList)        
        return cgMesh
        
    except Exception as error:
        _ui.messageBox("drawMesh Failed : " + str(error)) 
        return None
        
def drawLines(cgGroup):
    try:
        rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1, vecCoords, vecColors = calculateCoordinates(_numTeeth)        
        coordinates = adsk.fusion.CustomGraphicsCoordinates.create(vecCoords)       
        vertexIndexList = calculateTriangles(_numTeeth, rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1)        
            
        stripLen = calculateStripLen(_numTeeth)
        cgLines = cgGroup.addLines(coordinates, vertexIndexList, True, stripLen)
        
        if _lineStylePattern.selectedItem.name == 'Solid Line':
            cgLines.lineStylePattern = adsk.fusion.LineStylePatterns.continuousLineStylePattern
        elif _lineStylePattern.selectedItem.name == 'Center Line':
            cgLines.lineStylePattern = adsk.fusion.LineStylePatterns.centerLineStylePattern
        elif _lineStylePattern.selectedItem.name == 'Dashed Line':
            cgLines.lineStylePattern = adsk.fusion.LineStylePatterns.dashedLineStylePattern
        elif _lineStylePattern.selectedItem.name == 'Dot Line':
            cgLines.lineStylePattern = adsk.fusion.LineStylePatterns.dotLineStylePattern
        elif _lineStylePattern.selectedItem.name == 'Dashed Line':
            cgLines.lineStylePattern = adsk.fusion.LineStylePatterns.dashedLineStylePattern
        elif _lineStylePattern.selectedItem.name == 'Phantom Line':
            cgLines.lineStylePattern = adsk.fusion.LineStylePatterns.phantomLineStylePattern
        elif _lineStylePattern.selectedItem.name == 'Tracks Line':
            cgLines.lineStylePattern = adsk.fusion.LineStylePatterns.tracksLineStylePattern
        elif _lineStylePattern.selectedItem.name == 'ZigZag Line':
            cgLines.lineStylePattern = adsk.fusion.LineStylePatterns.zigzagLineStylePattern
            
        cgLines.weight = float(_lineStyleWeight.valueOne)
        cgLines.lineStyleScale = float(_lineStyleScale.valueOne)
        
        return cgLines
    except Exception as error:
        _ui.messageBox("drawLines Failed : " + str(error)) 
        return None   
        
def drawPointSet(cgGroup):
    try:
        rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1, vecCoords, vecColors = calculateCoordinates(_numTeeth)        
        coordinates = adsk.fusion.CustomGraphicsCoordinates.create(vecCoords)       
               
        cgPoints = cgGroup.addPointSet(coordinates, [], adsk.fusion.CustomGraphicsPointTypes.UserDefinedCustomGraphicsPointType, _pointSetImage)

        return cgPoints
    except Exception as error:
        _ui.messageBox("drawPointSet Failed : " + str(error)) 
        return None