#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/ValueInput.h>
#include <Core/Application/Document.h>

#include <Fusion/Fusion/Design.h>
#include <Fusion/Components/Component.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepFaces.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/Features/Features.h>
#include <Fusion/Features/FilletEdgeSetInputs.h>
#include <Fusion/Features/FilletFeatures.h>
#include <Fusion/Features/FilletFeature.h>
#include <Fusion/Features/FilletFeatureInput.h>
#include <Fusion/Features/ConstantRadiusFilletEdgeSetInput.h>
#include <Fusion/Features/ChordLengthFilletEdgeSetInput.h>
#include <Fusion/Features/VariableRadiusFilletEdgeSetInput.h>
#include <Fusion/Features/VariableRadiusFilletEdgeSet.h>
#include <Fusion/Features/ChordLengthFilletEdgeSetInput.h>
#include <Fusion/Features/ChordLengthFilletEdgeSet.h>
#include <Fusion/Features/ConstantRadiusFilletEdgeSet.h>



using namespace adsk::core;
using namespace adsk::fusion;

Ptr<UserInterface> ui;

extern "C" XI_EXPORT bool run(const char* context)
{
 Ptr<Application> app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;


 // Get active design
 Ptr<Product> product = app->activeProduct();
 if (!product)
  return false;

 Ptr<Design> design = product;
 if (!design)
  return false;

 // Get root component in this design
 Ptr<Component> rootComp = design->rootComponent();
 if (!rootComp)
  return false;

 // Get fillet features
 Ptr<Features> features = rootComp->features();
 if (!features)
  return false;

 Ptr<FilletFeatures> fillets = features->filletFeatures();
 if (!fillets)
  return false;

 // Get edges of top face in the first body
 Ptr<BRepBodies> bodies = rootComp->bRepBodies();
 if (!bodies)
  return false;

 Ptr<BRepBody> body1 = bodies->item(0);
 if (!body1)
  return false;

 Ptr<BRepFaces> faces1 = body1->faces();
 if (!faces1)
  return false;

 Ptr<BRepFace> face1 = faces1->item(0);
 if (!face1)
  return false;

 Ptr<BRepEdges> edges1 = face1->edges();
 if (!edges1)
  return false;

 // Create constant-radius fillet
 Ptr<ObjectCollection> edgeCollection1 = ObjectCollection::create();
 if (!edgeCollection1)
  return false;

 for (Ptr<BRepEdge> edge : edges1)
  edgeCollection1->add(edge);

 Ptr<ValueInput> radius1 = ValueInput::createByReal(0.3);
 if (!radius1)
  return false;

 Ptr<FilletFeatureInput> input1 = fillets->createInput();
 if (!input1)
  return false;

 input1->isRollingBallCorner(true);

 Ptr<FilletEdgeSetInputs> edgeSets1 = input1->edgeSetInputs();
 if (!edgeSets1)
  return false;

 Ptr<ConstantRadiusFilletEdgeSetInput> constRadiusInput = edgeSets1->addConstantRadiusEdgeSet(edgeCollection1, radius1, true);
 if (!constRadiusInput)
  return false;

 constRadiusInput->continuity(adsk::fusion::SurfaceContinuityTypes::TangentSurfaceContinuityType); 

 Ptr<FilletFeature> fillet1 = fillets->add(input1);
 if (!fillet1)
  return false;

 fillet1->deleteMe();

 // Create variable-radius fillet
 Ptr<ObjectCollection> edgeCollection2 = ObjectCollection::create();
 if (!edgeCollection2)
  return false;

 Ptr<BRepEdge> edge1 = edges1->item(0);
 if (!edge1)
  return false;
 edgeCollection2->add(edge1);

 Ptr<ValueInput> startRadius = ValueInput::createByString("1.0 mm");
 if (!startRadius)
  return false;

 Ptr<ValueInput> endRadius = ValueInput::createByString("5.0 mm");
 if (!endRadius)
  return false;

 Ptr<FilletFeatureInput> input2 = fillets->createInput();
 if (!input2)
  return false;

 input2->isRollingBallCorner(false);

 Ptr<FilletEdgeSetInputs> edgeSets2 = input2->edgeSetInputs();
 if (!edgeSets2)
  return false;

 Ptr<VariableRadiusFilletEdgeSetInput> varRadiusEdgeSetInput = edgeSets2->addVariableRadiusEdgeSet(edgeCollection2, startRadius, endRadius, false);

 std::vector<Ptr<ValueInput>> radii;
 radii.push_back(ValueInput::createByString("2.0 mm"));
 radii.push_back(ValueInput::createByString("3.0 mm"));
 std::vector<Ptr<ValueInput>> positions;
 positions.push_back(ValueInput::createByReal(0.3));
 positions.push_back(ValueInput::createByReal(0.6));
 varRadiusEdgeSetInput->setMidRadii(radii, positions);

 Ptr<FilletFeature> fillet2 = fillets->add(input2);
 if (!fillet2)
  return false;

 fillet2->deleteMe();

 // Create chord-length fillet
 Ptr<FilletFeatureInput> input3 = fillets->createInput(); 
 if (!input3)
  return false;

 input3->isRollingBallCorner(true); 

 Ptr<FilletEdgeSetInputs> edgeSets3 = input3->edgeSetInputs();
 if (!edgeSets3)
  return false;

 Ptr<ValueInput> chordLength = ValueInput::createByReal(1.0);
 if (!chordLength)
  return false;

 Ptr<ChordLengthFilletEdgeSetInput> chordLengthInputSet = edgeSets3->addChordLengthEdgeSet(edgeCollection1, chordLength, true);
 if (!chordLengthInputSet)
  return false;

 chordLengthInputSet->continuity(adsk::fusion::SurfaceContinuityTypes::TangentSurfaceContinuityType);
 Ptr<FilletFeature> fillet3 = fillets->add(input3);
 if (!fillet3)
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