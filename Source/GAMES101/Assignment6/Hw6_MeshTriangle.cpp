// Fill out your copyright notice in the Description page of Project Settings.


#include "Hw6_MeshTriangle.h"
#include "Kismet/KismetSystemLibrary.h"

/*******************************************************
               AHw6_Triangle 类
*******************************************************/

Intersection UHw6_Triangle::getIntersection(Ray ray)
{
	Intersection inter;

	if (FVector::DotProduct(ray.direction, normal) > 0)
		return inter;
	double u, v, t_tmp = 0;
	FVector pvec = FVector::CrossProduct(ray.direction, e2);
	double det = FVector::DotProduct(e1, pvec);
	if (fabs(det) < EPSILON)
		return inter;

	double det_inv = 1. / det;
	FVector tvec = ray.origin - v0;
	u = FVector::DotProduct(tvec, pvec) * det_inv;
	if (u < 0 || u > 1)
		return inter;
	FVector qvec = FVector::CrossProduct(tvec, e1);
	v = FVector::DotProduct(ray.direction, qvec) * det_inv;
	if (v < 0 || u + v > 1)
		return inter;
	t_tmp = FVector::DotProduct(e2, qvec) * det_inv;

	// TODO find ray triangle intersection
	if (t_tmp < 0)
		return inter;

	inter.distance = t_tmp;
	inter.coords = ray(t_tmp);
	inter.happened = true;
	inter.m = m;
	inter.normal = normal;
	inter.obj = Cast<IObjectInterface>(this);

	return inter;
}


/*******************************************************
               AHw6_MeshTriangle 类
*******************************************************/

AHw6_MeshTriangle::AHw6_MeshTriangle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	SetRootComponent(meshComp);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("StaticMesh'/Game/Assignment6/bunny.bunny'"));
	if (meshAsset.Succeeded())
	{
		meshComp->SetStaticMesh(meshAsset.Object);
	}
}

// Called when the game starts or when spawned
void AHw6_MeshTriangle::BeginPlay()
{
	Super::BeginPlay();
	
	getStaticMeshData();	
	buildBVHTree();
	
}


void AHw6_MeshTriangle::getStaticMeshData()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Start Getting Static Mesh !"));
	const uint32 LODIndex = 0;
	UStaticMesh* SM = meshComp->GetStaticMesh();
	if (SM)
	{
		FStaticMeshLODResources& LodResources = SM->RenderData->LODResources[LODIndex];
		FPositionVertexBuffer& VertexPosBuffer = LodResources.VertexBuffers.PositionVertexBuffer;

		
		vertexIndex.Empty();
		LodResources.IndexBuffer.GetCopy(vertexIndex);
		vertices.AddUninitialized(vertexIndex.Num());
		bounding_box = Bounds3();

		for (int i = 0; i < vertexIndex.Num(); i++) {
			// 按顶点序号添加顶点位置
			vertices[i] = VertexPosBuffer.VertexPosition(vertexIndex[i]);
			bounding_box.Union(vertices[i]);
		}
		FString outStr = FString::Printf(TEXT("Get Static Mesh Data success! vertices:%d, vertexIndex: %d"), vertices.Num(), vertexIndex.Num());
		UKismetSystemLibrary::PrintString(GetWorld(), outStr);
		outStr = FString::Printf(TEXT("Bounds %s   %s  %f"), *bounding_box.pMin.ToString(), *bounding_box.pMax.ToString(),bounding_box.Diagonal().Size());
		UKismetSystemLibrary::PrintString(GetWorld(), outStr);
	}
	else
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT(" Get Static Mesh Data Failed!"));
}

