#pragma once   //maybe should be static class
#include "display.h"
#include "renderer.h"
#include "game2.h"
#include <iostream>

const float PI = 3.14;
/*
pick in mouse callback, so until the mouse is realesed we are picking the same shape
rotate/translate in position callback and move shape "forward"/"backward" in scroll callback
*/
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	Game2* scn = (Game2*)rndr->GetScene();
	if (action == GLFW_PRESS)
	{
		//clearPicks
		scn->clearPicks();
		// set/clear the passStencil around here, so we get to the else part
		// after the else part set the stencil2 flag
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		//rndr->UpdatePosition(xpos, ypos);
		rndr->updatePress(xpos, ypos);
		if (!rndr->Picking((int)xpos, (int)ypos))		
			rndr->ClearDrawFlag(4, rndr->inAction2); // clear the flag so DrawAll draws the blend
	}
	if (action == GLFW_RELEASE) {
		rndr->SetDrawFlag(4, rndr->inAction2);
		rndr->pickMany();
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	Game2* scn = (Game2*)rndr->GetScene();
	double x2, y2;
	glfwGetCursorPos(window, &x2, &y2);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			scn->scrollShape(yoffset); 
		}
	scn->MyTranslate(glm::vec3(0, 0, xoffset), 0);

}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	Game2* scn = (Game2*)rndr->GetScene();
	scn->UpdatePosition(xpos, ypos);
	rndr->UpdatePosition(xpos, ypos);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		scn->WhenRotate();
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) { 
		scn->WhenTranslate();
		// this crap isnt working good, so for now its just based on mouse offset
		// use depth to update position (depth obtained inside Renderer::Picking, calculation taken from ps)
		//float depth = rndr->GetDepth();
		//float near = rndr->GetNear(0); // camera number
		//float far = rndr->GetFar(0);
		//float alpha = rndr->GetAngle(0);
		//float width = 800.0;
		//float height = 800.0;
		//float z = far + depth * (near - far);
		//float xnew = ((xpos * far) / (width * z)) * near * 2 * tan(alpha * PI / 180.0);
		//float ynew = ((ypos * far) / (height * z))* near * 2 * tan(alpha * PI / 180.0);
		//scn->UpdatePosition(xnew, ynew);
	}


	//rndr->UpdatePosition((float)xpos,(float)ypos);
	//scn->UpdatePosition((float)xpos, (float)ypos);

		/*if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			scn->UpdatePosition((float)xpos, (float)ypos);
			rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			scn->UpdatePosition((float)xpos, (float)ypos);
			rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
		}*/

}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);

	rndr->Resize(width, height);

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	Game2* scn = (Game2*)rndr->GetScene();

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_SPACE:
			if (scn->IsActive())
				scn->Deactivate();
			else
				scn->Activate();
			break;

		case GLFW_KEY_UP:
			rndr->MoveCamera(0, scn->zTranslate, 0.4f);
			break;
		case GLFW_KEY_DOWN:
			//scn->shapeTransformation(scn->xGlobalRotate,-5.f);
			//cout<< "down: "<<endl;
			rndr->MoveCamera(0, scn->zTranslate, -0.4f);
			break;
		case GLFW_KEY_C:
			scn->ContinuityStateToggle();
			break;
		case GLFW_KEY_2:
			scn->RemakeBezier(2);
			break;
		case GLFW_KEY_3:
			scn->RemakeBezier(3);
			break;
		case GLFW_KEY_4:
			scn->RemakeBezier(4);
			break;
		case GLFW_KEY_5:
			scn->RemakeBezier(5);
			break;
		case GLFW_KEY_6:
			scn->RemakeBezier(6);
			break;
		case GLFW_KEY_P:
			scn->ContinuityStateToggle();
			break;
		case GLFW_KEY_LEFT:
			break;
		default:
			break;
		}
	}
}

void Init(Display& display)
{
	display.AddKeyCallBack(key_callback);
	display.AddMouseCallBacks(mouse_callback, scroll_callback, cursor_position_callback);
	display.AddResizeCallBack(window_size_callback);
}
