
#include "yoshix.h"

#include <math.h>
#include <iostream>

using namespace gfx;

// Vertex Buffer for the billboard shader
struct SVertexBuffer
{
	float m_ViewProjectionMatrix[16];
	float m_WorldMatrix[16];
	float m_WSCameraPosition[3];
	float m_FILLER1;
	float m_WSLightPosition[3];
	float m_FILLER2;
};

// Pixel Buffer for the billboard shader
struct SPixelBuffer
{
	float m_AmbientLightColor[4];
	float m_DiffuseLightColor[4];
	float m_SpecularColor[4];
	float m_SpecularExponent;
	float FILLER[3];
};

// Vertex Buffer for the just textured shader
struct SGroundVertexBuffer
{
	float m_ViewProjectionMatrix[16];
	float m_WorldMatrix[16];
};

// -----------------------------------------------------------------------------

class CApplication : public IApplication
{
public:

	CApplication();
	virtual ~CApplication();

private:

	float   m_FieldOfViewY;             // Vertical view angle of the camera
	float   m_ViewMatrix[16];           // The view matrix to transform a mesh from world space into view space.
	float   m_ProjectionMatrix[16];     // The projection matrix to transform a mesh from view space into clip space.

	BHandle m_pVertexConstantBuffer;    // A pointer to a YoshiX constant buffer, which defines global data for a vertex shader.
	BHandle m_pPixelConstantBuffer;		// A pointer to a YoshiX constant buffer, which defines global data for a vertex shader.

	BHandle m_pVertexShader;            // A pointer to a YoshiX vertex shader, which processes each single vertex of the mesh.
	BHandle m_pPixelShader;             // A pointer to a YoshiX pixel shader, which computes the color of each pixel visible of the mesh on the screen.

	BHandle m_pMaterial;                // A pointer to a YoshiX material, spawning the surface of the mesh.
	BHandle m_pMesh;                    // A pointer to a YoshiX mesh, which represents a single triangle.

	BHandle m_pColorTexture;			// A pointer to a texture which contains a actual picture to display.
	BHandle m_pNormalTexture;			// A pointer to a texture which contains the normal for the the previous picture.

	// Ground
	BHandle m_pGroundVertexConstantBuffer;
	BHandle m_pGroundVertexShader;
	BHandle m_pGroundPixelShader;
	BHandle m_pGroundMaterial;
	BHandle m_pGroundMesh;
	BHandle m_pGroundTexture;

	// Camera
	float m_camPosX;
	float m_camPosY;
	float m_camPosZ;

	float m_camAtX;
	float m_camAtY;
	float m_camAtZ;

	// This can be turned on for a automatic rotation around the center point
	bool m_autoRotation;

	// Variables for calculating a point on a circle around the center point
	float m_radius;
	float m_interval;
	float m_theta;
	float m_alpha;

	// Config variables
	bool m_useTree;		// If this variable is set we use a tree texture instead of the wall
	bool m_showGround;	// This variable gets used to decide if the ground should be rendered

private:

	virtual bool InternOnCreateConstantBuffers();
	virtual bool InternOnReleaseConstantBuffers();
	virtual bool InternOnCreateShader();
	virtual bool InternOnReleaseShader();
	virtual bool InternOnCreateMaterials();
	virtual bool InternOnReleaseMaterials();
	virtual bool InternOnCreateMeshes();
	virtual bool InternOnReleaseMeshes();
	virtual bool InternOnCreateTextures();
	virtual bool InternOnReleaseTextures();
	virtual bool InternOnResize(int _Width, int _Height);
	virtual bool InternOnKeyEvent(unsigned int _Key, bool _IsKeyDown, bool _IsAltDown);
	virtual bool InternOnUpdate();
	virtual bool InternOnFrame();
};

// -----------------------------------------------------------------------------

