// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"

// 普通光源
class Light
{
public:
	Light(const FVector& p, const FVector& i) : position(p), intensity(i) {}
	virtual ~Light() = default;
	FVector position;
	FVector intensity = FVector::OneVector;
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