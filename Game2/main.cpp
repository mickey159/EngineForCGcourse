#include "InputManager.h"
#include "glm/glm.hpp"

int main(int argc,char *argv[])
{
	const int DISPLAY_WIDTH = 1600;
	const int DISPLAY_HEIGHT = 800;
	const float CAMERA_ANGLE = 60.0f;
	const float NEAR = 1.0f;
	const float FAR = 100.0f;
	const int infoIndx = 2; 
	std::list<int> x, y;
	x.push_back(DISPLAY_WIDTH / 2);
	//y.push_back(DISPLAY_HEIGHT / 2);
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "Assa4");
	// adding a perspective camera
	Renderer* rndr = new Renderer(CAMERA_ANGLE, (float)DISPLAY_WIDTH / 2 / DISPLAY_HEIGHT, NEAR, FAR); 
	Game2 *scn = new Game2();  //initializing scene
	
	Init(display); //adding callback functions
	scn->Init();    //adding shaders, textures, shapes to scene. also creating a second viewport with AddShapeViewport
	rndr->Init(scn,x,y); // adding scene and viewports to the renderer
	display.SetRenderer(rndr);  // updating renderer in as a user pointer in glfw

	//left - projective camera, right - Orthographic camera.
	rndr->AddCamera(glm::vec3(0, 0, 0), 0, 1, -1, 1, 3); // ortographic camera connected to drawinfo number 3, i.e. right screen

	//rndr->MoveCamera(1, rndr->zTranslate, 1); //<- moving the orthographic camera is bad
	rndr->ClearDrawFlag(2, rndr->sceneTrans); // im not sure what it does, but tamir has it

	
	//rndr->BindViewport2D(1);
	unsigned int indx = 0;
	while(!display.CloseWindow())
	{
		rndr->DrawAll();
		if(indx == 1)
			scn->Motion();
		display.SwapBuffers();
		display.PollEvents();
		indx++;
	}
	delete scn;
	delete rndr;
	return 0;
}
