 #version 130 

uniform vec4 eye;
uniform vec4 ambient;
uniform vec4[20] objects;
uniform vec4[20] objColors;
uniform vec4[10] lightsDirection;
uniform vec4[10] lightsIntensity;
uniform vec4[10] lightPosition;
uniform ivec4 sizes; //{number of objects , number of lights , width, hight}  

in vec3 position1;

#define delta 0.00001;

float mapRange(float num, float total, float minR, float maxR) {
	return num * ((maxR - minR) / total) + minR;
}

vec3 ConstructRayThroughPixel(float i, float j){
	return normalize(vec3(i, j, 0) - eye.xyz);
}

float intersectionSphere(vec4 sphere, vec3 sourcePoint, vec3 v){
	vec3 src_obj = sourcePoint - sphere.xyz;
	float a = 1.0;
	float b = 2.0 * dot(v, src_obj);
	float c = dot(src_obj, src_obj) - sphere.w * sphere.w;
	float d = b * b - 4.0 * a * c;
	if (d < 0.0) {}// no intersection
	else if ((-b - sqrt(d)) / (2.0 * a) > 0) // line intersects sphere	
		return (-b - sqrt(d)) / (2.0 * a);
	return 0;
}
float intersectionPlane(vec4 plane, vec3 sourcePoint, vec3 v){
	return 0;
}

float intersection(vec3 sourcePoint, vec3 v){
    float tMin = 0;
	for (int k = 0; k < sizes[0]; k++) {
		float t;
		if (objects[k].w < 0) //plane
			t = intersectionPlane(objects[k], sourcePoint, v);
		else //sphere
			t = intersectionSphere(objects[k], sourcePoint, v);

		if (t > 0 && (tMin < 0 || tMin > t))
			tMin = t;
	}
	return tMin;
}

//vec3 spotLight(int light){
//	float intensity = 0.0;
//    vec4 spec = vec4(0.0);
// 
//    vec3 ld = normalize(lightsDirection[light].xyz);
//    vec3 sd = normalize(vec3(-l_spotDir));  
// 
//    // inside the cone?
//    if (dot(sd,ld) > l_spotCutOff) {
// 
//        vec3 n = normalize(DataIn.normal);
//        intensity = max(dot(n,ld), 0.0);
// 
//        if (intensity > 0.0) {
//            vec3 eye = normalize(DataIn.eye);
//            vec3 h = normalize(ld + eye);
//            float intSpec = max(dot(h,n), 0.0);
//            spec = specular * pow(intSpec, shininess);
//        }
//    }
// 
//    return max(intensity * diffuse + spec, ambient);}

vec3 colorCalc(vec3 intersectionPoint)
{
	
    vec3 color = vec3(1,0,1);
    
    return color;
}

void main()
{  
	float i = 0;
	float j = 0;
	vec3 V = ConstructRayThroughPixel(i, j);
	float t = intersection(eye.xyz, V);
	if (t >= 0) {
		gl_FragColor = vec4(colorCalc(eye.xyz),1); //GetColor(scene, V, scene->eye.xyz + t * V);
	}
    else
		gl_FragColor = vec4(1,0,1,1);      
}
 