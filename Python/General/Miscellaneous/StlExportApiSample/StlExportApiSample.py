# Author-Autodesk Inc.
# Description-Demonstrates how to export f3d to STL format

import adsk.core, adsk.fusion, traceback
import os.path, sys
    
def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        
        # get active design        
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        
        # get root component in this design
        rootComp = design.rootComponent
        
        # create a single exportManager instance
        exportMgr = design.exportManager
        
        # export the root component to printer utility
        stlRootOptions = exportMgr.createSTLExportOptions(rootComp)

        # get all available print utilities
        printUtils = stlRootOptions.availablePrintUtilities

        # export the root component to the print utility, instead of a specified file            
        for printUtil in printUtils:
            stlRootOptions.sendToPrintUtility = True
            stlRootOptions.printUtility = printUtil

            exportMgr.execute(stlRootOptions)
            
        # get the script location
        scriptDir = os.path.dirname(os.path.realpath(__file__))  
        
        # export the occurrence one by one in the root component to a specified file
        allOccu = rootComp.allOccurrences
        for occ in allOccu:
            fileName = scriptDir + "/" + occ.component.name
            
            # create stl exportOptions
            stlExportOptions = exportMgr.createSTLExportOptions(occ, fileName)
            stlExportOptions.sendToPrintUtility = False
            
            exportMgr.execute(stlExportOptions)

        # export the body one by one in the design to a specified file
        allBodies = rootComp.bRepBodies
        for body in allBodies:
            fileName = scriptDir + "/" + body.parentComponent.name + '-' + body.name
            
            # create stl exportOptions
            stlExportOptions = exportMgr.createSTLExportOptions(body, fileName)
            stlExportOptions.sendToPrintUtility = False
            
            exportMgr.execute(stlExportOptions)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))