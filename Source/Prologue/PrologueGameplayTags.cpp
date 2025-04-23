
#include "PrologueGameplayTags.h"

namespace PrologueGameplayTags
{
	//Input Tags
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Hammer, "InputTag.Attack.Hammer");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Bow, "InputTag.Attack.Bow");

	//Comma Tags
	UE_DEFINE_GAMEPLAY_TAG(Comma_Weapon_Bow, "Comma.Weapon.Bow");
	UE_DEFINE_GAMEPLAY_TAG(Comma_Weapon_Hammer, "Comma.Weapon.Hammer");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Attack_Hammer, "Comma.Ability.Attack.Hammer");
	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Attack_Bow, "Comma.Ability.Attack.Bow");
	
	//State Tags
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_IsDead, "Comma.State.IsDead");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_IsAttacking, "Comma.State.IsAttacking");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_IsMoving, "Comma.State.IsMoving");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Cancel, "Comma.State.Cancel");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_CanCancel, "Comma.State.CanCancel");

	//Effect Tags
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_Damaging, "GameplayCue.Effect.Damaging");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_PlayerHit, "GameplayCue.Effect.PlayerHit");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_EnemyHit, "GameplayCue.Effect.EnemyHit");
}