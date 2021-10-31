// Fill out your copyright notice in the Description page of Project Settings.


#include "Hw7_Main.h"
#include "Hw7_Triangle.h"
#include "Hw7_Material.h"

#include "PathTracingCalcThread.h"

AHw7_Main::AHw7_Main()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("rootComp"));
	SetRootComponent(rootComp);

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	cameraComp->SetupAttachment(rootComp);
	cameraComp->SetWorldLocation(FVector(-300, 0, 0));
	cameraComp->bCameraMeshHiddenInGame = false;
}

// Called when the game starts or when spawned
void AHw7_Main::BeginPlay()
{
	Super::BeginPlay();
	CreateTexture();
	rayTraceQueue.Empty();
	pixelQueue.Empty();

	InitialScene();
	FLatentActionInfo info;
	UKismetSystemLibrary::Delay(GetWorld(), 1.0f, info);

	buildBVHTree();

	// 创建多线程
	for (int32 i = 0; i < FPlatformMisc::NumberOfCores(); i++)
	{
		new FPathTracingCalcThread(FString::Printf(TEXT("PathTracingCalcThread %d"), i), this);
	}
}

void AHw7_Main::InitialScene()
{
	Material* red = new Material(EMaterialType::DIFFUSE, FVector(0.0f));
	red->Kd = FVector(0.63f, 0.065f, 0.05f);
	Material* green = new Material(EMaterialType::DIFFUSE, FVector(0.0f));
	green->Kd = FVector(0.14f, 0.45f, 0.091f);
	Material* white = new Material(EMaterialType::DIFFUSE, FVector(0.0f));
	white->Kd = FVector(0.725f, 0.71f, 0.68f);
	Material* light = new Material(
		EMaterialType::DIFFUSE, 
		(
			8.0f * FVector(0.747f + 0.058f, 0.747f + 0.258f, 0.747f)
			+ 15.6f * FVector(0.740f + 0.287f, 0.740f + 0.160f, 0.740f)
			+ 18.4f * FVector(0.737f + 0.642f, 0.737f + 0.159f, 0.737f)
		)
	);

	light->Kd = FVector(0.65f);

	FTransform Spawntransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
	for (auto& mesh : StaticMeshes) {
		AHw7_MeshTriangle* Actor = GetWorld()->SpawnActor<AHw7_MeshTriangle>(AHw7_MeshTriangle::StaticClass(), Spawntransform);
		if (Actor && Actor->Implements<UHw7_Object>()) {
			objects.Add(Actor);
			Actor->meshComp->SetStaticMesh(mesh);
			//UKismetSystemLibrary::PrintString(GetWorld(), mesh->GetName());
			if (mesh->GetName()=="floor" || mesh->GetName()=="shortbox" || mesh->GetName()=="tallbox")
				Actor->Init(white);
			else if(mesh->GetName()=="left")
				Actor->Init(red);
			else if(mesh->GetName() == "right")
				Actor->Init(green);
			else if(mesh->GetName() == "light")
				Actor->Init(light);
		}
			
	}
	
}

void AHw7_Main::buildBVHTree()
{
	printf(" - Generating BVH...\n\n");
	bvhTree = NewObject<UBVHAccelTree>(this);
	bvhTree->Init(objects, 1, UBVHAccelTree::SplitMethod::NAIVE, true);
}

// Called every frame
void AHw7_Main::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (rowNumber < height)
		PreRender();

	UpdateTexture();
}

void AHw7_Main::PreRender()
{

	for (int i = 0; i < width; ++i)
	{	
		ensure(rayTraceQueue.Enqueue(ScreenCoords(i, rowNumber)));
	}
	rowNumber++;
}

//【多线程】
FVector AHw7_Main::Render(const Ray& ray, int depth)
{
	FVector hitColor = FVector::ZeroVector;
	for (int i = 0; i < SPP; i++)
	{
		hitColor += castRay(ray, depth);
	}
	hitColor /= SPP;
	return hitColor;
}

