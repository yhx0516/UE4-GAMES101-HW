// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Hw7_Global.h"
#include "Hw7_Bounds3.h"
#include "Hw7_Object.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHw7_Object : public UInterface
{
	GENERATED_BODY()
};

class GAMES101_API IHw7_Object
{
	GENERATED_BODY()
public:
	virtual bool intersect(const Ray& ray) = 0;
	virtual bool intersect(const Ray& ray, float&, uint32_t&) const = 0;
	virtual Intersection getIntersection(Ray _ray) = 0;
	virtual void getSurfaceProperties(const FVector&, const FVector&, const uint32_t&, const FVector2D&, FVector&, FVector2D&) const = 0;
	virtual FVector evalDiffuseColor(const FVector2D&) const = 0;
	virtual Bounds3 getBounds() const = 0;
	virtual float getArea() = 0;
	virtual void Sample(Intersection& pos, float& pdf) = 0;
	virtual bool hasEmit() = 0;
};
