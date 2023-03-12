# Author-Autodesk Inc.
# Description-Demonstrates using a custom event to process getting information in the background to display in a command dialog

import adsk.core, adsk.fusion, adsk.cam, traceback
import threading, time, json 
from datetime import datetime
import ctypes


handlers = []
_app = adsk.core.Application.cast(None)
_ui = adsk.core.UserInterface.cast(None)
_tableInput = adsk.core.TableCommandInput.cast(None)
_cmdDef = adsk.core.CommandDefinition.cast(None)
_workerThread = None
myCustomEvent = 'MyDialogDataEvent'


# The event handler that responds when the custom event is fired.
class DialogDataEventHandler(adsk.core.CustomEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            eventArgs = adsk.core.CustomEventArgs.cast(args)

            # Get the data passed through event.  In this case it is
            # formatted as JSON so it extracts the values named
            # "label" and "value".    
            dialogData = eventArgs.additionalInfo
            valueData = json.loads(dialogData)
            label = valueData['label']
            value = valueData['value']
            
            # Set the value of a string value input using the data passed in.
            stringInput = adsk.core.StringValueCommandInput.cast(_tableInput.getInputAtPosition(int(label), 1))
            stringInput.value = value
        except:
            if _ui:
                _ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
            

# The worker thread class. 
class GetDialogInfoThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.isStopped = False
    def run(self):
        try:
            # Iterate 5 steps to fill each of the 5 rows in the table.
            for i in range(5):
                # Check to see if the thread has been stopped.
                if not self.isStopped:
                    # Simulate calling a web service that will take some
                    # time and returns some data by sleeping and building
                    # some data using the current time.
                    time.sleep(2)

                    date_object = datetime.now()
                    current_time = date_object.strftime('%H:%M:%S')
                    returnInfo = {'label': str(i), 'value': current_time}
                    returnJson = json.dumps(returnInfo)

                    # Fire the custom event to allow the add-in to update the dialog.    
                    _app.fireCustomEvent(myCustomEvent, returnJson)                    
                else:
                    return
        except:
            ctypes.windll.user32.MessageBoxW(0, 'Failed:\n{}'.format(traceback.format_exc()), "Failed", 1)

    # Method to allow the thread to be stopped.                
    def stop(self):
        self.isStopped = True


# Event handler that is called when the add-in is destroyed. The custom event is
# unregistered here and the thread is stopped.
class MyDestroyHandler(adsk.core.CommandEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            eventArgs = adsk.core.CommandEventArgs.cast(args)
            _app.unregisterCustomEvent(myCustomEvent)
            _workerThread.stop()
        except:
            if _ui:
                _ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
        

# Event handler to handle when the command is run by the user.
class MyCommandCreatedHandler(adsk.core.CommandCreatedEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            eventArgs = adsk.core.CommandCreatedEventArgs.cast(args)
            inputs = eventArgs.command.commandInputs
            
            # Create a table command input and add five rows of two columns of string 
            # command inputs.  The first column is populated with values and the
            # second is left empty to be populated later as the data is obtained.
            global _tableInput
            _tableInput = inputs.addTableCommandInput('table', 'Info', 2, '1:2')
            _tableInput.maximumVisibleRows = 5
            for i in range(5):
                labelText = inputs.addStringValueInput('label' + str(i), 'Label ' + str(i), 'Item ' + str(i))
                labelText.isReadOnly = True
                _tableInput.addCommandInput(labelText, i, 0, 0, 0)
            
                valueText = inputs.addStringValueInput('value' + str(i), 'Value ' + str(i), '')
                valueText.isReadOnly = True
                _tableInput.addCommandInput(valueText, i, 1, 0, 0)
            
            # Register the custom event and connect the handler.
            customEvent = _app.registerCustomEvent(myCustomEvent)
            onDialogData = DialogDataEventHandler()
            customEvent.add(onDialogData)
            handlers.append(onDialogData)
            
            # Connect a handler to the command destroyed event.
            onDestroy = MyDestroyHandler()
            inputs.command.destroy.add(onDestroy)
            handlers.append(onDestroy)        
    
            # Start the seperate thread that will collect the data to populate
            # the second column of the dialog.
            global _workerThread
            _workerThread = GetDialogInfoThread()
            _workerThread.start()
        except:
            if _ui:
                _ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
        
        
def run(context):
    try:
        global _app, _ui, _cmdDef
        _app = adsk.core.Application.get()
        _ui  = _app.userInterface

        # Create a new command and add it to the ADD-INS panel in the model workspace.
        _cmdDef = _ui.commandDefinitions.addButtonDefinition('ThreadTestCmd', 'Thread Test', 'Thread Test') 
        addInsPanel = _ui.allToolbarPanels.itemById('SolidScriptsAddinsPanel')
        buttonControl = addInsPanel.controls.addCommand(_cmdDef)        

        # Connect the command created handler to the event.
        onCommandCreated = MyCommandCreatedHandler()
        _cmdDef.commandCreated.add(onCommandCreated)
        handlers.append(onCommandCreated)
    except:
        if _ui:
            _ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))


def stop(context):
    try:
        # Clean up the command.
        addInsPanel = _ui.allToolbarPanels.itemById('SolidScriptsAddinsPanel')
        cntrl = addInsPanel.controls.itemById('ThreadTestCmd')
        if cntrl:
            cntrl.deleteMe()

        if _cmdDef:
            _cmdDef.deleteMe()
    except:
        if _ui:
            _ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))