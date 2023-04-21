#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_UV;

uniform vec2 u_Point;
uniform vec2 u_Points[3];
uniform float u_Time;

const float c_PI = 3.141592;

void circle()
{
    vec2 newValue = v_UV - u_Points[1];
    float d = length(newValue);

    if(d < 0.1f)
    {
		FragColor = vec4(v_UV, 0.0f, 1.0f);
    }
	else
	{
		FragColor = vec4(0.f);
	}
}

void circles()
{
    vec2 newValue = v_UV - u_Point;
    float d = length(newValue);
    FragColor = vec4(sin(30 * d));
}

void radar()
{
    vec2 newValue = v_UV - vec2(0.5f, 0.5f);
    float d = length(newValue);
    float value = sin((2.f * c_PI * d) -  13.f * u_Time) - 0.9f;
    float ring_mask = ceil(value);

    float obj_mask = 0.0;
    for(int i=0; i<3; ++i)
    {    
        vec2 temp = v_UV - u_Points[i];
        float d = length(temp);
        if(d < 0.07)
        {
            obj_mask += 1.0f;
        }
    }

    FragColor = vec4(10.0f * value + ring_mask*obj_mask);
}

void UVTest()
{
    FragColor = vec4(0);
    
    float powValue = 10.f;
    float sinResultX = pow(sin((v_UV.x + v_UV.y) * 10.f * c_PI), powValue);
    float sinResultY = pow(sin((v_UV.x - v_UV.y) * 10.f * c_PI), powValue);
    float finalResult = max(sinResultX, sinResultY);

    FragColor = vec4(finalResult);
}
void sinGraph()
{
    FragColor = vec4(0);

    for(int i=0; i<10; ++i)
    {
        vec2 newUV = vec2(v_UV.x, (v_UV.y - 0.5f) * 2.f);

        float newTime = u_Time + 0.2f * i;
        float newInput = v_UV.x * 2.f * c_PI + c_PI;   // 0 ~ 2 * PI
        float sinValue = newUV.x * sin(newInput + newTime * 5);
        float width = 0.005f;
        float newAlpha = 1.f - newUV.x;
        float newLines = sin(newUV.x * 200.0f - newTime * 5); 

        if(newUV.y > sinValue&& newUV.y < sinValue + width)
            FragColor += vec4(1.f * newAlpha * newLines);
    }
}
void main()
{
    sinGraph();
}
