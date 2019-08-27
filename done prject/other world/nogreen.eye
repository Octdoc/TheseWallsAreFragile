Texture2D tex;
SamplerState ss;

cbuffer LightBuffer
{
	float4 lightColor;
	float3 lightPosition;
	float ambient;
};

struct PixelInputType
{
	float4 position:SV_POSITION;
	float3 pos:POSITION;
	float2 tex:TEXCOORD;
	float3 normal:NORMAL;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 color = tex.Sample(ss, input.tex);
	float3 lightDirection = normalize(lightPosition - input.pos);
	float intensity = saturate(dot(normalize(input.normal), lightDirection));
	intensity = ambient + (1.0f - ambient) * intensity;
	color.rgb *= lightColor.rgb * intensity;
	if (color.r == 0.0f && color.b == 0.0f)
		clip(-1.0f);
	color.g = 0.0f;
	return color;
}