# Author-Autodesk Inc.
# Description-Iterates over all top-level occurrences and if it's a referenced component, it will break the link

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface

        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)

        # Get the root component of the active design.
        rootComp = design.rootComponent

        # Build a list of occurrences because breaking a link will cause the
        # collection to be modified and causes a problem iterating over the colection.
        occs = []
        for occ in rootComp.occurrences:
            occs.append(occ)

        # Iterate through the top-level occurrences to see if any of them are external references.
        occ: adsk.fusion.Occurrence
        for occ in occs:
            if occ.isReferencedComponent:
                occ.breakLink()
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))