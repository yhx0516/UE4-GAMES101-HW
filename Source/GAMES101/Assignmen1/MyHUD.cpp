// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

void AMyHUD::DrawHUD()
{
	Super::DrawHUD();
	if (TriangleVerts.IsValidIndex(0)) {
		DrawLine(TriangleVerts[0].X, TriangleVerts[0].Y, TriangleVerts[1].X, TriangleVerts[1].Y,FLinearColor::Red);
		DrawLine(TriangleVerts[1].X, TriangleVerts[1].Y, TriangleVerts[2].X, TriangleVerts[2].Y,FLinearColor::Red);
		DrawLine(TriangleVerts[2].X, TriangleVerts[2].Y, TriangleVerts[0].X, TriangleVerts[0].Y,FLinearColor::Red);		
		
	}	
}

void AMyHUD::rasterize_wireframe(TArray<FVector>& t)
{
	TriangleVerts = t;
}