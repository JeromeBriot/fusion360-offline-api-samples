# Author-Autodesk Inc.
# Description-Demonstrates adding a new toolpath into the document using an existing CAM template

import adsk.core, adsk.cam, os, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        doc = app.activeDocument
        products = doc.products
        # Get the CAM product
        cam = adsk.cam.CAM.cast(products.itemByProductType("CAMProductType"))

        # List of all setups
        setups = cam.setups
        
        # Specify the full filename of the template.
        templateFilename = 'E:\\face.f3dhsm-template'
        
        # Check if the template exists (from the path specified above). Show an error if it doesn't exist.
        if not os.path.exists(templateFilename):
            ui.messageBox("The template '" + templateFilename + "' does not exist")
            return

        # Go through each setup in the document
        for setup in setups:
            # Add the template to each setup.
            results = setup.createFromTemplate(templateFilename)

            # Get the operation that was created. What's created will
            # vary depending on what's defined in the template so you
            # may need more logic to find the result you want.
            operation = results.item(0)

            # Change the operation name
            operation.name = "API added operation"
        
        # Generate all toolpaths, skipping any that are already valid
        cam.generateAllToolpaths(True)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))