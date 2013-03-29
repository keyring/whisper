/****************************************************************************
 *
 * 	Light.h by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 ***************************************************************************/

#ifndef __LIGHT_H__
#define __LIGHT_H__ 1

#include <windows.h>
#include <xnamath.h>

struct DirectLight{
	DirectLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	XMFLOAT3 Direction;
	float unuse;	//Pack into 4D vector
};

struct PointLight{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Attenuation;
	float unuse;
};

struct SpotLight{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specualr;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Direction;
	float Spot;

	XMFLOAT3 Attenuation;
	float unuse;
};

struct Material{
	Material() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT4 Reflect;
};

#endif    //__LIGHT_H__

