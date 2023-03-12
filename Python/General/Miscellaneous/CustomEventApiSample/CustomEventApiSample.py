#Author-Autodesk Inc.
#Description-Demonstrates the ability to call into the main thread from a worker thread


import adsk.core, adsk.fusion, adsk.cam, traceback
import threading, random, json

app = None
ui = adsk.core.UserInterface.cast(None)
handlers = []
stopFlag = None
myCustomEvent = 'MyCustomEventId'
customEvent = None


# The event handler that responds to the custom event being fired.
class ThreadEventHandler(adsk.core.CustomEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args):
        try:
            # Make sure a command isn't running before changes are made.
            if ui.activeCommand != 'SelectCommand':
                ui.commandDefinitions.itemById('SelectCommand').execute()
                            
            # Get the value from the JSON data passed through the event.
            eventArgs = json.loads(args.additionalInfo)
            newValue = float(eventArgs['Value'])
            
            # Set the parameter value.
            design = adsk.fusion.Design.cast(app.activeProduct)
            param = design.rootComponent.modelParameters.itemByName('Length')
            param.value = newValue
        except:
            if ui:
                ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))


# The class for the new thread.
class MyThread(threading.Thread):
    def __init__(self, event):
        threading.Thread.__init__(self)
        self.stopped = event

    def run(self):
        # Every five seconds fire a custom event, passing a random number.
        while not self.stopped.wait(2):
            args = {'Value': random.randint(1000, 10000)/1000}
            app.fireCustomEvent(myCustomEvent, json.dumps(args)) 
        
        
def run(context):
    global ui
    global app
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        # Register the custom event and connect the handler.
        global customEvent
        customEvent = app.registerCustomEvent(myCustomEvent)
        onThreadEvent = ThreadEventHandler()
        customEvent.add(onThreadEvent)
        handlers.append(onThreadEvent)

        # Create a new thread for the other processing.        
        global stopFlag        
        stopFlag = threading.Event()
        myThread = MyThread(stopFlag)
        myThread.start()
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))


def stop(context):
    try:
        if handlers.count:
            customEvent.remove(handlers[0])
        stopFlag.set() 
        app.unregisterCustomEvent(myCustomEvent)
        ui.messageBox('Stop addin')
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))