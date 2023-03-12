#Author-Autodesk Inc.
#Description-Delete empty components from an assembly.

import adsk, adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        
        # Get all components in the active design.
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        title = 'Delete Empty Components'
        if not design:
            ui.messageBox('No active Fusion design', title)
            return

        components = design.allComponents

        # Find all of the empty components.
        # It is empty if it has no occurrences, bodies, featres, sketches, or construction.
        root = design.rootComponent
        componentsToDelete = []

        for component in components:

            # Skip the root component.
            if root == component:
                continue

            if component.occurrences.count == 0 \
                and component.bRepBodies.count == 0 \
                and component.features.count == 0 \
                and component.sketches.count == 0 \
                and component.constructionPlanes.count == 0 \
                and component.constructionAxes.count == 0 \
                and component.constructionPoints.count == 0:

                componentsToDelete.append(component)

        # Delete all immediate occurrences of the empty components.
        deletedComponents = []
        for component in componentsToDelete:

            # Get the name first because deleting the final Occurrence will delete the Component.
            name = component.name

            # Build a list of unique immediate occurrences of the component.
            occurrences = root.allOccurrencesByComponent(component)
            uniqueOccurrences = []
            for occurrence in occurrences:
                index = 0
                for k in range(0, len(uniqueOccurrences)):
                    if occurrence is uniqueOccurrences[k]:
                        break
                    index = k+1
                if index == len(uniqueOccurrences):
                    uniqueOccurrences.append(occurrence)

            # Delete them.
            for uniqueOccurrencesI in uniqueOccurrences:
                uniqueOccurrencesI.deleteMe()

            deletedComponents.append(name)

        if len(deletedComponents) == 0:
            msg = 'No empty components found.'
        else:
            if len(deletedComponents) > 1:
                msg = str(len(deletedComponents)) + ' empty component' + 's'
            else:
                msg = str(len(deletedComponents)) + ' empty component' + ' deleted'
            msg += '\n\n'
            for deletedComponentI in deletedComponents:
                msg += '\n' + deletedComponentI

        ui.messageBox(msg, title)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))