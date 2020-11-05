#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler1D sampler1;
uniform vec4 lightDirection;

uniform float pW;
uniform float z;
uniform float p;
uniform float x;
uniform float y;
uniform float xOffset;
uniform float yOffset;

float mapRange(float num, float total, float minR, float maxR) {
	return num * ((maxR - minR) / total) + minR;
}

void main()
{
	float iter = 50;

	float cx = mapRange(texCoord0.x, 1, -pW + xOffset, pW + xOffset);
	float cy = mapRange(texCoord0.y, 1, -pW + yOffset, pW + yOffset);
	int counter = 0;
	float zx = 0;
	float zy = 0;
	while (counter < iter && zx*zx + zy*zy < 4.0) {
		float c = zx * zx + zy * zy;
		float t = pow(c, p/2) * cos(p * atan(zy,zx)) + cx;
		zy = pow(c, p / 2) * sin(p * atan(zy, zx)) + cy;
		zx = t;
		counter++;
	}

	float color = mapRange(counter, iter, 0, 1);
	if (counter == iter) {
		color = 0;
	}
	if(pow(texCoord0.x-x, 2) + pow(texCoord0.y-y, 2) < 0.001){
		gl_FragColor = vec4(0,1,1,1);
	}
	else{
		//gl_FragColor = texture(sampler1,texCoord0.y);
		gl_FragColor = vec4(0, color, color, color);
		gl_FragColor = texture(sampler1, color / z);// * vec4(color, color, color, 1);
	}
//	gl_FragColor = texture(sampler1, coord); //you must have gl_FragColor
	//gl_FragColor = texture(sampler1,texCoord0.s);
	//texture2D
}