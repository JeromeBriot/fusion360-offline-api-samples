# Author-Autodesk Inc.
# Description-Display the arc and circle geometric information from a selected circular edge

import adsk.core, adsk.fusion, traceback

def getArcGeometryInfo(arcGeom):
    result = arcGeom.getData()
    if (result[0]):
        (retVal, center, axis, refVector, radius, startAngle, endAngle) = result
                    
        arcInfo = "Center: %.6f, %.6f, %.6f\n" % (center.x, center.y, center.z)
        arcInfo += "Axis: %.6f, %.6f, %.6f\n" % (axis.x, axis.y, axis.z)
        arcInfo += "Reference vector: %.6f, %.6f, %.6f\n" % (refVector.x, refVector.y, refVector.z)
        arcInfo += "Radius: %.6f\n" % radius
        arcInfo += "Start angle: %.6f\n" % startAngle
        arcInfo += "End angle: %.6f" % endAngle
        return arcInfo
        
def getCircleGeometryInfo(circGeom):
    result = circGeom.getData()                
    if (result[0]):
        (retVal, center, axis, radius) = result

        circleInfo = "Center: %.6f, %.6f, %.6f\n" % (center.x, center.y, center.z)
        circleInfo += "Axis: %.6f, %.6f, %.6f\n" % (axis.x, axis.y, axis.z)
        circleInfo += "Radius: %.6f" % radius
        return circleInfo

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        
        ent = ui.selectEntity("Select a circular edge", "CircularEdges")
        
        if (isinstance(ent.entity.geometry, adsk.core.Arc3D)): 
            arcGeom = ent.entity.geometry
            
            arcInfo = getArcGeometryInfo(arcGeom)
                            
            ui.messageBox(arcInfo, "Arc Info")
        else:  
            if (isinstance(ent.entity.geometry, adsk.core.Circle3D)):
                circGeom = ent.entity.geometry
                
                circleInfo = getCircleGeometryInfo(circGeom)
                
                ui.messageBox( circleInfo, "Circle Info")
    except:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))