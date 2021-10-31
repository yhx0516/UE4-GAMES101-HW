// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include <algorithm>

static float EPSILON = 0.00001; //本处偏移需要放大十倍，否则有噪点
static float kInfinity = TNumericLimits<float>::Max();
// 材质类型
enum  EMaterialType
{
	DIFFUSE_AND_GLOSSY,
	REFLECTION_AND_REFRACTION,
	REFLECTION
};

// 材质
class Material {
public:
	EMaterialType m_type;
	FVector m_color;
	FVector m_emission;
	float ior;
	float Kd, Ks;
	float specularExponent;
	//Texture tex;

	inline Material(EMaterialType t = DIFFUSE_AND_GLOSSY, FVector c = FVector(1, 1, 1), FVector e = FVector(0, 0, 0)) {
		m_type = t;
		m_color = c;
		m_emission = e;
	}
	inline EMaterialType getType(){ return m_type; }
	inline FVector getColor(){ return m_color; }
	inline FVector getColorAt(double u, double v) { return FVector(); }
	inline FVector getEmission(){ return m_emission; }
};


// 普通光源
class Light
{
public:
	Light(const FVector& p, const FVector& i) : position(p), intensity(i) {}
	virtual ~Light() = default;
	FVector position;
	FVector intensity=FVector::OneVector;
};

 // 区域光源
class AreaLight : public Light
{
public:
	AreaLight(const FVector& p, const FVector& i) : Light(p, i)
	{
		normal = FVector(0, -1, 0);
		u = FVector(1, 0, 0);
		v = FVector(0, 0, 1);
		length = 100;
	}

	FVector SamplePoint() const
	{
		auto random_u = UKismetMathLibrary::RandomFloat();
		auto random_v = UKismetMathLibrary::RandomFloat();
		return position + random_u * u + random_v * v;
	}

	float length;
	FVector normal;
	FVector u;
	FVector v;
};

// 光线结构体
struct Ray {
	//Destination = origin + t*direction
	FVector origin;
	FVector direction, direction_inv;
	double t;//transportation time,
	double t_min, t_max;

	Ray(const FVector& ori, const FVector& dir, const double _t = 0.0) : origin(ori), direction(dir), t(_t) {
		direction_inv = FVector(1. / direction.X, 1. / direction.Y, 1. / direction.Z);
		t_min = 0.0;
		t_max = TNumericLimits<double>::Max(); 

	}

	FVector operator()(double _t) const { return origin + direction * _t; }
	/*
	friend std::ostream& operator<<(std::ostream& os, const Ray& r) {
		os << "[origin:=" << r.origin << ", direction=" << r.direction << ", time=" << r.t << "]\n";
		return os;
	}*/
};

class IObjectInterface;
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
	FVector normal;
	double distance;
	IObjectInterface* obj;

	Material* m;
};

struct Hit_payload
{
	float tNear;
	uint32 index;
	FVector2D uv;
	TScriptInterface<IObjectInterface> hit_obj;
};