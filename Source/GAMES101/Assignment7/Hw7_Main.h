// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hw7_Light.h"
#include "Hw7_Object.h"
#include "BVHAccelTree.h"
#include "Camera/CameraComponent.h"
#include "Components/BillboardComponent.h"
#include "Hw7_Main.generated.h"

struct ScreenCoords {
	int32 x;
	int32 y;
public:
	FORCEINLINE ScreenCoords() : x(0), y(0) { }
	FORCEINLINE ScreenCoords(int32 _x, int32 _y) : x(_x), y(_y) { }
};

struct ScreenPixel {
	int32 index;
	FColor color;
public:
	FORCEINLINE ScreenPixel() :index(0), color(FColor::Black) { }
	FORCEINLINE ScreenPixel(int32 _index, FVector _color) : index(_index) {
		FVector rangeVec = _color * 255;
		rangeVec = rangeVec.ComponentMin(FVector(255, 255, 255));
		color = FColor(rangeVec.X , rangeVec.Y , rangeVec.Z , 255);
	}

};

UCLASS()
class GAMES101_API AHw7_Main : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:	
	AHw7_Main();
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override { bTaskFinished = true; }
	void InitialScene();
	void buildBVHTree();
	void PreRender();
	FVector Render(const Ray &ray, int depth);
	FVector castRay(const Ray &ray, int depth);
	void sampleLight(Intersection& pos, float& pdf) const;
	void CreateTexture();
	void UpdateTexture();

public:
	UPROPERTY(EditAnywhere, Category = "Basic Settings")
		int32 width = 128;
	UPROPERTY(EditAnywhere, Category = "Basic Settings")
		int32 height = 128;
	UPROPERTY(EditAnywhere, Category = "Basic Settings")
		int maxDepth = 1;
	UPROPERTY(EditAnywhere, Category = "Basic Settings")
		int SPP = 8;
	UPROPERTY(EditAnywhere, Category = "Basic Settings")
		bool bAllowDrawDebug = false;
	float fov = 90;
	float RussianRoulette = 0.8; //P_RR, Russian Roulette 的概率
	FVector backgroundColor = FVector(0.235294, 0.67451, 0.843137);

	//  UPROPERTY()不能描述TSharedPtr  Unrecognized type 'TSharedPtr' - type must be a UCLASS, USTRUCT or UENUM
	TArray<TSharedPtr<Light>> lights;

	UPROPERTY()
		TArray<TScriptInterface<IHw7_Object>> objects;
	UPROPERTY()
		UBVHAccelTree* bvhTree;


	UPROPERTY(VisibleAnywhere)
		USceneComponent* rootComp;
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* cameraComp;
	UPROPERTY(EditAnywhere, Category = "Basic Settings")
		TArray<UStaticMesh*> StaticMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UTexture2D* T_Result;

	int32 rowNumber = 0;
	int32 bTaskFinished = false;
	TQueue<ScreenCoords> rayTraceQueue;
	TQueue<ScreenPixel,EQueueMode::Mpsc> pixelQueue;

	
};
