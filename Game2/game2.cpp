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
	
}

//Game2::Game2(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void Game2::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 0};
	unsigned int slots[3] = { 0 , 1, 0 };
	
	AddShader("../res/shaders/pickingShader");	
	AddShader("../res/shaders/basicShader");
	AddTexture("../res/textures/box0.bmp", 2);
	//TextureDesine(840, 840);

	AddMaterial(texIDs,slots, 1);
	
	AddShape(Cube, -1, TRIANGLES);
	AddShape(Octahedron, -1, TRIANGLES);
	
	AddShape(Axis, -1, LINES);
	
	AddShapeCopy(0, -1, TRIANGLES);

	SetShapeShader(0, 1);
	SetShapeShader(1, 1);
	SetShapeShader(2, 1);
	SetShapeShader(3, 1);

void Game2::FixControlPoints() {
	for (int seg = 0; seg < bez->GetSegmentsNum(); seg++) {
		for (int i = 0; i < 3; i++)
			RelocateControlPoint(seg, i);
	}
	RelocateControlPoint(bez->GetSegmentsNum() - 1, 3);
	for (int i = bez->GetSegmentsNum() * 3 + 1; i < 6 * 3 + 1; i++) //max control points
		HideControlPoint(i);
	pickedShape = -1;
	//SetShapeMaterial(0, 0);
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
	
		s->SetUniformMat4f("MVP", MVP);
		s->SetUniformMat4f("Normal", Model);
	
	s->SetUniform1i("sampler1", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	if(shaderIndx!=1)
		s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	s->SetUniform4f("lightColor", r, g, b, 0);
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
	if (pickedShape < 22) { // do control points stuff only when we want to
		if (pickedShape > pointsStartIndx + 6 * 3) {
	if (pickedShape > pointsStartIndx - 1){
		int currPoint = pickedShape - pointsStartIndx;
		int pointIndx = currPoint % 3;
		int pointSeg = currPoint / 3;
		if (pointIndx != 0) { //pressed p1 or p2
			float xOffset = (x - xprev);
			float yOffset = (y - yprev);
			if (xOffset > 0.1 || yOffset > 0.1)
				return;
			ShapeTransformation(xTranslate, xOffset);
			ShapeTransformation(yTranslate, yOffset);
		}
		else if (pickedShape > pointsStartIndx - 1) {
			int currPoint = pickedShape - pointsStartIndx;
			int pointIndx = currPoint % 3;
			int pointSeg = currPoint / 3;
			if (pointIndx != 0) { //pressed p1 or p2
				float xOffset = (x - xprev);
				float yOffset = (y - yprev);
				if (xOffset > 0.1 || yOffset > 0.1)
					return;
				glm::vec4 center = bez->GetControlPoint(pointSeg, pointIndx == 1 ? 0 : 3);
				glm::vec4 point = bez->GetControlPoint(pointSeg, pointIndx);
				glm::vec4 nextLoc = rotatePoint(center, point, xOffset, yOffset);
				xOffset = nextLoc.x - point.x;
				yOffset = nextLoc.y - point.y;
				bez->CurveUpdate(currPoint, xOffset, yOffset, isContinuityState);
				MoveControlPoint(currPoint, xOffset, yOffset);
				Update3DBezier();
			}
			else if (pointSeg != bez->GetSegmentsNum() && pointSeg != 0) { //option b
				glm::vec4 line = getLine(bez->GetControlPoint(pointSeg, 0), bez->GetControlPoint(pointSeg, 1));
				glm::vec4 p2 = bez->GetControlPoint(pointSeg - 1, 2);
				float yOffset = line[0] * p2.x + line[1] - p2.y;
				bez->CurveUpdate(currPoint - 1, 0, yOffset, false);
				MoveControlPoint(currPoint - 1, 0, yOffset);
				Update3DBezier();
			}
		}
		else if (x > 1) {// option e - convex hull
			int segment = bez->GetSectionIsMouseInConvexHull(2 * (x - 1.5), 2 * y - 1);
			if (segment == -1)
				return;
			bez->SplitSegment(segment, 0);
			FixControlPoints();
		}
	}
	else {
		bez2->rotateBezier(x - xprev);
	}
}

void Game2::WhenTranslate()
{
}


				}
				if (segment != 0) { //firstSeg
					bez->CurveUpdate(segment * 3 - 1, xOffset, yOffset, false);
					MoveControlPoint(segment * 3 - 1, xOffset, yOffset);
				}
			}
			Update3DBezier();
			pickedShape = -1;
		}
		pickedShape = -1;
	}

}

void Game2::Motion()
{
	if(isActive)
	{
		pickedShape = 3;
		ShapeTransformation(yRotate, 0.07);
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
