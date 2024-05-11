// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintTarget.h"
#include "PaintSampleDebugHUD.h"
#include "kismet/KismetRenderingLibrary.h"

APaintTarget::APaintTarget()
{
}

void APaintTarget::BeginPlay()
{
	Super::BeginPlay();
	
	// �e�N�X�`���쐬
	PaintRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA16f, FLinearColor::White);
	PaintRenderTargetMask = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA16f, FLinearColor::Transparent);

	// �����e�N�X�`����`��
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), PaintRenderTarget, StoneMaterial);

	// �}�e���A�����쐬���āA�e�N�X�`����ݒ�
	PaintMaterialInstance = UMaterialInstanceDynamic::Create(RenderTargetMaterialOriginal, this);
	PaintMaterialInstance->SetTextureParameterValue(FName("TextureSampleParam"), PaintRenderTarget);

	// ���b�V���̃}�e���A����u������
	StaticMeshComponent = FindComponentByClass<UStaticMeshComponent>();
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetMaterial(0, PaintMaterialInstance);

		UE_LOG(LogTemp, Warning, TEXT("Texture Replace Completed"));
	}

	// �f�o�b�O�`��
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APaintSampleDebugHUD* HUD = Cast<APaintSampleDebugHUD>(PC->GetHUD()))
		{
			{
				FPaintSampleDebugHUDDrawParam DrawParam;
				DrawParam.ScreenH = 300;
				DrawParam.ScreenW = 300;
				DrawParam.ScreenX = 10;
				DrawParam.ScreenY = 10;
				DrawParam.Texture = PaintRenderTarget;
				HUD->UpdateParam(APaintSampleDebugHUD::EIndex::RenderTarget, DrawParam);
			}
			{
				FPaintSampleDebugHUDDrawParam DrawParam;
				DrawParam.ScreenH = 300;
				DrawParam.ScreenW = 300;
				DrawParam.ScreenX = 320;
				DrawParam.ScreenY = 10;
				DrawParam.Texture = PaintRenderTargetMask;
				DrawParam.BlendMode = EBlendMode::BLEND_Translucent;
				HUD->UpdateParam(APaintSampleDebugHUD::EIndex::RenderTargetMask, DrawParam);
			}
		}
	}
}

void APaintTarget::PaintToPoint(UMaterialInstanceDynamic* BrushMaterial, const FHitResult& HitResult )
{
	const FVector& PaintPos = HitResult.ImpactPoint;
	;
	auto* World = GetWorld();
	check(World);
	check(BrushMaterial);
	check(PaintRenderTarget);

	const FVector Offset = HitResult.Normal * PaintInterval * 2;
	const FVector Start = PaintPos + Offset;
	const FVector End = PaintPos - Offset;
	FHitResult MyHitResult;
	if (StaticMeshComponent->LineTraceComponent(MyHitResult, Start, End, FCollisionQueryParams(SCENE_QUERY_STAT(GetUVFromWorldLocation), true)))
	{
		const auto OutUV = CalcUV(MyHitResult);
		check(OutUV.ContainsNaN() == false);
		UE_LOG(LogTemp, Log, TEXT("Painting %.2f %.2f"), OutUV.X, OutUV.Y);

		// �u���V��Material�Ƀp�����[�^��ݒ�
		BrushMaterial->SetVectorParameterValue(FName("PaintUV"), FVector(OutUV.X, OutUV.Y, 0));
		BrushMaterial->SetTextureParameterValue(FName("PaintTargetTexture"), PaintRenderTarget);

		UKismetRenderingLibrary::DrawMaterialToRenderTarget(World, PaintRenderTarget, BrushMaterial);
	}
}

FVector2f APaintTarget::CalcUV(const FHitResult& HitResult) const
{
	if (HitResult.FaceIndex < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Painting No Face"));
		return FVector2f(0, 0);
	}

	const int32 TriangleIndex = HitResult.FaceIndex;
	const FStaticMeshLODResources& LOD = StaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[0];
	const FIndexArrayView Indices = LOD.IndexBuffer.GetArrayView();

	// �ʂ��\�����钸�_�̃C���f�b�N�X���擾
	const int32 Index0 = Indices[TriangleIndex * 3];
	const int32 Index1 = Indices[TriangleIndex * 3 + 1];
	const int32 Index2 = Indices[TriangleIndex * 3 + 2];

	// ���_�� UV ���W���擾
	const FVector2f UV0 = LOD.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(Index0, 0);
	const FVector2f UV1 = LOD.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(Index1, 0);
	const FVector2f UV2 = LOD.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(Index2, 0);

	const FVector VertexPos0(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(Index0));
	const FVector VertexPos1(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(Index1));
	const FVector VertexPos2(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(Index2));

	// �o���Z���g���b�N���W���v�Z���� UV ����
	const FVector Barycentric = FMath::ComputeBaryCentric2D(HitResult.ImpactPoint
		, StaticMeshComponent->GetComponentTransform().TransformPosition(VertexPos0)
		, StaticMeshComponent->GetComponentTransform().TransformPosition(VertexPos1)
		, StaticMeshComponent->GetComponentTransform().TransformPosition(VertexPos2)
	);
	FVector2f OutUV = Barycentric.X * UV0
		+ Barycentric.Y * UV1
		+ Barycentric.Z * UV2;

	return OutUV;
}