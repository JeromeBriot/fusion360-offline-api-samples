#Author-Autodesk Inc.
#Description-TemporaryBRepManager related functions


import adsk.core, adsk.fusion, adsk.cam, traceback
import os

def CreateBox():  
    # Get TemporaryBRepManager
    tempBrepMgr = adsk.fusion.TemporaryBRepManager.get()

    centerPoint = adsk.core.Point3D.create(0.0, 10.0, 0.0);
    lengthDir = adsk.core.Vector3D.create(1.0, 0.0, 0.0)
    widthDir = adsk.core.Vector3D.create(0.0, 1.0, 0.0)
    orientedBoundingBox3D = adsk.core.OrientedBoundingBox3D.create(centerPoint, 
                                                                   lengthDir,
                                                                   widthDir,
                                                                   5.0,
                                                                   6.0,
                                                                   2.0
                                                                   )
    # Create box                                                              
    box = tempBrepMgr.createBox(orientedBoundingBox3D)
    return box

def CreateCylinderOrCone(radius1, radius2):
    # Get TemporaryBRepManager
    tempBrepMgr = adsk.fusion.TemporaryBRepManager.get()  
    
    point1 = adsk.core.Point3D.create(3.0, 3.0, 3.0)
    point2 = adsk.core.Point3D.create(-3.0, -3.0, -3.0)   
    
    # Create Cylinder Or Cone
    cylinderOrCone = tempBrepMgr.createCylinderOrCone(point1, 
                                                      radius1, 
                                                      point2, 
                                                      radius2
                                                      )   
    return cylinderOrCone
        
def CreateEllipticalCylinderOrCone(majorRadius1, minorRadius1, majorRadius2):
    # Get TemporaryBRepManager
    tempBrepMgr = adsk.fusion.TemporaryBRepManager.get()  
    
    pointOne = adsk.core.Point3D.create(8.0, 3.0, 0.0)
    pointTwo = adsk.core.Point3D.create(8.0, -3.0, -3.0) 
    majorAxisDirection = adsk.core.Vector3D.create(1.0, 1.0, 1.0)
    
    # Create EllipticalCylinder Or Cone
    ellipicalCylinderOrCone = tempBrepMgr.createEllipticalCylinderOrCone(pointOne,
                                                                   majorRadius1,
                                                                   minorRadius1,
                                                                   pointTwo,
                                                                   majorRadius2,
                                                                   majorAxisDirection
                                                                  )      
    return ellipicalCylinderOrCone
   
def CreateSphere(radius):
    # Get TemporaryBRepManager
    tempBrepMgr = adsk.fusion.TemporaryBRepManager.get()  
    
    sphereCenter = adsk.core.Point3D.create(0.0, 8.0, 8.0)
    # Create sphere
    sphere = tempBrepMgr.createSphere(sphereCenter, radius)
    
    return sphere
    
def CreateTorus(majorRadius, minorRadius):
    # Get TemporaryBRepManager
    tempBrepMgr = adsk.fusion.TemporaryBRepManager.get()  
    
    axis = adsk.core.Vector3D.create(0.0, 0.0, 10.0)
    torusCenter = adsk.core.Point3D.create(0.0, 2.0, 8.0)
    # Create torus
    torus = tempBrepMgr.createTorus(torusCenter, 
                                    axis,
                                    majorRadius,
                                    minorRadius
                                    )
    return torus  

def PlaneIntersection(body):  
    # Get TemporaryBRepManager
    tempBrepMgr = adsk.fusion.TemporaryBRepManager.get()  

    planeOrigin = adsk.core.Point3D.create(0.0, 0.0, 0.0)
    planeNormal = adsk.core.Vector3D.create(0.0, 0.0, 1.0)
    plane =  adsk.core.Plane.create(planeOrigin, planeNormal)
    
    # creates a brep body by the intersection between the input body and plane
    intersectionBody = tempBrepMgr.planeIntersection(body, plane) 
    return intersectionBody        
    
def TransformBody(body):
    # Get TemporaryBRepManager
    tempBrepMgr = adsk.fusion.TemporaryBRepManager.get() 
    
    transform = adsk.core.Matrix3D.create()
    transform.translation = adsk.core.Vector3D.create(0.0, 0.0, -5.0)
    
    # Transforms the brep body using the specified transformation matrix
    isSuccess = tempBrepMgr.transform(body, transform)
    return isSuccess
    
def GetWireFromBody(body, index):
    brepWires = body.wires;
    brepWire = brepWires.item(index)
    return brepWire
    
def DeleteFacesOnBody(body, indexes, isDeleteSpecifiedFaces):
    # Get TemporaryBRepManager
    tempBrepMgr = adsk.fusion.TemporaryBRepManager.get() 
    
    brepFaces = []
    faces = body.faces
    for index in indexes:
        face = faces.item(index)
        brepFaces.append(face)
    
    # Delete faces on body
    isSuccess = tempBrepMgr.deleteFaces(brepFaces, isDeleteSpecifiedFaces)
    return isSuccess
    
def CreateSilhouette(face):
    # Get TemporaryBRepManager
    tempBrepMgr = adsk.fusion.TemporaryBRepManager.get() 
    
    viewDirection = adsk.core.Vector3D.create(0.0, 0.0, 1.0)
    silhouetteBody = tempBrepMgr.createSilhouetteCurves(face, viewDirection, True)
    
    # Create silhouette body
    return silhouetteBody
    
