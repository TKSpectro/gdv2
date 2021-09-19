
// -----------------------------------------------------------------------------
// Define the constant buffers.
// -----------------------------------------------------------------------------
cbuffer VSBuffer : register(b0) // Register the constant buffer on slot 0
{
    float4x4 g_ViewProjectionMatrix;
    float4x4 g_WorldMatrix;
    float4 g_PositionInWorldSpace;
    float4 g_CameraAt;
    float4 g_CameraPos;
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
    // lower left , lower right, upper right, upper left
    float3 ll = { -1.0f + g_PositionInWorldSpace.x, -1.0f + g_PositionInWorldSpace.y, 0.0f + g_PositionInWorldSpace.z };
    float3 lr = { 1.0f + g_PositionInWorldSpace.x, -1.0f + g_PositionInWorldSpace.y, 0.0f + g_PositionInWorldSpace.z };
    float3 ur = { 1.0f + g_PositionInWorldSpace.x, 1.0f + g_PositionInWorldSpace.y, 0.0f + g_PositionInWorldSpace.z };
    float3 ul = { -1.0f + g_PositionInWorldSpace.x, 1.0f + g_PositionInWorldSpace.y, 0.0f + g_PositionInWorldSpace.z };

    // Nicht sicher ob das schon invertiert ist?
    float3 cameraDirection = normalize(g_CameraPos - g_CameraAt);
    
    // Kreuzprodukt -> von was?
    float3 xBaseVector = { 0.0f, 0.0f, 0.0f };
    // Rotation nur um y-Achse
    float3 yBaseVector = { 0.0f, 1.0f, 0.0f };
    // Entgegengesetzte Blickrichtung der Kamera und y = 0
    float3 zBaseVector = { cameraDirection.x, 0.0f, cameraDirection.z };
    
    float3x3 BaseVector =
    {
        normalize(xBaseVector),
        normalize(yBaseVector),
        normalize(zBaseVector)
    };
   
    float4 WSPosition;
    
    PSInput Output = (PSInput) 0;
    
	// -------------------------------------------------------------------------------
	// Get the world space position.
	// -------------------------------------------------------------------------------
    WSPosition = mul(float4(_Input.m_Position, 1.0f), g_WorldMatrix);

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
    return g_ColorMap.Sample(g_ColorMapSampler, _Input.m_TexCoord);
 //   return float4(1.0f, 0.0f, 0.0f, 1.0f);
}


