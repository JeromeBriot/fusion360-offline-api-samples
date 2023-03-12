#Author-Autodesk Inc.
#Description-Demonstrates creating BRep bodies by BRepBodyDefinition


import adsk.core, adsk.fusion, adsk.cam, traceback
        
def CreateTop1EdgeDefinitionByCurve(brepBodyDef):
    edgeDef = adsk.fusion.BRepEdgeDefinition.cast(None)
    # Create vertex definition
    startVertexDef = brepBodyDef.createVertexDefinition(adsk.core.Point3D.create(-3.519568866597467, 4.0, 1.5488831005318957))
    endVertexDef = brepBodyDef.createVertexDefinition(adsk.core.Point3D.create(-3.519568866597467, 4.0, -2.282227138902387)) 
    
    center = adsk.core.Point3D.create(-2.7379066676103037, 4.0, -0.3666720191852457)
    normal =adsk.core.Vector3D.create(0, 1, 0)
    curve = adsk.core.Circle3D.createByCenter(center, normal, 2.0689)
    # Create edge definition by curve
    edgeDef = brepBodyDef.createEdgeDefinitionByCurve(startVertexDef, endVertexDef, curve)
    
    return edgeDef    

def CreateBottom1EdgeDefinitionByCurve(brepBodyDef):
    edgeDef = adsk.fusion.BRepEdgeDefinition.cast(None)
    # Create vertex definition
    startVertexDef = brepBodyDef.createVertexDefinition(adsk.core.Point3D.create(-3.519568866597467, 0.0, -2.2822271389023863))
    endVertexDef = brepBodyDef.createVertexDefinition(adsk.core.Point3D.create(-3.519568866597467, 0.0, 1.5488831005318966)) 
    
    center = adsk.core.Point3D.create(-2.7379066676103037, 0.0, -0.36667201918524484)
    normal =adsk.core.Vector3D.create(0, -1, 0)
    curve = adsk.core.Circle3D.createByCenter(center, normal, 2.0689)
    # Create edge definition by curve
    edgeDef = brepBodyDef.createEdgeDefinitionByCurve(startVertexDef, endVertexDef, curve)
    
    return edgeDef

def CreateLinearEdgeDefinitionByCurve(brepBodyDef, startPoint, endPoint):
    edgeDef = adsk.fusion.BRepEdgeDefinition.cast(None)
    # Create vertex definition
    startVertexDef = brepBodyDef.createVertexDefinition(startPoint)
    endVertexDef = brepBodyDef.createVertexDefinition(endPoint)  
    curve = adsk.core.Line3D.create(startPoint, endPoint)
    # Create edge definition by curve
    edgeDef = brepBodyDef.createEdgeDefinitionByCurve(startVertexDef, endVertexDef, curve)
    
    return edgeDef   

