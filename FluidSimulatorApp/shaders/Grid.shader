#shader vertex
#version 330 core

layout(location=0) in vec4 position;


uniform mat4 u_MVP;

void main()
{
	 
	gl_Position = u_MVP * position;
	

};


#shader fragment
#version 330 core

layout(location=0) out vec4 color;

in vec4 v_CircleCenter;
in vec2 vertexPos;


void main()
{

	color = vec4(0.3f, 0.7f, 0.5f,  1.0f);

};