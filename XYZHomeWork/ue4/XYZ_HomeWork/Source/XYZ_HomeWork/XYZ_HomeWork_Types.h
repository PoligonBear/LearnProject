#pragma once

/** Trace channels**/
#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_InteractionVolume ECC_GameTraceChannel2
#define ECC_WallRunnable ECC_GameTraceChannel3
#define ECC_Bullet ECC_GameTraceChannel4

/** VFX **/
const FName FXParamTraceEnd = FName("TraceEnd");

/** Sockets **/
const FName SocketFPCamera = FName("CameraSocket");
const FName SocketHead = FName("HeadSocket");
const FName SocketCharacterWeapon = FName("CharacterWeaponSocket");
const FName SocketCharacterThrowable = FName("ThrowableSocket");
const FName SocketWeaponMuzzle = FName("MuzzleSocket");
const FName SocketWeaponForeGrip = FName("ForeGripSocket");

/** Collisions **/
const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");
const FName CollisionProfileRagDoll = FName("RagDoll");

/** Debug categories **/
const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryCharacterAttributes = FName("CharacterAttributes");
const FName DebugCategoryRangeWeapon = FName("RangeWeapon");

const FName MontageSectionReloadEnd = FName("ReloadEnd");

UENUM(BlueprintType)
enum class EEquipableItemType : uint8
{
	None,
	Pistol,
	Rifle,
	Throwable
};

UENUM(BlueprintType)
enum class EAmunitionType : uint8
{
	None,
	Pistol,
	Rifle,
	ShotgunShells,
	FragGrenades,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentSlots : uint8
{
	None,
	SideArm,
	PrimaryWeapon,
	SecondaryWeapon,
	PrimaryItemSlot,
	MAX UMETA(Hidden)
};