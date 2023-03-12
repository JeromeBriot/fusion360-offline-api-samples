# Author-Autodesk Inc.
# Description-Demonstrating how to pop up a file dialog and a folder dialog

import adsk.core, adsk.fusion, traceback
import os.path

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
            
        msg = ''
        # Set styles of file dialog.
        fileDlg = ui.createFileDialog()
        fileDlg.isMultiSelectEnabled = True
        fileDlg.title = 'Fusion Open File Dialog'
        fileDlg.filter = '*.*'
        
        # Show file open dialog
        dlgResult = fileDlg.showOpen()
        if dlgResult == adsk.core.DialogResults.DialogOK:
            msg += '\nFiles to Open:'
            for filename in fileDlg.filenames:
                msg += '\n\t{}'.format(filename)
        else:
            return       
        
        # Show file save dialog
        fileDlg.title = 'Fusion Save File Dialog'
        dlgResult = fileDlg.showSave()
        if dlgResult == adsk.core.DialogResults.DialogOK:
            msg += '\nFile to Save: {}'.format(fileDlg.filename)
        else:
            return
            
        # Set styles of file dialog.
        folderDlg = ui.createFolderDialog()
        folderDlg.title = 'Fusion Choose Folder Dialog' 
        
        # Show folder dialog
        dlgResult = folderDlg.showDialog()
        if dlgResult == adsk.core.DialogResults.DialogOK:
            msg += '\nSelected Folder: {}'.format(folderDlg.folder)
        else:
            return
        
        ui.messageBox(msg)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))