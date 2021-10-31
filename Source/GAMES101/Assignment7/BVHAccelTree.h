// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Hw7_Global.h"
#include "Hw7_Bounds3.h"
#include "Hw7_Object.h"
#include "BVHAccelTree.generated.h"

UCLASS()
class GAMES101_API UBVHTreeNode : public UObject
{
	GENERATED_BODY()
public:
	int splitAxis = 0, firstPrimOffset = 0, nPrimitives = 0;
	// BVHBuildNode Public Methods
	UBVHTreeNode() {
		bounds = Bounds3();
		left = nullptr; right = nullptr;
		object = nullptr;
	}
public:

	Bounds3 bounds;
	UPROPERTY()
		UBVHTreeNode* left;
	UPROPERTY()
		UBVHTreeNode* right;
	UPROPERTY()
		TScriptInterface<IHw7_Object> object;
	float area;
};

UCLASS()
class GAMES101_API UBVHAccelTree : public UObject
{
	GENERATED_BODY()
public:
	// BVHAccel Public Types
	enum class SplitMethod { NAIVE, SAH };

	// BVHAccel Public Methods
	UBVHAccelTree();
	void Init(TArray<TScriptInterface<IHw7_Object>> p,
		int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::NAIVE, bool _bDraw = false);

	Bounds3 WorldBound() const;

	Intersection Intersect(const Ray& ray) const;
	Intersection getIntersection(UBVHTreeNode* node, const Ray& ray)const;
	bool IntersectP(const Ray& ray) const;

	UBVHTreeNode* recursiveBuild(TArray<TScriptInterface<IHw7_Object>> objects);
	
	void getSample(UBVHTreeNode* node, float p, Intersection& pos, float& pdf);
	void Sample(Intersection& pos, float& pdf);
public:
	UPROPERTY()
		UBVHTreeNode* root;
	UPROPERTY()
		TArray<TScriptInterface<IHw7_Object>> primitives;
	
	int maxPrimsInNode;
	SplitMethod splitMethod;
	bool bDraw = false;
};