// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hw7_Global.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

/**
 * 
 */
class GAMES101_API Bounds3
{
public:

	FVector pMin, pMax; // two points to specify the bounding box
	Bounds3()
	{
		pMax = FVector(TNumericLimits<double>::Lowest());
		pMin = FVector(TNumericLimits<double>::Max());
	}
	Bounds3(const FVector p) : pMin(p), pMax(p) {}
	Bounds3(const FVector p1, const FVector p2)
	{
		pMin = FVector(fmin(p1.X, p2.X), fmin(p1.Y, p2.Y), fmin(p1.Z, p2.Z));
		pMax = FVector(fmax(p1.X, p2.X), fmax(p1.Y, p2.Y), fmax(p1.Z, p2.Z));
	}

	//对角线向量
	FVector Diagonal() const { return pMax - pMin; } 

	// 最大轴分量，0、1、2分别代表 X、Y、Z 轴
	int maxExtent() const
	{
		FVector d = Diagonal();
		if (d.X > d.Y && d.X > d.Z)
			return 0;
		else if (d.Y > d.Z)
			return 1;
		else
			return 2;
	}

	// 表面积
	double SurfaceArea() const 
	{
		FVector d = Diagonal();
		return 2 * (d.X * d.Y + d.X * d.Z + d.Y * d.Z);
	}

	// 中心点位置
	FVector Centroid() { return 0.5 * pMin + 0.5 * pMax; }

	// 相交空间
	Bounds3 Intersect(const Bounds3& b)
	{
		return Bounds3(pMin.ComponentMax(b.pMin), pMax.ComponentMin(b.pMax));
	}

	//
	FVector Offset(const FVector& p) const
	{
		FVector o = p - pMin;
		if (pMax.X > pMin.X)
			o.X /= pMax.X - pMin.X;
		if (pMax.Y > pMin.Y)
			o.Y /= pMax.Y - pMin.Y;
		if (pMax.Z > pMin.Z)
			o.Z /= pMax.Z - pMin.Z;
		return o;
	}

	// 两空间是否重叠
	bool Overlaps(const Bounds3& b1, const Bounds3& b2)
	{
		bool x = (b1.pMax.X >= b2.pMin.X) && (b1.pMin.X <= b2.pMax.X);
		bool y = (b1.pMax.Y >= b2.pMin.Y) && (b1.pMin.Y <= b2.pMax.Y);
		bool z = (b1.pMax.Z >= b2.pMin.Z) && (b1.pMin.Z <= b2.pMax.Z);
		return (x && y && z);
	}

	// 点是否在空间内
	bool Inside(const FVector& p, const Bounds3& b)
	{
		return ( p.X >= b.pMin.X && p.X <= b.pMax.X
			&& p.Y >= b.pMin.Y && p.Y <= b.pMax.Y
			&& p.Z >= b.pMin.Z && p.Z <= b.pMax.Z);
	}

	inline const FVector& operator[](int i) const
	{
		return (i == 0) ? pMin : pMax;
	}

	void Union(const Bounds3& b2)
	{
		pMin = pMin.ComponentMin(b2.pMin);
		pMax = pMax.ComponentMax(b2.pMax);
	}

	void Union(const FVector& p)
	{
		pMin = pMin.ComponentMin(p);
		pMax = pMax.ComponentMax(p);
	}

	 bool IntersectP(const Ray& ray, const FVector& invDir, const FVector& dirisNeg) const
	{
		// to-do
		float tEnter = -kInfinity;
		float tExit = kInfinity;
		for (int i = 0; i < 3; i++) {
			float tMin = (pMin[i] - ray.origin[i]) * invDir[i];
			float tMax = (pMax[i] - ray.origin[i]) * invDir[i];
			if (dirisNeg[i] < 0)
				std::swap(tMin, tMax);

			tEnter = std::max(tEnter, tMin);
			tExit = std::min(tExit, tMax);
		}

		return tEnter < tExit && tExit >= 0;
	}

	 void Draw(const UObject* worldContex, FLinearColor color, float time = 0.02f, float thickness = 1.5f) {
		 FVector extend = Diagonal().GetAbs() * 0.5;
		 UKismetSystemLibrary::DrawDebugBox(worldContex, Centroid(), extend, color, FRotator::ZeroRotator, time, thickness);
	}
};


