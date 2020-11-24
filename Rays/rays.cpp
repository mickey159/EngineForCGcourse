#define GLM_SWIZZLE
#include "rays.h"
#include <iostream>
#include "GL/glew.h"
#include <glm/glm.hpp>


static void printMat(const glm::mat4 mat)
{
	std::cout << " matrix:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << mat[j][i] << " ";
		std::cout << std::endl;
	}
}

Rays::Rays() : Scene()
{
	counter = 1;
	p = 2;
	z = 128;
	pW = 2;
	xOffset = 0;
	yOffset = 0;
}

void Rays::Init()
{
	unsigned int texIDs[3] = { 0 , 1, 0 };
	unsigned int slots[3] = { 0 , 1, 0 };
	std::string str = "../res/scenes/scene4.txt";
	SceneData scene;
	SceneParser(str, &scene);

	AddShader("../res/shaders/pickingShader"); //0 - click on 3d obj always in 0 pos.. even in 2d
	//AddShader("../res/shaders/rays"); // what we will write
	AddShader("../res/shaders/basicShader"); // 1
	// 2 for 2d
	//AddTexture("../res/textures/box0.bmp", 2); // try returning to 1 later when running or change back to 0
	TextureDesine(840, 840, &scene); // replace shader





	AddMaterial(texIDs, slots, 1);

	AddShape(Plane, -1, TRIANGLES);
	//AddShape(Cube, -1, TRIANGLES);
	SetShapeShader(0, 1); // according to shade index	

	//addShapeFromFile(url, -1, TRINGALE)
	//shapeTransformation
	//add shape copy

	//SetShapeMaterial(0, 0);
	//std::cout << "scene\n" << scene << std::endl;
}

void Rays::Update(const glm::mat4& MVP, const glm::mat4& Model, const int shaderIndx)
{
	if (counter)
		counter++;
	Shader* s = shaders[shaderIndx];
	int r = ((pickedShape + 1) & 0x000000FF) >> 0;
	int g = ((pickedShape + 1) & 0x0000FF00) >> 8;
	int b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	//textures[0]->Bind(0);
	s->Bind();
	if (shaderIndx != 1)
	{
		s->SetUniformMat4f("MVP", MVP); //projection + camera transform
		s->SetUniformMat4f("Normal", Model); //obj transform + scene
	}
	else
	{
		s->SetUniformMat4f("MVP", glm::mat4(1));
		s->SetUniformMat4f("Normal", glm::mat4(1));
	}
	s->SetUniform1i("sampler1", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	if (shaderIndx != 1)
		s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));

	s->SetUniform1f("p", p);
	s->Unbind();
}

void Rays::updateP(float change) {
}

void Rays::updateColorCounter(bool divide) {

}

void Rays::updatePixelWidth(float change) {
	if (change > 0) {
		pW /= (change + 1);
	}
	else {
		pW *= (-change + 1);
	}
	if (pW > 32) {
		pW = 32;
	}
	std::cout << "pixel width - " << pW / 840 << std::endl;
}

void Rays::UpdatePosition(float xpos, float ypos)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport); //??
	prevX = x;
	prevY = y;
	x = xpos / viewport[2]; // רוחב של המסך הספציפי// coord in px where window start in in case of split screen
	y = 1 - ypos / viewport[3]; //של המסך הספציפי אורך //// coord in px where window start in
}

void Rays::WhenRotate()
{
	xOffset += (x - prevX) * pW;
	yOffset += (y - prevY) * pW;
}

void Rays::WhenTranslate()
{
}

void Rays::Motion()
{
	if (isActive)
	{
	}
}

float norm(glm::vec3 p) {
	return sqrt(pow((p.x), 2) + pow((p.y), 2) + pow((p.z), 2));
}

