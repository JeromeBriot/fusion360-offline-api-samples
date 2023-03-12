#Author-Autodesk Inc.
#Description-Demonstrates creating save a new file and then inserting it into a design


import adsk.core, adsk.fusion, adsk.cam, traceback

# Global variable used to maintain a reference to all event handlers.
handlers = []

_app = adsk.core.Application.get()
_ui  = _app.userInterface

newFilename = 'SampleSave'
_docId = ''

def run(context):
    try:
        # Check that the active document has been saved.
        doc = _app.activeDocument
        if not doc.isSaved:
            _ui.messageBox('The active document must be saved before running this script.')
            return

        parentFolder = doc.dataFile.parentFolder

        # Connect to the dataFileComplete event, to watch for when the file has been fully saved on Fusion Team.
        onDataFileComplete = MyDataFileCompleteHandler()
        _app.dataFileComplete.add(onDataFileComplete)
        handlers.append(onDataFileComplete)

        # Create a new design with a cylinder.
        newDoc = CreateCylinderDesign(parentFolder, newFilename)

        adsk.autoTerminate(False)
    except:
        _ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))


# Event handler for the dataFileComplete event.
class MyDataFileCompleteHandler(adsk.core.DataEventHandler):
    def __init__(self):
        super().__init__()
    def notify(self, args: adsk.core.DataEventArgs):
        try:
            # Check to see if the document we care about is the one that saved.
            if args.file.name == newFilename:
                cylinderDoc = args.file
                
                topDoc = _app.activeDocument

                # Insert the saved document into the activate document.
                des: adsk.fusion.Design = topDoc.products.itemByProductType('DesignProductType')
                root = des.rootComponent

                cylOcc = root.occurrences.addByInsert(args.file, adsk.core.Matrix3D.create(), True)

                adsk.terminate()
        except:
            _ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
        

def CreateCylinderDesign(folder, filename):
    try:
        # Create a new document and have it be invisible.
        cylinderDoc: adsk.fusion.FusionDocument = _app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType, True)

        # Get the Design and root component from the document.
        des: adsk.fusion.Design = cylinderDoc.products.itemByProductType('DesignProductType')
        root = des.rootComponent

        # Create a sketch with a single circle.
        sk: adsk.fusion.Sketch = root.sketches.add(root.yZConstructionPlane)
        sk.sketchCurves.sketchCircles.addByCenterRadius(adsk.core.Point3D.create(3,2,0), 4)
        prof = sk.profiles[0]

        # Create an extrusion, using the circle.
        root.features.extrudeFeatures.addSimple(prof, adsk.core.ValueInput.createByReal(12), adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

        # Save the document.
        cylinderDoc.saveAs(filename, folder, 'Sample demonstrating watching for the save to complete.', '')

        global _docId
        _docId = cylinderDoc.creationId
        cylinderDoc.close(False)
    except:
        _ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))