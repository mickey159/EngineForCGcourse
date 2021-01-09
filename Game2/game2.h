#pragma once
#include "scene.h"
#include "Bezier1D.h"

class Game2 : public Scene
{
public:
	
	Game2();
	void Init();
	void AddControlPoint(int indx);
	void MoveControlPoint(int indx, float x, float y);
	void MoveControlPoint(int segment, int indx, float x, float y);
	void RelocateControlPoint(int segment, int indx);
	void HideControlPoint(int indx);
	void Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx);
	
	void WhenRotate();
	void WhenTranslate();
	void Motion();
	
	unsigned int TextureDesine(int width, int height);
	~Game2(void);
	inline void ResetCounter() { tmp = counter; counter = 0; }
	inline void SetCounter() { counter = tmp; }

	void UpdatePosition( float xpos, float ypos);
	void RemakeBezier(int segNum);
	void ContinuityStateToggle();
private:
	unsigned int counter;
	unsigned int tmp;
	float x, y;
	float xprev, yprev;
	bool isContinuityState;
	int pointsStartIndx;
	int pps;
	float pointsScale;
	float curveScale;
	Bezier1D* bez;
};

