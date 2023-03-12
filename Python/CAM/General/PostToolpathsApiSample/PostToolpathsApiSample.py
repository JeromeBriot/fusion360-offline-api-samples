# Author-Autodesk Inc.
# Description-Demonstrates posting toolpaths in the active document

# For this sample script to run, the active Fusion document must contain at least one CAM operation.

import adsk.core, adsk.fusion, adsk.cam, traceback, os

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        doc = app.activeDocument
        products = doc.products
        product = products.itemByProductType('CAMProductType')

        # check if the document has a CAMProductType.  I will not if there are no CAM operations in it.
        if product == None:
            ui.messageBox('There are no CAM operations in the active document.  This script requires the active document to contain at least one CAM operation.',
                            'No CAM Operations Exist',
                            adsk.core.MessageBoxButtonTypes.OKButtonType,
                            adsk.core.MessageBoxIconTypes.CriticalIconType)
            return

        cam = adsk.cam.CAM.cast(product)

        # specify the program name, post configuration to use and a folder destination for the nc file
        programName = '101'
        outputFolder = cam.temporaryFolder

        # set the post configuration to use based on Operation Type of the first Setup
        firstSetupOperationType = cam.setups.item(0).operationType
        if firstSetupOperationType == adsk.cam.OperationTypes.MillingOperation:
            postConfig = os.path.join(cam.genericPostFolder, 'fanuc.cps') 
        elif firstSetupOperationType == adsk.cam.OperationTypes.TurningOperation:
            postConfig = os.path.join(cam.genericPostFolder, 'fanuc turning.cps') 
        elif firstSetupOperationType == adsk.cam.OperationTypes.JetOperation:
            postConfig = cam.genericPostFolder + '/' + 'omax.cps' 

        # prompt the user with an option to view the resulting NC file.
        viewResults = ui.messageBox('View results when post is complete?', 'Post NC Files',
                                    adsk.core.MessageBoxButtonTypes.YesNoButtonType,
                                    adsk.core.MessageBoxIconTypes.QuestionIconType)
        if viewResults == adsk.core.DialogResults.DialogNo:
            viewResult = False
        else:
            viewResult = True

        # specify the NC file output units
        units = adsk.cam.PostOutputUnitOptions.DocumentUnitsOutput
#        units = adsk.cam.PostOutputUnitOptions.InchesOutput
#        units = adsk.cam.PostOutputUnitOptions.MillimetersOutput

        # create the postInput object
        postInput = adsk.cam.PostProcessInput.create(programName, postConfig, outputFolder, units)
        postInput.isOpenInEditor = viewResult

        # create the post properties
        postProperties = adsk.core.NamedValues.create()
        # create the disable sequence number property
        disableSequenceNumbers = adsk.core.ValueInput.createByBoolean(False)
        postProperties.add("showSequenceNumbers", disableSequenceNumbers)
        # add the post properties to the post process input
        postInput.postProperties = postProperties

        # set the value of scenario to 1, 2 or 3 to post all, post the first setup, or post only the first operation of the first setup.
        scenario = 3
        if scenario == 1:
            ui.messageBox('All toolpaths will be posted')
            cam.postProcessAll(postInput)
        elif scenario == 2:
            ui.messageBox('Toolpaths in the first Setup will be posted')
            setups = cam.setups
            setup = setups.item(0)
            cam.postProcess(setup, postInput)
        elif scenario == 3:
            ui.messageBox('The first Toolpath in the first Setup will be posted')
            setups = cam.setups
            setup = setups.item(0)
            operations = setup.allOperations
            operation = operations.item(0)
            if operation.hasToolpath == True:
                cam.postProcess(operation, postInput)
            else:
                ui.messageBox('Operation has no toolpath to post')
                return

        ui.messageBox('Post processing is complete. The results have been written to:\n"' + os.path.join(outputFolder, programName) + '.nc"') 

        # open the output folder in Finder on Mac or in Explorer on Windows
        if (os.name == 'posix'):
            os.system('open "%s"' % outputFolder)
        elif (os.name == 'nt'):
            os.startfile(outputFolder)

        # Prompt user with an option to switch to the CAM workspace if it's not already active
        if ui.activeWorkspace.name != 'CAM':
            activateCAMWorkspace = ui.messageBox('Activate the CAM Workspace?','CAM Workspace Activate',
                                                 adsk.core.MessageBoxButtonTypes.YesNoButtonType,
                                                 adsk.core.MessageBoxIconTypes.QuestionIconType)

            if activateCAMWorkspace == adsk.core.DialogResults.DialogYes:
                camWorkspace = ui.workspaces.itemById("CAMEnvironment")
                camWorkspace.activate()
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))