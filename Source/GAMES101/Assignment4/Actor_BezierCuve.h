// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Actor_BezierCuve.generated.h"

UCLASS()
class GAMES101_API AActor_BezierCuve : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_BezierCuve();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void naive_bezier();

	UFUNCTION(BlueprintCallable)
		void bezier();

	UFUNCTION(BlueprintCallable)
		FVector recursive_bezier(TArray<FVector>& points,float t);

public:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* root;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* point0;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* point1;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* point2;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* point3;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* point4;

	UPROPERTY();
	TArray<FVector> m_points;

	UPROPERTY(EditAnywhere);
	bool m_bUseRecursiveBezier;

};
