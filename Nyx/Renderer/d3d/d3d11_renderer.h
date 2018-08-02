/*


*/

#ifndef D3D11RENDERER_H
#define D3D11RENDERER_H

#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"

#include "irenderer.h"
#include "d3d11_effect.h" 

using namespace std;

namespace NYX {

class NYX_EXPORT D3D11Renderer : public IRenderer
{
public:

	// setup and state configuration
	D3D11Renderer(SDL_Window *win, SDL_SysWMinfo winInfo);
    ~D3D11Renderer();
	
	void SetViewport(int x, int y, int w, int h);
	void ClearScreen(float *clearColor = NULL);
	void SwapBuffers();
	void EnableCulling();
	void DisableCulling();
	void CullFrontFace();
	void CullBackFace();
	void SetFrontFaceToCCW();
	void SetFrontFaceToCW();
	
	// depracated: to be deleted
	void SetActiveCamera(Matrix4x4 &viewMatrix, Matrix4x4 &projectionMatrix,
								 float clipNear, float clipFar, float fov);
	
	// textures
	Texture* CreateTexture( void ) override;
	void UnbindAllTextures( void ) override;
	int GetActiveTexStackDepth( void ) override;

	// shaders
	std::list<ShaderUniform>& GetShaderUniforms(void);
	void LoadShaderUniforms(void);
	void UseShader(uint shaderID);
	
	// buffers
	void GenerateVBO(ModelPtr model);
	uint GenerateEmptyVBO(uint elementSize, uint count);
	void SetupVAO(MeshPtr mesh);
	uint SetupVAO_FX(MaterialPtr mat);
	uint SetupVAO_SkyBox(uint shaderId);
	
	void FinishVBOSetup();
	void DeleteVBO(uint vbo);
	void DeleteVAO(uint vao);
	float* UpdateVBO(uint vbo);
	void ReleaseVBO(uint vbo);

	// drawing
	void BeginScene();
	void EndScene();
	void DrawVertexBuffer(MeshPtr mesh);
	void DrawVertexBuffer(uint VBO, uint VAO, uint count, E_POLYGON_TYPE polyType);

private:

	static const DXGI_FORMAT mPixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[3]; 
	static const D3D11_INPUT_ELEMENT_DESC particlesVertexDesc[3]; 

	SDL_Window*				pSDLWindow;
	SDL_SysWMinfo			mSDLWindowInfo;
	
	ID3D11Device*			pDevice;
	ID3D11DeviceContext*	pContext;
	D3D_FEATURE_LEVEL		mFeatureLevel;	
	IDXGISwapChain*			pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;
	ID3D11Texture2D*		pDepthStencilBuffer;
	ID3D11DepthStencilView* pDepthStencilView;
	ID3D11RasterizerState*	pRasterizerState;
	D3D11_VIEWPORT			mViewport;
	UINT					m4xMsaa_quality;

	bool					mUseMSAA;

	std::map<uint, ID3D11ShaderResourceView*> mLoadedTextures;
	std::map<uint, ID3D11Buffer*> mVBOs;
	std::map<uint, ID3D11Buffer*> mIBOs;

private:

	bool ParseShaderPackage(std::string pkgName, std::string &fxFile, bool &binary);

	std::map<uint, D3D11Effect*> mEffects;
	std::map<std::string, uint> mEffectName2Id;
	D3D11Effect* mActiveEffect;
};

}

#endif // D3D11RENDERER_H
