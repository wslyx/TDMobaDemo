// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/TDMobaPlayerCameraManager.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

/**
 *相关内容可以参照jerish博客内容：https://blog.csdn.net/u012999985/article/details/68947410
 */

void ATDMobaPlayerCameraManager::MoveRight(float Val)
{
	FreeCamOffset.X += Val;
}

void ATDMobaPlayerCameraManager::MoveUp(float Val)
{
	FreeCamOffset.Y += Val;
}

void ATDMobaPlayerCameraManager::LockCamera(bool Lock)
{
	bLockCamera = Lock;
	FreeCamOffset = bLockCamera ? FVector::ZeroVector: GetPawnCamera()->GetComponentLocation() ;
}

UCameraComponent * ATDMobaPlayerCameraManager::GetPawnCamera()
{
	if (!PawnCamera)
	{
		PawnCamera = Cast<UCameraComponent>(PCOwner->GetPawn()->GetComponentByClass(UCameraComponent::StaticClass()));
	}
	return PawnCamera;
}

void ATDMobaPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	/**
	 *AActor::CalcCamera()貌似也能实现这个功能 对camera进行offset
	 */

	if ((PendingViewTarget.Target != NULL) && BlendParams.bLockOutgoing && OutVT.Equal(ViewTarget))
	{
		return;
	}

	FMinimalViewInfo OrigPOV = OutVT.POV;

	OutVT.POV.FOV = DefaultFOV;
	OutVT.POV.OrthoWidth = DefaultOrthoWidth;
	OutVT.POV.AspectRatio = DefaultAspectRatio;
	OutVT.POV.bConstrainAspectRatio = bDefaultConstrainAspectRatio;
	OutVT.POV.bUseFieldOfViewForLOD = true;
	OutVT.POV.ProjectionMode = bIsOrthographic
		                           ? ECameraProjectionMode::Orthographic
		                           : ECameraProjectionMode::Perspective;
	OutVT.POV.PostProcessSettings.SetBaseValues();
	OutVT.POV.PostProcessBlendWeight = 1.0f;
	
	UpdateViewTargetInternal(OutVT, DeltaTime);
	/*
	*锁住camera是指相机跟着pawn位移，反之跟着mouse走
	*/
	if (!bLockCamera)
	{
		OutVT.POV.Location = FreeCamOffset;
	}

	ApplyCameraModifiers(DeltaTime, OutVT.POV);

	SetActorLocationAndRotation(OutVT.POV.Location, OutVT.POV.Rotation, false);
	UpdateCameraLensEffects(OutVT);
}
