# Author-Autodesk Inc.
# Description-Demonstrates using materials and appearance using the API

import adsk.core, adsk.fusion, traceback

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        
        # Open a document.
        TestUtils.openTestData("APISampleTwoCubes.f3d")
        doc = app.activeDocument
        
        materialLibs = app.materialLibraries
        
        matLibPath = os.path.join(os.path.dirname(os.path.realpath(__file__)), '../../../../APISampleMaterialLibrary2.adsklib')
        matLib = materialLibs.load(os.path.abspath(matLibPath))
        bNative = matLib.isNative;
        appear = matLib.appearances.item(1)

        des = adsk.fusion.Design.cast(app.activeProduct)             
        appear.copyTo(des)
        favAppear = des.appearances.itemByName(appear.name)
        
        root = des.rootComponent
        body2 = root.bRepBodies.item(1)
        body2.appearance = favAppear
        
        if bNative == False:
          matLib.unload();
        
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))