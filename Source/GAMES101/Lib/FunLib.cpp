// Fill out your copyright notice in the Description page of Project Settings.


#include "FunLib.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UFunLib::GetStaticMeshVertexPositionByLODIndex(UStaticMeshComponent* SM, int32 LODIndex)
{
	TArray<FVector> OutPosition;

	FPositionVertexBuffer& posBuffer = SM->GetStaticMesh()->RenderData->LODResources[LODIndex].VertexBuffers.PositionVertexBuffer;

	for (uint32 i = 0; i < posBuffer.GetNumVertices(); ++i)
	{
		OutPosition.Emplace(posBuffer.VertexPosition(i));
	}
	return MoveTemp(OutPosition);
}

void UFunLib::ModfiyStaticMeshVertexColorByPointEffect(UStaticMeshComponent* SM, FVector HitPoint, float LimitDistance, FColor Color)
{
	for (int32 i = 0; i < SM->GetStaticMesh()->GetNumLODs(); ++i)
	{
		//获取顶点位置
		TArray<FVector> Position = GetStaticMeshVertexPositionByLODIndex(SM, i);
		TArray<FColor> VertexColor;
		VertexColor.AddUninitialized(Position.Num());

		//判断是否被应用顶点色
		if (SM->GetStaticMesh()->RenderData->LODResources[i].bHasColorVertexData)
		{
			for (int32 index = 0; index < Position.Num(); ++index)
			{
				FColor BaseColor = SM->LODData[i].OverrideVertexColors->VertexColor(index);
				if ((HitPoint - Position[index]).Size() < LimitDistance)
				{
					BaseColor = Color;
				}

				VertexColor[index] = BaseColor;
			}
		}
		else
		{

			for (int32 index = 0; index < Position.Num(); ++index)
			{
				FColor BaseColor = FColor(255, 255, 255, 255);
				if ((HitPoint - Position[index]).Size() < LimitDistance)
				{
					BaseColor = Color;
				}

				VertexColor[index] = BaseColor;
			}
		}

		SetStaticVertexColorOverride(SM, i, VertexColor);

		/* 另外一种方法
		* 
		
		//判断是否被应用顶点色
		if(SMComp->GetStaticMesh()->RenderData->LODResources[i].bHasColorVertexData){
			FColor OrigColor = SMComp->LODData[i].OverrideVertexColors->VertexColor(index);
		}
		
		TMap<FVector, FColor> NewColor;
		NewColor.Add(Position[index],Color);
		if (NewColor.Num() > 0) {
			SMComp->GetStaticMesh()->SetVertexColorData(NewColor);
			SMComp->GetStaticMesh()->PostEditChange();
		}
		*/
	}
}

void UFunLib::SetStaticVertexColorOverride(UStaticMeshComponent* SMComp, int32 LODIndex, const TArray<FColor>& VertexColor)
{
	if (!SMComp) return;

	//Get the static mesh that we're going to paint
	UStaticMesh* SM = SMComp->GetStaticMesh();
	
	if (SM)
	{
		//Get the vertex buffer from the 1st lod
		//FPositionVertexBuffer* PositionVertexBuffer = &SM->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;

		//Make sure that we have at least 1 LOD
		SMComp->SetLODDataCount(1, SMComp->LODData.Num());
		FStaticMeshComponentLODInfo* LODInfo = &SMComp->LODData[0]; //We're going to modify the 1st LOD only

		//Empty the painted vertices and assign a new color vertex buffer which will contain the new colors for each vertex
		LODInfo->PaintedVertices.Empty();
		LODInfo->OverrideVertexColors = new FColorVertexBuffer();

		//We're going to use the LODResources to get the total number of vertices that the provided mesh has
		FStaticMeshLODResources& LodResources = SM->RenderData->LODResources[0];
	

		//Initialize the new vertex colros with the array we created above
		LODInfo->OverrideVertexColors->InitFromColorArray(VertexColor);
		//LODInfo->OverrideVertexColors->VertexColor(LODIndex) = VertexColor;

		//SMComp->bCustomOverrideVertexColorPerLOD = true;
		//Initialize resource and mark render state of object as dirty in order for the engine to re-render it
		BeginInitResource(LODInfo->OverrideVertexColors);
		SMComp->MarkRenderStateDirty();
	}
}





void UFunLib::SetVertexColorOverride(UStaticMeshComponent* SMComp, FVector HitPoint, float LimitDistance, FColor Color)
{
	const uint32 LODIndex = 0;
	UStaticMesh* SM = SMComp->GetStaticMesh();
	if (SM)
	{
		//Get the vertex buffer from the 1st lod
		FPositionVertexBuffer* VertexPosBuffer = &SM->RenderData->LODResources[LODIndex].VertexBuffers.PositionVertexBuffer;

		//Make sure that we have at least 1 LOD
		SMComp->SetLODDataCount(1, SMComp->LODData.Num());
		FStaticMeshComponentLODInfo* LODInfo = &SMComp->LODData[LODIndex]; //We're going to modify the 1st LOD only

		//Empty the painted vertices and assign a new color vertex buffer which will contain the new colors for each vertex
		LODInfo->PaintedVertices.Empty();
		LODInfo->OverrideVertexColors = new FColorVertexBuffer();

		//We're going to use the LODResources to get the total number of vertices that the provided mesh has
		FStaticMeshLODResources& LodResources = SM->RenderData->LODResources[LODIndex];

		TArray<FColor> VertexColor;
		VertexColor.AddUninitialized(VertexPosBuffer->GetNumVertices());

		for (uint32 i = 0; i < VertexPosBuffer->GetNumVertices(); ++i)
		{
			VertexColor[i] = FColor(255, 255, 255, 255);
			if ((HitPoint - VertexPosBuffer->VertexPosition(i)).Size() < LimitDistance)
			{
				VertexColor[i] = Color;
			}
		}


		//Initialize the new vertex colros with the array we created above
		LODInfo->OverrideVertexColors->InitFromColorArray(VertexColor);
		//LODInfo->OverrideVertexColors->VertexColor(LODIndex) = VertexColor;

		//SMComp->bCustomOverrideVertexColorPerLOD = true;
		//Initialize resource and mark render state of object as dirty in order for the engine to re-render it
		BeginInitResource(LODInfo->OverrideVertexColors);
		SMComp->MarkRenderStateDirty();
	}
}