//【多线程】
FVector AHw7_Main::castRay(const Ray& ray, int depth)
{

	Intersection hit_inter =  bvhTree->Intersect(ray); // 获取相交信息
	FVector hitColor = FVector::ZeroVector;
	if (hit_inter.happened) {
		// 判断是否直接打中发光源
		if (hit_inter.m->hasEmission()) {

			if (depth == 0) {
				// 主线程中绘制
				hitColor = hit_inter.m->getEmission();
				if (bAllowDrawDebug) {
					AsyncTask(ENamedThreads::GameThread, [=]()
						{
							UKismetSystemLibrary::DrawDebugLine(GetWorld(), ray.origin, hit_inter.coords,
								hitColor, 0.1f, 1.0f);
						}
					);
				}
				return hitColor;
			}
			else // 间接打到光源
				return FVector::ZeroVector;
		}
		//return hitColor;

		FVector hit_pos = hit_inter.coords;
		FVector hit_normal = hit_inter.normal;

		// 直接光照
		FVector L_dir = FVector::ZeroVector;
		Intersection light_inter;
		float light_pdf = 0;
		sampleLight(light_inter, light_pdf);  //随机采样光照，用采样结果判断是否打到光源

		FVector light_dir = light_inter.coords - hit_pos;
		float light_distance2 = FVector::DotProduct(light_dir, light_dir);
		light_dir.Normalize();


		Ray light_ray = Ray(hit_pos, light_dir);
		Intersection Inter_light_2_point = bvhTree->Intersect(light_ray); // 反射光线


		// 如果打到光源
		if (Inter_light_2_point.happened && Inter_light_2_point.m->hasEmission()) {
			// L_dir = L_i * f_r * cos_theta * cos_theta_x / |x-p|^2 / pdf_light
			// L_dir = emit * eval(wo , ws , N) * dot(ws , N) * dot(ws , NN) / |x-p|^2 / pdf_light

			FVector L_i = light_inter.emit;
			FVector f_r = hit_inter.m->eval(ray.direction, light_dir, hit_normal);
			float cos_theta = FVector::DotProduct(hit_normal, light_dir);
			float cos_theta_x = FVector::DotProduct(-light_dir, light_inter.normal); //此处注意向量方向
			L_dir = L_i * f_r * cos_theta * cos_theta_x / light_distance2 / light_pdf;
		}

		// 间接光照
		FVector L_indir = FVector::ZeroVector;
		if (UKismetMathLibrary::RandomFloat() < RussianRoulette) {
			FVector next_dir = hit_inter.m->sample(ray.direction, hit_normal);
			next_dir.Normalize();

			Ray next_ray(hit_pos, next_dir);
			Intersection next_hit_inter = bvhTree->Intersect(next_ray);

			if (next_hit_inter.happened && !next_hit_inter.m->hasEmission()) {
				// L_indir = shade (q, wi) * f_r * cos_theta / pdf_hemi / P_RR
				// L_indir = shade (q, wi) * eval (wo , wi , N) * dot(wi , N) / pdf(wo , wi , N) / RussianRoulette

				FVector f_r = hit_inter.m->eval(ray.direction, next_dir, hit_normal);
				float pdf = hit_inter.m->pdf(ray.direction, next_dir, hit_normal);
				float cos_theta = FVector::DotProduct(hit_normal, next_dir);
				L_indir = castRay(next_ray, depth + 1) * f_r * cos_theta / pdf / RussianRoulette;

			}
		}
		hitColor = L_dir + L_indir;
		if (bAllowDrawDebug){
			AsyncTask(ENamedThreads::GameThread, [=]()
				{

					UKismetSystemLibrary::DrawDebugLine(GetWorld(), ray.origin, hit_pos, hitColor, 0.1f, 1);
				}
		);
	}
	}
	return hitColor;
}

//【多线程】
void AHw7_Main::sampleLight(Intersection& pos, float& pdf) const
{
	float emit_area_sum = 0;
	for (int32 k = 0; k < objects.Num(); ++k) {
		if (objects[k]->hasEmit()) {
			emit_area_sum += objects[k]->getArea();
		}
	}
	float p = UKismetMathLibrary::RandomFloat() * emit_area_sum;
	emit_area_sum = 0;
	for (int32 k = 0; k < objects.Num(); ++k) {
		if (objects[k]->hasEmit()) {
			emit_area_sum += objects[k]->getArea();
			if (p <= emit_area_sum) {
				objects[k]->Sample(pos, pdf);
				break;
			}
		}
	}
}

void AHw7_Main::CreateTexture()
{
	T_Result = UTexture2D::CreateTransient(width, height);
	FTexture2DMipMap& Mip = T_Result->PlatformData->Mips[0];
	FColor* Data = (FColor*)Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memzero(Data, width * height * sizeof(FColor));
	Mip.BulkData.Unlock();
	T_Result->UpdateResource();
	return;
	//T_Result->UpdateResource();
}

void AHw7_Main::UpdateTexture()
{
	// 更新texture
	if (T_Result && !pixelQueue.IsEmpty())
	{
		FTexture2DMipMap& Mip = T_Result->PlatformData->Mips[0];
		FColor* Data = (FColor*)Mip.BulkData.Lock(LOCK_READ_WRITE);

		for (int i = 0; i < width * 2 && !pixelQueue.IsEmpty(); i++) {
			ScreenPixel calc_res;
			ensure(pixelQueue.Dequeue(calc_res));
			if (calc_res.index < width * height)
				Data[calc_res.index] = calc_res.color;
		}
		Mip.BulkData.Unlock();
		T_Result->UpdateResource();

	}
}
