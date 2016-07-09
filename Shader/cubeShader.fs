#version 120

varying vec3 normal0;

void main()
{
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * clamp(dot(-lightDirection, normal0),0.0,1.0);
	gl_FragColor = (vec4(0.5, 0.5, 0.5, 1.0) * clamp(dot(-vec3(0,0,1), normal0),0.0,1.0));
	gl_FragColor.a = 0.2;
}