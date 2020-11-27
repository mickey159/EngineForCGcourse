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

const float delta = 0.0001;

float mapRange(float num, float total, float minR, float maxR) {
	return num * ((maxR - minR) / total) + minR;
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
	vec3 N = normalize(plane.xyz);
	vec3 p0 = vec3(0, 0, -plane.w / plane.z);
	if(dot(v, N) < delta)
		return 0;
	float t = dot(p0 - sourcePoint, N) / dot(v, N);
	return max(t, 0.0);
}
float intersection(vec3 sourcePoint, vec3 v, int skipped) {
	float tMin = 0;
	for (int k = 0; k < sizes[0]; k++) {
		if (skipped == k)
			continue;
		float t = 0;
		if (objects[k].w < 0) { //plane	
			t = intersectionPlane(objects[k], sourcePoint, v);
		}
		else {
			t = intersectionSphere(objects[k], sourcePoint, v);
		}
		if (t > 0 && (tMin == 0.0 || tMin > t)) {
			tMin = t;
		}
	}
	return tMin;
}
int getIntersectingObj(vec3 in_ray, vec3 hit) {
	for (int i = 0; i < sizes[0]; i++) {
		if (objects[i].w < 0) {
			if (dot(normalize(objects[i].xyz), in_ray) > delta)
				return i;
		}
		else if (dot(hit - objects[i].xyz, hit - objects[i].xyz) - (objects[i].w * objects[i].w) < delta) {
			return i;
		}
	}
	return -1;
}

vec3 calcdiffusecolor(vec3 N, int lIndex) {
	return max(0.5*dot(N, normalize(lightsDirection[lIndex].xyz)), 0.0) * lightsIntensity[lIndex].xyz;
}
vec3 calcspecularcolor(vec3 N,int oIndex, int lIndex, vec3 viewDir){
	vec3 reflectDir = reflect(normalize(lightsDirection[lIndex].xyz), N);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), objColors[oIndex].w);//lightsIntensity[lIndex].w);
	return 0.7 * spec * lightsIntensity[lIndex].xyz;
}
bool calcspotlight(vec3 hit, vec3 P, vec3 D, float Q) { 
	float theta = dot(normalize(P - hit), normalize(D)); //*/ size;
	if(degrees(acos(theta)) > 180 || acos(theta) < 0){
		return false;
	}
	return (theta < Q);
}
bool shadow(bool spotLight, vec3 hit, int oIndex, int lIndex) {
	vec3 V = spotLight ? (hit - lightPosition[lIndex].xyz) : -normalize(lightsDirection[lIndex].xyz);
	float t = intersection(hit, V, oIndex);
	return t > 0;
}
vec3 colorCalc(vec3 intersectionPoint){
    vec3 color = vec3(0,0,0);
	vec3 V = normalize(vec3(position1.xy, 0) - intersectionPoint);
	float t = intersection(eye.xyz, V, -1);
	if (t > 0.0){
		vec3 hit = intersectionPoint + t * V;
		int index = getIntersectingObj(V, hit);
		vec3 N = objects[index].w < 0.0 ? normalize(objects[index].xyz) : normalize(hit - objects[index].xyz);
		for (int i = 0; i < sizes[1]; i++) {
			vec3 diffuse = calcdiffusecolor(N, i);
			vec3 specular = calcspecularcolor(N, index, i, V);
			if (lightsDirection[i].w == 1.0 
				&& calcspotlight(hit, lightPosition[i].xyz, lightsDirection[i].xyz, lightPosition[i].w)) {
				//if (!shadow(true, hit, index, i))
				color += 3 * (diffuse + specular);
				//color += diffuse + specular;
				//continue;
			}
			else if (!shadow(lightsDirection[i].w == 1.0, hit, index, i))
				color += diffuse + specular;
		}
		bool sq = mod(int(1.5 * hit.x + 100), 2) == mod(int(1.5 * hit.y + 100), 2);
		if(objects[index].w < 0 && sq)
			color /= 2;
		return (ambient.xyz + color) * 2 * objColors[index].xyz;
	}
    return ambient.xyz;
}

//vec3 colorCalc(vec3 intersectionPoint){
//    vec3 color = vec3(0,0,0);
//	vec3 V = normalize(vec3(position1.xy, 0) - intersectionPoint);
//	for(int j =0; j < 5; j++){
//		float t = intersection(intersectionPoint, V, -1);
//		if (t > 0.0){
//			vec3 hit = intersectionPoint + t * V;
//			int index = getIntersectingObj(V, hit);
//			vec3 N = objects[index].w < 0.0 ? normalize(objects[index].xyz) : normalize(hit - objects[index].xyz);
//			
////			if(index < sizes[2]){
////				intersectionPoint = hit;
////				V = reflect(normalize(V), normalize(N));
////				return vec3(1,1,1);
////				continue;
////			}
//			for (int i = 0; i < sizes[1]; i++) {
//				vec3 diffuse = calcdiffusecolor(N, i);
//				vec3 specular = calcspecularcolor(N, index, i, V);
//				if (lightsDirection[i].w == 1.0 
//					&& calcspotlight(hit, lightPosition[i].xyz, lightsDirection[i].xyz, lightPosition[i].w)) {
//					//if (!shadow(true, hit, index, i))
//					color += (1.5 * (diffuse + specular))*objColors[index].xyz;
//				}
//				else if (!shadow(lightsDirection[i].w == 1.0, hit, index, i))
//					color += (diffuse + specular) * 2 * objColors[index].xyz;
//			}
//			bool sq = mod(int(1.5 * hit.x + 100), 2) == mod(int(1.5 * hit.y + 100), 2);
//			if(objects[index].w < 0 && sq)
//				color /= 2;
//			break;
//		}
//		else { break; }
//	}
//    return ambient.xyz + color;
//}


void main()
{  
	gl_FragColor = vec4(colorCalc(eye.xyz),1);
}