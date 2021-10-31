// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor_Assignmen1.generated.h"

UCLASS()
class GAMES101_API AActor_Assignmen1 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_Assignmen1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void DrawTriangleIn3D();
	

	FMatrix get_view_matrix(FVector eye_pos);
	FMatrix get_model_matrix(float rotation_angle);
	FMatrix get_model_matrix_anyAxis(FVector axis, float angle);
	FMatrix get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar);

	void RasterizerDraw();
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  meta = (MakeEditWidget))
	FTransform ponitA;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,  meta = (MakeEditWidget))
	FTransform ponitB;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,  meta = (MakeEditWidget))
	FTransform ponitC;

	UPROPERTY()
	USceneComponent* root;
	UPROPERTY()
	TArray<FVector> Points;

	int32 width, height;
	FVector eye_loc;
	float angle;

	FMatrix modelMatrix;
	FMatrix viewMatrix;
	FMatrix projectionMatrix;



};
