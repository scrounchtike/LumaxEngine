
cbuffer UniformBuffer {
	float4x4 projection;
	float4x4 view;
	float3 position;
	float padding1;
	float3 color;
	float padding2;
};

struct VertexInput {
	float3 position : POSITION;
};

struct PixelInput {
	float4 position : SV_POSITION;
};

PixelInput main(VertexInput input) {
	PixelInput output;

	output.position = float4(input.position + position, 1.0);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	return output;
}