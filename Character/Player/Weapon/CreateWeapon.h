#pragma once
#include "Weapon.h"
#include "WeaponType.h"

#include "Sword\Sword.h"
#include "Bow\Bow.h"
#include "Spear\Spear.h"

static Weapon* Create(WeaponType type)
{
	Weapon* weapon = nullptr;

	switch (type)
	{
	case WeaponType::Sword:
		weapon = new Sword();
		break;

	case WeaponType::Bow:
		weapon = new Bow();
		break;

	case WeaponType::Spear:
		weapon = new Spear();
		break;

	default:
		break;
	}

	return weapon;
}