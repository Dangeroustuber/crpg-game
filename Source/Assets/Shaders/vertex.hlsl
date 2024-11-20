struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct MeshVertex
{
    float3 position;
    float4 color;
};

cbuffer PushConstants : register(b0)
{
    uint vertexBufferIndex;
    uint cameraBufferIndex;
}

struct CameraData
{
    float4x4 viewProjection;
}; 

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    
    StructuredBuffer<MeshVertex> vertices = ResourceDescriptorHeap[vertexBufferIndex];
    ConstantBuffer<CameraData> camera = ResourceDescriptorHeap[cameraBufferIndex];
    
    MeshVertex vertex = vertices[vertexID];
    
    output.position = mul(float4(vertex.position, 1.0f), camera.viewProjection);
    output.color = vertex.color;
    
    return output;
}