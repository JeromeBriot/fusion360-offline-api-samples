#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <sstream>
#include <iomanip>


using namespace adsk::core;
using namespace adsk::fusion;

Ptr<UserInterface> ui;

extern "C" XI_EXPORT bool run(const char* context)
{
    Ptr<Application> app = Application::get();
    if (!app)
        return false;

    ui  = app->userInterface();

    // Create a new document.
    Ptr<Documents> documents = app->documents();
    if (!documents)
        return false;
    Ptr<Document> doc = documents->add(DocumentTypes::FusionDesignDocumentType);
    if (!doc)
        return false;

    Ptr<Product> product = app->activeProduct();
    if (!product)
        return false;

    Ptr<Design> design = product;
    if (!design)
        return false;

    // Set the design type to DirectDesignType (for non-parametric modelling)        
    design->designType(adsk::fusion::DesignTypes::DirectDesignType);

    // Get the root component of the active design
    Ptr<Component> rootComp = design->rootComponent();
    if (!rootComp)
        return false;
        
    // Create the first component - containing a box
    Ptr<Occurrences> occurrences = rootComp->occurrences();
    if (!occurrences)
        return false;
    Ptr<Matrix3D> matrix = adsk::core::Matrix3D::create();
    if (!matrix)
        return false;
    Ptr<Occurrence> firstComponentOccurrence = occurrences->addNewComponent(matrix);
    if (!firstComponentOccurrence)
        return false;
        
    // Create sketch
    Ptr<Sketches> sketches = firstComponentOccurrence->component()->sketches();
    if (!sketches)
        return false;
    Ptr<Sketch> sketch = sketches->add(rootComp->xZConstructionPlane());
    if (!sketch)
        return false;
        
    // Create a rectangle
    Ptr<SketchLines> sketchLines = sketch->sketchCurves()->sketchLines();
    if (!sketchLines)
        return false;
    Ptr<Point3D> startPoint = adsk::core::Point3D::create(0, 0, 0);
    if (!startPoint)
        return false;
    Ptr<Point3D> endPoint = adsk::core::Point3D::create(5, 5, 0);
    if (!endPoint)
        return false;
    sketchLines->addTwoPointRectangle(startPoint, endPoint);
        
    // Get the profile defined by the rectangle
    Ptr<Profiles> profiles = sketch->profiles();
    if (!profiles)
        return false;
    Ptr<Profile> prof = profiles->item(0);
    if (!prof)
        return false;
        
    // Create an extrusion input for the profile.
    Ptr<Features> features = firstComponentOccurrence->component()->features();
    if (!features)
        return false;
    Ptr<ExtrudeFeatures> extrudes = features->extrudeFeatures();
    if (!extrudes)
        return false;
    Ptr<ExtrudeFeatureInput> extInput = extrudes->createInput(prof, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
    if (!extInput)
        return false;
        
    // Define that the extent of the extrusion is a distance extent of 5 cm.
    Ptr<ValueInput> distance = adsk::core::ValueInput::createByReal(5);
    if (!distance)
        return false;
    extInput->setDistanceExtent(false, distance);
        
    // Create the extrusion.
    Ptr<ExtrudeFeature> ext = extrudes->add(extInput);
    if (!ext)
        return false;

    // Create the second component - containing a box that overlaps the box in the first component
    Ptr<Occurrence> secondComponentOccurrence = occurrences->addNewComponent(matrix);
    if (!secondComponentOccurrence)
        return false;
        
    // Create sketch
    sketches = secondComponentOccurrence->component()->sketches();
    sketch = sketches->add(rootComp->xZConstructionPlane());
        
    // Create a rectangle
    sketchLines = sketch->sketchCurves()->sketchLines();
    startPoint = adsk::core::Point3D::create(3, 3, 0);
    endPoint = adsk::core::Point3D::create(8, 8, 0);
    sketchLines->addTwoPointRectangle(startPoint, endPoint);
        
    // Get the profile defined by the rectangle
    prof = sketch->profiles()->item(0);
        
    // Create an extrusion input for the profile.
    features = secondComponentOccurrence->component()->features();
    extrudes = features->extrudeFeatures();
    extInput = extrudes->createInput(prof, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
        
    // Define that the extent of the extrusion is a distance extent of 5 cm.
    distance = adsk::core::ValueInput::createByReal(5);
    extInput->setDistanceExtent(false, distance);
        
    // Create the extrusion.
    ext = extrudes->add(extInput);

    // Create the third component - containing a cylinder that overlaps the box in the second component
    Ptr<Occurrence> thirdComponentOccurrence = occurrences->addNewComponent(matrix);
    if (!thirdComponentOccurrence)
        return false;

    // Create sketch
    sketches = thirdComponentOccurrence->component()->sketches();
    sketch = sketches->add(rootComp->xZConstructionPlane());
        
    // Create a circle
    Ptr<SketchCircles> sketchCircles = sketch->sketchCurves()->sketchCircles();
    if (!sketchCircles)
        return false;
    Ptr<Point3D> centerPoint = adsk::core::Point3D::create(8, 8, 0);
    if (!centerPoint)
        return false;
    sketchCircles->addByCenterRadius(centerPoint, 2);
        
    // Get the profile defined by the circle
    prof = sketch->profiles()->item(0);
        
    // Create an extrusion input for the profile.
    features = thirdComponentOccurrence->component()->features();
    extrudes = features->extrudeFeatures();
    extInput = extrudes->createInput(prof, adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
        
    // Define that the extent of the extrusion is a distance extent of 5 cm.
    distance = adsk::core::ValueInput::createByReal(5);
    extInput->setDistanceExtent(false, distance);
        
    // Create the extrusion.
    ext = extrudes->add(extInput);

    // Create a collection of the components to check for interference
    Ptr<ObjectCollection> inputOccurrences = adsk::core::ObjectCollection::create();
    if (!inputOccurrences)
    return false;
    inputOccurrences->add(firstComponentOccurrence);
    inputOccurrences->add(secondComponentOccurrence);
    inputOccurrences->add(thirdComponentOccurrence);

    // Create the interferenceInput object and run the analysis.
    Ptr<InterferenceInput> interferenceInput = design->createInterferenceInput(inputOccurrences);
    if (!interferenceInput)
    return false;
    interferenceInput->areCoincidentFacesIncluded(false);
    Ptr<InterferenceResults> results = design->analyzeInterference(interferenceInput);
    if (!results)
    return false;

    // Create bodies for every intersection.  This is not supported in Parametric designs.
    Ptr<ObjectCollection> interferenceBodies = results->createBodies(true);
    if (!interferenceBodies)
    return false;

    Ptr<Occurrence> resultsOccurrence = occurrences->item(occurrences->count()-1);
    if (!resultsOccurrence)
    return false;
    resultsOccurrence->activate();

    // Fit the view        
    Ptr<Viewport> viewport = app->activeViewport();
    if (!viewport)
    return false;
    viewport->fit();

    // Report the results
    int bodCount = 0;
    for (Ptr<InterferenceResult> result : results) {
        Ptr<BRepBody>entOneBody = result->entityOne();
        if (!entOneBody)
            return false;
        std::string comp1Name = entOneBody->parentComponent()->name();

        Ptr<BRepBody>entTwoBody = result->entityTwo();
        if (!entTwoBody)
            return false;
        std::string comp2Name = entTwoBody->parentComponent()->name();

        double bodyVolume = result->interferenceBody()->volume();
        std::stringstream bodyVolumeString;
        bodyVolumeString << std::setprecision(2) << std::fixed << bodyVolume;
            
        Ptr<BRepBody> interferenceBody = interferenceBodies->item(bodCount);
        if (!interferenceBody)
            return false;
        bodCount++;
        interferenceBody->name("Interference between " + comp1Name + " & " + comp2Name);
        ui->messageBox("There is interference between " + comp1Name + " and " + comp2Name + " with a volume of " + bodyVolumeString.str() + " cubic centimeters");
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