def CreateWireFromCurves():
    # Get TemporaryBRepManager
    tempBrepMgr = adsk.fusion.TemporaryBRepManager.get() 
    
    center = adsk.core.Point3D.create(10.0, 0.0, 5.0)
    normal = adsk.core.Vector3D.create(0.0, 0.0, 1.0)
    circle1 = adsk.core.Circle3D.createByCenter(center, normal, 3.0)
    circle2 = adsk.core.Circle3D.createByCenter(center, normal, 2.0)
    curves = []
    curves.append(circle1)
    curves.append(circle2)
    
    # Create wire from curves
    wireBody, edgeMap = tempBrepMgr.createWireFromCurves(curves)
    return wireBody, edgeMap
    
def CreateHelixBody():
    # Get TemporaryBRepManager
    tempBrepMgr = adsk.fusion.TemporaryBRepManager.get() 
    
    axisPoint = adsk.core.Point3D.create(10.0, 10.0, 10.0)
    axisVector = adsk.core.Vector3D.create(0.0, 1.0, 0.0)
    startPoint = adsk.core.Point3D.create(8.0, 2.0, 10.0)
    pitch = 1.0
    turns = 10
    taperAngle = 0.3
    
    # Create helix body
    helixBody = tempBrepMgr.createHelixWire(axisPoint,
                                axisVector,
                                startPoint,
                                pitch,
                                turns,
                                taperAngle
                                )
    return helixBody
    
def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        # Create a new document with parametric design
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        design.designType = adsk.fusion.DesignTypes.DirectDesignType
        
        # Get the root component of active design
        rootComp = design.rootComponent

        # Get bodies in root component
        bodies = rootComp.bRepBodies
        
        # Get TemporaryBRepManager
        tempBrepMgr = adsk.fusion.TemporaryBRepManager.get()
           
        # Create a temporary solid box
        box = CreateBox()
        
        # Check the temporary box property
        isTemporary = box.isTemporary
        print(("Is the Box temporary? {}").format(isTemporary))
        
        # Creates a temporary solid cylinder and cone BRepBody
        cylinder = CreateCylinderOrCone(3.0, 3.0)
        cone =  CreateCylinderOrCone(2.0, 4.0)
        
        # Creates a temporary elliptical solid cylinder and cone BrepBody
        ellipicalCylinder = CreateEllipticalCylinderOrCone(4.0, 2.0, 4.0)
        ellipicalCone = CreateEllipticalCylinderOrCone(3.0, 2.0, 4.5)
                      
        # Creates a temporary spherical BRepBody
        sphere = CreateSphere(3.0)
        
        # Creates a temporary toroidal BRepBody
        torus = CreateTorus(2.0, 1.0) 

        # creates a brep body by the intersection between the input body and plane
        intersectionBody = PlaneIntersection(box) 
        
        # Creates a temporary copy of the input BRepBody
        copiedBody = tempBrepMgr.copy(intersectionBody)
        
        # Transforms the brep body using the specified transformation matrix
        isSuccess = TransformBody(copiedBody)
        print(("Is the transformation of the copied body successed? {}").format(isSuccess))
  
        # Get brep wires 
        brepWire = GetWireFromBody(intersectionBody, 0)
        copiedWire = GetWireFromBody(copiedBody, 0)
        
        #  Creates a new body by creating a ruled surface between the two input wire bodies. 
        surfaceBody = tempBrepMgr.createRuledSurface(brepWire, copiedWire)
        
        # Performs the specified Boolean operation between the two input bodies
        intersection = adsk.fusion.BooleanTypes.IntersectionBooleanType
        isSuccess = tempBrepMgr.booleanOperation(cylinder, cone, intersection)
        print(("Is the intersection boolean operation successed? {}").format(isSuccess))
        
        union = adsk.fusion.BooleanTypes.UnionBooleanType 
        isSuccess = tempBrepMgr.booleanOperation(ellipicalCylinder, ellipicalCone, union)        
        print(("Is the union boolean operation successed? {}").format(isSuccess))
        
        # Delete face from a temporary BRepBody
        indexes = [0,2]
        isSuccess = DeleteFacesOnBody(ellipicalCylinder, indexes, True)
        print(("Is faces deletion successed? {}").format(isSuccess))
        
        # Calculates the silhouette curve geometry for a given face as viewed from a given direction. 
        face = torus.faces.item(0)
        silhouetteBody = CreateSilhouette(face)
 
        # Create wire from curves
        wireBody, edgeMap = CreateWireFromCurves()
        
        # Creates a body from multiple wires that all lie within the same plane
        wireBodies = []
        wireBodies.append(wireBody)
        planerBody = tempBrepMgr.createFaceFromPlanarWires(wireBodies)
        
        # Create helix wire body    
        helixBody = CreateHelixBody()
        # Display the helix edges
        group = rootComp.customGraphicsGroups.add()
        for edge in helixBody.edges:
            group.addCurve(edge.geometry)

        # Add the temprary bodies to direct modeling design, 
        # then the temprary bodies will be displayed
        bodies.add(box)
        bodies.add(cylinder)
        bodies.add(cone)
        bodies.add(ellipicalCylinder)
        bodies.add(ellipicalCone)
        bodies.add(sphere)
        bodies.add(torus)
        bodies.add(intersectionBody)
        bodies.add(surfaceBody)
        bodies.add(silhouetteBody)
        bodies.add(planerBody)
        
        #Exports the input bodies to the specified file.
        brepBodies = []
        brepBodies.append(box)
        brepBodies.append(surfaceBody)
        
        scriptDir = os.path.dirname(os.path.realpath(__file__))
        exportPath = os.path.join(scriptDir, "sampleFile.smt")
        
        isSuccess = tempBrepMgr.exportToFile(brepBodies, exportPath)
        print(("Is the export to file success? {}").format(isSuccess))
        
        #Creates new BRepBody objects based on the contents of the specified file
        filePath = os.path.join(scriptDir, "sampleFile.smt")
        newBodies = tempBrepMgr.createFromFile(filePath);

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))