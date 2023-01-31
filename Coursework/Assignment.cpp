#include "../NCLGL/window.h"
#include "Renderer.h"


int main() {
	Window w("Declan Agar Coursework", 1280, 720, false); //This is all boring win32 window creation stuff!
	//Window w("Declan Agar Coursework", 1920, 1080, true); //This is all boring win32 window creation stuff!
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w); //This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
			renderer.ToggleFog();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
			renderer.ToggleBlur();
		}


	}

	return 0;
}