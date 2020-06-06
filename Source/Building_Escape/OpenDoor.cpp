


#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	TargetYaw += InitialYaw;
	
	if(!PressurePlate){
		UE_LOG(LogTemp, Error, TEXT("Actor %s has no PressurePlate set"), *GetOwner()->GetName());
	}

	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens)){
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
		//DoorLastOpened When the door was opened
	}
	else{

		if(DoorLastOpened - GetWorld()->GetTimeSeconds() < DoorCloseDelay)
			CloseDoor(DeltaTime);
	}

	

}

void UOpenDoor::OpenDoor(float DeltaTime){

	FRotator RotateDoor = GetOwner()->GetActorRotation();
	RotateDoor.Yaw = FMath::Lerp(RotateDoor.Yaw, TargetYaw, DeltaTime * DoorOpenMultiplier);
	GetOwner()->SetActorRotation(RotateDoor);
}

void UOpenDoor::CloseDoor(float DeltaTime){
	FRotator RotateDoor = GetOwner()->GetActorRotation();
	RotateDoor.Yaw = FMath::Lerp(RotateDoor.Yaw, InitialYaw, DeltaTime * DoorCloseMultiplier);
	GetOwner()->SetActorRotation(RotateDoor);
}

