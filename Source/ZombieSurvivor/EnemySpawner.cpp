// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemySpawner::StartSpawnTimer()
{
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&AEnemySpawner::OnEnemySpawnTimerTimeout,
		SpawnRate,
		true,
		SpawnRate
	);
}

void AEnemySpawner::StopSpawnTimer()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

void AEnemySpawner::OnEnemySpawnTimerTimeout()
{
	SpawnEnemy();
}

void AEnemySpawner::DifficultySettings(const int& Count)
{
	// Every 10 Spawned Enemies
	if(Count %  DifficultySpikeInterval == 0)
	{
		// Check if we can decrease the Rate
		if(SpawnRate > SpawnRateMinimumLimit)
		{
			SpawnRate -= DecreaseSpawnRateByEveryInterval;
			if(SpawnRate < DecreaseSpawnRateByEveryInterval)
			{
				// Make sure Rate stays at minimum Lvl
				SpawnRate = SpawnRateMinimumLimit;
			}
			
			// To Update Timer vars, we need to Stop/Start
			StopSpawnTimer();
			StartSpawnTimer();
		}
	}
}

void AEnemySpawner::SpawnEnemy()
{
	FVector2D RandomPoint = FVector2D(FMath::VRand());
	RandomPoint.Normalize();
	RandomPoint *= SpawnDistance;
	const FVector SpawnRelativeToParent = GetActorLocation() + FVector(RandomPoint.X, 0.f,RandomPoint.Y);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	
	AEnemy* NewEnemy = GetWorld()->SpawnActor<AEnemy>(
		EnemyClass, SpawnRelativeToParent, FRotator::ZeroRotator, SpawnParams
	);
	
	if(NewEnemy)
	{
		NewEnemy->SetTarget(PlayerTarget);
		NewEnemy->EnemyDiedDelegate.AddDynamic(this, &AEnemySpawner::OnEnemyDied);
		
		SpawnedEnemies.AddUnique(NewEnemy);
		DifficultySettings(SpawnedEnemies.Num());
	}
}

void AEnemySpawner::OnEnemyDied()
{
	int ScoreToAdd = 10;
	GameModeTopDown->AddScore(ScoreToAdd);
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if(AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld()))
	{
		GameModeTopDown = static_cast<AGameModeTopDown*>(GameMode);
	}
	
	// Enemies should have the player from start
	if(PlayerTarget == nullptr)
	{
		if(AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATopDownCharacter::StaticClass()))
		{
			PlayerTarget = static_cast<ATopDownCharacter*>(Actor);
			PlayerTarget->OnPlayerDeath.AddDynamic(this, &AEnemySpawner::OnPlayerDeath);
			//DrawDebugLine(GetWorld(), GetActorLocation(),PlayerTarget->GetActorLocation(), FColor::Red, true);
		}
	}

	SpawnedEnemies.Empty();
	
	StartSpawnTimer();
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemySpawner::OnPlayerDeath()
{
	//StopSpawnTimer();
	if(SpawnedEnemies.Num() > 0)
	{
		for (AEnemy* Enemy : SpawnedEnemies)
		{
			if(Enemy->bIsAlive)
			{
				Enemy->MoveSpeed = 0.f;
			}
		}
	}
	
	// GameMode->RestartGame();
}