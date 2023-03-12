#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/Document.h>
#include <Core/Application/Documents.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Fusion/Design.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepWires.h>
#include <Fusion/BRep/BRepWire.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/BRep/TemporaryBRepManager.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/Geometry/OrientedBoundingBox3D.h>
#include <Core/Geometry/Plane.h>
#include <Core/Geometry/Matrix3D.h>
#include <Core/Geometry/Circle3D.h>
#include <Core/Geometry/Curve3D.h>
#include <Fusion/Graphics/CustomGraphicsGroups.h>
#include <Fusion/Graphics/CustomGraphicsGroup.h>


using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;
Ptr<UserInterface> ui;

std::string getDllPath();

Ptr<BRepBody> CreateBox()
{
 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return nullptr;

 Ptr<Point3D> centerPoint = Point3D::create(0.0, 10.0, 0.0);
 Ptr<Vector3D> lengthDir = Vector3D::create(1.0, 0.0, 0.0);
 Ptr<Vector3D> widthDir = Vector3D::create(0.0, 1.0, 0.0);

 Ptr<OrientedBoundingBox3D> orientedBoundingBox3D =
  OrientedBoundingBox3D::create(centerPoint, lengthDir, widthDir, 5.0, 6.0, 2.0);

 // Create box
 Ptr<BRepBody> box = tempBRepMgr->createBox(orientedBoundingBox3D);

 return box;
}

Ptr<BRepBody> CreateCylinderOrCone(double radius1, double radius2)
{
 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return nullptr;

 Ptr<Point3D> point1 = Point3D::create(3.0, 3.0, 3.0);
 Ptr<Point3D> point2 = Point3D::create(-3.0, -3.0, -3.0);

 // Create Cylinder Or Cone
 Ptr<BRepBody> cylinderOrCone = 
  tempBRepMgr->createCylinderOrCone(point1, radius1, point2, radius2);

 return cylinderOrCone;
}

Ptr<BRepBody> CreateEllipticalCylinderOrCone(double majorRadius1, double minorRadius1, double majorRadius2)
{
 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return nullptr;

 Ptr<Point3D> pointOne = Point3D::create(8.0, 3.0, 0.0);
 Ptr<Point3D> pointTwo = Point3D::create(8.0, -3.0, -3.0);
 Ptr<Vector3D> majorAxisDirection = Vector3D::create(1.0, 1.0, 1.0);

 // Create EllipticalCylinder Or Cone
 Ptr<BRepBody> ellipicalCylinderOrCone = 
  tempBRepMgr->createEllipticalCylinderOrCone(pointOne, 
             majorRadius1,
             minorRadius1,
             pointTwo,
             majorRadius2,
             majorAxisDirection
             );
 return ellipicalCylinderOrCone;
}

Ptr<BRepBody> CreateSphere(double radius)
{
 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return nullptr;

 Ptr<Point3D> sphereCenter = Point3D::create(0.0, 8.0, 8.0);
 // Create sphere
 Ptr<BRepBody> sphere = tempBRepMgr->createSphere(sphereCenter, radius);

 return sphere;
}

Ptr<BRepBody> CreateTorus(double majorRadius, double minorRadius)
{
 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return nullptr;

 Ptr<Vector3D> axis = Vector3D::create(0.0, 0.0, 10.0);
 Ptr<Point3D> torusCenter = Point3D::create(0.0, 2.0, 8.0);

 // Create torus
 Ptr<BRepBody> torus = tempBRepMgr->createTorus(torusCenter, axis, majorRadius, minorRadius);

 return torus;
}

Ptr<BRepBody> PlaneIntersection(const Ptr<BRepBody> &body)
{
 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return nullptr;

 Ptr<Point3D> planeOrigin = Point3D::create(0.0, 0.0, 0.0);
 Ptr<Vector3D> planeNormal = Vector3D::create(0.0, 0.0, 1.0);
 Ptr<Plane> plane = Plane::create(planeOrigin, planeNormal);

 // creates a brep body by the intersection between the input body and plane
 Ptr<BRepBody> intersectionBody = tempBRepMgr->planeIntersection(body, plane);

 return intersectionBody;
}

bool TransformBody(const Ptr<BRepBody> &body)
{
 bool isSuccess = false;

 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return isSuccess;

 Ptr<Matrix3D> transform = Matrix3D::create();
 Ptr<Vector3D> translation = Vector3D::create(0.0, 0.0, -5.0);
 isSuccess = transform->translation(translation);
 if(!isSuccess)
  return isSuccess;

 // Transforms the brep body using the specified transformation matrix
 isSuccess = tempBRepMgr->transform(body, transform);
 return isSuccess;
}

Ptr<BRepWire> GetWireFromBody(const Ptr<BRepBody> &body, size_t index)
{
 Ptr<BRepWires> brepWires = body->wires();
 if (!brepWires)
  return nullptr;

 Ptr<BRepWire> brepWire = brepWires->item(index);

 return brepWire;
}

