//
//  Shaders.metal
//  Nintendoish-iOS
//
//  Created by Drew Ingebretsen on 6/5/18.
//  Copyright © 2018 Drew Ingebretsen. All rights reserved.
//

// File for Metal kernel and shader functions

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct
{
    float2 position;
    float2 texturePosition;
} Vertex;

typedef struct
{
    float4 position [[position]];
    float2 texCoord;
} ColorInOut;

vertex ColorInOut vertexShader(uint vertexID [[ vertex_id ]],
                               constant Vertex *vertexArray [[ buffer(0) ]])
{
    ColorInOut out;

    float2 position = vertexArray[vertexID].position.xy;
    out.position = float4(position.x, position.y, 0.0, 1.0);
    out.texCoord = vertexArray[vertexID].texturePosition;
    return out;
}

fragment float4 fragmentShader(ColorInOut in [[stage_in]],
                               texture2d<uint> colorMap [[ texture(0) ]])
{
    constexpr sampler colorSampler(mag_filter::linear,
                                   min_filter::linear);

    uint4 colorSample = colorMap.sample(colorSampler, in.texCoord.xy);
    return float4(colorSample) / 255;
}
