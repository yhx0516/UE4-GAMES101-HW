// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rasterizer2.h"
#include "Rasterizer2Widget.h"
#include "Actor_Assignment2.generated.h"

UCLASS()
class GAMES101_API AActor_Assignment2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_Assignment2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FMatrix get_view_matrix(FVector eye_pos);
	FMatrix get_model_matrix(float rotation_angle);
	FMatrix get_model_matrix_anyAxis(FVector axis, float angle);
	FMatrix get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar);

	void TextureFromImage_Internal(const TArray<FColor>& SrcData, const bool UseAlpha = false);

	void SwitchMASS() { m_rasterizer->SwitchMASS();UKismetSystemLibrary::PrintString(GetWorld(), TEXT("SwitchMASS")); }

public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FVector> pos;

	UPROPERTY(BlueprintReadWrite)
		TArray<FIntVector> ind;

	UPROPERTY(BlueprintReadWrite)
		TArray<FVector> cols;

	UPROPERTY(EditAnywhere)
		FVector2D CanvasSize;

	UPROPERTY(EditAnywhere)
		TSubclassOf<URasterizer2Widget> RasterizerWidgetClass;

	UPROPERTY()
		URasterizer2Widget* screenUi;

	UPROPERTY(VisibleAnywhere)
		UTexture2D* T_Result;

private:
	rst::pos_buf_id pos_id;
	rst::ind_buf_id ind_id;
	rst::col_buf_id col_id;
	float angle;
	FVector eye_pos;
private:
	TSharedPtr<rst::Rasterizer2> m_rasterizer;
};