def GetNurbsSurfaceMiddle1():
    surface = adsk.core.NurbsSurface.cast(None)
    
    degreeU = 1
    degreeV = 2
    controlPointCountU = 2
    controlPointcountV = 7
    controlPoints = []
    controlPoints.append(adsk.core.Point3D.create(-3.519568866597467, 0.0, 1.5488831005318966))
    controlPoints.append(adsk.core.Point3D.create(-2.0550441756714655, 0.0, 2.1464976398353937))
    controlPoints.append(adsk.core.Point3D.create(-1.0943307562984703, 0.0, 0.8899128103250742))
    controlPoints.append(adsk.core.Point3D.create(-0.13361733692547517, 0.0, -0.36667201918524484))
    controlPoints.append(adsk.core.Point3D.create(-1.0943307562984705, 0.0, -1.6232568486955643))
    controlPoints.append(adsk.core.Point3D.create(-2.0550441756714664, 0.0, -2.8798416782058833))
    controlPoints.append(adsk.core.Point3D.create(-3.5195688665974676, 0.0, -2.2822271389023863))
    controlPoints.append(adsk.core.Point3D.create(-3.519568866597467, 4.0, 1.5488831005318957))
    controlPoints.append(adsk.core.Point3D.create(-2.0550441756714655, 4.0, 2.1464976398353928))
    controlPoints.append(adsk.core.Point3D.create(-1.0943307562984703, 4.0, 0.8899128103250733))
    controlPoints.append(adsk.core.Point3D.create(-0.13361733692547517, 4.0, -0.3666720191852457))
    controlPoints.append(adsk.core.Point3D.create(-1.0943307562984705, 4.0, -1.6232568486955652))
    controlPoints.append(adsk.core.Point3D.create(-2.0550441756714664, 4.0, -2.879841678205884))
    controlPoints.append(adsk.core.Point3D.create(-3.5195688665974676, 4.0, -2.282227138902387))
    knotsU = (-0.9666972787471603, -0.9666972787471603, 0.9666972787471603, 0.9666972787471603)
    knotsV = (-1.9582319586428225, -1.9582319586428225, -1.9582319586428225, -0.6527439862142743, -0.6527439862142743, 0.6527439862142739, 0.6527439862142739, 1.9582319586428225, 1.9582319586428225, 1.9582319586428225)
    weights = (1.0, 0.7944201804397665, 1.0, 0.7944201804397665, 1.0, 0.7944201804397665, 1.0, 1.0, 0.7944201804397665, 1.0, 0.7944201804397665, 1.0, 0.7944201804397665, 1.0)
    propertyU = 9
    propertyV = 9
    surface = adsk.core.NurbsSurface.create(degreeU, degreeV, controlPointCountU, controlPointcountV, controlPoints, knotsU, knotsV, weights, propertyU, propertyV)

    return surface
    
def GetNurbsSurfaceMiddle2():
    surface = adsk.core.NurbsSurface.cast(None)
    
    degreeU = 1
    degreeV = 1
    controlPointCountU = 2
    controlPointcountV = 2
    controlPoints = []
    controlPoints.append(adsk.core.Point3D.create(-3.519568866597467, 4.0, -2.282227138902387))
    controlPoints.append(adsk.core.Point3D.create(-3.519568866597467, 0, -2.2822271389023863))
    controlPoints.append(adsk.core.Point3D.create(-3.519568866597467, 4.0, 1.5488831005318957))
    controlPoints.append(adsk.core.Point3D.create(-3.519568866597467, 0, 1.5488831005318966))
    knotsU = (0, 0, 3.831110239434283, 3.831110239434283)
    knotsV = (0, 0, 4.0, 4.0)
    weights = ()
    propertyU = 1
    propertyV = 1
    surface = adsk.core.NurbsSurface.create(degreeU, degreeV, controlPointCountU, controlPointcountV, controlPoints, knotsU, knotsV, weights, propertyU, propertyV)
    
    return surface
    
def GetNurbsSurfaceTop():
    surface = adsk.core.NurbsSurface.cast(None)
    
    degreeU = 1
    degreeV = 1
    controlPointCountU = 2
    controlPointcountV = 2
    controlPoints = []
    controlPoints.append(adsk.core.Point3D.create(-3.519568866597467, 4.0, 1.7441010118322937))
    controlPoints.append(adsk.core.Point3D.create(-3.519568866597467, 4.0, -2.4732150714952117))
    controlPoints.append(adsk.core.Point3D.create(-0.6323626099308138, 4.0, 1.7441010118322937))
    controlPoints.append(adsk.core.Point3D.create(-0.6323626099308138, 4.0, -2.4732150714952117))
    knotsU = (-0.7816621989871636, -0.7816621989871636, 2.1055440576794897, 2.1055440576794897)
    knotsV = (-2.110773031017539, -2.110773031017539, 2.1065430523099655, 2.1065430523099655)
    weights = ()
    propertyU = 1
    propertyV = 1
    surface = adsk.core.NurbsSurface.create(degreeU, degreeV, controlPointCountU, controlPointcountV, controlPoints, knotsU, knotsV, weights, propertyU, propertyV)
    
    return surface
    
