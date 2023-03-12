#Author-Autodesk Inc.
#Description-Uses the UserInterface

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        design = app.activeProduct
        if not design:
            ui.messageBox('No active Fusion design', 'No Design')
            return

        # Prompt the user for a string and validate it's valid.
        isValid = False
        input = '1 in'  # The initial default value.
        while not isValid:
            # Get a string from the user.
            retVals = ui.inputBox('Enter a distance', 'Distance', input)
            if retVals[0]:
                (input, isCancelled) = retVals
            
            # Exit the program if the dialog was cancelled.
            if isCancelled:
                return
            
            # Check that a valid length description was entered.
            unitsMgr = design.unitsManager
            try:
                realValue = unitsMgr.evaluateExpression(input, unitsMgr.defaultLengthUnits)
                isValid = True
            except:
                # Invalid expression so display an error and set the flag to allow them
                # to enter a value again.
                ui.messageBox('"' + input + '" is not a valid length expression.', 'Invalid entry', 
                              adsk.core.MessageBoxButtonTypes.OKButtonType, 
                              adsk.core.MessageBoxIconTypes.CriticalIconType)
                isValid = False
        
        # Use the value for something.
        ui.messageBox('input: ' + input + ', result: ' + str(realValue))

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))