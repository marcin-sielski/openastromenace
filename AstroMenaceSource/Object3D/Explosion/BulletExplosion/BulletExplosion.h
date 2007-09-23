/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: BulletExplosion.h
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


#ifndef BULLETEXPLOSION_H
#define BULLETEXPLOSION_H


#include "../Explosion.h"





//-----------------------------------------------------------------------------
// Класс CBulletExplosion
//-----------------------------------------------------------------------------
class CBulletExplosion : public CExplosion
{
public:

	// Создание взрыва из частей объекта
	virtual void	Create(CObject3D *Object, CProjectile *Projectile, int ExplType, VECTOR3D ExplLocation, float Speed, bool NeedExplosionSFX = true);

};








#endif // BULLETEXPLOSION_H