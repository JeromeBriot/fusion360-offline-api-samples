# Author-Autodesk Inc.
# Description-Demonstrates generating the toolpaths in the active document

import adsk.core, adsk.fusion, traceback, time

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        
        # Get the CAM product.
        doc = app.activeDocument
        products = doc.products
        product = products.itemByProductType('CAMProductType')
        cam = adsk.cam.CAM.cast(product)
        if not cam:
            ui.messageBox('No CAM data exists in the active document.')
            return
            
        # Verify that there are any setups.
        if cam.allOperations.count == 0:
            ui.messageBox('No CAM operations exist in the active document.')
            return
            
        #set the value of scenario to 1, 2 or 3 to generate all, the first setup, or the first operation of the first setup.
        scenario = 1
        if scenario == 1:
            future = cam.generateAllToolpaths(False)
            message = 'The toolpaths for all operations in the document have been generated.'
        elif scenario == 2:
            setup = cam.setups.item(0)
            future = cam.generateToolpath(setup)
            message = 'The toolpaths for the operations of the first setup in the document have been generated.'
        elif scenario == 3:
            setup = cam.setups.item(0)
            operations = setup.operations
            operation = operations.item(0)
            future = cam.generateToolpath(operation)
            message = 'The toolpath for the first operation of the first setup in the document have been generated.'

        numOps = future.numberOfOperations

        #  create and show the progress dialog while the toolpaths are being generated.
        progress = ui.createProgressDialog()
        progress.isCancelButtonShown = False
        progress.show('Toolpath Generation Progress', 'Generating Toolpaths', 0, 10)

        # Enter a loop to wait while the toolpaths are being generated and update
        # the progress dialog.
        while not future.isGenerationCompleted:
            # since toolpaths are calculated in parallel, loop the progress bar while the toolpaths 
            # are being generated but none are yet complete.
            n = 0
            start = time.time()
            while future.numberOfCompleted == 0:
                if time.time() - start > .125: # increment the progess value every .125 seconds.
                    start = time.time()
                    n +=1
                    progress.progressValue = n
                    adsk.doEvents()
                if n > 10:
                    n = 0

            # The first toolpath has finished computing so now display better
            # information in the progress dialog.

            # set the progress bar value to the number of completed toolpaths
            progress.progressValue = future.numberOfCompleted

            # set the progress bar max to the number of operations to be completed.
            progress.maximumValue = numOps

            # set the message for the progress dialog to track the progress value and the total number of operations to be completed.
            progress.message = 'Generating %v of %m' + ' Toolpaths'
            adsk.doEvents()

        progress.hide()
        ui.messageBox(message)

        # Prompt user with an option to switch to the CAM workspace if it's not already active
        if ui.activeWorkspace.id != 'CAMEnvironment':
            dialogResult = ui.messageBox('Activate the CAM Workspace?','CAM Workspace Activate',
                                                 adsk.core.MessageBoxButtonTypes.YesNoButtonType,
                                                 adsk.core.MessageBoxIconTypes.QuestionIconType)

            if dialogResult == adsk.core.DialogResults.DialogYes:
                ws = ui.workspaces.itemById('CAMEnvironment')
                ws.activate()
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))