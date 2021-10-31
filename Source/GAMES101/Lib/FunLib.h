// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunLib.generated.h"

/**
 * 
 */
UCLASS()
class GAMES101_API UFunLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		static TArray<FVector> GetStaticMeshVertexPositionByLODIndex(UStaticMeshComponent* SM, int32 LODIndex);
	UFUNCTION(BlueprintCallable)
		static void ModfiyStaticMeshVertexColorByPointEffect(UStaticMeshComponent* SM, FVector HitPoint, float LimitDistance, FColor Color);
	
	UFUNCTION(BlueprintCallable)
		static void SetStaticVertexColorOverride(UStaticMeshComponent* SMComp, int32 LODIndex, const TArray<FColor>& VertexColor);
	UFUNCTION(BlueprintCallable)
		static void SetVertexColorOverride(UStaticMeshComponent* SMComp, FVector HitPoint, float LimitDistance, FColor Color);
};
