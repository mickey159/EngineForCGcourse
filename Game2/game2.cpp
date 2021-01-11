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
	pps = 20;
	x = 0;
	y = 0;
	xprev = 0;
	yprev = 0;
	curveScale = 0.9;
	pointsScale = 0.04;
	isContinuityState = false;
}

void Game2::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 0};
	unsigned int slots[3] = { 0 , 1, 0 };
	
	AddShader("../res/shaders/pickingShader");
	AddShader("../res/shaders/basicShader");
	AddShader("../res/shaders/basicShader2");
	//AddShader("../res/shaders/cubemapShader");

	AddTexture("../res/textures/box0.bmp", 2);
	//AddTexture("../res/textures/cubeMap/DayLight Box_", 3);

	AddMaterial(texIDs,slots, 1);
	//AddMaterial(texIDs + 1, slots + 1, 1);
	AddShape(Cube, -2, TRIANGLES);
	//SetShapeShader(0, 3);
	//SetShapeMaterial(0, 1);

	//---------------------2D BEZIER--------------------------
	AddShape(Axis, -1, LINES);
	AddShapeViewport(1, 1);
	RemoveShapeViewport(1, 0);

	AddShape(Cube, -1, LINES); // will be replaced by bez bez
	pointsStartIndx = 3;
	for (int i = 0; i < 6 * 3 + 1; i++) //max control points
		AddControlPoint(i);
	RemakeBezier(3);
}

void Game2::RemakeBezier(int segNum) {
	bez = new Bezier1D(segNum, pps, LINES);
	pickedShape = pointsStartIndx - 1;
	ReplaceShape(pickedShape, bez);
	
	SetShapeShader(pickedShape, 1);
	AddShapeViewport(pickedShape, 1);
	RemoveShapeViewport(pickedShape, 0);
	ShapeTransformation(xScale, curveScale);
	ShapeTransformation(yScale, curveScale);

	for (int seg = 0; seg < bez->GetSegmentsNum(); seg++) {
		for (int i = 0; i < 3; i++)
			RelocateControlPoint(seg, i);
	}
	RelocateControlPoint(bez->GetSegmentsNum() - 1, 3);
	for (int i = bez->GetSegmentsNum()*3 + 1; i < 6 * 3 + 1; i++) //max control points
		HideControlPoint(i);
	pickedShape = -1;
}

void Game2::AddControlPoint(int indx) {
	AddShape(Octahedron, 1, TRIANGLES);
	pickedShape = indx + pointsStartIndx;
	SetShapeShader(pickedShape, 2);
	AddShapeViewport(pickedShape, 1);
	RemoveShapeViewport(pickedShape, 0);
}

void Game2::MoveControlPoint(int indx, float x, float y) {
	pickedShape = indx + pointsStartIndx;
	ShapeTransformation(xTranslate, x / pointsScale);
	ShapeTransformation(yTranslate, y / pointsScale);
}

void Game2::MoveControlPoint(int segment, int indx, float x, float y) {
	pickedShape = segment * 3 + indx + pointsStartIndx;
	ShapeTransformation(xTranslate, x / pointsScale);
	ShapeTransformation(yTranslate, y / pointsScale);
}

void Game2::RelocateControlPoint(int segment, int indx) {
	glm::vec4 cp = bez->GetControlPoint(segment, indx);
	pickedShape = segment * 3 + indx + pointsStartIndx;
	ShapeTransformation(ZeroTrans, 1);
	ShapeTransformation(xScale, pointsScale);
	ShapeTransformation(yScale, pointsScale);
	ShapeTransformation(xTranslate, (cp.x * curveScale) / pointsScale);
	ShapeTransformation(yTranslate, (cp.y * curveScale) / pointsScale);
}

void Game2::HideControlPoint(int indx) {
	pickedShape = indx + pointsStartIndx;
	ShapeTransformation(xScale, 1.1e-5);
	ShapeTransformation(yScale, 1.1e-5);
}

void Game2::Update(const glm::mat4 &View, const glm::mat4 &Projection, const glm::mat4 &Model, const int  shaderIndx)
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
	
	s->SetUniformMat4f("View", View);
	s->SetUniformMat4f("Proj", Projection);
	//s->SetUniformMat4f("MVP", Projection * View);
	s->SetUniformMat4f("Model", Model);
	
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
	xprev = x;
	yprev = y;
	x = xpos / viewport[2];
	y =  1 - ypos / viewport[3]; 
}

