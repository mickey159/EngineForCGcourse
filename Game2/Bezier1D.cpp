#include "Bezier1D.h"
#include <iostream>
#include "GL/glew.h"
#include <renderer.h>

float max(float a, float b) {
	return a > b ? a : b;
}
float min(float a, float b) {
	return a < b ? a : b;
}

void Bezier1D::MoveControlPoint(int segment, int indx, float dx, float dy, bool preserveC1)
{
	glm::vec4 cp = segments[segment][indx];
	segments[segment][indx] = glm::vec4(max(-1, min(cp.x + dx, 1)), min(1, max(cp.y + dy , 0)), 0, 0);
	if (preserveC1) {}
}

Bezier1D::Bezier1D(int segNum, int res, int mode, int viewport)
	:Shape(Scene::modes::LINE_STRIP, mode)
{
	M = glm::mat4(
		-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 3, 0, 0,
		1, 0, 0, 0);
	resT = res;

	segments.push_back(glm::mat4(-1, 0, 0, 0, -0.88, 0.32, 0, 0, -0.42, 0.49, 0, 0, -0.5, 0.5, 0, 0));
	segments.push_back(glm::mat4(-0.5, 0.5, 0, 0, -0.2, 0.5, 0, 0, 0.2, 0.5, 0, 0, 0.5, 0.5, 0, 0));
	segments.push_back(glm::mat4(0.5, 0.5, 0, 0, 0.8, 0.4, 0, 0, 1, 0.21, 0, 0, 1, 0,0,0));

	IndexedModel line = GetLine();
	mesh = new MeshConstructor(line, false);
}

IndexedModel Bezier1D::GetLine() {
	IndexedModel line;
	for (int seg = 0; seg < GetSegmentsNum(); seg++) {
		for (int i = 0; i < resT; i++) {
			glm::vec4 pos = GetPointOnCurve(seg, i);
			line.positions.push_back(glm::vec3(pos));
			line.colors.push_back(glm::vec3(0, 1, 1));
			line.normals.push_back(glm::vec3());
			line.texCoords.push_back(glm::vec2(pos.x, pos.y));
		}
	}
	line.indices.push_back(0);
	for (int i = 1; i < GetSegmentsNum() * resT - 1; i++) {
		line.indices.push_back(i);
		line.indices.push_back(i);
	}
	line.indices.push_back(GetSegmentsNum() * resT - 1);
	return line;
}

glm::vec4 Bezier1D::GetControlPoint(int segment, int indx) const
{
	return segments[segment][indx];
}

glm::vec4 Bezier1D::GetPointOnCurve(int segment, int t)
{
	glm::mat4 seg = segments[segment];
	glm::vec4 p1 = seg[0];
	glm::vec4 p2 = seg[1];
	glm::vec4 p3 = seg[2];
	glm::vec4 p4 = seg[3];
	float tt = (float)t / resT;
	float a1 = pow((1 - tt), 3);
	float a2 = pow((1 - tt), 2) * 3 * tt;
	float a3 = 3 * tt * tt * (1 - tt);
	float a4 = tt * tt * tt;
	return glm::vec4(a1 * p1.x + a2 * p2.x + a3 * p3.x + a4 * p4.x,
		a1 * p1.y + a2 * p2.y + a3 * p3.y + a4 * p4.y,
		0, 0);
	//glm::vec4 tVec = glm::vec4(pow((float)t / resT, 3), pow((float)t / resT, 2), (float)t / resT, 1);
	//return tVec * M * seg;
}

glm::vec3 Bezier1D::GetVelosity(int segment, int t)
{
	return glm::vec3();
}

void Bezier1D::SplitSegment(int segment, int t)
{
}

void Bezier1D::AddSegment(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3)
{
}

void Bezier1D::CurveUpdate(int pointIndx, float dx, float dy, bool preserveC1)
{
	if (preserveC1) {}
	else if (pointIndx == GetSegmentsNum() * 3) {
		MoveControlPoint((pointIndx / 3) - 1, 3, dx, dy, preserveC1);
	}
	else if (pointIndx % 3 == 0 && pointIndx != 0) { // shared point
		int seg = pointIndx / 3;
		MoveControlPoint(seg - 1, 3, dx, dy, preserveC1);
		MoveControlPoint(seg, 0, dx, dy, preserveC1);
	}
	else {
		int seg = pointIndx / 3;
		MoveControlPoint(seg, pointIndx - seg * 3, dx, dy, preserveC1);
	}
	mesh = new MeshConstructor(GetLine(), false);
	//mesh->ChangeLine
}

Bezier1D::~Bezier1D(void)
{
}
