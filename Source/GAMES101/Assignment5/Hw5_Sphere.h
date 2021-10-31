// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <algorithm>
#include "Hw5_Shape.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Hw5_Sphere.generated.h"


inline bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0)
		return false;
	else if (discr == 0)
		x0 = x1 = -0.5 * b / a;
	else
	{
		float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	if (x0 > x1)
		std::swap(x0, x1);
	return true;
}


UCLASS()
class GAMES101_API AHw5_Sphere : public AHw5_Shape
{
	GENERATED_BODY()

public:
	void SetCenterAndRadius(const FVector& c, const float& r) { 
		center = c;
		radius = r; 
		radius2 = r * r; 
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), center, radius, 40.0f, FLinearColor::White, 3600, 1.0f);
	}

	bool intersect(const FVector& orig, const FVector& dir, float& tnear, uint32_t&, FVector2D&) const override
    {
        // analytic solution
        FVector L = orig - center;
        float a = FVector::DotProduct(dir, dir);
        float b = 2 * FVector::DotProduct(dir, L);
        float c = FVector::DotProduct(L, L) - radius2;
        float t0, t1;
        if (!solveQuadratic(a, b, c, t0, t1))
            return false;
        if (t0 < 0)
            t0 = t1;
        if (t0 < 0)
            return false;
        tnear = t0;

        return true;
    }

    void getSurfaceProperties(const FVector& P, const FVector&, const uint32_t&, const FVector2D&,
                              FVector& N, FVector2D&) const override
    {
        N = P - center;
		N.Normalize();
    }

	
private:
	FVector center;
	float radius, radius2;
};
