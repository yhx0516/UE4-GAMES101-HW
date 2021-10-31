// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_Assignment5.h"
#include "Hw5_Sphere.h"
#include "Hw5_MeshTriangle.h"
#include "Kismet/GameplayStatics.h"

static float kInfinity = TNumericLimits<float>::Max();

// Sets default values
AActor_Assignment5::AActor_Assignment5()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("rootComp"));
	SetRootComponent(rootComp);

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	cameraComp->SetupAttachment(rootComp);
	cameraComp->SetWorldLocation(FVector(-600, 0, 0));

	cameraComp->bCameraMeshHiddenInGame=false;

	light1 = CreateDefaultSubobject<UBillboardComponent>(TEXT("l1"));
	light2 = CreateDefaultSubobject<UBillboardComponent>(TEXT("l2"));
	light1->SetupAttachment(rootComp);
	light2->SetupAttachment(rootComp);
	light1->SetHiddenInGame(false);
	light2->SetHiddenInGame(false);
	static  ConstructorHelpers::FObjectFinder<UTexture2D> texAsset(TEXT("Texture2D'/Engine/EditorResources/LightIcons/S_LightDirectional.S_LightDirectional'"));
	if (texAsset.Succeeded()) {
		light1->SetSprite(texAsset.Object);
		light2->SetSprite(texAsset.Object);
	}
}

// Called when the game starts or when spawned
void AActor_Assignment5::BeginPlay()
{
	Super::BeginPlay();
	
	InitialScene();
	framebuffer.Init(FColor(0,0, 0, 255), width * height);
	CreateTexture();

	
}

// Called every frame
void AActor_Assignment5::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!bFinishedScan)
		Render();

}

void AActor_Assignment5::InitialScene()
{
	FTransform Spawntransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
	AHw5_Sphere* Sphere1 = GetWorld()->SpawnActorDeferred<AHw5_Sphere>(AHw5_Sphere::StaticClass(), Spawntransform);
	if (Sphere1) {
		Sphere1->materialType = EMaterialType::DIFFUSE_AND_GLOSSY;
		Sphere1->diffuseColor = FVector(0.6, 0.7, 0.8);
		Sphere1->SetCenterAndRadius(FVector(4, -1, 0) * 50, 2 * 50);
		UGameplayStatics::FinishSpawningActor(Sphere1, Spawntransform);
		objects.Add(Sphere1);
	}

	AHw5_Sphere* Sphere2 = GetWorld()->SpawnActorDeferred<AHw5_Sphere>(AHw5_Sphere::StaticClass(), Spawntransform);
	if (Sphere2) {
		Sphere2->materialType = EMaterialType::REFLECTION_AND_REFRACTION;//REFLECTION_AND_REFRACTION;
		Sphere2->ior = 1.5;
		Sphere2->SetCenterAndRadius(FVector(0, 0.5, 0) * 50, 1.5 * 50);
		UGameplayStatics::FinishSpawningActor(Sphere2, Spawntransform);
		objects.Add(Sphere2);
	}

	Spawntransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
	AHw5_MeshTriangle* mesh = GetWorld()->SpawnActorDeferred<AHw5_MeshTriangle>(AHw5_MeshTriangle::StaticClass(), Spawntransform);
	if (mesh) {
		mesh->materialType = EMaterialType::DIFFUSE_AND_GLOSSY;
		TArray<FVector> verts = { {-3, 6,-3}, {9, 6,-3}, {9, -6, -3}, {-3, -6, -3} };
		for (auto& v : verts)
			v *= 50;
		TArray<uint32> vertIndex = { 0, 1, 3, 1, 2, 3 };
		TArray<FVector2D> st = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };
		mesh->SetProperty(verts, vertIndex, 2, st);
		UGameplayStatics::FinishSpawningActor(mesh, Spawntransform);
		objects.Add(mesh);
	}

	lights_Intensity = 0.5  ;
	lights_pos.Add(FVector(-28, -20, 70) * 10);
	lights_pos.Add(FVector(4, 30, 50) * 10);
	light1->SetWorldLocation(lights_pos[0]);
	light2->SetWorldLocation(lights_pos[1]);
}

