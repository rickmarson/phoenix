/*


*/

#include "d3d11_renderer.h"
#include "d3d11_texture.h"
#include "hash.h"
#include "resource_cache.h"
#include "mesh.h"
#include "Rapidxml/rapidxml.hpp"

using namespace rapidxml;

namespace NYX {

const D3D11_INPUT_ELEMENT_DESC D3D11Renderer::vertexDesc[3] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC D3D11Renderer::particlesVertexDesc[3] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"VELOCITY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
};


D3D11Renderer::D3D11Renderer(SDL_Window *win, SDL_SysWMinfo winInfo) :
	pDevice(nullptr),
	pContext(nullptr),
	pSwapChain(nullptr),
	pRenderTargetView(nullptr),
	pDepthStencilBuffer(nullptr),
	pDepthStencilView(nullptr),
	pRasterizerState(nullptr),
	mUseMSAA(true),
	m4xMsaa_quality(0),
	mActiveEffect(nullptr)
{
	pSDLWindow = win;
	mSDLWindowInfo = winInfo;

	// store window size
	HWND windowHandle = winInfo.info.win.window;
	LPRECT windowDims = nullptr;
	GetWindowRect(windowHandle, windowDims);
	UINT width =  windowDims->right - windowDims->left;
	UINT height = windowDims->bottom - windowDims->top;

	// additional device flags
	UINT flags = 0;

#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// feature levels (0 -> first level used if available)
	const UINT max_feature_levels = 6;
	const D3D_FEATURE_LEVEL featureLevels[max_feature_levels] = { 
		D3D_FEATURE_LEVEL_11_1, 
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_1
	};

	HRESULT result = D3D11CreateDevice(nullptr, // display adapter
		D3D_DRIVER_TYPE_HARDWARE, // driver type, always hardware
		nullptr, // software driver
		flags,
		featureLevels,
		max_feature_levels,
		D3D11_SDK_VERSION,
		&pDevice,
		&mFeatureLevel,
		&pContext);

	if (FAILED(result))
	{
		LogManager::GetInstance()->LogMessage("Error! Failed to Create D3D11 Device!");
	}
	else
	{
		LogManager::GetInstance()->LogMessage("D3D11 Device Created.");

		// device has been created, we can perform additional setup

		// 4X MSAA quality support
		pDevice->CheckMultisampleQualityLevels(mPixelFormat, 4, &m4xMsaa_quality);
		// 4X MSAA is always supported, assert if it's not greater than 0
		assert(m4xMsaa_quality > 0);

		// create the swap chain (i.e. double buffering)
		DXGI_SWAP_CHAIN_DESC bufferDesc;

		bufferDesc.BufferDesc.Width = width;
		bufferDesc.BufferDesc.Height = height;
		bufferDesc.BufferDesc.RefreshRate.Numerator = 60; 
		bufferDesc.BufferDesc.RefreshRate.Denominator = 1;
		bufferDesc.BufferDesc.Format = mPixelFormat;
		bufferDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		bufferDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// msaa is always enabled, maybe add an option to disbale it somehow
		bufferDesc.SampleDesc.Count = mUseMSAA ? 4 : 1;
		bufferDesc.SampleDesc.Quality = mUseMSAA ? (m4xMsaa_quality - 1) : 0;

		bufferDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		bufferDesc.BufferCount = 1;
		bufferDesc.OutputWindow = windowHandle;
		bufferDesc.Windowed = true; // fullscreen?
		bufferDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		bufferDesc.Flags = 0;

		// a very convoluted way to get the pointer to the DXGIFactory used to create pDevice... 
		IDXGIDevice* dxgiDevice = nullptr;
		pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
		assert(dxgiDevice != nullptr);

		IDXGIAdapter* dxgiAdapter = nullptr;
		dxgiDevice->QueryInterface(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
		assert(dxgiDevice != nullptr);

		IDXGIFactory* dxgiFactory = nullptr;
		dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
		assert(dxgiDevice != nullptr);
		
		result = dxgiFactory->CreateSwapChain(pDevice, &bufferDesc, &pSwapChain);
	
		dxgiDevice->Release();
		dxgiAdapter->Release();
		dxgiFactory->Release();

		if (FAILED(result))
		{
			LogManager::GetInstance()->LogMessage("Error! Failed to create Swap Chain!");
		}
		else
		{
			// now that we have a render buffer, we have to create a render target view so that we can
			// bind the contents of the buffer to the window
			// this represents the color buffer
			ID3D11Texture2D* renderBuffer;
			pSwapChain->GetBuffer(0, // index of the buffer, we only have one
				__uuidof(ID3D10Texture2D), 
				reinterpret_cast<void**>(&renderBuffer)
			);

			pDevice->CreateRenderTargetView(renderBuffer, 
				nullptr, // render target view decription, if null inheriths the swap chain description
				&pRenderTargetView);

			renderBuffer->Release();

			// finally, create the depth/stencil buffer
			D3D11_TEXTURE2D_DESC depthStencilDesc;
			depthStencilDesc.Width = width;
			depthStencilDesc.Height = height;
			depthStencilDesc.MipLevels = 1;
			depthStencilDesc.ArraySize = 1;
			depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilDesc.SampleDesc.Count = mUseMSAA ? 4 : 1;
			depthStencilDesc.SampleDesc.Quality = mUseMSAA ? (m4xMsaa_quality - 1) : 0;
			depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
			depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthStencilDesc.CPUAccessFlags = 0;
			depthStencilDesc.MiscFlags = 0;

			pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &pDepthStencilBuffer);
			pDevice->CreateDepthStencilView(pDepthStencilBuffer, nullptr, &pDepthStencilView);

			// bind color, depth and stencil buffers to the context output (the window)
			pContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

			// setup the initial rasterizer state and we are done
			D3D11_RASTERIZER_DESC rasterizerDesc;
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.FrontCounterClockwise = true;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0.0f;
			rasterizerDesc.SlopeScaledDepthBias = 0.0f;
			rasterizerDesc.DepthClipEnable = true;
			rasterizerDesc.ScissorEnable = false;
			rasterizerDesc.MultisampleEnable = true;
			rasterizerDesc.AntialiasedLineEnable = true;

			pDevice->CreateRasterizerState(&rasterizerDesc, &pRasterizerState);
			pContext->RSSetState(pRasterizerState);
		}
	}
}

