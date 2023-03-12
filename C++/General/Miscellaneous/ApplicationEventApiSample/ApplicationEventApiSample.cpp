#include <Core/Application/Application.h>
#include <Core/Application/ApplicationEvents.h>
#include <Core/Application/DocumentEvents.h>
#include <Core/UserInterface/UserInterface.h>

using namespace adsk::core;

Ptr<Application> app;
Ptr<UserInterface> ui;

// Event handler for the documentCreated event.
class MyDocumentCreatedEventHandler : public adsk::core::DocumentEventHandler
{
public:
 void notify(const Ptr<DocumentEventArgs>& eventArgs) override
 {
  // Code to react to the event.
  ui->messageBox("In MyDocumentCreatedEventHandler event handler.");
 }
} documentCreated_;

// Event handler for the documentSaved event.
class MyDocumentSavedEventHandler : public adsk::core::DocumentEventHandler
{
public:
 void notify(const Ptr<DocumentEventArgs>& eventArgs) override
 {
  // Code to react to the event.
  ui->messageBox("In MyDocumentSavedEventHandler event handler.");
 }
} documentSaved_;

// Event handler for the documentSaving event.
class MyDocumentSavingEventHandler : public adsk::core::DocumentEventHandler
{
public:
 void notify(const Ptr<DocumentEventArgs>& eventArgs) override
 {
  // Code to react to the event.
  ui->messageBox("In MyDocumentSavingEventHandler event handler.");
 }
} documentSaving_;

// Event handler for the documentClosed event.
class MyDocumentClosedEventHandler : public adsk::core::DocumentEventHandler
{
public:
 void notify(const Ptr<DocumentEventArgs>& eventArgs) override
 {
  // Code to react to the event.
  ui->messageBox("In MyDocumentClosedEventHandler event handler.");
 }
} documentClosed_;

// Event handler for the documentClosing event.
class MyDocumentClosingEventHandler : public adsk::core::DocumentEventHandler
{
public:
 void notify(const Ptr<DocumentEventArgs>& eventArgs) override
 {
  // Code to react to the event.
  ui->messageBox("In MyDocumentClosingEventHandler event handler.");
 }
} documentClosing_;

class MyDocumentActivatedEventHandler : public adsk::core::DocumentEventHandler
{
public:
 void notify(const Ptr<DocumentEventArgs>& eventArgs) override
 {
  Ptr<Document> doc = eventArgs->document();

  // Code to react to the event.
  ui->messageBox("In MyDocumentActivatedEventHandler event handler.");
 }
} documentActivated_;

class MyDocumentDeactivatedEventHandler : public adsk::core::DocumentEventHandler
{
public:
 void notify(const Ptr<DocumentEventArgs>& eventArgs) override
 {
  Ptr<Document> doc = eventArgs->document();

  // Code to react to the event.
  ui->messageBox("In MyDocumentDeactivatedEventHandler event handler.");
 }
} documentDeactivated_;

class MyDocumentDeactivatingEventHandler : public adsk::core::DocumentEventHandler
{
public:
 void notify(const Ptr<DocumentEventArgs>& eventArgs) override
 {
  Ptr<Document> doc = eventArgs->document();

  // Code to react to the event.
  ui->messageBox("In MyDocumentDeactivatingEventHandler event handler.");
 }
} documentDeactivating_;

class MyDocumentActivatingEventHandler : public adsk::core::DocumentEventHandler
{
public:
 void notify(const Ptr<DocumentEventArgs>& eventArgs) override
 {
  Ptr<Document> doc = eventArgs->document();

  // Code to react to the event.
  ui->messageBox("In MyDocumentActivatingEventHandler event handler.");
 }
} documentActivating_;

class StartupCompletedHandler : public ApplicationEventHandler
{
public:
 void notify(const Ptr<ApplicationEventArgs>& eventArgs) override
 {
  if(ui)
   ui->messageBox("Startup completed");
 }
};
static StartupCompletedHandler startupCompletedHandler_;

class OnlineStatusChangedHandler : public ApplicationEventHandler
{
public:
 void notify(const Ptr<ApplicationEventArgs>& eventArgs) override
 {
  if(!eventArgs)
   return;

  std::string status("Online");
  if(eventArgs->isOffLine())
   status = "Offline";

  if(ui)
   ui->messageBox("Online status changed: " + status);
 }
};
static OnlineStatusChangedHandler onlineStatusChangedHandler_;

extern "C" XI_EXPORT bool run(const char* context)
{
 app = Application::get();
 if (!app)
  return false;

 ui = app->userInterface();
 if (!ui)
  return false;

 Ptr<ApplicationEvent> startupCompleted = app->startupCompleted();
 if (!startupCompleted)
  return false;
 startupCompleted->add(&startupCompletedHandler_);

 Ptr<ApplicationEvent> onlineStatusChanged = app->onlineStatusChanged();
 if (!onlineStatusChanged)
  return false;
 onlineStatusChanged->add(&onlineStatusChangedHandler_);

 Ptr<DocumentEvent> documentCreated = app->documentCreated();
 if (!documentCreated)
  return false;
 documentCreated->add(&documentCreated_);

 Ptr<DocumentEvent> documentSaved = app->documentSaved();
 if (!documentSaved)
  return false;
 documentSaved->add(&documentSaved_);

 Ptr<DocumentEvent> documentSaving = app->documentSaving();
 if (!documentSaving)
  return false;
 documentSaving->add(&documentSaving_);

 Ptr<DocumentEvent> documentClosed = app->documentClosed();
 if (!documentClosed)
  return false;
 documentClosed->add(&documentClosed_);

 Ptr<DocumentEvent> documentClosing = app->documentClosing();
 if (!documentClosing)
  return false;
 documentClosing->add(&documentClosing_);

 Ptr<DocumentEvent> documentActivated = app->documentActivated();
 if (!documentActivated)
  return false;
 documentActivated->add(&documentActivated_);

 Ptr<DocumentEvent> documentActivating = app->documentActivating();
 if (!documentActivating)
  return false;
 documentActivating->add(&documentActivating_);

 Ptr<DocumentEvent> documentDeactivating = app->documentDeactivating();
 if (!documentDeactivating)
  return false;
 documentDeactivating->add(&documentDeactivating_);

 Ptr<DocumentEvent> documentDeactivated = app->documentDeactivated();
 if (!documentDeactivated)
  return false;
 documentDeactivated->add(&documentDeactivated_);

 return true;
}

extern "C" XI_EXPORT bool stop(const char* context)
{
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