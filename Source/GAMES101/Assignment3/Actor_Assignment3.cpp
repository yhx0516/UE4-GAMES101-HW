// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_Assignment3.h"
#include "RHI.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AActor_Assignment3::AActor_Assignment3()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AActor_Assignment3::BeginPlay()
{
	Super::BeginPlay();
	VideoTextureColor = UTexture2D::CreateTransient(640, 480);
	//VideoTextureColor->LODGroup = TEXTUREGROUP_Pixels2D;
	//VideoTextureColor->UpdateResource();
	FUpdateTextureRegion2D* RegionColor = new FUpdateTextureRegion2D(0, 0, 0, 0, 640, 480);


	// Populate the texture with blue color
	TArray<FColor> PixelDepthData;
	PixelDepthData.Init(FColor(255, 255, 0, 255), 640 * 480);


	VideoTextureColor->UpdateTextureRegions(
		(int32)0,
		(uint32)1,
		RegionColor,
		(uint32)(4 * 640),
		(uint32)4,
		(uint8*)PixelDepthData.GetData()
	);

	

	VideoTextureColor->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	//VideoTextureColor->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	VideoTextureColor->SRGB = false;
	//VideoTextureColor->LODGroup = TEXTUREGROUP_Pixels2D;
	VideoTextureColor->UpdateResource();
	
	FTexture2DMipMap& Mip = VideoTextureColor->PlatformData->Mips[0];
	uint8* Data = (uint8*)Mip.BulkData.Lock(LOCK_READ_ONLY);
	// read here in low level:
	//let's say I want pixel x = 300, y = 23
	//basic formula, data[channels * (width * y + x)];
	FColor pixelColor = FColor(0, 0, 0, 255);
	int x = 200, y = 2;
	pixelColor.B = Data[4 * (640 * y + x) + 0];
	pixelColor.G = Data[4 * (640 * y + x) + 1];
	pixelColor.R = Data[4 * (640 * y + x) + 2];
	pixelColor.A = 255;


	Mip.BulkData.Unlock();
	VideoTextureColor->UpdateResource();

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("font color"), true, true, pixelColor);

	/*
	ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)(
		[=](FRHICommandListImmediate& RHICmdList)
		{
		   FTextureResource * uTex2DRes = VideoTextureColor->Resource;
		   uint32 Stride = 0; // Assigned by RHILockTexture2D.
		   float* cpuDataPtr = (float*)RHILockTexture2D(
			   uTex2DRes->GetTexture2DRHI(),
			   0,
			   RLM_ReadOnly,
			   Stride,
			   false);

		   for (uint32 j = 0; j < 480; j++) {
			   for (uint32 i = 0; i < 640; i++) {
				   uint32 idx = j * 640 + i;
				   // TODO Read the pixel data right here
			   }
		   }
		   RHIUnlockTexture2D(uTex2DRes->GetTexture2DRHI(), 0, false);
		}
	);*/
}

// Called every frame
void AActor_Assignment3::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

