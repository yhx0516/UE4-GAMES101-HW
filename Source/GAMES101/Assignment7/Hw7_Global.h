// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <algorithm>
#include <cmath>
#include <cassert>

static float EPSILON = 0.0001; 
static float kInfinity = TNumericLimits<float>::Max();

// 光线结构体
struct Ray {
	
	FVector origin;
	FVector direction, direction_inv;
	double t;//transportation time,
	double t_min, t_max;
	FORCEINLINE Ray() : origin(0), direction(0), t(0) {}
	FORCEINLINE Ray(const FVector& ori, const FVector& dir, const double _t = 0.0f) 
		: origin(ori), direction(dir), t(_t) 
	{
		direction_inv = FVector(1. / direction.X, 1. / direction.Y, 1. / direction.Z);
		t_min = 0.0;
		t_max = TNumericLimits<double>::Max(); 

	}

	FORCEINLINE FVector operator()(double _t) const  //Destination = origin + t*direction
	{ 
		return origin + direction * _t;
	}

};

class IHw7_Object;
class Material;
// 相交信息结构体
struct Intersection
{
	Intersection() {
		happened = false;
		coords = FVector();
		normal = FVector();
		distance = TNumericLimits<double>::Max();
		obj = nullptr;
	}
	bool happened;
	FVector coords;
	FVector tcoords;
	FVector normal;
	FVector emit;
	double distance;
	IHw7_Object* obj;

	Material* m;
};

struct Hit_payload
{
	float tNear;
	uint32 index;
	FVector2D uv;
	TScriptInterface<IHw7_Object> hit_obj;
};