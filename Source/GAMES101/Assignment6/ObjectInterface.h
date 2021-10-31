// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Hw6_Global.h"
#include "Bounds3.h"
#include "ObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UObjectInterface : public UInterface
{
	GENERATED_BODY()
};


class GAMES101_API IObjectInterface
{
	GENERATED_BODY()
public:
	virtual bool intersect(const Ray& ray) = 0;
	virtual bool intersect(const Ray& ray, float&, uint32_t&) const = 0;
	virtual Intersection getIntersection(Ray _ray) = 0;
	virtual void getSurfaceProperties(const FVector&, const FVector&, const uint32_t&, const FVector2D&, FVector&, FVector2D&) const = 0;
	virtual FVector evalDiffuseColor(const FVector2D&) const = 0;
	virtual Bounds3 getBounds() const = 0;
};
