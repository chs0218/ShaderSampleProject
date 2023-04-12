#version 330

layout(location=0) out vec4 FragColor;

varying vec2 v_Texcoord;

void circle()
{
    vec2 newValue = v_Texcoord - vec2(0.5f, 0.5f);
    float d = length(newValue);

    if(d < 0.5f)
    {
		FragColor = vec4(v_Texcoord, 0.0f, 1.0f);
    }
	else
	{
		FragColor = vec4(0.f);
	}
}


void circles()
{
    vec2 newValue = v_Texcoord - vec2(0.5f, 0.5f);
    float d = length(newValue);
    FragColor = vec4(sin(30 * d));
}

void main()
{
    circles();
}
