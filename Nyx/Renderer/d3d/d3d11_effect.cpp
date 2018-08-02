/*

*/

#include "d3d11_effect.h"
#include "hash.h"
#include "resource_cache.h"
#include "Rapidxml/rapidxml.hpp"

using namespace rapidxml;

namespace NYX {

D3D11Effect::ShaderProgram::ShaderProgram() :
	pVertexShader(nullptr),
	pPixelShader(nullptr),
	pGeometryShader(nullptr)
{}

D3D11Effect::ShaderProgram::~ShaderProgram()
{
	
}

void D3D11Effect::ShaderProgram::Release()
{
	pVertexShader->Release();
	pPixelShader->Release();
	pGeometryShader->Release();
}

D3D11Effect::D3D11Effect(std::string name) :
	Effect(name),
	pDevice(nullptr),
	pContext(nullptr)
{

}

void D3D11Effect::Initialise(ID3D11Device* device, ID3D11DeviceContext* context)
{
	pDevice = device;
	pContext = context;
}

D3D11Effect::~D3D11Effect()
{
	mProgram.Release();
}

void D3D11Effect::Begin(void)
{
	pContext->VSSetShader(mProgram.pVertexShader, nullptr, 0);
	pContext->PSSetShader(mProgram.pPixelShader, nullptr, 0);

	if (mProgram.pGeometryShader)
		pContext->GSSetShader(mProgram.pGeometryShader, nullptr, 0);
}

void D3D11Effect::End(void)
{
	
}

void D3D11Effect::LoadUniforms( void )
{
	if ( constantBuffer == nullptr ) 
	{
		//log message
		return;
	}

	//assume for now that all the programs in a technique share common uniforms
	bufferCursor = constantBuffer;

	for (auto it = mShaderUniforms.begin(); it != mShaderUniforms.end(); ++it)
	{
		memcpy(bufferCursor, (*it).AsRawPointer(), (*it).GetSizeInBytes());
		bufferCursor += (*it).GetLenght();
	}

	// update d3d 11 buffer

	pContext->UpdateSubresource(d3d11ConstantBuffer, 0, 0, constantBuffer, 0, 0 );


	// set buffer in each shader

	pContext->VSSetConstantBuffers(0, 1, &d3d11ConstantBuffer );

}

bool D3D11Effect::LoadEffectFromFile(std::string shaderName)
{
	
	// TODO
	return true;
}

}
