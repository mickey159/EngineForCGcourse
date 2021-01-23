#pragma once   //maybe should be static class
#include "display.h"
#include "renderer.h"
#include "game2.h"
#include <iostream>


	void mouse_callback(GLFWwindow* window,int button, int action, int mods)
	{	
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Game2* scn = (Game2*)rndr->GetScene();
		if (action == GLFW_PRESS)
		{
			// i dont want it here. i want the bezier to rotate and move with the mouse movement...
		/*	double x2, y2;
			glfwGetCursorPos(window, &x2, &y2);
			if (rndr->Picking((int)x2, (int)y2))
			{
				rndr->UpdatePosition(x2, y2);
				scn->UpdatePosition(x2, y2);
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
					scn->WhenRotate();  // this was here for Game - to make the cube/octahedron rotate (the commented part in WhenRotate)
				}
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
					scn->WhenTranslate(); // this was here for Game - to make the cube/octahedron translate (the commented part in WhenTranslate)
				}

			}*/
			scn->ResetCounter();
		}
		else
			scn->SetCounter();
		
	}
	
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Game2* scn = (Game2*)rndr->GetScene();
		double x2, y2;
		glfwGetCursorPos(window, &x2, &y2);
		if (rndr->Picking((int)x2, (int)y2)) {
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				scn->scrollShape(yoffset); // simillar to the zoom in rays.
			}	
		}
		scn->MyTranslate(glm::vec3(0,0,xoffset),0);
		
	}
	
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Game2* scn = (Game2*)rndr->GetScene();
		
		rndr->UpdatePosition((float)xpos,(float)ypos);
		scn->UpdatePosition((float)xpos, (float)ypos);

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
		
		rndr->Resize(width,height);
		
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

	void Init(Display &display)
	{
		display.AddKeyCallBack(key_callback);
		display.AddMouseCallBacks(mouse_callback,scroll_callback,cursor_position_callback);
		display.AddResizeCallBack(window_size_callback);
	}
