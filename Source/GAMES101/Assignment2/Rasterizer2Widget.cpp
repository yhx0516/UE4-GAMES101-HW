// Fill out your copyright notice in the Description page of Project Settings.


#include "Rasterizer2Widget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Actor_Assignment2.h"
#include "Kismet/GameplayStatics.h"


void URasterizer2Widget::NativePreConstruct()
{
	Super::NativePreConstruct();
	PreConstruct(IsDesignTime());
	if (Btn_Mass!=nullptr)
	{
		Btn_Mass->OnClicked.AddDynamic(this, &URasterizer2Widget::SwitchMASS);
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("NativeConstruct"));
	}
}
void URasterizer2Widget::SwitchMASS()
{	
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_Assignment2::StaticClass(), OutActors);
	if (OutActors.IsValidIndex(0)) {
		Cast<AActor_Assignment2>(OutActors[0])->SwitchMASS();		
	}
}
/*
int32 URasterizer2Widget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (bHasScriptImplementedPaint)
	{
		FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
		//OnPaint(Context);
		//DrawBackground(Context);

		return FMath::Max(LayerId, Context.MaxLayer);
	}

	return LayerId;
}
*/
void URasterizer2Widget::DrawBackground(FPaintContext& Context) const
{
	if (CanvasSize == FVector2D::ZeroVector) return;

	UWidgetBlueprintLibrary::DrawBox(Context, FVector2D::ZeroVector, CanvasSize, SlateBrushAsset, FColor(0, 0, 0, 0.6 * 255));
	for (int i = 0; i <= CanvasSize.X / 20; i++) {
		int start = i * 20;
		//UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(start, 0), FVector2D(start, CanvasSize.Y), FColor(120, 120, 120, 255));
		//UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(0, start), FVector2D(CanvasSize.X, start), FColor(120, 120, 120, 255));
	}
	
}

void URasterizer2Widget::Draw(TArray<FVector>& frame_buf)
{
	for (int i = 0; i < frame_buf.Num(); i++) {
//		UWidgetBlueprintLibrary::draw
		//UWidgetBlueprintLibrary::DrawBox(Context, FVector2D::ZeroVector, CanvasSize, SlateBrushAsset, FColor(0, 0, 0, 0.6 * 255));
	
	}
}


