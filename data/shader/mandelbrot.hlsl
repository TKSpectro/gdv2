//覧覧覧覧覧覧覧覧覧覧覧覧覧覧�
// Constant Buffer Variables
//覧覧覧覧覧覧覧覧覧覧覧覧覧覧�

cbuffer PSConstants : register(b0)
{
    //int Iterations;
    float2 Pan;
    float Zoom;
    float Aspect;
    float3 Color;
    float dummy1;
    //int dummy2;
};

//覧覧覧覧覧覧覧覧覧覧覧覧覧覧�
// Input structures
//覧覧覧覧覧覧覧覧覧覧覧覧覧覧�
struct VS_INPUT
{
    float4 C : POSITION;
    float2 TEX : TEXCOORD0;
};

struct PS_INPUT
{
    float4 C : SV_POSITION;
    float2 TEX : TEXCOORD0;
};

//覧覧覧覧覧覧覧覧覧覧覧覧覧覧�
// Vertex Shader
//覧覧覧覧覧覧覧覧覧覧覧覧覧覧�
PS_INPUT VSShader(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.C = input.C;
    output.TEX = input.TEX;

    return output;
}

//覧覧覧覧覧覧覧覧覧覧覧覧覧覧�
// Pixel Shader
//覧覧覧覧覧覧覧覧覧覧覧覧覧覧�
float4 PSShader(PS_INPUT input) : SV_Target
{
    float2 C = (input.TEX - 0.5) * Zoom * float2(1, Aspect) - Pan;
    float2 v = C;

    int iterations = 10;
    int prevIteration = iterations;
    int i = 0;

    do
    {
        v = float2((v.x * v.x) - (v.y * v.y), v.x * v.y * 2) + C;

        i++;

        if ((prevIteration == iterations) && ((v.x * v.x) + (v.y * v.y)) > 4.0)
        {
            prevIteration = i + 1;
        }
    }
    while (i < prevIteration);

    float NIC = (float(i) - (log(log(sqrt((v.x * v.x) + (v.y * v.y)))) / log(2.0))) / float(iterations);

    return float4(sin(NIC * Color.x), sin(NIC * Color.y), sin(NIC * Color.z), 1);
}