D3D11Renderer::~D3D11Renderer()
{
	pDevice->Release();
	pContext->Release();
	pSwapChain->Release();
	pRenderTargetView->Release();
	pDepthStencilBuffer->Release();
	pDepthStencilView->Release();
	pRasterizerState->Release();
}

void D3D11Renderer::SetViewport(int x, int y, int w, int h)
{
	mViewport.TopLeftX = x;
	mViewport.TopLeftY = y;
	mViewport.Width =  w;
	mViewport.Height = h;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	pContext->RSSetViewports(1, &mViewport);
}

void D3D11Renderer::ClearScreen(float *clearColor)
{
	pContext->ClearRenderTargetView(pRenderTargetView, clearColor);
	pContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D11Renderer::SwapBuffers()
{
	pSwapChain->Present(0, 0);
}

void D3D11Renderer::EnableCulling() 
{
	D3D11_RASTERIZER_DESC *rasterizerDesc = nullptr;
	pRasterizerState->GetDesc(rasterizerDesc);

	if (!rasterizerDesc)
		return;

	rasterizerDesc->CullMode = D3D11_CULL_BACK;

	// necessary?
	pContext->RSSetState(pRasterizerState);
}

void D3D11Renderer::DisableCulling() 
{
	D3D11_RASTERIZER_DESC *rasterizerDesc = nullptr;
	pRasterizerState->GetDesc(rasterizerDesc);

	if (!rasterizerDesc)
		return;

	rasterizerDesc->CullMode = D3D11_CULL_NONE;

	// necessary?
	pContext->RSSetState(pRasterizerState);
}

void D3D11Renderer::CullFrontFace() 
{
	D3D11_RASTERIZER_DESC *rasterizerDesc = nullptr;
	pRasterizerState->GetDesc(rasterizerDesc);

	if (!rasterizerDesc)
		return;

	rasterizerDesc->CullMode = D3D11_CULL_FRONT;

	// necessary?
	pContext->RSSetState(pRasterizerState);
}

void D3D11Renderer::CullBackFace() 
{
	D3D11_RASTERIZER_DESC *rasterizerDesc = nullptr;
	pRasterizerState->GetDesc(rasterizerDesc);

	if (!rasterizerDesc)
		return;

	rasterizerDesc->CullMode = D3D11_CULL_BACK;

	// necessary?
	pContext->RSSetState(pRasterizerState);
}

void D3D11Renderer::SetFrontFaceToCCW() 
{
	D3D11_RASTERIZER_DESC *rasterizerDesc = nullptr;
	pRasterizerState->GetDesc(rasterizerDesc);

	if (!rasterizerDesc)
		return;

	rasterizerDesc->FrontCounterClockwise = true;

	// necessary?
	pContext->RSSetState(pRasterizerState);
}

void D3D11Renderer::SetFrontFaceToCW() 
{
	D3D11_RASTERIZER_DESC *rasterizerDesc = nullptr;
	pRasterizerState->GetDesc(rasterizerDesc);

	if (!rasterizerDesc)
		return;

	rasterizerDesc->FrontCounterClockwise = false;

	// necessary?
	pContext->RSSetState(pRasterizerState);
}

//deprecated
void D3D11Renderer::SetActiveCamera(Matrix4x4 &viewMatrix, Matrix4x4 &projectionMatrix,
								 float clipNear, float clipFar, float fov)
{

}
//--------------------------------

Texture* D3D11Renderer::CreateTexture( void )
{
	D3D11Texture* new_texture = new D3D11Texture();
	return dynamic_cast<Texture*>(new_texture);
}

uint D3D11Renderer::LoadTexture(ubyte *pixels, uint w, uint h) 
{ 
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = w;
	textureDesc.Height = h;
	// TODO: should determine the max mip level from w and h and pass it here
	// for the moment turn off mipmapping
	textureDesc.MipLevels = textureDesc.ArraySize = 1;
	// for the moment we are only using uncompressed textures so this is always true,
	// but eventually the texture format would need to be determine somehow. 
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = mUseMSAA ? 4 : 1;
	textureDesc.SampleDesc.Quality = mUseMSAA ? (m4xMsaa_quality - 1) : 0;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D *texture = nullptr;
	// note: if the texture is multisampled, we have to initialise the data after creation,
	// pass null here
	pDevice->CreateTexture2D( &textureDesc, nullptr, &texture);

	// and initialise pixel data
	// this would need to change for mipmapped textures
	UINT subresource_index = D3D11CalcSubresource(0, // mip slice
		0, // array slice
		1 // total mip levels
	);

	D3D11_MAPPED_SUBRESOURCE *subresource = nullptr;
	pContext->Map( texture, subresource_index, D3D11_MAP_WRITE_DISCARD, 0, subresource);
	memcpy(subresource->pData, pixels, w*h*4);
	pContext->Unmap(texture, subresource_index);

	ID3D11ShaderResourceView *textureSRV;
	pDevice->CreateShaderResourceView(texture, nullptr, &textureSRV);

	// generate mipmaps?
	// pContext->GenerateMips(textureSRV);

	// store the resource view and map it to a unique int, so that the rest of the
	// code only sees an integer texture index, same as OpenGL
	string hashseed("d3d9texture");
	hashseed += std::to_string(rand());
	uint id = GenerateHash(hashseed.c_str(), hashseed.size(), 5);

	mLoadedTextures[id] = textureSRV;

	return id; 
}

void D3D11Renderer::DeleteTexture(uint texID) 
{
	auto it = mLoadedTextures.find(texID);
	if (it == mLoadedTextures.end())
		return;

	(*it).second->Release();
	mLoadedTextures.erase(it);
}

void D3D11Renderer::BindTexture(uint texID, bool bIsCubeMap) 
{
	auto tex_it = mLoadedTextures.find(texID);

	if (tex_it == mLoadedTextures.end())
	{
		LogManager::GetInstance()->LogMessage("Warning: Texture Not Found.");
		return;
	}

	// assume samplers are always set in the pixel shader, to mirror opengl behaviour
	ID3D11ShaderResourceView *samplerResource = (*tex_it).second;

	pContext->PSSetShaderResources(0, 1, &samplerResource);
}

void D3D11Renderer::UnbindAllTextures() 
{
	// not sure, possibly nothing to do
}

int D3D11Renderer::GetActiveTexStackDepth() 
{
	// not used
	return 0;
}

uint D3D11Renderer::CreateShaderProgram(std::string shaderName)
{
	auto fxid_it = mEffectName2Id.find(shaderName);

	if (fxid_it != mEffectName2Id.end())
	{
		//effect already loaded, no need to do anything
		return (*fxid_it).second;
	}
	
	//else load effect
	Effect *fx= Effect::CreateEffect(shaderName, D3D);

	D3D11Effect* d3d11fx = dynamic_cast<D3D11Effect*>(fx);

	d3d11fx->Initialise(pDevice, pContext);

	//fetch shader package
	char* fx_pkg = ResourceCache::GetInstance()->RequestSourceCode(shaderName+".xml");

	if (!d3d11fx->LoadEffectFromFile(fx_pkg))
	{
		delete fx_pkg;
		return 0;
	}
	else
	{
		mEffects[d3d11fx->ProgramId()] = d3d11fx;
		mEffectName2Id[shaderName] = d3d11fx->ProgramId();
		delete fx_pkg;
		// temporary eventually will return Effect*
		return d3d11fx->ProgramId();
	}
}

std::list<ShaderUniform>& D3D11Renderer::GetShaderUniforms(void)
{
	assert(mActiveEffect);

	return mActiveEffect->GetShaderUniforms();
}

void D3D11Renderer::LoadShaderUniforms(void)
{
	if (mActiveEffect == nullptr)
	{
		return;
	}

	mActiveEffect->LoadUniforms();
}

void D3D11Renderer::UseShader(uint shaderID)
{
	auto fx_it = mEffects.find(shaderID);

	if (fx_it == mEffects.end())
	{
		LogManager::GetInstance()->LogMessage("Warning: Shader Not Found.");
		return;
	}

	mActiveEffect = (*fx_it).second;
	
}

void D3D11Renderer::GenerateVBO(ModelPtr model) 
{
	// vertex buffer
	D3D11_BUFFER_DESC vboDesc;
	vboDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vboDesc.ByteWidth = model->getVertexSize()*model->getNumberOfVertices();
	vboDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vboDesc.CPUAccessFlags = 0;
	vboDesc.MiscFlags = 0;
	vboDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initVBOData;
	initVBOData.pSysMem = model->getVertexBuffer();

	ID3D11Buffer *vbo;
	pDevice->CreateBuffer(&vboDesc, &initVBOData, &vbo);

	// index buffer
	D3D11_BUFFER_DESC indexDesc;
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexDesc.ByteWidth = model->getIndexSize()*model->getNumberOfIndices();
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initIndexData;
	initIndexData.pSysMem = model->getIndexBuffer();

	ID3D11Buffer *ibo;
	pDevice->CreateBuffer(&indexDesc, &initIndexData, &ibo);

	// store both buffers and mapped to integer so that they can be accessed in an
	// OpenGL-like way by the rest of the code
	string hashseed(model->getPath());
	uint id = GenerateHash(hashseed.c_str(), hashseed.size(), 5);
	mVBOs[id] = vbo;
	mIBOs[id] = ibo;

	model->SetVBO(id, id);
}

uint D3D11Renderer::GenerateEmptyVBO(uint elementSize, uint count) 
{
	// vertex buffer
	D3D11_BUFFER_DESC vboDesc;
	vboDesc.Usage = D3D11_USAGE_DYNAMIC;
	vboDesc.ByteWidth = elementSize * count;
	vboDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vboDesc.CPUAccessFlags = 0;
	vboDesc.MiscFlags = 0;
	vboDesc.StructureByteStride = 0;

	ID3D11Buffer *vbo;
	pDevice->CreateBuffer(&vboDesc, nullptr, &vbo);

	string hashseed("empty vbo");
	hashseed += std::to_string(rand());
	uint id = GenerateHash(hashseed.c_str(), hashseed.size(), 5);
	mVBOs[id] = vbo;

	return id;
}

void D3D11Renderer::SetupVAO(MeshPtr mesh) 
{
	// nothing to do
}

uint D3D11Renderer::SetupVAO_FX(MaterialPtr mat) 
{
	// nothing to do
	return 0;
}

uint D3D11Renderer::SetupVAO_SkyBox(uint shaderId) 
{
	// nothing to do
	return 0; 
}
	
void D3D11Renderer::FinishVBOSetup() 
{
	// nothing to do
}

void D3D11Renderer::DeleteVBO(uint vbo) 
{
	auto it_v = mVBOs.find(vbo);
	if (it_v == mVBOs.end())
		return;

	(*it_v).second->Release();
	mVBOs.erase(it_v);

	auto it_i = mIBOs.find(vbo);
	if (it_i == mIBOs.end())
		return;

	(*it_i).second->Release();
	mIBOs.erase(it_i);
}

void D3D11Renderer::DeleteVAO(uint vao) 
{
	// nothing to do
}

float* D3D11Renderer::UpdateVBO(uint vbo) 
{ 
	auto it = mVBOs.find(vbo);
	if (it == mVBOs.end())
		return nullptr;

	D3D11_MAPPED_SUBRESOURCE *subresource = nullptr;
	pContext->Map( (*it).second, 0, D3D11_MAP_WRITE_DISCARD, 0, subresource);
	
	return (float*)subresource->pData; 
}

void D3D11Renderer::ReleaseVBO(uint vbo) 
{
	auto it = mVBOs.find(vbo);
	if (it == mVBOs.end())
		return;

	pContext->Unmap( (*it).second, 0 );
}

void D3D11Renderer::BeginScene()
{
	// nothing to do
}

void D3D11Renderer::EndScene()
{
	// nothing to do
}

void D3D11Renderer::DrawVertexBuffer(MeshPtr mesh)
{
	// set vertex buffer
	auto it_v = mVBOs.find(mesh->GetParent()->GetVBO());
	if (it_v == mVBOs.end())
	{
		LogManager::GetInstance()->LogMessage("Warning. Trying to draw non-existant vertex buffer.");
		return;
	}

	UINT stride = mesh->GetParent()->getVertexSize();
	UINT offset = mesh->StartIndex()*stride;

	pContext->IASetVertexBuffers(0, 1, &(*it_v).second, &stride, &offset);
	
	// set index buffer
	auto it_i = mIBOs.find(mesh->GetParent()->GetIBO());
	if (it_i == mIBOs.end())
	{
		LogManager::GetInstance()->LogMessage("Warning. Trying to draw non-existant index buffer.");
		return;
	}

	pContext->IASetIndexBuffer((*it_i).second, DXGI_FORMAT_R32_UINT, mesh->StartIndex());

	UINT vertexCount = 0;

	mActiveEffect->Begin();

	switch (mesh->PolygonType())
	{
	case E_POINT:
		vertexCount = mesh->PolyCount();
		break;
	case E_LINE:
		vertexCount = mesh->PolyCount() * 2;
		break;
	case E_TRIANGLE:
		vertexCount = mesh->PolyCount() * 3;
		break;
	case E_QUAD:
		vertexCount = mesh->PolyCount() * 4;
		break;
	case E_POLYGON:
		vertexCount = mesh->PolyCount()*mesh->CustomPolySize();
		break;
	default:
		break;
	}

	pContext->DrawIndexed(vertexCount, mesh->StartIndex(), 0);

	mActiveEffect->End();

}

void D3D11Renderer::DrawVertexBuffer(uint VBO, uint VAO, uint count, E_POLYGON_TYPE polyType)
{
	//we are drawing a particle FX buffer

	// set vertex buffer
	auto it = mVBOs.find(VBO);
	if (it == mVBOs.end())
	{
		LogManager::GetInstance()->LogMessage("Warning. Trying to draw non-existant vertex buffer.");
		return;
	}

	UINT stride = 7*4; // each vertex in this case has 7 components (3 pos, 3 vel), all floats
	UINT offset = 0;

	pContext->IASetVertexBuffers(0, 1, &(*it).second, &stride, &offset);
	
	mActiveEffect->Begin();

	pContext->Draw(count, 0);
	
	mActiveEffect->End();
}

}