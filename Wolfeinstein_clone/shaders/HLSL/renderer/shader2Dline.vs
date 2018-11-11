
cbuffer UniformBuffer {
	float2 start;
	float2 end;
	float3 color;
	float padding;
};

struct VertexInput {
	float2 geometry : POSITION;
};

struct PixelInput {
	float4 position : SV_POSITION;
};

PixelInput main(VertexInput input){
	PixelInput output;

	output.position = float4(start*(float2(1.0, 1.0) - input.geometry) + end*input.geometry, 0.0, 1.0);

	return output;
}