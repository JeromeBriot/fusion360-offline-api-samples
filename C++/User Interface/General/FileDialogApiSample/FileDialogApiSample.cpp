#include <Core/Application/Application.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/FileDialog.h>
#include <Core/UserInterface/FolderDialog.h>

using namespace adsk::core;

Ptr<UserInterface> ui;

extern "C" XI_EXPORT bool run(const char* context)
{
 Ptr<Application> app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;

 std::string msg("");

 // Set styles of file dialog.
 Ptr<FileDialog> fileDlg = ui->createFileDialog();
 if (!fileDlg)
  return false;
 fileDlg->title("Fusion File Dialog");
 fileDlg->filter("*.*");
 fileDlg->isMultiSelectEnabled(true);

 // Show file open dialog
 DialogResults dlgResult = fileDlg->showOpen();
 if (dlgResult == DialogOK) {
  msg += "\nFiles to Open:";
  std::vector<std::string> fileNames = fileDlg->filenames();
  for (std::string fileName : fileNames) {
   msg += "\n\t" + fileName;
  }
 }
 else {
  return false;
 }


 // Show file save dialog
 fileDlg->title("Fusion Save File Dialog");
 dlgResult = fileDlg->showSave();
 if (dlgResult == DialogOK) {
  std::string fileName = fileDlg->filename();
  msg += "\nFile to Save: " + fileName;
 }
 else {
  return false;
 }

 // Set styles of file dialog.
 Ptr<FolderDialog> folderDlg = ui->createFolderDialog();
 if (!folderDlg)
  return false;
 folderDlg->title("Fusion Choose Folder Dialog");

 // Show folder dialog
 dlgResult = folderDlg->showDialog();
 if (dlgResult == DialogOK) {
  std::string folderName = folderDlg->folder();
  msg += "\nSelected Folder: " + folderName;
 }
 else {
  return false;
 }

 if (!msg.empty())
  ui->messageBox(msg);
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