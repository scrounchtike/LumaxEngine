
cbuffer UniformBuffer {
	float2 position;
	float2 extents;
	float3 color;
	float padding2;
};

struct VertexInput {
	float2 geometry : POSITION;
};

struct PixelInput {
	float4 position : SV_POSITION;
};

PixelInput main(VertexInput input) {
	PixelInput output;

	output.position = float4(input.geometry * extents + position, 0.0, 1.0);

	return output;
}