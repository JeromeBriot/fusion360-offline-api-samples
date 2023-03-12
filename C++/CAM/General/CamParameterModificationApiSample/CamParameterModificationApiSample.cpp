#include <Core/CoreAll.h>
#include <CAM/CAMAll.h>

using namespace adsk::core;
using namespace adsk::cam;

extern "C" XI_EXPORT bool run(const char* context)
{
 Ptr<Document> doc = Application::get()->activeDocument();
 Ptr<CAM> cam = doc->products()->itemByProductType("CAMProductType");

 // List of CAM setups
 Ptr<Setups> setups = cam->setups();

 for each (Ptr<Setup> setup in setups) {
  // Change the program name for each setup
  setup->parameters()->itemByName("job_programName")->expression("12345");
  for each (Ptr<Operation> operation in setup->allOperations()) {
   // Change the tolerance of each operation
   operation->parameters()->itemByName("tolerance")->expression("0.1mm");
  }
 }

 // Generate all toolpaths
 cam->generateAllToolpaths(true);

 return true;
}