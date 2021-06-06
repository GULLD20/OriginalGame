#include "../../../pch.h"

#include "WeaponType.h"
#include "Weapon.h"

#include "Sword\Sword.h"
#include "Bow\Bow.h"
#include "Spear\Spear.h"

const float Weapon::BETWEENATTACKSTIME = 0.5f;

Weapon::Weapon()
{
}


Weapon::~Weapon()
{
	if (m_pModel != nullptr)
	{
		m_pModel.reset();
	}
}