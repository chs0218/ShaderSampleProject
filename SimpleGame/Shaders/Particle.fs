#version 330

layout(location=0) out vec4 FragColor;

uniform sampler2D u_Texture;

in vec4 v_Color;
in vec2 v_UV;

void circle()
{
    vec2 newValue = v_UV - vec2(0.5f, 0.5f);
    float d = length(newValue);

    if(d < 0.5f)
    {
		FragColor = v_Color;
    }
	else
	{
		FragColor = vec4(0.f);
	}
}

void Textured()
{
    vec4 result = texture(u_Texture, v_UV) * v_Color;
    FragColor = result;
}

void main()
{
	//circle();
    Textured();
}
