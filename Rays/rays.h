#pragma once
#include "scene.h"

class Rays : public Scene
{
public:
	
	Rays();
	void Init();
	void Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx);

	void updateP(float change);

	void updateColorCounter(bool divide);

	void updatePixelWidth(float change);
	
	void WhenRotate();
	void WhenTranslate();
	void Motion();
	
	unsigned int TextureDesine(int width, int height);
	~Rays(void);
	inline void ResetCounter() { tmp = counter; counter = 0; }
	inline void SetCounter() { counter = tmp; }

	void UpdatePosition( float xpos, float ypos);
private:
	unsigned int counter;
	unsigned int tmp;
	float x, y;
	float prevX, prevY;
	float xOffset, yOffset;
	float p, z;
	float pW;
};

