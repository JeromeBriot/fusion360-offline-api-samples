# Author-Autodesk Inc.
# Description-Demonstrates changing parameters of existing toolpaths

import adsk.core, adsk.cam, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        doc = app.activeDocument
        products = doc.products
        
        # Get the CAM product
        cam = adsk.cam.CAM.cast(products.itemByProductType("CAMProductType"))

        # List of all setups
        setups = cam.setups
        
        for setup in setups:
            # Change the program name of each setup to 1234
            programNameParam = setup.parameters.itemByName('job_programName')
            programNameParam.expression = "1234"
            
            for operation in setup.operations:
                # Change tolerance in all operations
                toleranceParam = operation.parameters.itemByName('tolerance')
                toleranceParam.expression = "0.1mm"
        
        # Generate all toolpaths, skipping any that are already valid
        cam.generateAllToolpaths(True)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))