bool DeleteFacesOnBody(const Ptr<BRepBody> &body, const std::vector<size_t> &indexes, bool isDeleteSpecifiedFaces)
{
 bool isSuccess = false;

 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return isSuccess;

 Ptr<BRepFaces> faces = body->faces();
 if (!faces)
  return isSuccess;

 std::vector< Ptr<BRepFace> > brepFaces;
 for each (size_t index in indexes)
 {
  Ptr<BRepFace> face = faces->item(index);
  if (!face)
   return isSuccess;

  brepFaces.push_back(face);
 }

 // Delete faces on body
 isSuccess = tempBRepMgr->deleteFaces(brepFaces, isDeleteSpecifiedFaces);
 return isSuccess;
}

Ptr<BRepBody> CreateSilhouette(const Ptr<BRepFace> &face)
{
 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return nullptr;

 Ptr<Vector3D> viewDirection = Vector3D::create(0.0, 0.0, 1.0);

 // Create silhouette body
 Ptr<BRepBody> silhouetteBody = tempBRepMgr->createSilhouetteCurves(face, viewDirection, true);

 return silhouetteBody;
}

Ptr<BRepBody> CreateWireFromCurves(std::vector< Ptr<BRepEdge> > &edgeMap)
{
 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return nullptr;

 Ptr<Point3D> center = Point3D::create(10.0, 0.0, 5.0);
 Ptr<Vector3D> normal = Vector3D::create(0.0, 0.0, 1.0);
 Ptr<Circle3D> circle1 = Circle3D::createByCenter(center, normal, 3.0);
 Ptr<Circle3D> circle2 = Circle3D::createByCenter(center, normal, 2.0);

 std::vector< Ptr<Curve3D> > curves;
 curves.push_back(circle1);
 curves.push_back(circle2);

 // Create wire from curves
 Ptr<BRepBody> wireBody = tempBRepMgr->createWireFromCurves(curves, edgeMap);

 return wireBody;
}

Ptr<BRepBody> CreateHelixBody()
{
 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return nullptr;

 Ptr<Point3D> axisPoint = Point3D::create(10.0, 10.0, 10.0);
 Ptr<Vector3D> axisVector = Vector3D::create(0.0, 1.0, 0.0);
 Ptr<Point3D> startPoint = Point3D::create(8.0, 2.0, 10.0);
 double pitch = 1.0;
 double turns = 10.0;
 double taperAngle = 0.3;

 // Create helix body
 Ptr<BRepBody> helixBody = tempBRepMgr->createHelixWire(axisPoint,
                 axisVector,
                 startPoint,
                 pitch,
                 turns,
                 taperAngle
                 );
 return helixBody;
}

