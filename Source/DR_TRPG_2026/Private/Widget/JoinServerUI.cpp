// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/JoinServerUI.h"
#include "ServerListSegment.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/ScrollBox.h"
#include "Multiplayer/MultiplayerGameLogicHandler.h"

void UJoinServerUI::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UJoinServerUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	UGameInstance* GameInstance = GetGameInstance();
	
	if (GameInstance)
	{
		MultiplayerJoiningHandler = GameInstance->GetSubsystem<UMultiplayerGameLogicHandler>();
	}

	SearchButton->OnClicked.AddDynamic(this, &UJoinServerUI::SearchForServers);
	MultiplayerJoiningHandler->OnServersFound.AddDynamic(this, &UJoinServerUI::OnServersFound);
	BackButton->OnClicked.AddDynamic(this, &UJoinServerUI::BackButtonClicked);
}

void UJoinServerUI::SearchForServers()
{
	MultiplayerJoiningHandler->FindGameSessions();
}

void UJoinServerUI::BackButtonClicked()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UJoinServerUI::OnServersFound(bool bSuccess)
{
	ServerListScrollBox->ClearChildren();
	
	if (!bSuccess) {return;}

	for (int32 i = 0; i < MultiplayerJoiningHandler->GetFoundSessionsNumber(); i++)
	{

		UServerListSegment* NewServerListSegment = CreateWidget<UServerListSegment>(this, ServerListItemBase);
		ServerListScrollBox->AddChild(NewServerListSegment);
		UE_LOG(LogTemp, Warning, TEXT("Server List Segment"));
		if (NewServerListSegment)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server List Segment Naming"));
			NewServerListSegment->PureTextServerName = MultiplayerJoiningHandler->GetFoundSessionsName(i);
			NewServerListSegment->SessionIndex = i;
		}
	}
}
