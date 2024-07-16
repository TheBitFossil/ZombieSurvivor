// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Pawn.h"
#include "Framework/Application/SlateApplication.h"
#include "TopDownCharacter.generated.h"

UENUM()
enum class EDirectionFacing : uint8
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

UCLASS()
class ZOMBIESURVIVOR_API ATopDownCharacter : public APawn
{
	GENERATED_BODY()

public:
	ATopDownCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperFlipbookComponent> FlipBookComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon)
	TObjectPtr<UChildActorComponent> GunChildActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> Marker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperFlipbookComponent> MarkerSprite;

	TSharedPtr<FSlateDynamicImageBrush> CustomCursorBrush;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Weapon)
	TSubclassOf<class AGun> GunClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon)
	bool bHasGunEquipped {false};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> IA_Shoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> IA_EquipGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D HorizontalMapBounds { 10.f, 200.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VerticalMapBounds { -50.f, -250.f };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	float MoveSpeed {200.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	bool bCanMove {true};

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Gameplay);
	float CollisionShapeRadius{50.f};
	
	// UP/ DOWN / RIGHT / LEFT
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Walk;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Shoot_Idle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Shoot_Walk;

	UFUNCTION()
	EDirectionFacing GetDirectionFacing() const { return DirectionFacing; }

	UFUNCTION()
	FVector2D GetDirection() const { return Direction; }
	
private:
	UPROPERTY(VisibleInstanceOnly)
	FVector2D Direction {};
	
	UPROPERTY(VisibleAnywhere)
	EDirectionFacing DirectionFacing {};

	UFUNCTION()
	EDirectionFacing CalculateFacingDirection(const FVector2D& Value);
	
	UFUNCTION()
	void MoveTriggered(const FInputActionValue& Value);

	UFUNCTION()
	void MoveCompleted(const FInputActionValue& Value);

	UFUNCTION()
	void Shoot(const FInputActionValue& Value);

	UFUNCTION()
	void EquipGun();

	UFUNCTION()
	void CalculateMousePositionInWorld();
	
	UFUNCTION()
	void ChangeFlipBookAnimation(bool bEquipped);

	UFUNCTION()
	void UpdateGunAnimation(bool bEquipped);

	UFUNCTION()
	bool IsInMapBoundsHorizontal(float XPos);

	UFUNCTION()
	bool IsInMapBoundsVertical(float ZPos);
	void CalculateNextLocation(FVector& NewLocation);
	void SetTraceDirection(FVector& StartLocation, FVector& EndPoint);
	void TraceForClosestTargetInDirection(FVector StartLocation, FVector EndPoint);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
