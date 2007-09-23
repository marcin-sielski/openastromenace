/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: EarthSpaceFighter.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#ifndef EARTHSPACEFIGHTER_H
#define EARTHSPACEFIGHTER_H


#include "../SpaceShip.h"



//-----------------------------------------------------------------------------
// Класс CEarthSpaceFighter
//-----------------------------------------------------------------------------
class CEarthSpaceFighter : public CSpaceShip
{
public:

	// не должно быть своего диструктора!, т.к. вызываем только c ~CSpaceShip

	// Создание нужного объекта
	virtual void	Create(int	SpaceShipNum);

};





//-----------------------------------------------------------------------------
// Дополнительные функции для CEarthSpaceFighter
//-----------------------------------------------------------------------------

// Установка системы двигателей
void SetEarthSpaceFighterEngine(CEarthSpaceFighter *SpaceShip, int EngineType);
// Установка брони для кораблей землян
void SetEarthSpaceFighterArmour(CEarthSpaceFighter *SpaceShip, int ArmourType);
// Установка оружия на корабль
bool SetEarthSpaceFighterWeapon(CEarthSpaceFighter *SpaceShip, int WeaponSlot, int WeaponNum);
// Получаем возможный поворот орудия в данном слоте
void GetShipWeaponSlotAngle(int ShipNum, int SlotNum, float *Min, float *Max);



#endif // EARTHSPACEFIGHTER_H