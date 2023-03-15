#version 330

in vec3 a_Position;		//사용자가 외부에서 입력할 attribute, in 이라는 키워드를 사용해 정의
//layout(location = 0)in vec3 a_Position;		// 이와 같이 번호를 지정해 줄 수도 있음
												// layout 없이도 사용할 수 있다.
in vec3 a_Position1;
uniform vec4 u_Trans;

uniform float u_Scale;

void main()
{
	vec4 newPosition;
	newPosition.xy = u_Scale * ((a_Position.xy) + (a_Position1.xy)) * u_Trans.w + u_Trans.xy;
	newPosition.z = 0;
	newPosition.w= 1;
	gl_Position = newPosition;
}