extern "C" XI_EXPORT bool run(const char* context)
{
 app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;

 // Create a new document
 Ptr<Documents> docs = app->documents();
 if (!docs)
  return false;

 Ptr<Document> doc = docs->add(DocumentTypes::FusionDesignDocumentType);
 if (!doc)
  return false;

 Ptr<Design> design = app->activeProduct();
 if (!design)
  return false;

 //Set the design is a direct modelling design
 design->designType(adsk::fusion::DirectDesignType);

 //Get the root component of active design
 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 //Get bodies in root component
 Ptr<BRepBodies> bodies = rootComp->bRepBodies();
 if (!bodies)
  return false;

 //Get TemporaryBRepManager
 Ptr<TemporaryBRepManager> tempBRepMgr = TemporaryBRepManager::get();
 if (!tempBRepMgr)
  return false;


 // Create a temporary solid box
 Ptr<BRepBody> box = CreateBox();
 if (!box)
  return false;

 // Check the temporary box property
 bool isTemporary = box->isTemporary();

 // Creates a temporary solid cylinder and cone BRepBody
 Ptr<BRepBody> cylinder = CreateCylinderOrCone(3.0, 3.0);
 if (!cylinder)
  return false;

 Ptr<BRepBody> cone = CreateCylinderOrCone(2.0, 4.0);
 if (!cone)
  return false;

 // Creates a temporary elliptical solid cylinder and cone BrepBody
 Ptr<BRepBody> ellipicalCylinder = CreateEllipticalCylinderOrCone(4.0, 2.0, 4.0);
 if (!ellipicalCylinder)
  return false;

 Ptr<BRepBody> ellipicalCone = CreateEllipticalCylinderOrCone(3.0, 2.0, 4.5);
 if (!ellipicalCone)
  return false;

 // Creates a temporary spherical BRepBody
 Ptr<BRepBody> sphere = CreateSphere(3.0);
 if (!sphere)
  return false;

 // Creates a temporary toroidal BRepBody
 Ptr<BRepBody> torus = CreateTorus(2.0, 1.0);
 if (!torus)
  return false;

 // creates a brep body by the intersection between the input body and plane
 Ptr<BRepBody> intersectionBody = PlaneIntersection(box);
 if (!intersectionBody)
  return false;

 // Creates a temporary copy of the input BRepBody
 Ptr<BRepBody> copiedBody = tempBRepMgr->copy(intersectionBody);
 if (!copiedBody)
  return false;

 // Transforms the brep body using the specified transformation matrix
 bool isSuccess = TransformBody(copiedBody);
 if(!isSuccess)
  return false;

 // Transforms the brep body using the specified transformation matrix
 isSuccess = TransformBody(copiedBody);
 if (!isSuccess)
  return false;

 // Get brep wires
 Ptr<BRepWire> brepWire = GetWireFromBody(intersectionBody, 0);
 if (!brepWire)
  return false;
 Ptr<BRepWire> copiedWire = GetWireFromBody(copiedBody, 0);
 if (!copiedWire)
  return false;

 //  Creates a new body by creating a ruled surface between the two input wire bodies.
 Ptr<BRepBody> surfaceBody = tempBRepMgr->createRuledSurface(brepWire, copiedWire);
 if (!surfaceBody)
  return false;

 // Performs the specified Boolean operation between the two input bodies
 isSuccess = tempBRepMgr->booleanOperation(cylinder, cone, BooleanTypes::IntersectionBooleanType);
 if (!isSuccess)
  return false;

 isSuccess = tempBRepMgr->booleanOperation(ellipicalCylinder, ellipicalCone, BooleanTypes::UnionBooleanType);
 if (!isSuccess)
  return false;

 // Delete face from a temporary BRepBody
 std::vector<size_t> indexes;
 indexes.push_back(0);
 indexes.push_back(2);
 isSuccess = DeleteFacesOnBody(ellipicalCylinder, indexes, true);
 if (!isSuccess)
  return false;

 // Calculates the silhouette curve geometry for a given face as viewed from a given direction.
 Ptr<BRepFaces> faces = torus->faces();
 if (!faces)
  return false;

 Ptr<BRepFace> face = faces->item(0);
 if (!face)
  return false;

 Ptr<BRepBody> silhouetteBody = CreateSilhouette(face);
 if (!silhouetteBody)
  return false;

 // Create wire from curves
 std::vector< Ptr<BRepEdge> > edgeMap;
 Ptr<BRepBody> wireBody = CreateWireFromCurves(edgeMap);
 if (!wireBody)
  return false;

 // Creates a body from multiple wires that all lie within the same plane
 std::vector< Ptr<BRepBody> > wireBodies;
 wireBodies.push_back(wireBody);
 Ptr<BRepBody> planerBody = tempBRepMgr->createFaceFromPlanarWires(wireBodies);
 if (!planerBody)
  return false;

 // Create helix wire body
 Ptr<BRepBody> helixBody = CreateHelixBody();
 if (!helixBody)
  return false;

 // Display the helix edges
 Ptr<CustomGraphicsGroups> groups = rootComp->customGraphicsGroups();
 if (!groups)
  return false;

 Ptr<CustomGraphicsGroup> group = groups->add();
 if (!group)
  return false;

 Ptr<BRepEdges> edges = helixBody->edges();

 for each (Ptr<BRepEdge> edge in edges)
 {
  Ptr<Curve3D> curve = edge->geometry();
  if (!curve)
   return false;

  group->addCurve(curve);
 }
 
 // Add the temprary bodies to direct modeling design, then the temprary bodies will be displayed
 bodies->add(box);
 bodies->add(cylinder);
 bodies->add(cone);
 bodies->add(ellipicalCylinder);
 bodies->add(ellipicalCone);
 bodies->add(sphere);
 bodies->add(torus);
 bodies->add(intersectionBody);
 bodies->add(surfaceBody);
 bodies->add(silhouetteBody);
 bodies->add(planerBody);
 
 // Exports the input bodies to the specified file.
 std::vector< Ptr<BRepBody> > brepBodies;
 brepBodies.push_back(box);
 brepBodies.push_back(surfaceBody);

 std::string dllPath = getDllPath();
 std::string exportPath = dllPath + "/" + "sampleFile.smt";

 isSuccess = tempBRepMgr->exportToFile(brepBodies, exportPath);
 if (!isSuccess)
  return false;

 // Creates new BRepBody objects based on the contents of the specified file
 std::string filePath = dllPath + "/" + "sampleFile.smt";
 Ptr<BRepBodies> newBodies = tempBRepMgr->createFromFile(filePath);
 if (!newBodies)
  return false;

 return true;
}

#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
 switch (reason)
 {
 case DLL_PROCESS_ATTACH:
 case DLL_THREAD_ATTACH:
 case DLL_THREAD_DETACH:
 case DLL_PROCESS_DETACH:
  break;
 }
 return TRUE;
}

#endif // XI_WIN

std::string getDllPath()
{
#if defined(_WINDOWS) || defined(_WIN32) || defined(_WIN64)
 HMODULE hModule = NULL;
 if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
  (LPCSTR)&getDllPath,
  &hModule))
  return "";

 char winTempPath[2048];
 ::GetModuleFileNameA(hModule, winTempPath, 2048);

 std::string strPath = winTempPath;
 size_t stPos = strPath.rfind('\\');
 return strPath.substr(0, stPos);
#else
 Dl_info info;
 dladdr((void*)getDllPath, &info);

 std::string strPath = info.dli_fname;
 int stPos = (int)strPath.rfind('/');
 if (stPos != -1)
  return strPath.substr(0, stPos);
 else
  return "";;
#endif
}