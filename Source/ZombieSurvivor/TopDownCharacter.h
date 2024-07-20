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

class AEnemy;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDeathDelegate);

UCLASS()
class ZOMBIESURVIVOR_API ATopDownCharacter : public APawn
{
	GENERATED_BODY()

public:
	ATopDownCharacter();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=VisualDebug)
	bool DebugEnabled{false};

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay);
	float TraceDistance{30.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	float TraceOffset{20.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsAlive{true};

	FPlayerDeathDelegate OnPlayerDeath;
	
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

	UPROPERTY(EditAnywhere)
	TArray<FHitResult> HitResults;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AEnemy> ClosestTarget = nullptr;
	
	UFUNCTION()
	EDirectionFacing CalculateFacingDirection(const FVector2D& Value);
	
	UFUNCTION()
	void MoveTriggered(const FInputActionValue& Value);

	UFUNCTION()
	void MoveCompleted(const FInputActionValue& Value);

	UFUNCTION()
	void Shoot(const FInputActionValue& Value);
	void ResetTarget();

	UFUNCTION()
	void EquipGun();

	UFUNCTION()
	void CalculateMousePositionInWorld();

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void ChangeFlipBookAnimation(bool bEquipped);

	UFUNCTION()
	void UpdateGunAnimation(bool bEquipped);

	UFUNCTION()
	bool IsInMapBoundsHorizontal(float XPos);

	UFUNCTION()
	bool IsInMapBoundsVertical(float ZPos);

	UFUNCTION()
	void CalculateNextLocation(FVector& NewLocation);

	UPROPERTY()
	FVector TraceDirection {};

	UFUNCTION()
	void SetTraceDirection();

	UFUNCTION()
	FVector GetTraceDirection() const {return TraceDirection;}
	
	UFUNCTION()
	void TraceForClosestTargetInDirection();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
