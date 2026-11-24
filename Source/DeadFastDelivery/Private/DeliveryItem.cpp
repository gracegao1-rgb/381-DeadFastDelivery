// Fill out your copyright notice in the Description page of Project Settings.


#include "DeliveryItem.h"

#include "DeadFastDeliveryCharacter.h"
#include "DeadFastDeliveryGameMode.h"
#include "HandsComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADeliveryItem::ADeliveryItem()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeliveryItemMesh"));
	RootComponent = Mesh;
	
	// Get material assets
	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/Game/Materials/M_GlowingAura.M_GlowingAura"));
	if (MaterialAsset.Succeeded())
	{
		GreenGlowMaterial = MaterialAsset.Object;
	}
	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset2(TEXT("/Game/Materials/M_GlowingAuraRed.M_GlowingAuraRed"));
	if (MaterialAsset2.Succeeded())
	{
		RedGlowMaterial = MaterialAsset2.Object;
	}
	
	// Get effect
	ExplosionEffect = CreateDefaultSubobject<UNiagaraComponent>("ExplosionEffect");
	ExplosionEffect->SetupAttachment(Mesh);
	ExplosionEffect->bAutoActivate = false;
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemAsset(
		TEXT("/Game/Particles/NS_Confetti.NS_Confetti")
	);
	
	if (NiagaraSystemAsset.Succeeded())
	{
		ExplosionEffect->SetAsset(NiagaraSystemAsset.Object);
	}
}

// Called when the game starts or when spawned
void ADeliveryItem::BeginPlay()
{
	Super::BeginPlay();
	
	// Setup physics
	if (Mesh)
	{
		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetSimulatePhysics(true);
		Mesh->SetEnableGravity(true);
		Mesh->OnComponentHit.AddDynamic(this, &ADeliveryItem::HandleHit);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetNotifyRigidBodyCollision(true);
	}
	
	// report cost to game mode
	if (ADeadFastDeliveryGameMode* Gm = GetWorld()->GetAuthGameMode<ADeadFastDeliveryGameMode>())
	{
		Gm->ReportMaxCost(Health*CostPerHealth);
	}
}

void ADeliveryItem::HandleStartZoneOverlap(int Zone)
{
	if (Zone == ZoneIdentifier)
	{
		bIsDelivered = true;
		if (Mesh && GreenGlowMaterial)
		{
			Mesh->SetOverlayMaterial(GreenGlowMaterial);
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,TEXT("Object delivered!"));
		
		if (ADeadFastDeliveryGameMode* Gm = GetWorld()->GetAuthGameMode<ADeadFastDeliveryGameMode>())
		{
			Gm->UpdateNumberOfDeliveredItems();
			Gm->ReportCost(CostPerHealth*Health); // add total to score
		}
	}
	else
	{
		if (Mesh && RedGlowMaterial)
		{
			Mesh->SetOverlayMaterial(RedGlowMaterial);
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,TEXT("Object delivered to wrong zone!"));
	}
}

void ADeliveryItem::HandleEndZoneOverlap()
{
	if (ADeadFastDeliveryGameMode* Gm = GetWorld()->GetAuthGameMode<ADeadFastDeliveryGameMode>())
	{
		Gm->UpdateNumberOfDeliveredItems();
		if (bIsDelivered)
		{
			Gm->ReportCost(-CostPerHealth*Health);
		}
	}
	bIsDelivered = false;
	if (Mesh)
	{
		Mesh->SetOverlayMaterial(nullptr);
	}
}


void ADeliveryItem::HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Ignore hitting ourselves, our owner, or hands
	if (OtherActor == this || OtherActor == GetOwner())
		return;
	if (OtherActor && (OtherActor->IsA<ADeadFastDeliveryCharacter>() || OtherActor->IsA<UHandsComponent>()))
		return;

	if (!HasDurability)
		return;
	if (bIsBeingGrabbed) // don't apply damage when being grabbed
		return;
	if (bIsDestroyed)
		return;
	
	UClass* ActorClass = OtherActor->GetClass();
	float ImpactForce = NormalImpulse.Size()/Mesh->GetMass()/100; // Newton Seconds
	if (ImpactForce > DamageThreshold)
	{
		const float HealthLost = FMath::Min(FMath::Min(DamageCeiling, ImpactForce), Health);
		Health -= HealthLost;

		const float CostLost = HealthLost * CostPerHealth;
		if (ADeadFastDeliveryGameMode* Gm = GetWorld()->GetAuthGameMode<ADeadFastDeliveryGameMode>())
		{
			Gm->ReportMaxCost(-CostLost);
			if (bIsDelivered)
			{
				Gm->ReportCost(-CostLost);
			}
		}
		
		if (Health <= 0.0f)
		{
			bIsDestroyed = true;
			HandleDestroy();
		}
		
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,FString::Printf(TEXT("Object hit! %s"), *ActorClass->GetName()));
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,FString::Printf(TEXT("Health Remaining! %f"), Health));
	}
}

void ADeliveryItem::HandleDestroy()
{
	if (ADeadFastDeliveryGameMode* GameMode = Cast<ADeadFastDeliveryGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->IncrementNumberOfDestroyedItems();
	}
	
	Mesh->SetVisibility(false);
	ExplosionEffect->Activate();
	
	if (bIsDelivered)
	{
		bIsDelivered = false;
		if (ADeadFastDeliveryGameMode* Gm = GetWorld()->GetAuthGameMode<ADeadFastDeliveryGameMode>())
		{
			Gm->UpdateNumberOfDeliveredItems();
		}
	}
	
	SetLifeSpan(3.0f);
}

