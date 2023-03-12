# Author-Autodesk Inc.
# Description-Traverses the entire structure of the currently open assemlby using a recursive function and displays the result in a message box

import adsk.core, adsk.fusion, traceback

# Performs a recursive traversal of an entire assembly structure.
def traverseAssembly(occurrences, currentLevel, inputString):
    for i in range(0, occurrences.count):
        occ = occurrences.item(i)
        inputString += spaces(currentLevel * 5) + occ.name + '\n'
        
        if occ.childOccurrences:
            inputString = traverseAssembly(occ.childOccurrences, currentLevel + 1, inputString)
    return inputString


# Returns a string containing the especified number of spaces.
def spaces(spaceCount):
    result = ''
    for i in range(0, spaceCount):
        result += ' '

    return result


def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        if not design:
            ui.messageBox('No active Fusion design', 'No Design')
            return

        # Get the root component of the active design.
        rootComp = design.rootComponent
        

        # Create the title for the output.
        resultString = 'Root (' + design.parentDocument.name + ')\n'
        
        # Call the recursive function to traverse the assembly and build the output string.
        resultString = traverseAssembly(rootComp.occurrences.asList, 1, resultString)

        # Display the result.
        # Write the results to the TEXT COMMANDS window.
        textPalette = ui.palettes.itemById('TextCommands')
        if not textPalette.isVisible:
            textPalette.isVisible = True
        textPalette.writeText(resultString)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))