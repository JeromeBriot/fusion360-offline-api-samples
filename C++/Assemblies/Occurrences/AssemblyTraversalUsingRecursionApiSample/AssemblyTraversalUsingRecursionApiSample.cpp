#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Geometry/Matrix3D.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/Palettes.h>
#include <Core/UserInterface/TextCommandPalette.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Components/Occurrence.h>
#include <Fusion/Components/Occurrences.h>
#include <Fusion/Components/OccurrenceList.h>
#include <Fusion/Fusion/Design.h>

using namespace adsk::core;
using namespace adsk::fusion;

Ptr<UserInterface> ui;

// Returns a string containing the especified number of spaces.
std::string spaces(size_t spaceCount) {
 std::string result = "";
 for (size_t i = 0; i < spaceCount; ++i) {
  result += " ";
 }
 return result;
}

// Performs a recursive traversal of an entire assembly structure.
std::string traverseAssembly(Ptr<OccurrenceList> occurrences, size_t currentLevel, std::string inputString) {
 if(occurrences) {
  for (size_t i = 0; i < occurrences->count(); ++i) {
   Ptr<Occurrence> occ = occurrences->item(i);
   if(occ) {
    inputString += spaces(currentLevel * 5) + occ->name() + "\n";

    if (occ->childOccurrences()) {
     inputString = traverseAssembly(occ->childOccurrences(), currentLevel + 1, inputString);
    }
   }
  }
 }

 return inputString;
}

extern "C" XI_EXPORT bool run(const char* context)
{
 Ptr<Application> app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;


 Ptr<Product> product = app->activeProduct();
 if (!product)
  return false;

 Ptr<Design> design = product;
 if (!design)
  return false;

 // Get the root component of the active design
 Ptr<Component> rootComp = design->rootComponent();
 if(!rootComp)
  return false;

 Ptr<Occurrences> occurrences = rootComp->occurrences();
 if(!occurrences)
  return false;
 // Create the title for the output.
 std::string resultString = "Root (" + design->parentDocument()->name() + ")\n";

 // Call the recursive function to traverse the assembly and build the output string.
 resultString = traverseAssembly(occurrences->asList(), 1, resultString);
 
     // Write the results to the TEXT COMMANDS window.
     Ptr<TextCommandPalette> textPalette = ui->palettes()->itemById("TextCommands");
     if (!textPalette->isVisible())
         textPalette->isVisible(true);
     textPalette->writeText(resultString);
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