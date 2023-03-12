# Author-Autodesk Inc.
# Description-Demonstrates exporting the active design to IGES, STEP, SAT, SMT, F3D and STL formats

import adsk.core, adsk.fusion, traceback, tempfile

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        design = app.activeProduct
        # Get the ExportManager from the active design.
        exportMgr = design.exportManager
        
        tmpDir = tempfile.gettempdir()

        # Create an IgesExportOptions object and do the export.
        igesOptions = exportMgr.createIGESExportOptions(tmpDir + '/test.igs')
        res = exportMgr.execute(igesOptions)
        
        # Create an STEPExportOptions object and do the export.
        stepOptions = exportMgr.createSTEPExportOptions(tmpDir+ '/test.step')
        res = exportMgr.execute(stepOptions)
        
        # Create a SATExportOptions object and do the export.
        satOptions = exportMgr.createSATExportOptions(tmpDir + '/test.sat')
        res = exportMgr.execute(satOptions)
        
        # Create a SMTExportOptions object and do the export.
        smtOptions = exportMgr.createSMTExportOptions(tmpDir + '/test.smt')
        res = exportMgr.execute(smtOptions)
        
        # Create a FusionArchiveExportOptions object and do the export.
        fusionArchivevOptions = exportMgr.createFusionArchiveExportOptions(tmpDir + '/test.f3d')
        res = exportMgr.execute(fusionArchivevOptions)
        ui.messageBox(f'Design exported to: {tmpDir}')
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))