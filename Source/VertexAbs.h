#pragma once
#include <d3d9.h>

extern const DWORD dwTex1FVF;
extern const DWORD dwTex2FVF;

struct Tex1Vertex
{
	float x, y, z;
	float u, v;
};

struct Tex2Vertex
{
	float x, y, z;
	float u0, v0;
	float u1, v1;
};