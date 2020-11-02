#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler1D sampler1;
uniform vec4 lightDirection;

uniform uint counter;
uniform float x;
uniform float y;


void main()
{
	//gl_FragColor = vec4(1,1,0,1);
	//new comment
	float coord = sqrt(pow(texCoord0.s*sin(counter*0.001),2)) + pow(texCoord0.t*cos(counter*0.001), 2);
	if(pow(texCoord0.x-x, 2) + pow(texCoord0.y-y, 2) < 0.001){
		gl_FragColor = vec4(0,1,1,1);
	}
	else{
//		gl_FragColor = texture(sampler1,texCoord0.s);
		gl_FragColor = texture(sampler1, coord);
	}
//	gl_FragColor = texture(sampler1, coord); //you must have gl_FragColor
	//gl_FragColor = texture(sampler1,texCoord0.s);
	//texture2D
}
