

#include "OpenDoor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#define OUT

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

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor %s has no PressurePlate set"), *GetOwner()->GetName());
	}

	FindAudioComponent();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() > MassToOpenDoor)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
		//DoorLastOpened When the door was opened
	}
	else
	{

		if (DoorLastOpened - GetWorld()->GetTimeSeconds() < DoorCloseDelay)
			CloseDoor(DeltaTime);
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{

	FRotator RotateDoor = GetOwner()->GetActorRotation();
	RotateDoor.Yaw = FMath::Lerp(RotateDoor.Yaw, TargetYaw, DeltaTime * DoorOpenMultiplier);
	GetOwner()->SetActorRotation(RotateDoor);

	if(!IsOpenSoundPlayed){
		if(!AudioComponent) return;
		AudioComponent->Play();
		IsOpenSoundPlayed = true;
	}

}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	FRotator RotateDoor = GetOwner()->GetActorRotation();
	RotateDoor.Yaw = FMath::Lerp(RotateDoor.Yaw, InitialYaw, DeltaTime * DoorCloseMultiplier);
	GetOwner()->SetActorRotation(RotateDoor);
		if(IsOpenSoundPlayed){
		if(!AudioComponent) return;
		AudioComponent->Play();
		IsOpenSoundPlayed = false;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	//Find all Overlapping Actors
	TArray<AActor *> OverlappingActors;
	if(!PressurePlate)
		return 0.f;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for (auto *Actor : OverlappingActors)
	{
		if(!Actor)
			return 0.f;
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	//Add up the masses
	return TotalMass;
}

void UOpenDoor::FindAudioComponent(){
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if(!AudioComponent){
		UE_LOG(LogTemp, Error, TEXT("%s Missing Audio Component!"), *GetOwner()->GetName())
	}
}
