// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class GAMES101_API AMyHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	// 重载用于绘制
	void rasterize_wireframe(TArray<FVector>& t);
	// 存储三角形的三个点
	TArray<FVector> TriangleVerts;
};
