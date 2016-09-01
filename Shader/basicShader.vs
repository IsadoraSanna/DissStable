#version 120

//taken from the mesh
attribute vec3 position;
attribute vec3 normal;

//shared between Vertex and Fragment shaders
varying vec3 normal0;
varying vec3 FragPos;

//taken from Shader class
uniform mat4 model_camera;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model_mesh;

void main()
{	//position of the vertex, in clip space
	gl_Position = projection * view * model_camera * model_mesh * vec4(position, 1.0);
	// Position of the vertex, in worldspace
	FragPos = vec3(model_camera * model_mesh * vec4(position, 1.0f));
    //normal in clip space
	normal0 = (projection * view * model_camera * model_mesh *vec4(normal, 0.0)).xyz;
}
