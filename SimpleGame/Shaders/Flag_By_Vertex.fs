#version 330

in float v_greyScale;
in vec2 v_TexPos;

uniform sampler2D u_Texture;
uniform float u_Time;

const float PI = 3.141592;

out vec4 FragColor;

void realFlag()
{
    float period = (v_TexPos.x + 1.0f) * 1.0f;
    float xValue = v_TexPos.x * 2.0f * PI * period;
    float yValue = ((1.0f - v_TexPos.y) - 0.5f) * 2.0f;
    float sinValue = 0.25f * sin(xValue - 1.0f * u_Time);

    if(sinValue * v_TexPos.x + 0.75f > yValue 
    && sinValue * v_TexPos.x - 0.75f < yValue)
    {
        float vX = v_TexPos.x;
        float yWidth = 1.5f;
        float yDistance = yValue - (sinValue * v_TexPos.x - 0.75f);
        float vY = 1.0f - yDistance / yWidth;
        //FragColor = vec4(vY);
        FragColor = texture(u_Texture, vec2(vX, vY));
    }
    else
    {
        FragColor = vec4(0.0f);
    }
}

void main()
{
    vec2 newTexPos = fract(v_TexPos * 2.0f);
	//FragColor = v_greyScale * vec4(v_TexPos, 0.0f, 1.0f);
    FragColor = v_greyScale * texture(u_Texture, newTexPos);
    //realFlag();
}
