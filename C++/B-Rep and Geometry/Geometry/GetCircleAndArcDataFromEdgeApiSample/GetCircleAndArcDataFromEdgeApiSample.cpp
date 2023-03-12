#include <Core/Application/Application.h>
#include <Core/Geometry/Arc3D.h>
#include <Core/Geometry/Circle3D.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/UserInterface/Selection.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/BRep/BRepEdge.h>


using namespace adsk::core;
using namespace adsk::fusion;

Ptr<UserInterface> ui;

namespace {
 std::string getArcGeometryInfo(Ptr<Arc3D> arcGeom)
 {
  std::string arcInfo;
  if(!arcGeom)
   return arcInfo;

  Ptr<Point3D> center;
  Ptr<Vector3D> axis;
  Ptr<Vector3D> refVector;
  double radius = 0.0;
  double startAngle = 0.0;
  double endAngle = 0.0;
  bool result = arcGeom->getData(center, axis, refVector, radius, startAngle, endAngle);
  if(!result)
   return arcInfo;

  std::stringstream ss;
  ss.precision(6);
  ss << std::fixed;
  ss << "Center: " << center->x() << ", " << center->y() << ", " << center->z() << "\n";
  ss << "Axis: " << axis->x() << ", " << axis->y() << ", " << axis->z() << "\n";
  ss << "Reference vector: " << refVector->x() << ", " << refVector->y() << ", " << refVector->z() << "\n";
  ss << "Radius: " << radius << "\n";
  ss << "Start angle: " << startAngle << "\n";
  ss << "End angle: " << endAngle << "\n";

  arcInfo += ss.str();
  return arcInfo;
 }

 std::string getCircleGeometryInfo(Ptr<Circle3D> circGeom)
 {
  std::string circleInfo;
  if(!circGeom)
   return circleInfo;

  Ptr<Point3D> center;
  Ptr<Vector3D> axis;
  double radius = 0.0;
  bool result = circGeom->getData(center, axis, radius);
  if(!result)
   return circleInfo;

  std::stringstream ss;
  ss.precision(6);
  ss << std::fixed;
  ss << "Center: " << center->x() << ", " << center->y() << ", " << center->z() << "\n";
  ss << "Axis: " << axis->x() << ", " << axis->y() << ", " << axis->z() << "\n";
  ss << "Radius: " << radius << "\n";

  circleInfo += ss.str();
  return circleInfo;
 }
}

extern "C" XI_EXPORT bool run(const char* context)
{
 Ptr<Application> app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;

 Ptr<Selection> selection = ui->selectEntity("Select a circular edge", "CircularEdges");
 if (!selection)
  return false;

 Ptr<BRepEdge> edge = selection->entity();
 if (!edge)
  return false;

 if (Ptr<Arc3D> arcGeom = edge->geometry()) {
  std::string arcInfo = getArcGeometryInfo(arcGeom);
  ui->messageBox(arcInfo, "Arc Info");
 }
 else if (Ptr<Circle3D> circGeom = edge->geometry()) {
  std::string circleInfo = getCircleGeometryInfo(circGeom);
  ui->messageBox(circleInfo, "Circle Info");
 }

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