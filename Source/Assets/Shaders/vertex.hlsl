struct VSInput
{
    uint vertexID : SV_VertexID;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct Vertex
{
    float3 position;
    float4 color;
};

StructuredBuffer<Vertex> vertices : register(t0, space0);

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    
    Vertex vertex = vertices[input.vertexID]; // vertex pulling
    
    output.position = float4(vertex.position, 1.0f);
    output.color = vertex.color;
    
    return output;
}