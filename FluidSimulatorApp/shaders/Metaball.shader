#shader vertex
#version 330 core

layout(location=0) in vec4 position;

out vec4 v_CircleCenter;
out vec2 vertexPos;
uniform mat4 u_MVP;
uniform float u_ratioH;
uniform float u_ratioW;

void main()
{
	v_CircleCenter =  u_MVP *position;
	gl_Position = v_CircleCenter;
	

	float x = gl_Position.x;
	float y = gl_Position.y;
	if (gl_Position.x > u_ratioW) {
		x -= 2* u_ratioW;
	}
	if (gl_Position.x < -u_ratioW) {
		x += 2* u_ratioW;
	}
	if (gl_Position.y < -u_ratioH) {
		y+= 2* u_ratioH;
	}
	if (gl_Position.y > u_ratioH) {
		y -= 2* u_ratioH;
	}
	gl_Position = vec4(vec2(x, y), 0, 1);

	vertexPos = vec2(gl_Position.x, gl_Position.y);

};


#shader fragment
#version 330 core

layout(location=0) out vec4 color;


uniform vec4 u_Color;
void main()
{

	color = u_Color;

};