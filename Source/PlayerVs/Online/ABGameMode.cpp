// Fill out your copyright notice in the Description page of Project Settings.

#include "Online/ABGameMode.h"
#include "Online/ABGameState.h"
#include "Player/ABCharacter.h"
#include "Player/ABPlayerController.h"
#include "Player/ABPlayerState.h"
#include "GameFramework/PlayerStart.h"

AABGameMode::AABGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Blueprints/Player/ABCharacter_BP"));
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerOb(TEXT("/Game/Blueprints/Player/ABPlayerController_BP"));

	static ConstructorHelpers::FClassFinder<APawn> CharacterOb(TEXT("/Game/Blueprints/Player/ABCharacter_BP"));
	static ConstructorHelpers::FClassFinder<APawn> FPSCharacterOb(TEXT("/Game/Blueprints/Player/FPS_ABCharacter_BP"));
	static ConstructorHelpers::FClassFinder<APawn> SpectatorOb(TEXT("/Game/Blueprints/Player/ABSpectator_BP"));

	PlayerControllerClass = PlayerControllerOb.Class;
	PlayerStateClass = AABPlayerState::StaticClass();
	GameStateClass = AABGameState::StaticClass();

	FPSCharacterTemplate = FPSCharacterOb.Class;
	VRCharacterTemplate = CharacterOb.Class;
	VRSpectatorTemplate = SpectatorOb.Class;

	bUseSeamlessTravel = true;
}

// called before any gameplay begins
void AABGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AABGameMode::PreLogin(const FString & Options, const FString & Address, const FUniqueNetIdRepl & UniqueId, FString & ErrorMessage)
{
	AABGameState* const MyGameState = GetABGameState();
	if (MyGameState->HasMatchEnded()) 
	{
		ErrorMessage = TEXT("Match has finished.");
		UE_LOG(LogTemp, Warning, TEXT("Player could not join %s"), *ErrorMessage)
	}
	else
	if (MyGameState->HasMatchStarted())
	{
		ErrorMessage = TEXT("Match has started.");
		UE_LOG(LogTemp, Warning, TEXT("Player could not join %s"), *ErrorMessage)
	}
	else
	{
		Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	}
}

void AABGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AABGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
}

void AABGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AssignAliens(FMath::Max(1, NumPlayers / 3));
	UnassignedToInnocent();
}

bool AABGameMode::ReadyToStartMatch_Implementation()
{
	bool bIsReadyToStart = NumPlayers >= MinimumPlayers;

	return bIsReadyToStart;
}

AABGameState* AABGameMode::GetABGameState()
{
	return Cast<AABGameState>(GameState);
}


void AABGameMode::AssignAliens(int AlienCount)
{
	while (AlienCount > 0)
	{
		int AlienIndex = FMath::RandRange(0, NumPlayers - 1);
		auto PS = Cast<AABPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), AlienIndex));
		if (PS->GetTeam() != ETeam::Alien)
		{
			PS->ServerSetTeam(ETeam::Alien);
			AlienCount--;
		}
	}
}

void AABGameMode::UnassignedToInnocent()
{
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		auto PS = Cast<AABPlayerController>((*It));
		if (PS->GetTeam() == ETeam::Unassigned)
		{
			PS->ServerSetTeam(ETeam::Innocent);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Game Logic 

void AABGameMode::ControllerNeedsCharacter(AController* Controller, bool HMDEnabled, FVector HMDOffset, FRotator HMDRotation)
{
	FTransform SpawnTransform;

	TArray<AActor*> Spawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Spawns);
	if (!Spawns.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("No PlayerStart found."))
	}
	else
	{
		int PlayerStartIndex = FMath::FRandRange(0, Spawns.Num() - 1);
		AActor *Spawn = Spawns[PlayerStartIndex];
		SpawnTransform = Spawn->GetActorTransform();
	}

	FActorSpawnParameters SpawnInfo;
	AABCharacter *Character;
	if (!HMDEnabled && FPSCharacterTemplate)
	{
		Character = GetWorld()->SpawnActor<AABCharacter>(FPSCharacterTemplate, SpawnTransform, SpawnInfo);
		UE_LOG(LogTemp, Warning, TEXT("Spawn FPSCharacterTemplate"))
	}
	else if (VRCharacterTemplate)
	{
		Character = GetWorld()->SpawnActor<AABCharacter>(VRCharacterTemplate, SpawnTransform, SpawnInfo);
		UE_LOG(LogTemp, Warning, TEXT("Spawn VRCharacterTemplate"))
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FPS or VR Character templates are not set properly."))
			return;
	}

	Controller->Possess(Character);
}

void AABGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
{
	ControllerNeedsSpectator(KilledPlayer);
}

void AABGameMode::ControllerNeedsSpectator(AController* Controller)
{
	AABPlayerController* KilledPC = Cast<AABPlayerController>(Controller);
	bool bHMDEnabled = KilledPC ? KilledPC->HMDEnable : false;

	FTransform SpawnTransform;
	if (Controller->GetPawn())
		SpawnTransform = Controller->GetPawn()->GetActorTransform();

	APawn* Spectator;
	FActorSpawnParameters SpawnInfo;
	if (VRSpectatorTemplate && bHMDEnabled)
	{
		Spectator = GetWorld()->SpawnActor<APawn>(VRSpectatorTemplate, SpawnTransform, SpawnInfo);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FPS or VR Spectator templates are not set properly."))
		return;
	}

	Controller->Possess(Spectator);
}
