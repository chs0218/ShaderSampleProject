#version 330

layout(location=0) out vec4 FragColor;

uniform sampler2D uTexSampler;
uniform sampler2D uMultiTexSampler[2];

uniform vec2 u_XYRepeat;
uniform float u_Step;
uniform float u_SeuqNum;

in vec2 v_TexPos;

void P1()
{
    float x = v_TexPos.x;
    float y = 1.f - abs(v_TexPos.y * 2.f - 1.f);

    FragColor = texture(uTexSampler, vec2(x, y));

}
void P2()
{
    float x = fract(v_TexPos.x * 3.0f);
    float dy = v_TexPos.y / 3.0f;
    float y = floor(3.0f * (1.0f - v_TexPos.x)) / 3.0f + dy;

    vec2 newTexPos = vec2(x, y);

    FragColor = texture(uTexSampler, newTexPos);
}
void P3()
{
    float x = fract(v_TexPos.x * 3.0f);
    float dy = v_TexPos.y / 3.0f;
    float y = floor(3.0f * v_TexPos.x) / 3.0f + dy;

    vec2 newTexPos = vec2(x, y);

    FragColor = texture(uTexSampler, newTexPos);
}
void P4()
{
    float x = v_TexPos.x;
    float dy = fract(v_TexPos.y * 3.0f) / 3.0f;
    float y = floor(3.0f * (1.0f - v_TexPos.y)) / 3.0f + dy;

    vec2 newTexPos = vec2(x, y);

    FragColor = texture(uTexSampler, newTexPos);
}
void P5()
{
    float x_repeat = u_XYRepeat.x;
    float y_repeat = u_XYRepeat.y;
    float dx = v_TexPos.x * x_repeat;
    float x = fract(dx + floor((1.0f - v_TexPos.y) * y_repeat) * 0.5f);
    float y = fract(v_TexPos.y * y_repeat);

    vec2 newTexPos = vec2(x, y);
    FragColor = texture(uTexSampler, newTexPos);
}
void P6()
{
    float x_repeat = u_XYRepeat.x;
    float y_repeat = u_XYRepeat.y;

    float dy = v_TexPos.y * y_repeat;
    float x = fract(v_TexPos.x * x_repeat);
    float y = fract(dy + floor(v_TexPos.x * x_repeat) * 0.5f);

    vec2 newTexPos = vec2(x, y);
    FragColor = texture(uTexSampler, newTexPos);
}
// 시험에 나올 예정
void P7()
{
    //float x_repeat = u_XYRepeat.x;
    //float y_repeat = u_XYRepeat.y;

    float x = fract(v_TexPos.x + (1.0f - v_TexPos.y));
    float y = fract(v_TexPos.x + v_TexPos.y);

    vec2 newTexPos = vec2(x, y);
    FragColor = texture(uTexSampler, newTexPos);
    //FragColor = vec4(x);
}

void MultiTexture()
{
    float x = v_TexPos.x;
    float y = (u_Step + v_TexPos.y) / 6;
    //float y = u_Step * 0.25f + v_TexPos.y * 0.5f;

    vec2 newTexPos = vec2(x, y);
    FragColor = texture(uTexSampler, fract(newTexPos));
}

void SpriteAnim()
{
    float nX = float(int(u_SeuqNum) % 8);
    float nY = floor(u_SeuqNum / 8.0f);

    float x = nX / 8.0f + v_TexPos.x / 8.0f;
    float y = nY / 6.0f + v_TexPos.y / 6.0f;

    vec2 newTexPos = vec2(x, y);
    FragColor = texture(uTexSampler, fract(newTexPos));
}

void main()
{
    //FragColor = vec4(v_TexPos, 0.f, 1.f);
    //P1();
    //P2();
    //P3();
    //P4();
    //P5();
    //P6();
    //P7();
    //MultiTexture();
    SpriteAnim();
}
