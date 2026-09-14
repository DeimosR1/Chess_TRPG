// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PChessPiece.generated.h"

class UFloatingPawnMovement;

USTRUCT(BlueprintType)
struct FUnitStats
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName UnitDisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UnitMaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UnitCurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UnitStrength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UnitDefense;
};

UENUM(BlueprintType)
enum class EChessPieceType : uint8
{
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

UCLASS()
class APChessPiece : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APChessPiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Which side this piece belongs to. Used to tell allies from enemies when generating moves.
	UPROPERTY(EditAnywhere, Category="Stats")
	bool bIsPlayerTeam = true;
 
	// What kind of chess piece this is - drives how GetValidMoves() behaves.
	UPROPERTY(EditAnywhere, Category="Movement")
	EChessPieceType PieceType = EChessPieceType::Pawn;
 
	// This piece's current position on the logical board grid (NOT world space).
	UPROPERTY(VisibleAnywhere, Category="Movement")
	FIntPoint GridPosition = FIntPoint(0, 0);
 
	// Set true once a pawn has made its first move.
	UPROPERTY(VisibleAnywhere, Category="Movement")
	bool bHasMoved = false;
 
	// Returns every square this piece could legally move to or capture into this turn,
	// given the current occupancy of the board.
	UFUNCTION(BlueprintCallable, Category="Movement")
	TArray<FIntPoint> GetValidMoves(const TMap<FIntPoint, APChessPiece*>& OccupiedSquares, int32 BoardWidth, int32 BoardHeight) const;
 
	UFUNCTION(BlueprintCallable, Category="Grid")
	void SetGridPosition(const FIntPoint& NewGridPosition) { GridPosition = NewGridPosition; }
 
private:
	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(EditAnywhere, Category="Stats")
	FUnitStats UnitStats;
	UPROPERTY(EditAnywhere, Category="Stats")
	int32 UnitID;
	
	//For Grid Movement
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFloatingPawnMovement> MovementComponent;
	UPROPERTY(Category="Movement", VisibleAnywhere)
	float GridSize = 100.0f; //This size should stay the same between pieces
	UFUNCTION(BlueprintCallable, Category="Movement")
	FVector GetGridLocation(FVector Location);
 
	// Fires when the piece's mesh is clicked in the world; tells the ChessBoardManager to select it.
	UFUNCTION()
	void HandleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
 
	UPROPERTY()
	TObjectPtr<class AChessBoardManager> ChessBoardManager;
 
	// Sliding-move helper shared by Bishop/Rook/Queen (keeps going until blocked).
	UFUNCTION(BlueprintCallable, Category="Movement")
	void AddSlidingMoves(TArray<FIntPoint>& OutMoves, const TArray<FIntPoint>& Directions, const TMap<FIntPoint, APChessPiece*>& OccupiedSquares, int32 BoardWidth, int32 BoardHeight) const;
	// Single-step helper shared by Knight/King.
	UFUNCTION(BlueprintCallable, Category="Movement")
	void AddStepMoves(TArray<FIntPoint>& OutMoves, const TArray<FIntPoint>& Offsets, const TMap<FIntPoint, APChessPiece*>& OccupiedSquares, int32 BoardWidth, int32 BoardHeight) const;
	// Specific Mover for the Pawn so they can only move two squares infront in normal movement
	UFUNCTION(BlueprintCallable, Category="Movement")
	void AddPawnTwoMoves(TArray<FIntPoint>& OutMoves, const TArray<FIntPoint>& Directions, const TMap<FIntPoint, APChessPiece*>& OccupiedSquares, int32 BoardWidth, int32 BoardHeight) const;

public:
	UFUNCTION(BlueprintCallable, Category="Movement")
	void ChangeHasMoved(bool change){bHasMoved = change;}
	
	//Functions worked to Control the health
	UFUNCTION(BlueprintCallable, Category="Combat")
	void ReceiveDamage(int32 Attack);
	
	//Stat Getters
	UFUNCTION(BlueprintCallable, Category="Stats")
	int32 GetDamage() const {return UnitStats.UnitStrength;}
	UFUNCTION(BlueprintCallable, Category="Stats")
	int32 GetDefense() const {return UnitStats.UnitDefense;}
	UFUNCTION(BlueprintCallable, Category="Stats")
	int32 GetMaxHealth() const {return UnitStats.UnitMaxHealth;}
	UFUNCTION(BlueprintCallable, Category="Stats")
	int32 GetCurrentHealth() const {return UnitStats.UnitCurrentHealth;}
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	class UWidgetComponent* OverheadWidgetComponent;

	void ConfigureOverheadWidgetComponent();
};
