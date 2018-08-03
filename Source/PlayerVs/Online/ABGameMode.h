// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ABGameMode.generated.h"

class AABCharacter;
class AABGameState;
class AController;
class APawn;
class AABPlayerState;

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
	
protected:	
	AABGameState* GetABGameState();

protected:
	UFUNCTION()
	virtual void ControllerNeedsSpectator(AController* Controller);

	UFUNCTION()
	virtual void FinishMatch();

public:

	UFUNCTION()
	virtual void ControllerNeedsCharacter(AController* Controller, bool HMDEnabled, FVector HMDOffset, FRotator HMDRotation);

	UPROPERTY(EditAnywhere, Category = "Initialization")
	TSubclassOf<AABCharacter> FPSCharacterTemplate;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	TSubclassOf<AABCharacter> VRCharacterTemplate;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	TSubclassOf<APawn> VRSpectatorTemplate;

	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	int MinimumPlayers;

	/* Bellow are some likely override functions!*/
protected:
	/* Overrides need to set some variable here about the winner / winning team.*/
	virtual void DetermineMatchWinner();

	virtual bool IsWinner(AABPlayerState* PlayerState) const;
public:
	/* Default behavior is to make player possess spectator*/
	UFUNCTION()
	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);
};
