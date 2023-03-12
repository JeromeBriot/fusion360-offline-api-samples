#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <CAM/CAMAll.h>

using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;

extern "C" XI_EXPORT bool run(const char* context)
{
 Ptr<Application> app = Application::get();
 Ptr<UserInterface> ui = app->userInterface();
 // Get the CAM product
 Ptr<CAM> cam = app->activeDocument()->products()->itemByProductType("CAMProductType");

 // Define the path for the template
 std::string path = "E://face.f3dhsm-template";
 
 // iterate through each setup and insert the template into the setup
 for each (Ptr<Setup> setup in cam->setups()) 
 {
  // Add the template to each setup.
  Ptr<ObjectCollection> results = setup->createFromTemplate(path);

  // Get the operation that was created.What's created will
  // vary depending on what's defined in the template so you
  // may need more logic to find the result you want.
  Ptr<Operation> operation = results->item(0);

  operation->name("API added operation");
 }

 // Generate all toolpaths, skipping any that are already valid
 cam->generateAllToolpaths(true);
 return true;
}