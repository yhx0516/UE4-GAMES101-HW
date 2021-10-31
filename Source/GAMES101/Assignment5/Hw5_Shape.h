// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hw5_Shape.generated.h"

enum  EMaterialType
{
	DIFFUSE_AND_GLOSSY,
	REFLECTION_AND_REFRACTION,
	REFLECTION
};

UCLASS(Abstract)
class GAMES101_API AHw5_Shape : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHw5_Shape() 
		: materialType(EMaterialType::DIFFUSE_AND_GLOSSY)
		, ior(1.3)
		, Kd(0.8)
		, Ks(0.2)
		, diffuseColor(0.2)
		, specularExponent(25) {};

public:
	virtual bool intersect(const FVector&, const FVector&, float&, uint32_t&, FVector2D&) const { return false; };

	virtual void getSurfaceProperties(const FVector&, const FVector&, const uint32_t&, const FVector2D&, FVector&,
		FVector2D&) const {};

	virtual FVector evalDiffuseColor(const FVector2D&) const
	{
		return diffuseColor;
	}

	// material properties
	EMaterialType materialType;
	float ior;
	float Kd, Ks;
	FVector diffuseColor;
	float specularExponent;

};
