// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/MultiplayerRelayTower.h"

#include "GridMovement/ChessBoardManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMultiplayerRelayTower::AMultiplayerRelayTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMultiplayerRelayTower::BeginPlay()
{
	Super::BeginPlay();
	
	ChessBoardManagerServer = Cast<AChessBoardManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AChessBoardManager::StaticClass()));
}

// Called every frame
void AMultiplayerRelayTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMultiplayerRelayTower::GetAndRelayClientSelectedPiece(APChessPiece* SelectedPiece)
{
	if (IsValid(ChessBoardManagerServer))
	{
		ChessBoardManagerServer->SelectPiece(SelectedPiece);
	}
}

