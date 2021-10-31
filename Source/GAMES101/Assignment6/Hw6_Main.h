// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hw6_Global.h"
#include "Hw6_MeshTriangle.h"
#include "Camera/CameraComponent.h"
#include "Components/BillboardComponent.h"
#include "Hw6_Main.generated.h"

UCLASS()
class GAMES101_API AHw6_Main : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHw6_Main();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitialScene();
	
	void buildBVHTree();
	Intersection intersect(const Ray& ray) const;

	void Render();
	FVector castRay(const Ray &ray, int depth);
	bool trace(const Ray& ray, float& tNear, uint32_t& index, AHw6_MeshTriangle** hitObject);

	FVector reflect(const FVector& I, const FVector& N); //∑¥…‰
	FVector refract(const FVector& I, const FVector& N, const float& ior); //’€…‰
	float fresnel(const FVector& I, const FVector& N, const float& ior); //∑∆ƒ˘∂˚

	void CreateTexture();

public:	
	UPROPERTY(EditAnywhere)
		int32 width = 128;
	UPROPERTY(EditAnywhere)
		int32 height = 96;

	float fov = 90;
	FVector backgroundColor = FVector(0.235294, 0.67451, 0.843137);

	UPROPERTY(EditAnywhere)
		int maxDepth = 5;
	

	//  UPROPERTY()≤ªƒ‹√Ë ˆTSharedPtr  Unrecognized type 'TSharedPtr' - type must be a UCLASS, USTRUCT or UENUM
		TArray<TSharedPtr<Light>> lights;

	UPROPERTY()
		TArray<TScriptInterface<IObjectInterface>> objects;
	UPROPERTY()
		UBVHAccel* bvhTree;


	UPROPERTY(VisibleAnywhere)
		USceneComponent* rootComp;
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* cameraComp;
	UPROPERTY(VisibleAnywhere)
		UBillboardComponent* light1;
	UPROPERTY(VisibleAnywhere)
		UBillboardComponent* light2;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UTexture2D* T_Result;

	bool bStartScan = false;
	int32 rowNumber = 0;
	
	UPROPERTY(EditAnywhere)
		int32 updateTextureIntval = 2;
	int32 updateCounter = 0;

	UPROPERTY()
		TArray<FColor> framebuffer;
};
