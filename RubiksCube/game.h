#pragma once
#include "scene.h"
#include <iostream>
#include <queue>

class Game : public Scene
{
public:
	
	Game();
	void Init();
	void Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx);
	void ReadOperation();

	void WhenRotate();
	void WhenTranslate();
	void Motion();

	void UpdateAnimationSpeed(int change);

	void toggleRotationDir();

	void AddOp(int op);

	void RotateWall(int type, std::vector<int> indexs);

	void RotateUpWall();

	void RotateLeftWall();
	
	unsigned int TextureDesine(int width, int height);
	~Game(void);

private:
	int cubeSize;
	int animSpeed;
	bool isRotateClockWise;
	Cube[] cubesIndexs;
};

