# Author-Autodesk Inc.
# Description-BrepWires and BrepWire related functions

import adsk.core, adsk.fusion, adsk.cam, traceback

def CreateWireBody():
    # Get temproray brep manager
    tempBrepMrg = adsk.fusion.TemporaryBRepManager.get() 
    
    conter = adsk.core.Point3D.create(10.0, 0.0, 5.0)
    normal = adsk.core.Vector3D.create(0.0, 0.0, 1.0)
    circle1 = adsk.core.Circle3D.createByCenter(conter, normal, 3.0)
    circle2 = adsk.core.Circle3D.createByCenter(conter, normal, 2.0)
    curves = []
    curves.append(circle1)
    curves.append(circle2)     
    
    # Create wire from curves
    wireBody, edgeMap = tempBrepMrg.createWireFromCurves(curves)
    return wireBody
    
def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        # Create a new document
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        
        # Set the design is a direct modelling design
        design.designType = adsk.fusion.DesignTypes.DirectDesignType
        
        #Get the root component
        rootComp = design.rootComponent
        
        # Create a sub component under root component
        occs = rootComp.occurrences
        transform = adsk.core.Matrix3D.create()
        subOcc = occs.addNewComponent(transform)  
        subComp = subOcc.component
    
        # Create temproray wire body 
        wireBody = CreateWireBody()
        
        # Add wireBody to sub-component
        bodies = subComp.bRepBodies
        bodies.add(wireBody)
        
        # Get the brep wire body
        brepWireBody = bodies.item(0)
        
        # Get brep wires
        brepWires = brepWireBody.wires
        
        # Get brep wires count
        count = brepWires.count
        print("BrepWires count is {}".format(count))
        
        # Get brep wire
        brepWire = brepWires.item(0)
        
        # Get brep edges from brepWire
        brepEdges = brepWire.edges
        
        # Get vertices from brepWire
        vertices = brepWire.vertices
        
        # Get brep coEdges from brepWire
        coEdges = brepWire.coEdges
        
        # Get the brep wire is planar or not
        isPlanar = brepWire.isPlanar
        print("Is the brep wire planar? {}".format(isPlanar))
        
        # Get wire's parent brep body
        parent = brepWire.parent
        
        # offse the planar brep wire
        offsetType = adsk.fusion.OffsetCornerTypes.LinearOffsetCornerType
        planeNormal = adsk.core.Vector3D.create(0.0, 0.0, 1.0)
        offsetBody = brepWire.offsetPlanarWire(planeNormal, 3.0, offsetType)

        # Create brep wire proxy    
        wireProxy = brepWire.createForAssemblyContext(subOcc)
        
        # Get native brep wire
        nativeBrepWire = wireProxy.nativeObject
        
        # Get the assembly occurrence
        occurence = wireProxy.assemblyContext
        
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))