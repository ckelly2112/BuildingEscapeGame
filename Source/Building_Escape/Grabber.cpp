

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT
//Line showing reach of actor
// #define DEBUGLINE
// #define DEBUGLOGS

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	//Check for Physics Handle Component
	FindPhysicsHandle();
	SetupInputComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//If physics handle is attached.
	if (PhysicsHandle->GrabbedComponent)
	{
		//Move the object we are holding
		PhysicsHandle->SetTargetLocation(GetPlayerReach());
	}
	//Get Players Viewpoint
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle =
		GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("No Physics Handle Component on %s found!"), *(GetOwner()->GetName()))
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::Grab()
{
#ifdef DEBUGLOGS
	UE_LOG(LogTemp, Warning, TEXT("Grabber Pressed"));
#endif
	GetPlayerReach();

	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent *ComponentToGrab = HitResult.GetComponent();

	//If we hit something attach the physics handle
	if (HitResult.GetActor())
	{
		if (!PhysicsHandle)
			return;

		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			GetPlayerReach());
	}
}

void UGrabber::Release()
{
#ifdef DEBUGLOGS
	UE_LOG(LogTemp, Warning, TEXT("Grabber Released"));
#endif
	if (!PhysicsHandle)
		return;

	PhysicsHandle->ReleaseComponent();
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
#ifdef DEBUGLINE
	//Draw Line From Player Showing Reach
	DrawDebugLine(
		GetWorld(),
		PlayerViewPointVector,
		LineTraceEnd,
		FColor(0, 255, 0),
		false,
		0.f,
		0,
		5.f);
#endif

	//Ray-Cast out to a certain distance (Reach)
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayerPos(),
		GetPlayerReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams);

//Log out to test
#ifdef DEBUGLOGS
	//See what it hits
	AActor *HitActor = Hit.GetActor();
	if (HitActor)
		UE_LOG(LogTemp, Warning, TEXT("Actor Hit: %s"), *(HitActor->GetName()));
#endif

	return Hit;
}

FVector UGrabber::GetPlayerReach() const
{
	FVector PlayerViewPointVector;
	FRotator PlayerViewPointRotator;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointVector, OUT PlayerViewPointRotator);
	//Get Line end point
	return PlayerViewPointVector + PlayerViewPointRotator.Vector() * Reach;
}

FVector UGrabber::GetPlayerPos() const
{
	FVector PlayerViewPointVector;
	FRotator PlayerViewPointRotator;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointVector, OUT PlayerViewPointRotator);
	return PlayerViewPointVector;
}
