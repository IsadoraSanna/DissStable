#version 120

attribute vec3 position;
attribute vec3 normal;

varying vec3 normal0;

uniform mat4 model_camera;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model_OBB;
void main()
{
	gl_Position = projection * view * model_camera * model_OBB * vec4(position, 1.0);
	//gl_Position = MVP_camera * vec4(position, 1.0);
	//lambertian lighting
	normal0 = (projection * view * model_camera * model_OBB * vec4(normal, 0.0)).xyz;

}
