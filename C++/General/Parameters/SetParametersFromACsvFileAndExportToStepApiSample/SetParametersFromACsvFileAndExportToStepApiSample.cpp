#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Fusion/Fusion/UserParameters.h>
#include <Fusion/Fusion/UserParameter.h>
#include <Fusion/Fusion/STEPExportOptions.h>
#include <Fusion/Fusion/ExportManager.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


using namespace adsk::core;
using namespace adsk::fusion;

Ptr<UserInterface> ui;

typedef std::vector<std::vector<std::string>> DataSet;

void loadCSVFile(const std::string& csvFilePath, DataSet& dataSet)
{
 std::ifstream infile( csvFilePath );

 while (infile)
 {
  std::string s;
  if (!getline( infile, s )) break;

  std::istringstream ss( s );
  std::vector<std::string> record;

  while (ss)
  {
   std::string s;
   if (!getline( ss, s, ',' )) break;
   record.push_back( s );
  }

  dataSet.push_back( record );
 }
}

std::string getTempPath()
{
 std::string strTempPath;
#ifdef XI_WIN
 char chPath[MAX_PATH];
 if (::GetTempPathA(MAX_PATH, chPath))
  strTempPath = chPath;
#else // Mac
 NSString * tempDir = NSTemporaryDirectory();
 if (tempDir == nil)
  tempDir = @"/tmp";
 strTempPath = [tempDir UTF8String];
#endif // XI_WIN
 return strTempPath;
}

extern "C" XI_EXPORT bool run(const char* context)
{
 Ptr<Application> app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;


 // Read the csv file.
 int cnt = 0;

 DataSet dataSet;
 loadCSVFile("C:\\Temp\\values.csv", dataSet);

 for (auto data : dataSet)
 {
  // Get the values from the csv file.
  std::string length = data.at(0);
  std::string width = data.at(1);
  std::string height = data.at(2);

  // Set the parameters.
  Ptr<UserParameters> userParams = design->userParameters();
  if (!userParams)
   return false;
  Ptr<UserParameter> lengthParam = userParams->itemByName("Length");
  if (!lengthParam)
   return false;
  lengthParam->expression(length);

  Ptr<UserParameter> widthParam = userParams->itemByName("Width");
  if (!widthParam)
   return false;
  widthParam->expression(width);

  Ptr<UserParameter> heightParam = userParams->itemByName("Height");
  if (!heightParam)
   return false;
  heightParam->expression(height);

  // Export the STEP file.
  Ptr<ExportManager> exportMgr = design->exportManager();
  if (!exportMgr)
   return false;
  std::string filename = "C:\\Temp\\test_box" + std::to_string(cnt) + ".stp";
  Ptr<STEPExportOptions> stepOptions = exportMgr->createSTEPExportOptions(filename);
  if (!stepOptions)
   return false;

  ++cnt;
  exportMgr->execute(stepOptions);
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