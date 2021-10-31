// Fill out your copyright notice in the Description page of Project Settings.


#include "Hw6_Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AHw6_Main::AHw6_Main()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("rootComp"));
	SetRootComponent(rootComp);

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	cameraComp->SetupAttachment(rootComp);
	cameraComp->SetWorldLocation(FVector(-300, 0, 0));
	cameraComp->bCameraMeshHiddenInGame = false;

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
void AHw6_Main::BeginPlay()
{
	Super::BeginPlay();
	InitialScene();
	framebuffer.Init(FColor(0, 0, 0, 255), width * height);
	CreateTexture();

	FLatentActionInfo info;
	UKismetSystemLibrary::Delay(GetWorld(), 1.0f, info);
	
	buildBVHTree();
	bStartScan = true;
}

// Called every frame
void AHw6_Main::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bStartScan)
		Render();
}

void AHw6_Main::InitialScene()
{
	FTransform Spawntransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
	AHw6_MeshTriangle* Mesh = GetWorld()->SpawnActor<AHw6_MeshTriangle>(AHw6_MeshTriangle::StaticClass(), Spawntransform);
	if (Mesh && Mesh->Implements<UObjectInterface>()) 
		objects.Add(Mesh);

	
	lights.Add(MakeShared<Light>((FVector(-20, -20, 70) * 6),FVector::OneVector));
	light1->SetWorldLocation(FVector(-20, -20, 70) * 6);

	lights.Add(MakeShared<Light>((FVector(-20, 20, 70) * 6),FVector::OneVector));
	light2->SetWorldLocation(FVector(-20, 20, 70) * 6);
}

void AHw6_Main::buildBVHTree()
{
	printf(" - Generating BVH...\n\n");
	bvhTree = NewObject<UBVHAccel>(this);
	bvhTree->Init(objects, 1, UBVHAccel::SplitMethod::NAIVE, true);
}

Intersection AHw6_Main::intersect(const Ray& ray) const
{
	return bvhTree->Intersect(ray);
}

void AHw6_Main::Render()
{
	FTexture2DMipMap& Mip = T_Result->PlatformData->Mips[0];
	FColor* Data = (FColor*)Mip.BulkData.Lock(LOCK_READ_ONLY);


	float scale = UKismetMathLibrary::DegTan(fov * 0.5f);
	float imageAspectRatio = width / (float)height;

	// Use this variable as the eye position to start your rays.
	FVector eye_pos = cameraComp->GetComponentLocation();
	int m = rowNumber * width;
	for (int i = 0; i < width; ++i)
	{
		float x = (2 * (i + 0.5) / (float)width - 1) * imageAspectRatio * scale;
		float y = (1 - 2 * (rowNumber + 0.5) / (float)height) * scale;

		FVector dir = FVector(1, x, y); // Don't forget to normalize this direction!
		dir.Normalize();
		FVector finalColor = castRay(Ray(eye_pos, dir), 0) * 255;

		framebuffer[m] = FColor(finalColor.X, finalColor.Y, finalColor.Z, 255);
		Data[m] = framebuffer[m]; //texture Data
		m++;
	}
	updateCounter++;
	Mip.BulkData.Unlock();
	if (updateCounter >= updateTextureIntval) {
		updateCounter = 0;
		T_Result->UpdateResource();
	}
	
	rowNumber++;
	if (m == width * height)
	{
		bStartScan = false;
		//TextureFromImgArr(framebuffer);
	}
}

