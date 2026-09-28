// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardGridHighlight.generated.h"

class AChessBoardManager;

UCLASS()
class ABoardGridHighlight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoardGridHighlight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(VisibleAnywhere, Category="Tile")
	TObjectPtr<UStaticMeshComponent> HighlightMesh;
 
	// Which board square this tile represents.
	UPROPERTY(Replicated)
	FIntPoint GridCoord;
 
	UPROPERTY()
	TObjectPtr<AChessBoardManager> OwningGridManager;
 
private:
	UFUNCTION()
	void HandleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
};
