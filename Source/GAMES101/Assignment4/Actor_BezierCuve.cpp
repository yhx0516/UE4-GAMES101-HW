// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_BezierCuve.h"
#include "DrawDebugHelpers.h"
#include <cmath>
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AActor_BezierCuve::AActor_BezierCuve()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	SetRootComponent(root);

	point0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("point0"));
	point1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("point1"));
	point2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("point2"));
	point3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("point3"));
	point4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("point4"));

	point0->SetupAttachment(root);
	point1->SetupAttachment(root);
	point2->SetupAttachment(root);
	point3->SetupAttachment(root);
	point4->SetupAttachment(root);
	m_points.Init(FVector::ZeroVector, 5);

	m_bUseRecursiveBezier = false;
}

// Called when the game starts or when spawned
void AActor_BezierCuve::BeginPlay()
{
	Super::BeginPlay();
	m_points[0] = point0->GetComponentLocation();
	m_points[1] = point1->GetComponentLocation();
	m_points[2] = point2->GetComponentLocation();
	m_points[3] = point3->GetComponentLocation();
	m_points[4] = point4->GetComponentLocation();


	if (!m_bUseRecursiveBezier)
		naive_bezier();
	else
		bezier();
}

// Called every frame
void AActor_BezierCuve::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

// 多项式 
void AActor_BezierCuve::naive_bezier()
{
	FVector& p_0 = m_points[0];
	FVector& p_1 = m_points[1];
	FVector& p_2 = m_points[2];
	FVector& p_3 = m_points[3];
	FVector& p_4 = m_points[4];
	for (double t = 0.0; t <= 1.0; t += 0.001)
	{
		auto point = std::pow(1 - t, 4) * p_0 + 4 * t * std::pow(1 - t, 3) * p_1 +
			6 * std::pow(t, 2) * std::pow((1 - t), 2) * p_2 + 4 * std::pow(t, 3) * (1 - t) * p_3 + std::pow(t, 4) * p_4;
		DrawDebugPoint(GetWorld(), point, 2.0f, FColor::Green,true,5.0f);
		//UKismetSystemLibrary::PrintString(GetWorld(), point.ToString());
	}
	
}

void AActor_BezierCuve::bezier()
{
	for (double t = 0.0; t <= 1.0; t += 0.001)
	{
		FVector point = recursive_bezier(m_points, t);
		DrawDebugPoint(GetWorld(), point, 2.0f, FColor(10,214,255,255),true,5.0f);
	}
}

// De Casteljau 算法，递归
FVector AActor_BezierCuve::recursive_bezier(TArray<FVector>& points, float t)
{
	if (points.Num() < 3) {
		return (1 - t) * points[0] + t * points[1];
	}

	TArray<FVector> newPoint;
	for (int i = 0; i < points.Num() - 1; i++) {
		newPoint.Add((1 - t) * points[i] + t * points[i + 1]);
	}
	return recursive_bezier(newPoint, t);
}