FVector AHw6_Main::castRay(const Ray &ray, int depth)
{
	if (depth > maxDepth) {
		return FVector::ZeroVector;
	}

	Intersection intersection = intersect(ray);
	Material* m = intersection.m;
	IObjectInterface* hitObject = intersection.obj;
	FVector hitColor = this->backgroundColor;
	//    float tnear = kInfinity;
	FVector2D uv;
	uint32_t index = 0;
	if (intersection.happened) {

		FVector hitPoint = intersection.coords;
		FVector N = intersection.normal; // normal
		FVector2D st; // st coordinates
		hitObject->getSurfaceProperties(hitPoint, ray.direction, index, uv, N, st);
		//        Vector3f tmp = hitPoint;
		switch (m->getType()) {
			case REFLECTION_AND_REFRACTION:
			{
				FVector reflectionDirection = reflect(ray.direction, N);
				FVector refractionDirection = refract(ray.direction, N, m->ior);
				FVector reflectionRayOrig = (FVector::DotProduct(reflectionDirection, N) < 0) ?
					hitPoint - N * EPSILON :
					hitPoint + N * EPSILON;
				FVector refractionRayOrig = (FVector::DotProduct(refractionDirection, N) < 0) ?
					hitPoint - N * EPSILON :
					hitPoint + N * EPSILON;

				FVector	reflectionColor = castRay(Ray(reflectionRayOrig, reflectionDirection), depth + 1);
				FVector refractionColor = castRay(Ray(refractionRayOrig, refractionDirection), depth + 1);
				float kr = fresnel(ray.direction, N, m->ior);
				hitColor = reflectionColor * kr + refractionColor * (1 - kr);
				break;
			}
			case REFLECTION:
			{
				float kr = fresnel(ray.direction, N, m->ior);
				FVector reflectionDirection = reflect(ray.direction, N);
				FVector reflectionRayOrig = (FVector::DotProduct(reflectionDirection, N) < 0) ?
					hitPoint - N * EPSILON :
					hitPoint + N * EPSILON;
				hitColor = castRay(Ray(reflectionRayOrig, reflectionDirection), depth + 1) * kr;
				break;
			}
			default:
			{
				FVector lightAmt = FVector::ZeroVector, specularColor = FVector::ZeroVector;
				FVector shadowPointOrig = (FVector::DotProduct(ray.direction, N) < 0) ?
					hitPoint + N * EPSILON :
					hitPoint - N * EPSILON;
				// [comment]
				// Loop over all lights in the scene and sum their contribution up
				// We also apply the lambert cosine law
				// [/comment]
				for (auto& light : lights) {

					TSharedPtr<AreaLight> area_ptr = StaticCastSharedPtr<AreaLight>(light);
	
					if (std::is_same<std::remove_reference<decltype(*light)>::type, AreaLight>::value)
					{
						UKismetSystemLibrary::PrintString(GetWorld(), "is AreaLight");
					}
					else
					{

						FVector lightDir = light.Get()->position - hitPoint;
						// square of the distance between hitPoint and the light
						float lightDistance2 = FVector::DotProduct(lightDir, lightDir);
						lightDir.Normalize();
						float LdotN = std::max(0.f, FVector::DotProduct(lightDir, N));

						IObjectInterface* shadowHitObject = nullptr;
						float tNearShadow = kInfinity;

						bool inShadow = bvhTree->Intersect(Ray(shadowPointOrig, lightDir)).happened;

						lightAmt += false ? FVector::ZeroVector : light.Get()->intensity * LdotN * FVector::OneVector;

						FVector reflectionDirection = reflect(-lightDir, N);
						specularColor += powf(std::max(0.f, -FVector::DotProduct(reflectionDirection, ray.direction)),
							m->specularExponent) * light.Get()->intensity * FVector::OneVector;

						UKismetSystemLibrary::DrawDebugLine(GetWorld(), hitPoint, light.Get()->position, FColor(0, 255, 0, 48), .04f, 1.0f);

					}	
				}
				hitColor = lightAmt * (hitObject->evalDiffuseColor(st) * m->Kd + specularColor * m->Ks);
				break;
			}
			
		}
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), ray.origin, hitPoint, FColor(255, 0, 0, 48), .04f, 1.0f);
	}
	return hitColor;
}

// 本作业没用上
bool AHw6_Main::trace(const Ray& ray, float& tNear, uint32_t& index, AHw6_MeshTriangle** hitObject)
{
	*hitObject = nullptr;
	for (int32 k = 0; k < objects.Num(); ++k) {
		float tNearK = kInfinity;
		uint32_t indexK;
		FVector2D uvK;
		if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
			*hitObject = Cast<AHw6_MeshTriangle>(objects[k].GetObject());
			tNear = tNearK;
			index = indexK;
		}
	}

	return (*hitObject != nullptr);
}

FVector AHw6_Main::reflect(const FVector& I, const FVector& N)
{
	FVector res = I - 2 * FVector::DotProduct(I, N) * N;
	res.Normalize();
	return res;
}

FVector AHw6_Main::refract(const FVector& I, const FVector& N, const float& ior)
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

float AHw6_Main::fresnel(const FVector& I, const FVector& N, const float& ior)
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

void AHw6_Main::CreateTexture()
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



