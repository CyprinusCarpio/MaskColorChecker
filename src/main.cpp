#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Pixmap.H>

#include "MaskColorCheckerGUI.h"
#include "Program.h"
#include "icon.xpm"

#if defined(_WIN32) && defined(RELEASE_BUILD)
#include <Windows.h>
#include <shellapi.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
	Fl_Pixmap iconPixmap(icon_xpm);
	Fl_RGB_Image icon(&iconPixmap);
	Fl_Window::default_icon(&icon);

	Fl_Double_Window* wnd = make_window();
	Fl::scheme("gtk+");
	InitializeProgram();

#if defined(_WIN32) && defined(RELEASE_BUILD)
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	wnd->show(argc, (char**)argv);
	LocalFree(argv);
#else
	wnd->show(argc, argv);
#endif
	while (true)
	{
		int ret = Fl::check();

		if (ret == 0)
		{
			break;
		}
	}
	ShutdownProgram();
	return 0;
}