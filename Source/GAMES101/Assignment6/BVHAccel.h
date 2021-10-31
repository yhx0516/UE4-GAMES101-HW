// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hw6_Global.h"
#include "Bounds3.h"
#include "ObjectInterface.h"
#include "BVHAccel.generated.h"


UCLASS()
class GAMES101_API UBVHBuildNode : public UObject
{
	GENERATED_BODY()
public:
	int splitAxis = 0, firstPrimOffset = 0, nPrimitives = 0;
	// BVHBuildNode Public Methods
	UBVHBuildNode() {
		bounds = Bounds3();
		left = nullptr; right = nullptr;
		object = nullptr;
	}
public:
	
	Bounds3 bounds;
	UPROPERTY()
		UBVHBuildNode* left;
	UPROPERTY()
		UBVHBuildNode* right;
	UPROPERTY()
		TScriptInterface<IObjectInterface> object;
};

UCLASS()
class GAMES101_API UBVHAccel : public UObject
{
	GENERATED_BODY()
public:
	// BVHAccel Public Types
	enum class SplitMethod { NAIVE, SAH };
	
	// BVHAccel Public Methods
	UBVHAccel();
	void Init(TArray<TScriptInterface<IObjectInterface>> p,
		int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::NAIVE, bool _bDraw = false);

	Bounds3 WorldBound() const;

	Intersection Intersect(const Ray& ray) const;
	Intersection getIntersection(UBVHBuildNode* node, const Ray& ray)const;
	bool IntersectP(const Ray& ray) const;
	
	UPROPERTY()
		UBVHBuildNode* root;

	// BVHAccel Private Methods
	UBVHBuildNode* recursiveBuild(TArray<TScriptInterface<IObjectInterface>> objects);

	// BVHAccel Private Data
	int maxPrimsInNode;
	SplitMethod splitMethod;
	bool bDraw = false;
	
	UPROPERTY()
		TArray<TScriptInterface<IObjectInterface>> primitives;
	
	
};