void AActor_Assignment5::Render()
{
	FTexture2DMipMap& Mip = T_Result->PlatformData->Mips[0];
	FColor* Data = (FColor*)Mip.BulkData.Lock(LOCK_READ_ONLY);
	
	
	float scale = UKismetMathLibrary::DegTan(fov * 0.5f);
	float imageAspectRatio = width / (float)height;

	// Use this variable as the eye position to start your rays.
	FVector eye_pos=cameraComp->GetComponentLocation();
	int m = rowNumber * width;
	for (int i = 0; i < width; ++i)
	{
		// generate primary ray direction
		float pixelCenterX = (float)(i + 0.5f) / width;
		float pixelCenterY = (float)(rowNumber + 0.5f) / height;

		float x = (2 * pixelCenterX - 1) * imageAspectRatio * scale;
		float y = (1- 2 * pixelCenterY) * scale; //Öáµßµ¹       

		FVector dir = FVector(1, x, y); // Don't forget to normalize this direction!
		dir.Normalize();
		FVector finalColor = castRay(eye_pos, dir, 0)*255;
		
		framebuffer[m] = FColor(finalColor.X, finalColor.Y, finalColor.Z, 255);
		//Data[(height - 1 - rowNumber) * width + i] = framebuffer[m];
		Data[m] = framebuffer[m]; //texture Data
		m++;
	}

	Mip.BulkData.Unlock();
	T_Result->UpdateResource();
	rowNumber++;
	if (m==width*height)
	{
		bFinishedScan = true;
		TextureFromImgArr(framebuffer);
	}	
}

FVector AActor_Assignment5::castRay(const FVector& orig, const FVector& dir, int depth)
{
	if (depth > maxDepth) {
		return FVector(0.0, 0.0, 0.0);
	}

	FVector hitColor = backgroundColor;
	auto payload = trace(orig, dir);
	if (payload.IsSet())
	{
		FVector hitPoint = orig + dir * payload->tNear;
		//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), hitPoint, 1, FColor::Red, 20);
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), orig, hitPoint, FColor(255,0,0,48), .02f, 1.0f);

		FVector N; // normal
		FVector2D st; // st coordinates
		payload->hit_obj->getSurfaceProperties(hitPoint, dir, payload->index, payload->uv, N, st);
		switch (payload->hit_obj->materialType) {
		case EMaterialType::REFLECTION_AND_REFRACTION:
		{
			FVector reflectionDirection = reflect(dir, N);
			FVector refractionDirection = refract(dir, N, payload->hit_obj->ior);
			FVector reflectionRayOrig = (FVector::DotProduct(reflectionDirection, N) < 0) ?
				hitPoint - N * epsilon :
				hitPoint + N * epsilon;
			FVector refractionRayOrig = (FVector::DotProduct(refractionDirection, N) < 0) ?
				hitPoint - N * epsilon :
				hitPoint + N * epsilon;
			
			
			FVector	reflectionColor = castRay(reflectionRayOrig, reflectionDirection, depth + 1);
			FVector refractionColor=FVector::ZeroVector;
			if (reflectionDirection.Size() != 0)
				refractionColor = castRay(refractionRayOrig, refractionDirection, depth + 1);
			float kr = fresnel(dir, N, payload->hit_obj->ior);
			hitColor = reflectionColor * kr + refractionColor * (1 - kr);
			break;

		}
		case EMaterialType::REFLECTION:
		{
			float kr = fresnel(dir, N, payload->hit_obj->ior);
			FVector reflectionDirection = reflect(dir, N);
			FVector reflectionRayOrig = (FVector::DotProduct(reflectionDirection, N) < 0) ?
				hitPoint - N * epsilon :
				hitPoint + N * epsilon;
			hitColor = castRay(reflectionRayOrig, reflectionDirection, depth + 1) * kr;
			break;
		}
		default:
		{
			// [comment]
			// We use the Phong illumation model int the default case. The phong model
			// is composed of a diffuse and a specular reflection component.
			// [/comment]
			FVector lightAmt = FVector::ZeroVector, specularColor = FVector::ZeroVector;
			FVector shadowPointOrig = (FVector::DotProduct(dir, N) < 0) ?
				hitPoint + N * epsilon :
				hitPoint - N * epsilon;
			// [comment]
			// Loop over all lights in the scene and sum their contribution up
			// We also apply the lambert cosine law
			// [/comment]
			for (auto& light_position : lights_pos) {
				FVector lightDir = light_position - hitPoint;
				// square of the distance between hitPoint and the light
				float lightDistance2 = FVector::DotProduct(lightDir, lightDir);
				lightDir.Normalize();
				float LdotN = std::max(0.f, FVector::DotProduct(lightDir, N));
				// is the point in shadow, and is the nearest occluding object closer to the object than the light itself?
				auto shadow_res = trace(shadowPointOrig, lightDir);				

				bool inShadow = shadow_res.IsSet() && (shadow_res->tNear * shadow_res->tNear < lightDistance2);
				if(!inShadow)
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), hitPoint, light_position, FColor(255,0,0,48), .02f, 1.0f);

				lightAmt += inShadow ? FVector::ZeroVector : lights_Intensity * LdotN* FVector::OneVector;
				FVector reflectionDirection = reflect(-lightDir, N);

				specularColor += powf(std::max(0.f, -FVector::DotProduct(reflectionDirection, dir)),
					payload->hit_obj->specularExponent) * lights_Intensity* FVector::OneVector;
			}

			hitColor = lightAmt * payload->hit_obj->evalDiffuseColor(st) * payload->hit_obj->Kd +specularColor * payload->hit_obj->Ks;
			//hitColor = payload->hit_obj->evalDiffuseColor(st) * payload->hit_obj->Kd;
			break;
		}
		}
	}

	return hitColor;
}

