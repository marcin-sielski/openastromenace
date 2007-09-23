/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Particle2D.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

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


#ifndef PARTICLE2D_H
#define PARTICLE2D_H

#include "../Base.h"
#include "../Math/Math.h"





//-----------------------------------------------------------------------------
// Структура цвета
//-----------------------------------------------------------------------------
struct COLORVALUE2D
{
	float r;
	float g;
	float b;
	float a;
};





//-----------------------------------------------------------------------------
// Класс eParticle2D
//-----------------------------------------------------------------------------
struct eParticle2D
{
	// конструктор и деструктор
	eParticle2D();
	~eParticle2D();

	// обновление информации в частице
	bool Update(float TimeDelta, VECTOR3D ParentLocation = VECTOR3D(0.0f,0.0f,0.0f), bool Attractive = false, float AttractiveValue = 25.0f);
	// текущее место расположения частицы
	VECTOR3D		Location;
	// текущая скорость частицы
	VECTOR3D		Velocity;

	// текущий цвет частицы
	COLORVALUE2D	Color;
	// значение приращение цвета
	COLORVALUE2D	ColorDelta;

	// время жизни частицы в секундах
	float		Age;

	// оставщееся время жизни частицы
	float		Lifetime;

	// размер частицы
	float		Size;
	// значение изменения размера
	float		SizeDelta;

	// прозрачность
	float		Alpha;

	// если нужно замедлять и остановить
	bool	NeedStop;

	// указатели на цепь частиц
	eParticle2D *Next;
	eParticle2D *Prev;
};


#endif // PARTICLE2D_H