def GetNurbsSurfaceBottom():
    surface = adsk.core.NurbsSurface.cast(None)
    
    degreeU = 1
    degreeV = 1
    controlPointCountU = 2
    controlPointcountV = 2
    controlPoints = []
    controlPoints.append(adsk.core.Point3D.create(-0.6323626099308726, 0.0, 1.7398710331248253))
    controlPoints.append(adsk.core.Point3D.create(-0.6323626099308726, 0.0, -2.4774450502028094))
    controlPoints.append(adsk.core.Point3D.create(-3.5195688665974676, 0.0, 1.739871033124825))
    controlPoints.append(adsk.core.Point3D.create(-3.5195688665974676, 0.0, -2.47744505020281))
    knotsU = (-2.105544057679431, -2.105544057679431, 0.7816621989871635, 0.7816621989871635)
    knotsV = (-2.10654305231007, -2.10654305231007, 2.110773031017565, 2.110773031017565)
    weights = ()
    propertyU = 1
    propertyV = 1
    surface = adsk.core.NurbsSurface.create(degreeU, degreeV, controlPointCountU, controlPointcountV, controlPoints, knotsU, knotsV, weights, propertyU, propertyV)    
    
    return surface
    
def CreateFaceForBodyDefination(brepBodyDef, surface, edgeDefs):   
    faceDef = adsk.fusion.BRepFaceDefinition.cast(None)
    
    # Create lump definition
    lumpDefs = brepBodyDef.lumpDefinitions
    lumpDef = lumpDefs.add()
    # Create shell definition
    shellDefs = lumpDef.shellDefinitions
    shellDef = shellDefs.add()
    # Create face definition
    faceDefs = shellDef.faceDefinitions
    faceDef = faceDefs.add(surface, True)

    # Create loop definition
    loopDefs = faceDef.loopDefinitions
    loopdef = loopDefs.add()
    
    # Create coEdge definitions
    brepCoEdgeDefs = loopdef.bRepCoEdgeDefinitions   
    for edgeDef in edgeDefs:
        brepCoEdgeDefs.add(edgeDef, True)

    return faceDef   
    
