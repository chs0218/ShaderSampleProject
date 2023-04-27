#version 330

layout(location=0) out vec4 FragColor;

uniform sampler2D uTexSampler;
in vec2 v_TexPos;

void problem1()
{
    float x = v_TexPos.x;
    float y = 1.f - abs(v_TexPos.y * 2.f - 1.f);

    FragColor = texture(uTexSampler, vec2(x, y));

}
void problem2()
{
    float x = v_TexPos.x * 3.f;
    float correction = (2 - int(x)) * 0.33f;
    float y = v_TexPos.y;

    FragColor = texture(uTexSampler, vec2(fract(x), y / 3.f + correction));
}
void problem3()
{
    float x = v_TexPos.x * 3.f;
    float correction = int(x) * 0.33f;
    float y = v_TexPos.y;

    FragColor = texture(uTexSampler, vec2(fract(x), y / 3.f + correction));
}
void problem4()
{
    float x = v_TexPos.x;
    float y = v_TexPos.y * 3.f;
    float correction = float(int(y) - 1) * 1.f / 3.f;

    FragColor = texture(uTexSampler, vec2(x, y / 3.f + correction));
}
void main()
{
    //FragColor = vec4(v_TexPos, 0.f, 1.f);
    //problem1();
    //problem2();
    //problem3();
    problem4();
}
