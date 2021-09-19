
// -----------------------------------------------------------------------------
// Define the constant buffers.
// -----------------------------------------------------------------------------
cbuffer VSBuffer : register(b0) // Register the constant buffer on slot 0
{
    float4x4 g_ViewProjectionMatrix;
    float4x4 g_WorldMatrix;
    float3 g_CameraPos;
    float FILLER;
};

// -----------------------------------------------------------------------------
// Texture variables.
// -----------------------------------------------------------------------------
Texture2D g_ColorMap : register(t0); // Register the color map on texture slot 0

// -----------------------------------------------------------------------------
// Sampler variables.
// -----------------------------------------------------------------------------
sampler g_ColorMapSampler : register(s0); // Register the sampler on sampler slot 0

// -----------------------------------------------------------------------------
// Define input and output data of the vertex shader.
// -----------------------------------------------------------------------------
struct VSInput
{
    float3 m_Position : POSITION;
    float2 m_TexCoord : TEXCOORD;
};

struct PSInput
{
    float4 m_Position : SV_POSITION;
    float2 m_TexCoord : TEXCOORD0;
};

// -----------------------------------------------------------------------------
// Vertex Shader
// -----------------------------------------------------------------------------
PSInput VSShader(VSInput _Input)
{
    PSInput Output = (PSInput) 0;
    
    // Rotation only happens only around the y axis
    float3 yBaseVector = { 0.0f, 1.0f, 0.0f };
    yBaseVector = normalize(yBaseVector);
    
    // the zBaseVector describes the negative direction of where the camera is looking
    float3 zBaseVector = -g_CameraPos;
    zBaseVector.y = 0.0f;
    zBaseVector = normalize(zBaseVector);
    
    // x describes the cross product of the y and z vectors
    float3 xBaseVector = cross(yBaseVector, zBaseVector);
    xBaseVector = normalize(xBaseVector);
    
    float3x3 rotationMatrix =
    {
        xBaseVector,
        yBaseVector,
        zBaseVector
    };
  
	// -------------------------------------------------------------------------------
	// Get the world space position.
	// -------------------------------------------------------------------------------
    float3 transformVector = mul(_Input.m_Position, rotationMatrix);
    float4 WSPosition = mul(float4(transformVector, 1.0f), g_WorldMatrix);

	// -------------------------------------------------------------------------------
	// Get the clip space position.
	// -------------------------------------------------------------------------------
    Output.m_Position = mul(WSPosition, g_ViewProjectionMatrix);
    Output.m_TexCoord = _Input.m_TexCoord;
    
    return Output;
}

// -----------------------------------------------------------------------------
// Pixel Shader
// -----------------------------------------------------------------------------
float4 PSShader(PSInput _Input) : SV_Target
{
    // Render the given texture
    return g_ColorMap.Sample(g_ColorMapSampler, _Input.m_TexCoord);
}


