// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ABGameMode.generated.h"

class AABGameState;
class AController;
class APawn;

UCLASS()
class PLAYERVS_API AABGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

	virtual void PreInitializeComponents() override;

	virtual void HandleMatchIsWaitingToStart() override;

	virtual void HandleMatchHasStarted() override;

	virtual bool ReadyToStartMatch_Implementation() override;

	/** Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string. */
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/** starts match warmup */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** update remaining time */
	virtual void DefaultTimer();

	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);
	
protected:
	/** Handle for efficient management of DefaultTimer timer */
	FTimerHandle TimerHandle_DefaultTimer;

	
	AABGameState* GetABGameState();

	/** 
		Assign PlayerStates to Alien team, 
		Alien Count must be <= NumPlayers!
	*/
	void AssignAliens(int AlienCount);

	/**
		PlayerStates that Unassigned will be set to Innocent
	*/
	void UnassignedToInnocent();
};
