#version 120

varying vec3 normal0;

void main()
{
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * clamp(dot(-lightDirection, normal0),0.0,1.0);
	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * clamp(dot(-vec3(0,0,1), normal0),0.0,1.0);
}