void Game2::ContinuityStateToggle() {
	isContinuityState = !isContinuityState;
	std::cout << "cont" << isContinuityState << std::endl;
}
glm::vec4 getLine(glm::vec4 p0, glm::vec4 p1) {
	float a = (p0.y - p1.y) / (p0.x - p1.x);
	float b = p0.y - a * p0.x;
	return glm::vec4(a, b, 0, 0);
}
glm::vec4 rotatePoint(glm::vec4 center, glm::vec4 point, glm::vec4 amt) {
	point -= center;
	return glm::vec4(center.x + point.x *amt.y - point.y*amt.x, center.y + point.x * amt.x + point.y * amt.y, 0, 0);
}

void Game2::WhenRotate()
{
	float scale = 21;
	int currPoint = pickedShape - 2;
	if (pickedShape > 2){
		if (currPoint % 3 == 0 && currPoint != bez->GetSegmentsNum() * 3) {
			int segment = currPoint / 3;
			glm::vec4 line = getLine(bez->GetControlPoint(currPoint / 3, 0), bez->GetControlPoint(currPoint / 3, 1));
			glm::vec4 p2 = bez->GetControlPoint(-1 + currPoint / 3, 2);
			float yOffset = line[0] * p2.x + line[1] - p2.y;
			bez->CurveUpdate(currPoint - 1, 0, yOffset, false);
			pickedShape -= 1;
			ShapeTransformation(yTranslate, yOffset * scale);
		}
		float xOffset = (x - xprev);
		float yOffset = (y - yprev);
		if (xOffset > 0.1 || yOffset > 0.1)
			return;
		glm::vec4 p0 = bez->GetControlPoint(currPoint / 3, 0);
		glm::vec4 p3 = bez->GetControlPoint(currPoint / 3, 3);
		glm::vec4 cp = bez->GetControlPoint(currPoint / 3, currPoint - currPoint/3);
		glm::vec4 p = bez->GetPointOnCurve(currPoint / 3, pps * (cp.x - p0.x)/(p3.x - p0.x));
		cp -= p; //translate to origin

	}
}

void Game2::WhenTranslate()
{
	float xOffset = (x - xprev);
	float yOffset = (y - yprev);
	if (xOffset > 0.1 || yOffset > 0.1)
		return;

	if (pickedShape > pointsStartIndx - 1) {
		int currPoint = pickedShape - pointsStartIndx;

		bez->CurveUpdate(currPoint, xOffset, yOffset, isContinuityState);
		MoveControlPoint(currPoint, xOffset, yOffset);

		if (currPoint % 3 == 0) {
			if (currPoint != bez->GetSegmentsNum() * 3) { //lastPoint
				bez->CurveUpdate(currPoint + 1, xOffset, yOffset, false);
				MoveControlPoint(currPoint + 1, xOffset, yOffset);
				//RelocateControlPoint((currPoint + 1) / 3, (currPoint + 1) % 3);

			}
			if (currPoint != 0) { //firstPoint
				bez->CurveUpdate(currPoint - 1, xOffset, yOffset, false);
				MoveControlPoint(currPoint - 1, xOffset, yOffset);
			}
		}
		else if(isContinuityState){
			if (currPoint % 3 == 1) {//p1 moves tg with p0
				MoveControlPoint(currPoint - 1, xOffset, yOffset);
				//RelocateControlPoint((currPoint - 1) / 3, (currPoint - 1) % 3);
			}
			else {//p2 moves tg with p3
				MoveControlPoint(currPoint + 1, xOffset, yOffset);
				//RelocateControlPoint((currPoint + 1) / 3, (currPoint + 1) % 3);
			}
		}
		pickedShape = currPoint + pointsStartIndx;
	}
	else if(x > 1){
		int segment = bez->GetSectionIsMouseInConvexHull(2*(x - 1.5), 2*y -1);
		if (segment == -1)
			return;
		for (int i = 0; i < 4; i++) {
			bez->CurveUpdate(segment * 3 + i, xOffset, yOffset, false);
			MoveControlPoint(segment * 3 + i, xOffset, yOffset);
		}
		if (isContinuityState) {
			if (segment != bez->GetSegmentsNum() - 1) { //lastSeg
				bez->CurveUpdate((segment + 1) * 3 + 1, xOffset, yOffset, false);
				MoveControlPoint((segment + 1) * 3 + 1, xOffset, yOffset);

			}
			if (segment != 0) { //firstSeg
				bez->CurveUpdate(segment * 3 - 1, xOffset, yOffset, false);
				MoveControlPoint(segment * 3 - 1, xOffset, yOffset);
			}
		}
		pickedShape = -1;
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
