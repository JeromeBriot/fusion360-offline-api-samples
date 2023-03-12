# Author-Autodesk Inc.
# Description-Demonstrates how to import different formats to Fusion document

import adsk.core, adsk.fusion, traceback
import os.path, sys

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        # Get import manager
        importManager = app.importManager
        
        # Get iges import options
        igesFileName = 'C:\\APISampleImportNeutralIgs.igs'
        igesOptions = importManager.createIGESImportOptions(igesFileName)
        
        # Import iges file to new document
        importManager.importToNewDocument(igesOptions)
        
        # Get sat import options
        satFileName = 'C:\\APISampleImportNeutralSat.sat'
        satOptions = importManager.createSATImportOptions(satFileName)
        satOptions.isViewFit = False
        
        # Get active design
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        
        # Get root component
        rootComp = design.rootComponent
        
        # Import sat file to root component
        importManager.importToTarget(satOptions, rootComp)
        
        # Get smt import options
        smtFileName = 'C:\\APISampleImportNeutralSmt.smt'
        smtOptions = importManager.createSMTImportOptions(smtFileName)
        smtOptions.isViewFit = False
        
        # Import smt file to root component
        importManager.importToTarget(smtOptions, rootComp)
        
        # Get step import options
        stpFileName = 'C:\\APISampleImportNeutralStp.stp'
        stpOptions = importManager.createSTEPImportOptions(stpFileName)
        stpOptions.isViewFit = False
        
        # Import step file to root component
        importManager.importToTarget(stpOptions, rootComp)
        
        # Get dxf import options
        dxfFileName = 'C:\\APISampleImportNeutralDxf2D.dxf'
        dxfOptions = importManager.createDXF2DImportOptions(dxfFileName, rootComp.xZConstructionPlane)
        dxfOptions.isViewFit = False
        
        # Import dxf file to root component
        importManager.importToTarget(dxfOptions, rootComp)
        
        dxfOptions = importManager.createDXF2DImportOptions(dxfFileName, rootComp.xZConstructionPlane)
        dxfOptions.isViewFit = False
        # Set the flag true to merge all the layers of DXF into single sketch.
        dxfOptions.isSingleSketchResult = True
        
        # Import dxf file to root component
        importManager.importToTarget(dxfOptions, rootComp)
        
        # Get archive import options
        archiveFileName = 'C:\\APISampleImportNeutralF3d2.f3d'
        archiveOptions = importManager.createFusionArchiveImportOptions(archiveFileName)
        
        # Import archive file to root component
        importManager.importToTarget(archiveOptions, rootComp)
        
        # Close the new created document
        doc = app.activeDocument
        doc.close(False)
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))