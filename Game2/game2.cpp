#include "game2.h"
#include <iostream>
#include "GL/glew.h"

static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

Game2::Game2() : Scene()
{
	counter = 1;
	bez = new Bezier1D(3, 20, LINES);
}

void Game2::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 0};
	unsigned int slots[3] = { 0 , 1, 0 };
	
	AddShader("../res/shaders/pickingShader");
	AddShader("../res/shaders/basicShader");
	AddShader("../res/shaders/basicShader2");
	AddTexture("../res/textures/box0.bmp", 2);

	AddMaterial(texIDs,slots, 1);
	int segments = 3;

	Bezier1D* bezier = bez;

	AddShape(Axis, -1, LINES);
	AddShape(bezier, 0);
	SetShapeShader(1, 1);
	pickedShape = 1;
	ShapeTransformation(xTranslate, - 0.5);
	ShapeTransformation(yTranslate, - 0.5);
	//bezier->GetSegmentsNum()
	for (int seg = 0; seg < bezier->GetSegmentsNum(); seg++) {
		for (int i = 0; i < 4; i++) {
			glm::vec4 cp = bezier->GetControlPoint(seg, i);
			AddShape(Octahedron, 0, TRIANGLES);
			
			pickedShape = seg*4 + i + 2;
			SetShapeShader(pickedShape, 1);
			ShapeTransformation(xScale, 0.04);
			ShapeTransformation(yScale, 0.04);
			ShapeTransformation(xTranslate, (cp.x - 0.5)/0.04);
			//ShapeTransformation(yTranslate, (cp.y - 0.5)/0.04);
		}
	}
	/*for (int i = 0; i < 1 * 4 + 2; i++) {
		AddShapeViewport(i, 1);
		RemoveShapeViewport(i, 0);
	}*/
	pickedShape = -1;
	//AddShape(Cube, -1, TRIANGLES);
	//AddShapeCopy(segments * 4 + 1 + 1 + 1, -1, TRIANGLES);
}

void Game2::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
{	
	if(counter)
		counter++;
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	//textures[0]->Bind(0);
	s->Bind();
	
		s->SetUniformMat4f("MVP", MVP);
		s->SetUniformMat4f("Normal", Model);
	
	s->SetUniform1i("sampler1", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	if(shaderIndx!=1)
		s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	s->SetUniform4f("lightColor", r / 255.0, g / 255.0, b / 255.0, 0);
	s->SetUniform1ui("counter", counter);
	s->SetUniform1f("x", x);
	s->SetUniform1f("y", y);
	s->Unbind();
}

void Game2::UpdatePosition(float xpos,  float ypos)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	x = xpos / viewport[2];
	y =  1 - ypos / viewport[3]; 
}

void Game2::WhenRotate()
{
	std::cout << "x "<<x<<", y "<<y<<std::endl;
}

void Game2::WhenTranslate()
{
	if (pickedShape >= 2) {
		bez->CurveUpdate(pickedShape - 2, x, y, false);
		ShapeTransformation(xTranslate, x / 0.04);
		ShapeTransformation(yTranslate, y / 0.04);
	}
}

void Game2::Motion()
{
	if(isActive)
	{
		//pickedShape = 3;
		//ShapeTransformation(yRotate, 0.07);
	}
}

unsigned int Game2::TextureDesine(int width, int height)
{
	unsigned char* data = new unsigned char[width * height * 4];
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			data[(i * height + j) * 4] = (i + j) % 256;
			data[(i * height + j) * 4 + 1] =  (i + j * 2) % 256;
			data[(i * height + j) * 4 + 2] = (i * 2 + j) % 256;
			data[(i * height + j) * 4 + 3] = (i * 3 + j) % 256;
		}
	}
	textures.push_back(new Texture(width, height));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save 
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;
	return(textures.size() - 1);
}

Game2::~Game2(void)
{

}