void AHw6_MeshTriangle::buildBVHTree()
{

	if (ensure(vertices.Num()))
	{
		triangles.Empty();
		//triangles.AddUninitialized(vertexIndex.Num()/3);
		auto new_mat = new Material(EMaterialType::DIFFUSE_AND_GLOSSY, FVector(0.5, 0.5, 0.5), FVector(0, 0, 0));
		new_mat->Kd = 0.6;
		new_mat->Ks = 0.0;
		new_mat->specularExponent = 0;

		for (int32 i = 0; i < vertexIndex.Num(); i += 3)
		{
			UHw6_Triangle* triangle = NewObject<UHw6_Triangle>();
			triangle->Init(vertices[i], vertices[i + 1], vertices[i + 2], new_mat);

			if (triangle && triangle->Implements<UObjectInterface>())
				triangles.Add(triangle);
		}
		bvhTree = NewObject<UBVHAccel>(this);
		bvhTree->Init(triangles);
	}

}

bool AHw6_MeshTriangle::rayTriangleIntersect(
	const FVector& v0, const FVector& v1,
	const FVector& v2, const FVector& orig,
	const FVector& dir, float& tnear,
	float& u, float& v) const
{
	// TODO: Implement this function that tests whether the triangle
	// that's specified bt v0, v1 and v2 intersects with the ray (whose
	// origin is *orig* and direction is *dir*)
	// Also don't forget to update tnear, u and v.
	FVector E1 = v1 - v0;
	FVector E2 = v2 - v0;
	FVector S = orig - v0;
	FVector S1 = FVector::CrossProduct(dir, E2);
	FVector S2 = FVector::CrossProduct(S, E1);

	float factor = FVector::DotProduct(S1, E1);
	float t = 1.0f / factor * FVector::DotProduct(S2, E2);
	float b1 = 1.0f / factor * FVector::DotProduct(S1, S);
	float b2 = 1.0f / factor * FVector::DotProduct(S2, dir);

	if (t > 0 && b1 > 0 && b2 > 0 && (1 - b1 - b2) > 0) {
		tnear = t;
		u = b1;
		v = b2;
		return true;
	}
	return false;
}

bool AHw6_MeshTriangle::intersect(const Ray& ray, float& tnear, uint32_t& index) const
{
	bool intersect = false;
	for (int32 k = 0; k < vertexIndex.Num(); k += 3) {
		const FVector& v0 = vertices[k * 3];
		const FVector& v1 = vertices[k * 3 + 1];
		const FVector& v2 = vertices[k * 3 + 2];
		float t, u, v;
		if (rayTriangleIntersect(v0, v1, v2, ray.origin, ray.direction, t, u, v) && t < tnear) {
			tnear = t;
			index = k;
			intersect |= true;
		}
	}
	return intersect;
}

void AHw6_MeshTriangle::getSurfaceProperties(
	const FVector& P, const FVector& I, 
	const uint32_t& index, const FVector2D& uv, 
	FVector& N, FVector2D& st) const
{
	const FVector& v0 = vertices[vertexIndex[index * 3]];
	const FVector& v1 = vertices[vertexIndex[index * 3 + 1]];
	const FVector& v2 = vertices[vertexIndex[index * 3 + 2]];
	FVector e0 = v1 - v0;
	FVector e1 = v1 - v2; // v2 - v1;
	e0.Normalize();
	e1.Normalize();
	N = FVector::CrossProduct(e0, e1);
	N.Normalize();
	const FVector2D& st0 = stCoordinates[vertexIndex[index * 3]];
	const FVector2D& st1 = stCoordinates[vertexIndex[index * 3 + 1]];
	const FVector2D& st2 = stCoordinates[vertexIndex[index * 3 + 2]];
	st = st0 * (1 - uv.X - uv.Y) + st1 * uv.X + st2 * uv.Y;
}

FVector AHw6_MeshTriangle::evalDiffuseColor(const FVector2D& st) const
{
	float scale = 5;
	float pattern = (fmodf(st.X * scale, 1) > 0.5) ^ (fmodf(st.Y * scale, 1) > 0.5);
	return UKismetMathLibrary::VLerp(FVector(0.815, 0.235, 0.031), FVector(0.937, 0.937, 0.231), pattern);
}

Intersection AHw6_MeshTriangle::getIntersection(Ray ray)
{
	Intersection intersec;

	if (bvhTree) {
		intersec = bvhTree->Intersect(ray);
	}

	return intersec;
}