float dot(glm::vec3 p1, glm::vec3 p2) {
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

float intersection(glm::vec3 sourcePoint, glm::vec3 v, int size, std::vector<glm::vec4> objects) {
	float tMin = std::numeric_limits<float>::infinity();
	float delta = 0.0001;
	for (int k = 0; k < size; k++) {
		float t = std::numeric_limits<float>::infinity();
		glm::vec4 obj = objects[k];
		if (obj.w < 0) { //plane	
			if (dot(obj.xyz, v) > delta)
				t = -(dot(obj.xyz, sourcePoint) + obj.w) / dot(obj.xyz, v);
		}
		else {
			glm::vec3 src_obj = sourcePoint - obj.xyz;
			float a = 1.0;
			float b = 2.0 * dot(v, src_obj);
			float c = dot(src_obj, src_obj) - obj.w * obj.w;
			float d = b * b - 4.0 * a * c;
			if (d < 0.0) {}// no intersection
			else // line intersects sphere			
			{
				if ((-b - sqrt(d)) / (2.0 * a) > 0)
					t = (-b - sqrt(d)) / (2.0 * a);
			}
		}
		if (tMin > t) {
			tMin = t;
		}
	}
	if (tMin != std::numeric_limits<float>::infinity()) return tMin;
	return -1;
}

float mapRange(float num, float total, float minR, float maxR) {
	return num * ((maxR - minR) / total) + minR;
}

glm::vec3 ConstructRayThroughPixel(glm::vec4 scene_eye, float i, float j) {
	glm::vec3 eye = scene_eye.xyz;
	glm::vec3 p = glm::vec3(i, j, 0);
	glm::vec3 p1 = p - eye;
	float nrm = norm(p1);
	glm::vec3 ray = glm::vec3(p1.x / nrm, p1.y / nrm, p1.z / nrm);
	return ray;
}

glm::vec3 normalize(glm::vec3 v) {
	float nrm = norm(v);
	return glm::vec3(v.x / nrm, v.y / nrm, v.z / nrm);
}

glm::vec3 calcAmbientColor(SceneData* scene, int objIndex) {
	glm::vec3 color = glm::vec3(scene->ambient.x * scene->colors[objIndex].x,
		scene->ambient.y * scene->colors[objIndex].y,
		scene->ambient.z * scene->colors[objIndex].z);
	return color;
}

glm::vec3 getSphereNormal(glm::vec4 sphere, glm::vec3 hit) {
	return glm::vec3(2 * (hit.x - sphere.x), 2 * (hit.y - sphere.y), 2 * (hit.z - sphere.z));
}

float max(float a, float b) {
	if (a > b)
		return a;
	return b;
}

glm::vec3 calcdiffusecolor(glm::vec4 obj, glm::vec3 hit,
	glm::vec4 objColor, glm::vec4 D, glm::vec4 I0, glm::vec4 pos) {
	glm::vec3 surfaceToLight;
	if (obj.w < 0) {
		surfaceToLight = normalize(obj.xyz); // plane n
	}
	else {
		surfaceToLight = normalize(getSphereNormal(obj, hit)); // sphere n
	}
	float attenuation = 1.0;
	if (D.w == 0.0) {
		//directional light
		surfaceToLight = normalize(D.xyz);
		attenuation = 1.0; //no attenuation for directional lights
	}
	else {
		//point light
		surfaceToLight = normalize(pos.xyz - hit);
		float distanceToLight = norm(pos.xyz - hit);
		attenuation = 1.0 / (1.0 + 0.5 * //0.5 = light.attenuation ?
			pow(distanceToLight, 2));

		//cone restrictions (affects attenuation)
		float lightToSurfaceCosa = acos(dot(-surfaceToLight, normalize(D.xyz)));
		if (lightToSurfaceCosa > acos(pos.w)) {
			attenuation = 0.0;
		}
	}
	//glm::vec3 L;
	//if (obj.w < 0) {
	//	L = normalize(obj.xyz); // plane n
	//}
	//else {
	//	L = normalize(getSphereNormal(obj, hit)); // sphere n
	//}
	glm::vec3 diff = max(dot(surfaceToLight, D.xyz), 0.0) * I0.xyz;
	return attenuation * diff;
}

glm::vec3 calcspecularcolor(SceneData* scene, glm::vec3 hit, glm::vec4 light) {
	//vec3 viewPos = eye;
	//float specularStrength = 0.5;
	//vec3 viewDir = normalize(viewPos - FragPos); // view dir
	//vec3 reflectDir = reflect(-lightDir, norm);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	//vec3 specular = specularStrength * spec * lightColor;
	//vec3 result = (ambient + diffuse + specular) * objectColor;
	//FragColor = vec4(result, 1.0);
	glm::vec3 color = glm::vec3(0, 0, 0);
	return color;
}

glm::vec3 GetColor(SceneData* scene, glm::vec3 in_ray, glm::vec3 hit, float i, float j)
{
	int index = -1;
	float delta = 0.0001;
	for (int k = 0; k < scene->sizes[0]; k++) {
		if (scene->objects[k].w < 0) {
			if (dot(scene->objects[k].xyz, in_ray) > delta) {
				//i' j' (point) i' = i + V.x
				float a = acos(dot(normalize(scene->objects[k].xyz), in_ray));
				//float newX = /*(cos(a) - sin(a)) * i + (in_ray.x * cos(a) - in_ray.y * sin(a)) * j;
				//float newY = */
				float newX = i * cos(a) - j * sin(a) + in_ray.x;
				float newY = i * sin(a) + j * cos(a) + in_ray.y;
				if (int(1.5 * mapRange(hit.x + 10, 2, 0, 5)) % 2 == int(1.5 * mapRange(hit.y+10, 2, 0, 5)) % 2) {
					return calcAmbientColor(scene, k);
				}
				index = k;
				break;
			}
		} // plane
		else if (dot(hit - scene->objects[k].xyz, hit - scene->objects[k].xyz) - (scene->objects[k].w * scene->objects[k].w) < delta) {
			index = k;
			break;
		}
	}
	glm::vec3 color = calcAmbientColor(scene, index);
	for (int i = 0; i < scene->sizes[1]; i++) {
		if (scene->directions[i].w == 1.0) {
			color += calcdiffusecolor(scene->objects[index], hit, scene->colors[index], scene->directions[i], scene->intensities[i], scene->lights[i]);
		}
		else
			color += calcdiffusecolor(scene->objects[index], hit, scene->colors[index], scene->directions[i], scene->intensities[i], scene->directions[i]);
		// light.w == 1 - spotlight if 0 direction
		//intensities - light color
		/*color += calcdiffusecolor(scene->objects[index], hit, scene->colors[index], scene->directions[i], scene->intensities[i], scene->directions[i])
			+ calcspecularcolor(scene, hit, light);*/
	}
	return color;
}

unsigned int Rays::TextureDesine(int width, int height, SceneData* scene)
{
	unsigned char* data = new unsigned char[width * height * 4];
	for (size_t i = 0; i < width; i++)
	{
		for (size_t j = 0; j < height; j++)
		{
			glm::vec3 color = glm::vec3(0, 0, 0);
			glm::vec3 V = ConstructRayThroughPixel(scene->eye, mapRange(i, width, -1, 1), mapRange(j, height, -1, 1));
			float t = intersection(scene->eye.xyz, V, scene->sizes[0], scene->objects);
			if (t >= 0) {
				color = GetColor(scene, V, scene->eye.xyz + t * V, mapRange(i, width, -1, 1), mapRange(j, height, -1, 1));
				color.x = mapRange(color.x, 1, 0, 255);
				color.y = mapRange(color.y, 1, 0, 255);
				color.z = mapRange(color.z, 1, 0, 255);
			}
			data[(i * height + j) * 4] = color.x;
			data[(i * height + j) * 4 + 1] = color.y;
			data[(i * height + j) * 4 + 2] = color.z;;
			data[(i * height + j) * 4 + 3] = 0;
		}
	}
	textures.push_back(new Texture(width, height));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save 
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;
	return(textures.size() - 1);
}

Rays::~Rays(void)
{

}
