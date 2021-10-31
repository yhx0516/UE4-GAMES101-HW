// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hw5_Shape.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Hw5_MeshTriangle.generated.h"




UCLASS()
class GAMES101_API AHw5_MeshTriangle : public AHw5_Shape
{
	GENERATED_BODY()

public:
	void DarwWireframe() {
		for (uint32_t k = 0; k < numTriangles; ++k)
		{
			const FVector& v0 = vertices[vertexIndex[k * 3]];
			const FVector& v1 = vertices[vertexIndex[k * 3 + 1]];
			const FVector& v2 = vertices[vertexIndex[k * 3 + 2]];
			UKismetSystemLibrary::DrawDebugLine(GetWorld(), v0, v1, FLinearColor::White, 3600, 2.0f);
			UKismetSystemLibrary::DrawDebugLine(GetWorld(), v1, v2, FLinearColor::White, 3600, 2.0f);
			UKismetSystemLibrary::DrawDebugLine(GetWorld(), v2, v0, FLinearColor::White, 3600, 2.0f);
		}
	}
	void SetProperty(const TArray<FVector>& verts, const TArray<uint32_t>& vertsIndex, const uint32_t& numTris, const TArray<FVector2D> st)
	{
		vertices = verts;
		vertexIndex = vertsIndex;
		numTriangles = numTris;
		stCoordinates = st;
		DarwWireframe();
		
	}


	bool rayTriangleIntersect(const FVector& v0, const FVector& v1, const FVector& v2, const FVector& orig,
		const FVector& dir, float& tnear, float& u, float& v) const
	{
		// TODO: Implement this function that tests whether the triangle
		// that's specified bt v0, v1 and v2 intersects with the ray (whose
		// origin is *orig* and direction is *dir*)
		// Also don't forget to update tnear, u and v.
		FVector E1 = v1 - v0;
		FVector E2 = v2 - v0;
		FVector S = orig - v0;
		FVector S1 = FVector::CrossProduct(dir, E2);
		FVector S2 = FVector::CrossProduct(S, E1);

		float factor = FVector::DotProduct(S1, E1);
		float t = 1.0f / factor * FVector::DotProduct(S2, E2);
		float b1 = 1.0f / factor * FVector::DotProduct(S1, S);
		float b2 = 1.0f / factor * FVector::DotProduct(S2, dir);

		if (t > 0 && b1 > 0 && b2 > 0 && (1 - b1 - b2) > 0) {
			tnear = t;
			u = b1;
			v = b2;
			return true;
		}
		return false;
	}

	bool intersect(const FVector& orig, const FVector& dir, float& tnear, uint32_t& index,
		FVector2D& uv) const override
	{
		bool intersect = false;
		for (uint32_t k = 0; k < numTriangles; ++k)
		{
			const FVector& v0 = vertices[vertexIndex[k * 3]];
			const FVector& v1 = vertices[vertexIndex[k * 3 + 1]];
			const FVector& v2 = vertices[vertexIndex[k * 3 + 2]];
			float t, u, v;
			if (rayTriangleIntersect(v0, v1, v2, orig, dir, t, u, v) && t < tnear)
			{
				tnear = t;
				uv.X = u;
				uv.Y = v;
				index = k;
				intersect |= true;
			}
		}

		return intersect;
	}

	void getSurfaceProperties(const FVector&, const FVector&, const uint32_t& index, const FVector2D& uv, FVector& N,
		FVector2D& st) const override
	{
		const FVector& v0 = vertices[vertexIndex[index * 3]];
		const FVector& v1 = vertices[vertexIndex[index * 3 + 1]];
		const FVector& v2 = vertices[vertexIndex[index * 3 + 2]];
		FVector e0 = v1 - v0;
		FVector e1 = v1 - v2;
		e0.Normalize();
		e1.Normalize();
		N = FVector::CrossProduct(e0, e1);
		N.Normalize();
		const FVector2D& st0 = stCoordinates[vertexIndex[index * 3]];
		const FVector2D& st1 = stCoordinates[vertexIndex[index * 3 + 1]];
		const FVector2D& st2 = stCoordinates[vertexIndex[index * 3 + 2]];
		st = st0 * (1 - uv.X - uv.Y) + st1 * uv.X + st2 * uv.Y;
	}

	FVector evalDiffuseColor(const FVector2D& st) const override
	{
		float scale = 5;
		float pattern = (fmodf(st.X * scale, 1) > 0.5) ^ (fmodf(st.Y * scale, 1) > 0.5);
		return UKismetMathLibrary::VLerp(FVector(0.815, 0.235, 0.031), FVector(0.937, 0.937, 0.231), pattern);
	}

	UPROPERTY()
		TArray<FVector> vertices;

		uint32_t numTriangles;
	UPROPERTY()
		TArray<uint32> vertexIndex;
	UPROPERTY()
		TArray<FVector2D> stCoordinates;
};
