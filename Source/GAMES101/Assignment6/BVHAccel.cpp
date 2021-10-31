// Fill out your copyright notice in the Description page of Project Settings.
#include "BVHAccel.h"
#include <algorithm>
#include <cassert>
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UBVHAccel::UBVHAccel(){}

void UBVHAccel::Init(TArray<TScriptInterface<IObjectInterface>> p, int _maxPrimsInNode /*= 1*/,
	SplitMethod _splitMethod /*= SplitMethod::NAIVE*/, bool _bDraw /*= false*/)
{
	primitives = MoveTemp(p);
	maxPrimsInNode = _maxPrimsInNode;
	splitMethod = splitMethod;
	bDraw = _bDraw;
	
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Start build BVH Tree ."));
	FDateTime start = UKismetMathLibrary::Now();

	if (primitives.Num() == 0)
		return;

	// 构造根节点
	root = recursiveBuild(primitives);

	FDateTime end = UKismetMathLibrary::Now();

	FString outStr = FString::Printf(TEXT("Build BVH Tree Succ.Time Taken: %i Milliseconds"), UKismetMathLibrary::GetMilliseconds(start - end));
	UKismetSystemLibrary::PrintString(GetWorld(), outStr);
}



// 构造子树 ，递归
UBVHBuildNode* UBVHAccel::recursiveBuild(TArray<TScriptInterface<IObjectInterface>> objects)
{

	if (objects.Num() == 0)
		return nullptr;

	UBVHBuildNode* node=NewObject<UBVHBuildNode>();

	// Compute bounds of all primitives in BVH node
	Bounds3 bounds;
	for (int i = 0; i < objects.Num(); ++i)
		bounds.Union(objects[i]->getBounds());

	if (objects.Num() == 1) {
		// Create leaf _BVHBuildNode_
		node->bounds = objects[0]->getBounds();
		node->object = objects[0];
		node->left = nullptr;
		node->right = nullptr;
		//return node;
	}
	else if (objects.Num() == 2) {
		node->left = recursiveBuild(TArray<TScriptInterface<IObjectInterface>>{objects[0]});
		node->right = recursiveBuild(TArray<TScriptInterface<IObjectInterface>>{objects[1]});

		node->bounds.Union(node->left->bounds);
		node->bounds.Union(node->right->bounds);
		//return node;
	}
	else {
		Bounds3 centroidBounds;
		for (int i = 0; i < objects.Num(); ++i)
			centroidBounds.Union( objects[i]->getBounds().Centroid());
		
		// 获取最长轴
		int dim = centroidBounds.maxExtent();
		
		// 按最长轴坐标 排序
		objects.Sort(
			[dim](const TScriptInterface<IObjectInterface>& f1, const TScriptInterface<IObjectInterface>& f2) {
				return f1->getBounds().Centroid()[dim] < f2->getBounds().Centroid()[dim];
			});
		// 分成两部分
		TArray<TScriptInterface<IObjectInterface>> leftshapes, rightshapes;
		for (int i = 0; i < objects.Num(); i++) {
			if (i < objects.Num() / 2)
				leftshapes.Add(objects[i]);
			else
				rightshapes.Add(objects[i]);
		}

		ensure(objects.Num() == (leftshapes.Num() + rightshapes.Num()));

		node->left = recursiveBuild(leftshapes);
		node->right = recursiveBuild(rightshapes);

		node->bounds.Union(node->left->bounds);
		node->bounds.Union(node->right->bounds);
	}

	if (bDraw) {
		bounds.Draw(GetOuter()->GetWorld(), FLinearColor::Yellow, 1.0f);
	}
		
	return node;
}



// 光线与空间相交
Intersection UBVHAccel::Intersect(const Ray& ray) const
{
	Intersection isect;
	if (!root)
		return isect;
	isect = getIntersection(root, ray);
	return isect;
}

Intersection UBVHAccel::getIntersection(UBVHBuildNode* node, const Ray& ray) const
{
	// TODO Traverse the BVH to find intersection
	Intersection isect;
	if (node ==nullptr)
		return isect;

	FVector dirisNeg = FVector::ZeroVector; //轴正负
	dirisNeg.X = ray.direction.X > 0 ? 1 : -1;
	dirisNeg.Y = ray.direction.Y > 0 ? 1 : -1;
	dirisNeg.Z = ray.direction.Z > 0 ? 1 : -1;
	if (false == node->bounds.IntersectP(ray, ray.direction_inv, dirisNeg)) //判断是否在包围盒
		return  isect;
	
	if (node->left == nullptr && node->right == nullptr) { //直到叶子节点
		isect = node->object->getIntersection(ray);
		if(isect.happened)
			node->bounds.Draw(GetOuter()->GetWorld(), FLinearColor::Blue, 0.05f);
		return isect;
	}

	//node->bounds.Draw(worldContex, FLinearColor::Yellow, 0.1f);

	Intersection leftInter = getIntersection(node->left, ray); // 不到叶子，就一直递归下去
	Intersection rightInter = getIntersection(node->right, ray);

	return leftInter.distance < rightInter.distance ? leftInter : rightInter; //选比较近的
}