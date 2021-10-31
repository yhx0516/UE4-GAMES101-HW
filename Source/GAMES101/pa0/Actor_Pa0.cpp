// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_Pa0.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
AActor_Pa0::AActor_Pa0()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActor_Pa0::BeginPlay()
{
	Super::BeginPlay();

	// 给定一个点P=(2,1), 将该点绕原点先逆时针旋转45◦，再平移(1,2), 计算出变换后点的坐标（要求用齐次坐标进行计算）。
	float fcos = UKismetMathLibrary::DegCos(45);
	float fsin = UKismetMathLibrary::DegSin(45);
	FPlane row1 = FPlane(fcos, -fsin, 1, 0);
	FPlane row2 = FPlane(fsin, fcos, 2, 0);
	FPlane row3 = FPlane(0, -0, 1, 0);
	FPlane row4 = FPlane(0, -0, 0, 0);
	FMatrix matrix = FMatrix(row1, row2, row3, row4);
	FVector4 originPos = FVector4(2, 1, 1, 0);

	matrix = matrix.GetTransposed(); //行向量乘以矩阵，所以矩阵取转置
	FVector4 res = matrix.TransformFVector4(originPos);
	
	UE_LOG(LogTemp, Warning, TEXT("%s"), *matrix.ToString());
	UE_LOG(LogTemp, Warning, TEXT("[ %.2f, %.2f, %.2f ]"), res.X, res.Y, res.Z);
}

// Called every frame
void AActor_Pa0::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