CApplication::CApplication()
	: m_FieldOfViewY(60.0f)        // Set the vertical view angle of the camera to 60 degrees.
	, m_pMesh(nullptr)
	, m_pVertexConstantBuffer(nullptr)
	, m_pPixelConstantBuffer(nullptr)
	, m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pMaterial(nullptr)
	, m_pColorTexture(nullptr)
	, m_pNormalTexture(nullptr)
	, m_pGroundVertexConstantBuffer(nullptr)
	, m_pGroundVertexShader(nullptr)
	, m_pGroundPixelShader(nullptr)
	, m_pGroundTexture(nullptr)
	, m_pGroundMesh(nullptr)
	, m_pGroundMaterial(nullptr)
	, m_camPosX(0.0f)
	, m_camPosY(1.0f)
	, m_camPosZ(-4.0f)
	, m_camAtX(0.0f)
	, m_camAtY(0.0f)
	, m_camAtZ(0.0f)
	, m_autoRotation(false)
	, m_radius(4)
	, m_interval(0.025)
	, m_theta(5)
	, m_alpha(90)
	, m_useTree(false) 		// You can toggle useTree here to get the tree texture instead of the wall
	, m_showGround(true)
{
}

// -----------------------------------------------------------------------------

CApplication::~CApplication()
{
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateConstantBuffers()
{
	// -----------------------------------------------------------------------------
	// Create a constant buffer with global data for the vertex shader. We use this
	// buffer to upload the data defined in the 'SVertexBuffer' struct. Note that it
	// is not possible to use the data of a constant buffer in vertex and pixel
	// shader. Constant buffers are specific to a certain shader stage. If a 
	// constant buffer is a vertex or a pixel buffer is defined in the material info
	// when creating the material.
	// -----------------------------------------------------------------------------
	CreateConstantBuffer(sizeof(SVertexBuffer), &m_pVertexConstantBuffer);
	CreateConstantBuffer(sizeof(SPixelBuffer), &m_pPixelConstantBuffer);

	CreateConstantBuffer(sizeof(SGroundVertexBuffer), &m_pGroundVertexConstantBuffer);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseConstantBuffers()
{
	// -----------------------------------------------------------------------------
	// Important to release the buffer again when the application is shut down.
	// -----------------------------------------------------------------------------
	ReleaseConstantBuffer(m_pVertexConstantBuffer);
	ReleaseConstantBuffer(m_pPixelConstantBuffer);

	ReleaseConstantBuffer(m_pGroundVertexConstantBuffer);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateShader()
{
	// -----------------------------------------------------------------------------
	// Load and compile the shader programs.
	// -----------------------------------------------------------------------------
	CreateVertexShader("..\\data\\shader\\billboard.hlsl", "VSShader", &m_pVertexShader);
	CreatePixelShader("..\\data\\shader\\billboard.hlsl", "PSShader", &m_pPixelShader);

	CreateVertexShader("..\\data\\shader\\textured.fx", "VSShader", &m_pGroundVertexShader);
	CreatePixelShader("..\\data\\shader\\textured.fx", "PSShader", &m_pGroundPixelShader);


	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseShader()
{
	// -----------------------------------------------------------------------------
	// Important to release the shader again when the application is shut down.
	// -----------------------------------------------------------------------------
	ReleaseVertexShader(m_pVertexShader);
	ReleasePixelShader(m_pPixelShader);

	ReleaseVertexShader(m_pGroundVertexShader);
	ReleasePixelShader(m_pGroundPixelShader);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateMaterials()
{
	// -----------------------------------------------------------------------------
	// Create a material spawning the mesh. This material will be used for the
	// actual billboard.
	// -----------------------------------------------------------------------------
	SMaterialInfo MaterialInfo;

	MaterialInfo.m_NumberOfTextures = 3;									// The material does not need textures, because the pixel shader just returns a constant color.
	MaterialInfo.m_pTextures[0] = m_pColorTexture;							// The handle to the texture.
	MaterialInfo.m_pTextures[1] = m_pNormalTexture;
	MaterialInfo.m_pTextures[2] = m_pGroundTexture;

	MaterialInfo.m_NumberOfVertexConstantBuffers = 1;						// We need one vertex constant buffer to pass world matrix and view projection matrix to the vertex shader.
	MaterialInfo.m_pVertexConstantBuffers[0] = m_pVertexConstantBuffer;     // Pass the handle to the created vertex constant buffer.

	MaterialInfo.m_NumberOfPixelConstantBuffers = 1;						// We do not need any global data in the pixel shader.
	MaterialInfo.m_pPixelConstantBuffers[0] = m_pPixelConstantBuffer;

	MaterialInfo.m_pVertexShader = m_pVertexShader;							// The handle to the vertex shader.
	MaterialInfo.m_pPixelShader = m_pPixelShader;							// The handle to the pixel shader.

	MaterialInfo.m_NumberOfInputElements = 5;								// The vertex shader requests the position as only argument.
	MaterialInfo.m_InputElements[0].m_pName = "POSITION";					// The semantic name of the argument, which matches exactly the identifier in the 'VSInput' struct.
	MaterialInfo.m_InputElements[0].m_Type = SInputElement::Float3;			// The position is a 3D vector with floating points.
	MaterialInfo.m_InputElements[1].m_pName = "TANGENT";
	MaterialInfo.m_InputElements[1].m_Type = SInputElement::Float3;
	MaterialInfo.m_InputElements[2].m_pName = "BINORMAL";
	MaterialInfo.m_InputElements[2].m_Type = SInputElement::Float3;
	MaterialInfo.m_InputElements[3].m_pName = "NORMAL";
	MaterialInfo.m_InputElements[3].m_Type = SInputElement::Float3;
	MaterialInfo.m_InputElements[4].m_pName = "TEXCOORD";              // The semantic name of the second argument, which matches exactly the second identifier in the 'VSInput' struct.
	MaterialInfo.m_InputElements[4].m_Type = SInputElement::Float2;   // The texture coordinates are a 2D vector with floating points.

	CreateMaterial(MaterialInfo, &m_pMaterial);

	// -----------------------------------------------------------------------------
	// Create a material spawning the mesh. This material will be used for the
	// non billboard objects which should just be textured objects.
	// -----------------------------------------------------------------------------
	SMaterialInfo MaterialGroundInfo;

	MaterialGroundInfo.m_NumberOfTextures = 1;									// The material does not need textures, because the pixel shader just returns a constant color.
	MaterialGroundInfo.m_pTextures[0] = m_pGroundTexture;

	MaterialGroundInfo.m_NumberOfVertexConstantBuffers = 1;						// We need one vertex constant buffer to pass world matrix and view projection matrix to the vertex shader.
	MaterialGroundInfo.m_pVertexConstantBuffers[0] = m_pGroundVertexConstantBuffer;     // Pass the handle to the created vertex constant buffer.					// We need one vertex constant buffer to pass world matrix and view projection matrix to the vertex shader.
	MaterialGroundInfo.m_NumberOfPixelConstantBuffers = 0;						// We do not need any global data in the pixel shader.

	MaterialGroundInfo.m_pVertexShader = m_pGroundVertexShader;							// The handle to the vertex shader.
	MaterialGroundInfo.m_pPixelShader = m_pGroundPixelShader;							// The handle to the pixel shader.

	MaterialGroundInfo.m_NumberOfInputElements = 2;								// The vertex shader requests the position as only argument.
	MaterialGroundInfo.m_InputElements[0].m_pName = "POSITION";					// The semantic name of the argument, which matches exactly the identifier in the 'VSInput' struct.
	MaterialGroundInfo.m_InputElements[0].m_Type = SInputElement::Float3;			// The position is a 3D vector with floating points.
	MaterialGroundInfo.m_InputElements[1].m_pName = "TEXCOORD";              // The semantic name of the second argument, which matches exactly the second identifier in the 'VSInput' struct.
	MaterialGroundInfo.m_InputElements[1].m_Type = SInputElement::Float2;   // The texture coordinates are a 2D vector with floating points.

	CreateMaterial(MaterialGroundInfo, &m_pGroundMaterial);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseMaterials()
{
	// -----------------------------------------------------------------------------
	// Important to release the material again when the application is shut down.
	// -----------------------------------------------------------------------------
	ReleaseMaterial(m_pMaterial);
	ReleaseMaterial(m_pGroundMaterial);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateTextures()
{
	if(m_useTree)
	{
		CreateTexture("..\\data\\images\\tree_color_map.dds", &m_pColorTexture);
		CreateTexture("..\\data\\images\\tree_normal_map.png", &m_pNormalTexture);
	}
	else
	{
		CreateTexture("..\\data\\images\\wall_color_map.dds", &m_pColorTexture);
		CreateTexture("..\\data\\images\\wall_normal_map.dds", &m_pNormalTexture);
	}

	CreateTexture("..\\data\\images\\ground.dds", &m_pGroundTexture);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseTextures()
{
	ReleaseTexture(m_pColorTexture);
	ReleaseTexture(m_pNormalTexture);

	ReleaseTexture(m_pGroundTexture);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateMeshes()
{
	// -----------------------------------------------------------------------------
	// Define the vertices of the mesh. This is a relatively complex data structure
	// in the form of an interleaved storage, where we place all information for one
	// point into the same array. Layout: Position(3D), TextureCoords(2D), 
	// Tangent (3D), Binormal (3D), Normal (3D)
	// -----------------------------------------------------------------------------
	float SquareVertices[][14] =
	{
		{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f  },
		{  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f  },
		{  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f  },
		{ -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f  },
	};

	// -----------------------------------------------------------------------------
	// Define the topology of the mesh via indices. An index addresses a vertex from
	// the array above. Three indices represent one triangle. When defining the 
	// triangles of a mesh imagine that you are standing in front of the triangle 
	// and looking to the center of the triangle. If the mesh represents a closed
	// body such as a cube, your view position has to be outside of the body. Now
	// define the indices of the addressed vertices of the triangle in counter-
	// clockwise order.
	// -----------------------------------------------------------------------------
	int SquareIndices[][3] =
	{
		{  0,  1,  2 },
		{  0,  2,  3 },
	};

	// -----------------------------------------------------------------------------
	// Define the mesh and its material. The material defines the look of the 
	// surface covering the mesh. Note that you pass the number of indices and not
	// the number of triangles.
	// -----------------------------------------------------------------------------
	SMeshInfo MeshInfo;

	MeshInfo.m_pVertices = &SquareVertices[0][0];      // Pointer to the first float of the first vertex.
	MeshInfo.m_NumberOfVertices = 4;                            // The number of vertices.
	MeshInfo.m_pIndices = &SquareIndices[0][0];       // Pointer to the first index.
	MeshInfo.m_NumberOfIndices = 6;                            // The number of indices (has to be dividable by 3).
	MeshInfo.m_pMaterial = m_pMaterial;                  // A handle to the material covering the mesh.

	CreateMesh(MeshInfo, &m_pMesh);

	// -----------------------------------------------------------------------------
	// Build up the mesh for a simple ground with a texture laying on it.
	// -----------------------------------------------------------------------------
	float GroundVertices[][5] =
	{
		{ -4.0f, -1.0f, -4.0f, 0.0f, 1.0f  },
		{  4.0f, -1.0f, -4.0f, 1.0f, 1.0f  },
		{  4.0f, -1.0f,  4.0f, 1.0f, 0.0f  },
		{ -4.0f, -1.0f,  4.0f, 0.0f, 0.0f  },
	};

	int GroundIndices[][3] =
	{
		{  0,  1,  2 },
		{  0,  2,  3 },
	};

	SMeshInfo GroundMeshInfo;

	GroundMeshInfo.m_pVertices = &GroundVertices[0][0];      // Pointer to the first float of the first vertex.
	GroundMeshInfo.m_NumberOfVertices = 4;                            // The number of vertices.
	GroundMeshInfo.m_pIndices = &GroundIndices[0][0];       // Pointer to the first index.
	GroundMeshInfo.m_NumberOfIndices = 6;                            // The number of indices (has to be dividable by 3).
	GroundMeshInfo.m_pMaterial = m_pGroundMaterial;                  // A handle to the material covering the mesh.

	CreateMesh(GroundMeshInfo, &m_pGroundMesh);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseMeshes()
{
	// -----------------------------------------------------------------------------
	// Important to release the mesh again when the application is shut down.
	// -----------------------------------------------------------------------------
	ReleaseMesh(m_pMesh);
	ReleaseMesh(m_pGroundMesh);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnResize(int _Width, int _Height)
{
	// -----------------------------------------------------------------------------
	// The projection matrix defines the size of the camera frustum. The YoshiX
	// camera has the shape of a pyramid with the eye position at the top of the
	// pyramid. The horizontal view angle is defined by the vertical view angle
	// and the ratio between window width and window height. Note that we do not
	// set the projection matrix to YoshiX. Instead we store the projection matrix
	// as a member and upload it in the 'InternOnFrame' method in a constant buffer.
	// -----------------------------------------------------------------------------
	GetProjectionMatrix(m_FieldOfViewY, static_cast<float>(_Width) / static_cast<float>(_Height), 0.1f, 100.0f, m_ProjectionMatrix);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnUpdate()
{
	float Eye[3];
	float At[3];
	float Up[3];

	// -----------------------------------------------------------------------------
	// Define position and orientation of the camera in the world. The result is
	// stored in the 'm_ViewMatrix' matrix and uploaded in the 'InternOnFrame'
	// method. We use variables for the position of the camera, so it can be changed
	// via inputs of the user.
	// -----------------------------------------------------------------------------
	Eye[0] = m_camPosX; At[0] = m_camAtX; Up[0] = 0.0f;
	Eye[1] = m_camPosY; At[1] = m_camAtZ; Up[1] = 1.0f;
	Eye[2] = m_camPosZ; At[2] = m_camAtY; Up[2] = 0.0f;

	GetViewMatrix(Eye, At, Up, m_ViewMatrix);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnFrame()
{
	SetAlphaBlending(true);

	// -----------------------------------------------------------------------------
	// Upload the world matrix and the view projection matrix to the GPU. This has
	// to be done before drawing the mesh, though not necessarily in this method.
	// -----------------------------------------------------------------------------
	SVertexBuffer VertexBuffer;
	float RotationMatrix[16];
	float TranslationMatrix[16];

	// Set world matrix in the vertex buffer for this frame
	GetIdentityMatrix(VertexBuffer.m_WorldMatrix);

	// Set the ViewProjectionMatrix in the vertex buffer for this frame
	MulMatrix(m_ViewMatrix, m_ProjectionMatrix, VertexBuffer.m_ViewProjectionMatrix);

	// Rotation of the camera around the center point 0,0,0 with the offset of m_alpha 
	// which can be changed by either pressing a or d or enabling automatic rotation
	float x = m_radius * cos(m_theta);
	float y = 0;
	float z = m_radius * sin(m_theta);
	m_camPosX = z * cos(m_alpha) - x * sin(m_alpha);
	m_camPosZ = x * cos(m_alpha) + z * sin(m_alpha);

	// Automatic rotation
	if(m_autoRotation)
	{
		m_alpha += m_interval;
	}

	// Setting the cameraPos in the vertex buffer to the actual camera position (y should always be 0)
	VertexBuffer.m_WSCameraPosition[0] = m_camPosX;
	VertexBuffer.m_WSCameraPosition[1] = m_camPosY;
	VertexBuffer.m_WSCameraPosition[2] = m_camPosZ;

	// Set light to a constant position, so we can se reflections on the texture.
	VertexBuffer.m_WSLightPosition[0] = 5.0f;
	VertexBuffer.m_WSLightPosition[1] = 5.0f;
	VertexBuffer.m_WSLightPosition[2] = -20.0f;

	UploadConstantBuffer(&VertexBuffer, m_pVertexConstantBuffer);

	SPixelBuffer PixelBuffer;

	// Set the Lights Colors and Specular Color to static values
	// which work well for lighting
	PixelBuffer.m_AmbientLightColor[0] = 0.2f;
	PixelBuffer.m_AmbientLightColor[1] = 0.2f;
	PixelBuffer.m_AmbientLightColor[2] = 0.2f;
	PixelBuffer.m_AmbientLightColor[3] = 1.0f;

	PixelBuffer.m_DiffuseLightColor[0] = 0.7f;
	PixelBuffer.m_DiffuseLightColor[1] = 0.7f;
	PixelBuffer.m_DiffuseLightColor[2] = 0.7f;
	PixelBuffer.m_DiffuseLightColor[3] = 1.0f;

	PixelBuffer.m_SpecularColor[0] = 1.0f;
	PixelBuffer.m_SpecularColor[1] = 1.0f;
	PixelBuffer.m_SpecularColor[2] = 1.0f;
	PixelBuffer.m_SpecularColor[3] = 1.0f;

	PixelBuffer.m_SpecularExponent = 100.0f;

	UploadConstantBuffer(&PixelBuffer, m_pPixelConstantBuffer);

	// -----------------------------------------------------------------------------
	// Draw the mesh. This will activate the shader, constant buffers, and textures
	// of the material on the GPU and render the mesh to the current render targets.
	// -----------------------------------------------------------------------------
	DrawMesh(m_pMesh);


	if(m_showGround)
	{
		// -----------------------------------------------------------------------------
		// Upload the world matrix and the view projection matrix to the GPU. This has
		// to be done before drawing the mesh, though not necessarily in this method.
		// -----------------------------------------------------------------------------
		SGroundVertexBuffer GroundVertexBuffer;

		GetIdentityMatrix(GroundVertexBuffer.m_WorldMatrix);

		MulMatrix(m_ViewMatrix, m_ProjectionMatrix, GroundVertexBuffer.m_ViewProjectionMatrix);

		UploadConstantBuffer(&GroundVertexBuffer, m_pGroundVertexConstantBuffer);

		// -----------------------------------------------------------------------------
		// Draw the mesh. This will activate the shader, constant buffers, and textures
		// of the material on the GPU and render the mesh to the current render targets.
		// -----------------------------------------------------------------------------

		DrawMesh(m_pGroundMesh);
	}


	return true;
}

bool CApplication::InternOnKeyEvent(unsigned int _Key, bool _IsKeyDown, bool _IsAltDown)
{
	// Movement of the camera position
	if(_Key == 'W' && _IsKeyDown)
	{
		m_radius -= m_interval * 2;
		std::cout << "Move camera forward" << std::endl;
	}
	if((_Key == 'A' || _Key == 37) && _IsKeyDown)
	{
		m_alpha += m_interval;
		std::cout << "Move camera left" << std::endl;
	}
	if(_Key == 'S' && _IsKeyDown)
	{
		m_radius += m_interval * 2;
		std::cout << "Move camera backward" << std::endl;
	}
	if((_Key == 'D' || _Key == 39) && _IsKeyDown)
	{
		m_alpha -= m_interval;
		std::cout << "Move camera right" << std::endl;
	}
	if(_Key == 40 && _IsKeyDown)
	{
		m_camPosY -= m_interval * 2;
		std::cout << "Move camera up" << std::endl;
	}
	if(_Key == 38 && _IsKeyDown)
	{
		m_camPosY += m_interval * 2;
		std::cout << "Move camera down" << std::endl;
	}

	// Toggle automatic rotation of camera with spacebar
	if(_Key == 32 && _IsKeyDown)
	{
		m_autoRotation = !m_autoRotation;
		std::cout << "Toggle automatic rotation" << std::endl;
	}

	// Toggle drawing of ground
	if(_Key == 'G' && _IsKeyDown)
	{
		m_showGround = !m_showGround;
		std::cout << "Toggle drawing of ground" << std::endl;
	}

	return true;
}

// -----------------------------------------------------------------------------

void main()
{
	CApplication Application;

	RunApplication(800, 600, "Billbord + Normal Mapping Shader", &Application);
}