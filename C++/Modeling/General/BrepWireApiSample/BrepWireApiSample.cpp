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
#include <Fusion/BRep/BRepCoEdges.h>
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/BRep/BRepVertices.h>
#include <Core/Geometry/Matrix3D.h>
#include <Core/Geometry/Circle3D.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Fusion/Components/Occurrences.h>
#include <Fusion/Components/Occurrence.h>
#include <Fusion/Components/Component.h>
#include <Fusion/BRep/TemporaryBRepManager.h>


using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;
Ptr<UserInterface> ui;

Ptr<BRepBody> CreateWireBody(std::vector< Ptr<BRepEdge> > &edgeMap)
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

    // Create a sub component under root component
 Ptr<Occurrences> occs = rootComp->occurrences();
 if (!occs)
  return false;

 Ptr<Matrix3D> transform = Matrix3D::create();
 if (!transform)
  return false;

 Ptr<Occurrence> subOcc = occs->addNewComponent(transform);
 if (!subOcc)
  return false;

 Ptr<Component> subComp = subOcc->component();
 if (!subComp)
  return false;

 // Create temproray wire body
 std::vector< Ptr<BRepEdge> > edgeMap;
 Ptr<BRepBody> wireBody = CreateWireBody(edgeMap);
 if (!wireBody)
  return false;

 // Add wireBody to sub - component
 Ptr<BRepBodies> bodies = subComp->bRepBodies();
 if (!bodies)
  return false;
 bodies->add(wireBody);

 // Get the brep wire body
 Ptr<BRepBody> brepWireBody = bodies->item(0);
 if (!brepWireBody)
  return false;

 // Get brep wires
 Ptr<BRepWires> brepWires = brepWireBody->wires();
 if (!brepWires)
  return false;

 // Get brep wires count
 size_t count = brepWires->count();

 // Get brep wire
 Ptr<BRepWire> brepWire = brepWires->item(0);
 if (!brepWire)
  return false;

 // Get brep edges from brepWire
 Ptr<BRepEdges> brepEdges = brepWire->edges();
 if (!brepEdges)
  return false;

 // Get vertices from brepWire
 Ptr<BRepVertices> vertices = brepWire->vertices();
 if (!vertices)
  return false;

 // Get brep coEdges from brepWire
 Ptr<BRepCoEdges> coEdges = brepWire->coEdges();
 //ON_ERROR_RETURN(coEdges);
 //if (!coEdges)
 // return false;

 // Get the brep wire is planar or not
 bool isPlanar = brepWire->isPlanar();

 // Get wire's parent brep body
 Ptr<BRepBody> parent = brepWire->parent();
 if (!parent)
  return false;

 // offse the planar brep wire
  
 Ptr<Vector3D> planeNormal = Vector3D::create(0.0, 0.0, 1.0);
 Ptr<BRepBody> offsetBody = brepWire->offsetPlanarWire(planeNormal, 3.0, OffsetCornerTypes::LinearOffsetCornerType);
 if (!offsetBody)
  return false;

 // Create brep wire proxy
 Ptr<BRepWire> wireProxy = brepWire->createForAssemblyContext(subOcc);
 if (!wireProxy)
  return false;

 // Get native brep wire
 Ptr<BRepWire> nativeBrepWire = wireProxy->nativeObject();
 if (!nativeBrepWire)
  return false;

 // Get the assembly occurrence
 Ptr<Occurrence> occurence = wireProxy->assemblyContext();
 if (!occurence)
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