#Author-Autodesk Inc.
#Description-Mesh body related functions


import adsk.core, adsk.fusion, traceback
import os.path

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        # Create a document
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
         
        # Get the root component
        rootComp = design.rootComponent
        
        # Create a sub component under root component
        occs = rootComp.occurrences
        transform = adsk.core.Matrix3D.create()
        subOcc = occs.addNewComponent(transform)
        
        # Insert a stl mesh body in the sub component
        subComp = subOcc.component
        meshBodies = subComp.meshBodies
        scriptDir = os.path.dirname(os.path.realpath(__file__))
        stlFilePath = os.path.join(scriptDir, 'APIMeshFacetColors.stl')
        meshBodies.add(stlFilePath, adsk.fusion.MeshUnits.InchMeshUnit)
        stlMeshBody = meshBodies.item(0)
        meshBodyPorxy = stlMeshBody.createForAssemblyContext(subOcc)
        
        # Get and set mesh body name
        print("Get and set mesh body name.")
        print(stlMeshBody.name)
        stlMeshBody.name = "NewName"
        print(stlMeshBody.name)
        
        # Get and set mesh body selectable
        print("Get and set mesh body selectable.")
        print(stlMeshBody.isSelectable)
        stlMeshBody.isSelectable = False
        print(stlMeshBody.isSelectable)
        stlMeshBody.isSelectable = True
        print(stlMeshBody.isSelectable)
        
        # Get and set Mesh body bulb light and visibility
        print("Get and set Mesh body bulb light and visibility.")
        print(stlMeshBody.isLightBulbOn)
        stlMeshBody.isLightBulbOn = False
        print(stlMeshBody.isLightBulbOn)
        print(stlMeshBody.isVisible)
        stlMeshBody.isLightBulbOn = True
        print(stlMeshBody.isLightBulbOn)
        print(stlMeshBody.isVisible) 
        
        # Get and set mesh body Opacity
        print("Get and set mesh body Opacity.")
        print(stlMeshBody.opacity)
        subComp.opacity = 0.5
        stlMeshBody.opacity = 0.6
        print(stlMeshBody.opacity)
        print(meshBodyPorxy.visibleOpacity)
        
        # Get and set mesh body material
        print('Get and set meshbody material.')
        print(stlMeshBody.material.name)
        MaterialLib = app.materialLibraries.itemByName('Fusion 360 Material Library')
        material = MaterialLib.materials.itemByName("Glass, Bronze Glazing, Low E")
        stlMeshBody.material = material
        print(stlMeshBody.material.name)
        
        # Get and set mesh body appearance
        print('Get and set meshbody appearance.')
        print(stlMeshBody.appearance.name)
        appearanceLib = app.materialLibraries.itemByName('Fusion 360 Appearance Library')
        appearance = appearanceLib.appearances.itemByName('Glass - Window')
        stlMeshBody.appearance = appearance
        print(stlMeshBody.appearance.name)
  
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))