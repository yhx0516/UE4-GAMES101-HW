// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_Assignment2.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AActor_Assignment2::AActor_Assignment2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CanvasSize = FVector2D(700, 700);
	
}

// Called when the game starts or when spawned
void AActor_Assignment2::BeginPlay()
{
	Super::BeginPlay();

	angle = 0;
	eye_pos = { 0,0,5 };

	pos = {
		{2, 0, -2},
		{0, 2, -2},
		{-2, 0, -2},
		{3.5, -1, -5},
		{2.5, 1.5, -5},
		{-1, 0.5, -5}
	};

	ind = {
			{0, 1, 2},
			{3, 4, 5}
	};

	cols = {
			{217.0, 238.0, 185.0},
			{217.0, 238.0, 185.0},
			{217.0, 238.0, 185.0},
			{185.0, 217.0, 238.0},
			{185.0, 217.0, 238.0},
			{185.0, 217.0, 238.0}
	};

	m_rasterizer = MakeShareable(new rst::Rasterizer2(CanvasSize.X, CanvasSize.Y));
	pos_id = m_rasterizer->load_positions(pos);
	ind_id = m_rasterizer->load_indices(ind);
	col_id = m_rasterizer->load_colors(cols);

	
	if (RasterizerWidgetClass != nullptr) {
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PC->SetShowMouseCursor(true);
		screenUi = CreateWidget<URasterizer2Widget>(PC, RasterizerWidgetClass);
		T_Result = UTexture2D::CreateTransient(CanvasSize.X, CanvasSize.Y, PF_B8G8R8A8);
		screenUi->TCanvas->SetBrushFromTexture(T_Result);
		screenUi->AddToViewport();
	}
	else {
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("RasterizerWidgetClass is null"));
	}
}

// Called every frame
void AActor_Assignment2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_rasterizer->clear(rst::Buffers::Color | rst::Buffers::Depth);

	//angle += 0.2f;
	m_rasterizer->set_model(get_model_matrix_anyAxis(FVector(0, 0.2, 1), angle));
	m_rasterizer->set_view(get_view_matrix(eye_pos));
	m_rasterizer->set_projection(get_projection_matrix(45, 1, 0.1, 50)); //此处 Znear 和 Zfar 取负号

	m_rasterizer->draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
	TextureFromImage_Internal(m_rasterizer->frame_buffer());
	
}

FMatrix AActor_Assignment2::get_view_matrix(FVector eye_loc)
{
	FMatrix view = FMatrix::Identity;

	FMatrix translate = FMatrix(
		FPlane(1, 0, 0, -eye_loc.X),
		FPlane(0, 1, 0, -eye_loc.Y),
		FPlane(0, 0, 1, -eye_loc.Z),
		FPlane(0, 0, 0, 1));

	view = translate * view;
	return view;
}

// 绕 Z 轴旋转
FMatrix AActor_Assignment2::get_model_matrix(float rotation_angle)
{
	FMatrix model = FMatrix::Identity;

	// TODO: Implement this function
	// Create the model matrix for rotating the triangle around the Z axis.
	// Then return it.
	float fcos = UKismetMathLibrary::DegCos(rotation_angle);
	float fsin = UKismetMathLibrary::DegSin(rotation_angle);
	FMatrix rotate = FMatrix(
		FPlane(fcos, -fsin, 0, 0),
		FPlane(fsin, fcos, 0, 0),
		FPlane(0, 0, 1, 0),
		FPlane(0, 0, 0, 1));

	model = rotate * model;

	return model;
}

// 任意轴旋转
FMatrix AActor_Assignment2::get_model_matrix_anyAxis(FVector axis, float rotation_angle)
{
	FMatrix model = FMatrix::Identity;

	axis.Normalize(0.0001);
	FMatrix N = FMatrix(
		FPlane(0, -axis.Z, axis.Y, 0),
		FPlane(axis.Z, 0, -axis.X, 0),
		FPlane(-axis.Y, axis.X, 0, 0),
		FPlane(0, 0, 0, 0));


	FMatrix rotate4f = FMatrix::Identity * UKismetMathLibrary::DegCos(rotation_angle);

	// nnt = axis x axis的转置
	FMatrix nnT = FMatrix(
		FPlane(axis.X * axis.X, axis.X * axis.Y, axis.X * axis.Z, 0),
		FPlane(axis.Y * axis.X, axis.Y * axis.Y, axis.Y * axis.Z, 0),
		FPlane(axis.Z * axis.X, axis.Z * axis.Y, axis.Z * axis.Z, 0),
		FPlane(0, 0, 0, 0));

	rotate4f += nnT * (1 - UKismetMathLibrary::DegCos(rotation_angle));

	rotate4f += N * UKismetMathLibrary::DegSin(rotation_angle);

	rotate4f.M[3][3] = 1;
	model = rotate4f * model;
	return model;
}

FMatrix AActor_Assignment2::get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
	// Students will implement this function
	FMatrix projection = FMatrix::Identity;

	float t = zNear * UKismetMathLibrary::DegTan(eye_fov / 2);
	float b = -t;
	float r = t * aspect_ratio;
	float l = -r;

	FMatrix translate = FMatrix(
		FPlane(2 * zNear / (r - l), 0, -(r + l) / (r - l), 0),
		FPlane(0, 2 * zNear / (t - b), -(t + b) / (t - b), 0),
		FPlane(0, 0, -(zNear + zFar) / (zNear - zFar), 2 * zNear * zFar / (zNear - zFar)),
		FPlane(0, 0, -1, 0));
	projection = translate * projection;

	return projection;
}

// 修改Texture里的内容
void AActor_Assignment2::TextureFromImage_Internal( const TArray<FColor>& SrcData, const bool UseAlpha)
{
	const int32 SrcWidth = CanvasSize.X;
	const int32 SrcHeight = CanvasSize.Y;
	// Create the texture
	
	//T_Result->ReleaseResource();
	// Lock the texture so it can be modified
	uint8* MipData = static_cast<uint8*>(T_Result->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	// Create base mip.
	uint8* DestPtr = NULL;
	const FColor* SrcPtr = NULL;
	for (int32 y = 0; y < SrcHeight; y++)
	{
		DestPtr = &MipData[(SrcHeight - 1 - y) * SrcWidth * sizeof(FColor)];
		SrcPtr = const_cast<FColor*>(&SrcData[(SrcHeight - 1 - y) * SrcWidth]);
		for (int32 x = 0; x < SrcWidth; x++)
		{
			*DestPtr++ = SrcPtr->B;
			*DestPtr++ = SrcPtr->G;
			*DestPtr++ = SrcPtr->R;
			if (UseAlpha)
			{
				*DestPtr++ = SrcPtr->A;
			}
			else
			{
				*DestPtr++ = 0xFF;
			}
			SrcPtr++;
		}
	}

	// Unlock the texture
	T_Result->PlatformData->Mips[0].BulkData.Unlock();
	T_Result->UpdateResource();
	
}