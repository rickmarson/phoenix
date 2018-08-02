/*


*/

#ifndef D3D11_EFFECT_H
#define D3D11_EFFECT_H

#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <dxgi.h>

#include "../effect.h"

namespace NYX {

class NYX_EXPORT D3D11Effect : public Effect
{
public:

	D3D11Effect(std::string name);
    virtual ~D3D11Effect();

	void Initialise(ID3D11Device* device, ID3D11DeviceContext* context);

	virtual bool LoadEffectFromFile(std::string shaderName) override;
	virtual void LoadUniforms( void ) override;

	virtual void Begin( void ) override;
	virtual void End( void ) override;


private:

	ID3D11Device*			pDevice;
	ID3D11DeviceContext*	pContext;
	
	
	class ShaderProgram 
	{
	public:

	 	ShaderProgram();
		~ShaderProgram();
		void Release();

		ID3D11VertexShader   *pVertexShader;
		ID3D11PixelShader    *pPixelShader;
		ID3D11GeometryShader *pGeometryShader;
	}; 

	ShaderProgram mProgram;

	float *constantBuffer = nullptr;
	float *bufferCursor = nullptr;

	D3D11_BUFFER_DESC constantBufferDesc;
	D3D11_SUBRESOURCE_DATA constantBufferInitData;
	ID3D11Buffer *d3d11ConstantBuffer = nullptr;
};

}


#endif //D3D11_EFFECT_H
