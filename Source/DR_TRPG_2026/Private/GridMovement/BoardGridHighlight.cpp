// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMovement/BoardGridHighlight.h"

#include "ChessBoardManager.h"
#include "Kismet/GameplayStatics.h"
#include "Multiplayer/ChessPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/TPlayerController.h"

// Sets default values
ABoardGridHighlight::ABoardGridHighlight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HighlightMesh = CreateDefaultSubobject<UStaticMeshComponent>("HighlightMesh");
	RootComponent = HighlightMesh;
	HighlightMesh->OnClicked.AddDynamic(this, &ABoardGridHighlight::HandleClicked);
	
	HighlightMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HighlightMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	HighlightMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	bReplicates = true;
	SetReplicatingMovement(false);
}

// Called when the game starts or when spawned
void ABoardGridHighlight::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABoardGridHighlight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoardGridHighlight::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABoardGridHighlight, GridCoord);
}

void ABoardGridHighlight::HandleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (ATPlayerController* LocalPlayerController = Cast<ATPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		if (AChessPlayerState* LocalPlayerState = LocalPlayerController->GetPlayerState<AChessPlayerState>())
		{
			LocalPlayerState->ServerTryMoveSelectedPieceTo(GridCoord);
		}
	}
}

