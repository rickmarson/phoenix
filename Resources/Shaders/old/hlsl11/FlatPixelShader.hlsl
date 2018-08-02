//input, must match VS output type
struct PixelInputType 
{
	float4 inPos : SV_POSITION;
	float3 inColor : COLOR;
};

//actual pixel shader
void main(in PixelInputType input, out float4 color : SV_TARGET)
{ 
	color = float4(input.inColor, 1.0f);
}
