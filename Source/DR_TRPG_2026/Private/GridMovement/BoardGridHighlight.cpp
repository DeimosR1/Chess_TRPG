// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMovement/BoardGridHighlight.h"

#include "ChessBoardManager.h"

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

void ABoardGridHighlight::HandleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (IsValid(OwningGridManager))
	{
		OwningGridManager->TryMoveSelectedPieceTo(GridCoord);
	}
}

