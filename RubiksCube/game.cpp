#include "game.h"
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

Game::Game() : Scene()
{
	cubeSize = 3;
	animSpeed = 1;
	isRotateClockWise = true;
	cubesIndexs[cubeSize * cubeSize * cubeSize];
}

//Game::Game(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void Game::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 0};
	unsigned int slots[3] = { 0 , 1, 0 };
	
	AddShader("../res/shaders/pickingShader2");	
	AddShader("../res/shaders/basicShader2");
	
	TextureDesine(840, 840);
	float center = (cubeSize - 1.0) / 2.0;
	for (int i = 0; i < cubeSize; i++) {
		for (int j = 0; j < cubeSize; j++) {
			for (int k = 0; k < cubeSize; k++) {
				AddShape(Cube, -1, TRIANGLES);
				pickedShape = i*cubeSize*cubeSize + j*cubeSize + k;
				ShapeTransformation(xTranslate, 2 * (k - center));
				ShapeTransformation(yTranslate, 2 * (j - center));
				ShapeTransformation(zTranslate, 2 * (i - center));
				cubesIndexs[pickedShape] = pickedShape;
			}
		}
	}
	
	pickedShape = -1;
	SetShapeShader(0, 1);

	//ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Game::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int shaderIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	textures[0]->Bind(0);
	s->Bind();
	if (shaderIndx != 2)
	{
		s->SetUniformMat4f("View", MVP);
		s->SetUniformMat4f("Model", Model);
	}
	else
	{
		s->SetUniformMat4f("View", glm::mat4(1));
		s->SetUniformMat4f("Model", glm::mat4(1));
	}
	s->SetUniformMat4f("Proj", glm::mat4(1));
	s->SetUniform1i("sampler1", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	if(shaderIndx!=2)
		s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	s->SetUniform4f("lightColor", r, g, b, 0.0f);
	s->Unbind();
}

void Game::WhenRotate()
{
}

void Game::WhenTranslate()
{
}

void Game::Motion()
{
	if(isActive)
	{
	}
}

void Game::UpdateAnimationSpeed(int change) {
	animSpeed += change;
	if (animSpeed < 1)
		animSpeed = 1;
	else if (animSpeed > 18)
		animSpeed = 18;
}

void Game::toggleRotationDir() {
	isRotateClockWise = !isRotateClockWise;
}

std::vector<int> getIndexes(int type, int cubeSize, int cubeIndexs []) {
	std::vector<int> indexs;
	switch (type) {
		case 1: //right
			for (int i = 0; i < cubeSize * cubeSize; i++) {
				indexs.push_back(cubeSize * i + (cubeSize -1));
			}
			break;
		case 2: //left
			for (int i = 0; i < cubeSize * cubeSize; i++) {
				indexs.push_back(cubeSize * i);
			}
			break;
		case 3: //up
			for (int i = 0; i < cubeSize; i++) {
				int index = i * cubeSize * cubeSize + (cubeSize - 1) * cubeSize;
				for (int j = 0; j < cubeSize; j++) {
					indexs.push_back(index + j);
				}
			}
			break;
		case 4: //down
			for (int i = 0; i < cubeSize; i++) {
				int index = i * cubeSize * cubeSize;
				for (int j = 0; j < cubeSize; j++) {
					indexs.push_back(index + j);
				}
			}
			break;
		default:
			break;
	}
	return indexs;
}

void Game::AddOp(int op) {
	Operation op1;
	op1.type = op;
	if (op == 0) { // change rotation direction
		operations.push(op1);
	}
	else {
		op1.indexs = getIndexes(op, cubeSize, cubesIndexs); // [cubeSize * cubeSize] ;
		int t = op1.indexs[cubeSize * cubeSize - 1];
		for (int i = cubeSize * cubeSize -1; i > 0; i--) {
			cubesIndexs[op1.indexs[i]] = cubesIndexs[op1.indexs[i-1]];
		}
		cubesIndexs[op1.indexs[0]] = t;
		for (int i = 0; i < 90 / animSpeed; i++) {
			operations.push(op1);
		}
	}
}

void Game::ReadOperation() {
	if (operations.empty())
		return;
	Operation op = operations.front();
	operations.pop();
	if (op.type == 0)
		toggleRotationDir();
	else
		RotateWall(op.type, op.indexs);
}

void Game::RotateWall(int type, std::vector<int> indexs) {
	for (int i = 0; i < cubeSize * cubeSize; i++) {
		pickedShape = indexs[i];
		if(type < 3)
			ShapeTransformation(xRotate, 1);
		else
			ShapeTransformation(yRotate, 1);
	}
	pickedShape = -1;
}

void Game::RotateUpWall() {
	int t = cubesIndexs[cubeSize * cubeSize * cubeSize -1];
	for (int i = 0; i < cubeSize; i++) {
		int index = i * cubeSize * cubeSize + (cubeSize - 1) * cubeSize; // side + currRow
		for (int j = 0; j < cubeSize; j++) {
			pickedShape = cubesIndexs[index + j];
			ShapeTransformation(yRotate, 1);
			//cubesIndexs[index + j] = t;
			t = pickedShape;
		}
	}
	pickedShape = -1;
}

void Game::RotateLeftWall() {
	int t = cubesIndexs[cubeSize * (cubeSize * cubeSize - 1)];
	for (int i = 0; i < cubeSize * cubeSize; i++) {
		pickedShape = cubesIndexs[cubeSize * i];
		ShapeTransformation(xRotate, 1);
		//cubesIndexs[cubeSize * i] = t;
		t = pickedShape;
	}
	pickedShape = -1;
}

unsigned int Game::TextureDesine(int width, int height)
{
	unsigned char* data = new unsigned char[width * height * 4];
	for (size_t i = 0; i < width; i++)
	{
		for (size_t j = 0; j < height; j++)
		{
			bool isBlack = !(i < 40 || j < 40 || i > 800 || j > 800);
			data[(i * height + j) * 4] = isBlack ? 255 : 0;
			data[(i * height + j) * 4 + 1] = isBlack ? 255 : 0;
			data[(i * height + j) * 4 + 2] = isBlack ? 255 : 0;
			data[(i * height + j) * 4 + 3] = 0;
		}
	}
	textures.push_back(new Texture(width, height));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save 
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;
	return(textures.size() - 1);
}

Game::~Game(void)
{

}