def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface
        
        # Create a new document
        doc = app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        
        # Create a base feature
        rootComp = design.rootComponent
        
        # Create brep body definition     
        brepBodyDef = adsk.fusion.BRepBodyDefinition.create()
        
        # Create edge definition
        top1EdgeDef = CreateTop1EdgeDefinitionByCurve(brepBodyDef)
        bottom1EdgeDef = CreateBottom1EdgeDefinitionByCurve(brepBodyDef)
        
        startPoint = adsk.core.Point3D.create(-3.519568866597467, 4.0, -2.282227138902387)
        endPoint = adsk.core.Point3D.create(-3.519568866597467, 4.0, 1.5488831005318957)
        top2EdgeDef = CreateLinearEdgeDefinitionByCurve(brepBodyDef, startPoint, endPoint)
        
        startPoint = adsk.core.Point3D.create(-3.519568866597467, 0.0, 1.5488831005318966)
        endPoint = adsk.core.Point3D.create(-3.519568866597467, 0.0, -2.2822271389023863)
        bottom2EdgeDef = CreateLinearEdgeDefinitionByCurve(brepBodyDef, startPoint, endPoint)
        
        startPoint = adsk.core.Point3D.create(-3.519568866597467, 0.0, -2.2822271389023863)
        endPoint = adsk.core.Point3D.create(-3.519568866597467, 4.0, -2.282227138902387)
        middle1EdgeDef = CreateLinearEdgeDefinitionByCurve(brepBodyDef, startPoint, endPoint)
        
        startPoint = adsk.core.Point3D.create(-3.519568866597467, 4.0, 1.5488831005318957)
        endPoint = adsk.core.Point3D.create(-3.519568866597467, 0.0, 1.5488831005318966)
        middle2EdgeDef = CreateLinearEdgeDefinitionByCurve(brepBodyDef, startPoint, endPoint)

        # Create the top face for body definition
        topSurface = GetNurbsSurfaceTop() 
        topEdgeDefs = []
        topEdgeDefs.append(top1EdgeDef)
        topEdgeDefs.append(top2EdgeDef)
        CreateFaceForBodyDefination(brepBodyDef, topSurface, topEdgeDefs)
        # Create one middle face for body definition
        middel1Surface = GetNurbsSurfaceMiddle1()
        middel1EdgeDefs = []
        middel1EdgeDefs.append(top1EdgeDef)
        middel1EdgeDefs.append(middle1EdgeDef)
        middel1EdgeDefs.append(bottom1EdgeDef)
        middel1EdgeDefs.append(middle2EdgeDef)
        CreateFaceForBodyDefination(brepBodyDef, middel1Surface, middel1EdgeDefs)
        # Create another middle face for body defination        
        middel2Surface = GetNurbsSurfaceMiddle2()
        middel2EdgeDefs = []
        middel2EdgeDefs.append(top2EdgeDef)
        middel2EdgeDefs.append(middle1EdgeDef)
        middel2EdgeDefs.append(bottom2EdgeDef)
        middel2EdgeDefs.append(middle2EdgeDef)         
        CreateFaceForBodyDefination(brepBodyDef, middel2Surface, middel2EdgeDefs)
        # Create the bottom face for body definition        
        bottomSurface = GetNurbsSurfaceBottom()
        bottomEdgeDefs = []
        bottomEdgeDefs.append(bottom1EdgeDef)
        bottomEdgeDefs.append(bottom2EdgeDef)
        CreateFaceForBodyDefination(brepBodyDef, bottomSurface, bottomEdgeDefs)

        # Create brep body      
        body = brepBodyDef.createBody() 
        outcomeInfo = brepBodyDef.outcomeInfo
        print('The outcome info is :\n{}'.format(outcomeInfo))
        
        # Create wire brep body definition
        wireBrepBodyDef = adsk.fusion.BRepBodyDefinition.create() 
        # Create lump definition
        lumpDefs = wireBrepBodyDef.lumpDefinitions
        lumpDef = lumpDefs.add()
        # Create shell definition
        shellDefs = lumpDef.shellDefinitions
        shellDef = shellDefs.add()
        # Create wire edge definitions        
        wireDef = shellDef.wireDefinition
        wireEdgeDefs = wireDef.wireEdgeDefinitions
        
        # Add wire edge definition
        startPoint = adsk.core.Point3D.create(2.499830308814756, 4.786795576310287, 0.0)
        endPoint = adsk.core.Point3D.create(6.000163479757444, 2.0712231354540322, 0.0)
        line = adsk.core.Line3D.create(startPoint, endPoint)
                
        center = adsk.core.Point3D.create(3.3848741924869366, 2.313878842520367, 0.0)
        normal =adsk.core.Vector3D.create(0, 0, 1.0)
        circle = adsk.core.Circle3D.createByCenter(center, normal, 2.626)

        startVertexDef = wireBrepBodyDef.createVertexDefinition(startPoint)
        endVertexDef = wireBrepBodyDef.createVertexDefinition(endPoint) 
        lineWireEdgeDef = wireEdgeDefs.add(startVertexDef, endVertexDef, line)
        circleWireEdgeDef = wireEdgeDefs.add(startVertexDef, endVertexDef, circle)
        
        # Set wire edge associative id
        lineWireEdgeDef.associativeID = 10
        print('The linear wire edge defination associative id is: {}'.format(lineWireEdgeDef.associativeID))
              
        # Create wire brep body 
        wireBrepBodyDef.doFullHealing = False
        wireBrepBody = wireBrepBodyDef.createBody()
        print('The wire brep body defination do full healing is: {}'.format(wireBrepBodyDef.doFullHealing))
        
        # Show the temporary brep bodies
        group = rootComp.customGraphicsGroups.add()
        group.addBRepBody(body)
        
        for edge in wireBrepBody.edges:
            group.addCurve(edge.geometry)

    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))