TOptional<FHit_payload> AActor_Assignment5::trace(const FVector& orig, const FVector& dir)
{
	float tNear = kInfinity;
	TOptional<FHit_payload> payload;
	for (const auto& object : objects)
	{
		float tNearK = kInfinity;
		uint32_t indexK;
		FVector2D uvK;
		if (object->intersect(orig, dir, tNearK, indexK, uvK) && tNearK <= tNear)
		{
			payload.Emplace();
			payload->hit_obj = object;
			payload->tNear = tNearK;
			payload->index = indexK;
			payload->uv = uvK;
			tNear = tNearK;
		}
	}

	return payload;
}

FVector AActor_Assignment5::reflect(const FVector& I, const FVector& N)
{
	FVector res = I - 2 * FVector::DotProduct(I, N) * N;
	res.Normalize();
	return res;
}

FVector AActor_Assignment5::refract(const FVector& I, const FVector& N, const float& ior)
{
	float cosi = UKismetMathLibrary::FClamp(FVector::DotProduct(I, N), -1, 1);
	float etai = 1, etat = ior;
	FVector n = N;
	if (cosi < 0) { cosi = -cosi; }
	else { std::swap(etai, etat); n = -N; }
	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	FVector res = k < 0 ? FVector::ZeroVector : eta * I + (eta * cosi - sqrtf(k)) * n;
	res.Normalize();
	return res;
		
}

float AActor_Assignment5::fresnel(const FVector& I, const FVector& N, const float& ior)
{

	float cosi = UKismetMathLibrary::FClamp(FVector::DotProduct(I, N), -1, 1);
	float etai = 1, etat = ior;
	if (cosi > 0) { std::swap(etai, etat); }
	// Compute sini using Snell's law
	float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1) {
		return 1;
	}
	else {
		float cost = sqrtf(std::max(0.f, 1 - sint * sint));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		return (Rs * Rs + Rp * Rp) / 2.0;
	}
}

void AActor_Assignment5::CreateTexture()
{
	T_Result = UTexture2D::CreateTransient(width, height, PF_B8G8R8A8);
	T_Result->UpdateResource();
	FUpdateTextureRegion2D* RegionColor = new FUpdateTextureRegion2D(0, 0, 0, 0, width, height);

	T_Result->UpdateTextureRegions(
		(int32)0,
		(uint32)1,
		RegionColor,
		(uint32)(4 * width),
		(uint32)4,
		(uint8*)framebuffer.GetData()
	);
}



void AActor_Assignment5::TextureFromImgArr(const TArray<FColor>& SrcData)
{
	const int32 SrcWidth = width;
	const int32 SrcHeight = height;
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
			*DestPtr++ = SrcData[SrcWidth * y + x].B;
			*DestPtr++ = SrcData[SrcWidth * y + x].G;
			*DestPtr++ = SrcData[SrcWidth * y + x].R;
			*DestPtr++ = 0xFF;
			SrcPtr++;
		}
	}

	// Unlock the texture
	T_Result->PlatformData->Mips[0].BulkData.Unlock();
	
	if (T_Result == nullptr)
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("T_Result is null"));
}
