// XFilesHack.cpp : main project file.
#include "stdafx.h"
#include "../InjectionCore/InjectionCore.h"
#pragma comment(lib, "../Release/InjectionCore.lib")
#include "FormLogin.h"
#include "FormSelectGame.h"
#include "FormUpdate.h"
#include "Form1.h"


using namespace XFilesHack;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 
	// Create the main window and run it
	Application::Run(gcnew Form1());

	while(GetFormMainState() == false)Sleep(1000);
	return 0;
}
