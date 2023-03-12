# Author-Autodesk Inc.
# Description-Demonstrates generating the setup sheets for an existing toolpath

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

        # check if the document has a CAMProductType.  It will not if there are no CAM operations in it.
        if not product:
            ui.messageBox('There are no CAM operations in the active document.  This script requires the active document to contain at least one CAM operation.',
                            'No CAM Operations Exist',
                            adsk.core.MessageBoxButtonTypes.OKButtonType,
                            adsk.core.MessageBoxIconTypes.CriticalIconType)
            return

        cam = adsk.cam.CAM.cast(product)

        # specify the output folder and format for the setup sheets
        outputFolder = cam.temporaryFolder
        sheetFormat = adsk.cam.SetupSheetFormats.HTMLFormat
        #sheetFormat = adsk.cam.SetupSheetFormats.ExcelFormat (not currently supported on Mac)

        # prompt the user with an option to view the resulting setup sheets.
        viewResults = ui.messageBox('View setup sheets when done?', 'Generate Setup Sheets',
                                    adsk.core.MessageBoxButtonTypes.YesNoButtonType,
                                    adsk.core.MessageBoxIconTypes.QuestionIconType)
        if viewResults == adsk.core.DialogResults.DialogNo:
            viewResult = False
        else:
            viewResult = True

        # set the value of scenario to 1, 2 or 3 to generate setup sheets for all, for the first setup, or for the first operation of the first setup.
        scenario = 1
        if scenario == 1:
            ui.messageBox('Setup sheets for all operations will be generated.')
            cam.generateAllSetupSheets(sheetFormat, outputFolder, viewResult)
        elif scenario == 2:
            ui.messageBox('Setup sheets for operations in the first setup will be generated.')
            setup = cam.setups.item(0)
            cam.generateSetupSheet(setup, sheetFormat, outputFolder, viewResult)
        elif scenario == 3:
            ui.messageBox('A setup sheet for the first operation in the first setup will be generated.')
            setup = cam.setups.item(0)
            operations = setup.allOperations
            operation = operations.item(0)
            if operation.hasToolpath:
                cam.generateSetupSheet(operation, sheetFormat, outputFolder, viewResult)
            else:
                ui.messageBox('This operation has no toolpath.  A valid toolpath must exist in order for a setup sheet to be generated.')
                return

        ui.messageBox('Setup Sheets have been generated in:\n' + outputFolder)

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
                workspaces = ui.workspaces
                camWorkspace = workspaces.itemById("CAMEnvironment")
                camWorkspace.activate()

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))