/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Object3DFunctions.cpp

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


/// подключаем нужные файлы
#include "Object3D.h"





//-----------------------------------------------------------------------------
// Проверяем, нужно ли для данного объекта проверка коллизии и наведение на него
//-----------------------------------------------------------------------------
bool NeedCheckCollision(CObject3D* Object3D)
{
	// по типу
	switch (Object3D->ObjectType)
	{
		// 1 - Earth Fighter
		case 1: return true;

		// 2 - Alien Fighter
		case 2: return true;

		// 3 - Alien MotherShip
		case 3: return true;

		// 4 - Pirate Ship
		case 4: return true;

		// 5 - Pirate Vehicle (Wheeled + Tracked)
		case 5: return true;

		// 6 - Pirate Building
		case 6: return true;

		// 7 - Asteroids
		case 7: return true;

		// 8 - ShipPart
		case 8: return true;

		// 9 - ShipWeapon
		case 9: return true;

		// 10 - Projectile
		case 10: return true;

		// 11 - Explosion
		case 11: return false;

		// 12 - Civilian Building
		case 12: return false;

		// 13 - BasePart
		case 13: return false;

		// 14 - Planet
		case 14: return false;

		// 15 - Big Asteroid
		case 15: return false;

	}

	return false;
}






//-----------------------------------------------------------------------------
// Загрузка в модель нужной геометрии
//-----------------------------------------------------------------------------
void LoadObjectData(const char *Name, CObject3D* Object3D, int ObjectNum)
{
	// получение геометрии модели
	eModel3D  *Model;
	Model = vw_LoadModel3D(Name);


	// т.е. нужны все объекты
	if (ObjectNum == 0)
	{
		// берем то, что нужно
		Object3D->DrawObjectQuantity = Model->DrawObjectCount;
		Object3D->DrawObjectList = new eObjectBlock[Object3D->DrawObjectQuantity];
		// копируем все данные
		memcpy(Object3D->DrawObjectList, Model->DrawObjectList, sizeof(eObjectBlock)*Object3D->DrawObjectQuantity);
	}
	else
	{
		// работаем только с одним объектом ( так работаем с оружием для кораблей землян )

		// берем то, что нужно
		Object3D->DrawObjectQuantity = 1;
		Object3D->DrawObjectList = new eObjectBlock[Object3D->DrawObjectQuantity];
		// копируем данные нужного объекта
		memcpy(Object3D->DrawObjectList, &(Model->DrawObjectList[ObjectNum-1]), sizeof(eObjectBlock));
	}

	// резервируем память для текстур
	Object3D->Texture = new eTexture*[Object3D->DrawObjectQuantity];
	Object3D->TextureIllum = new eTexture*[Object3D->DrawObjectQuantity];

	for (int i=0; i<Object3D->DrawObjectQuantity; i++)
	{
		Object3D->Texture[i] = 0;
		Object3D->TextureIllum[i] = 0;
	}

	// резервируем память для HitBB
	Object3D->HitBBLocation = new VECTOR3D[Object3D->DrawObjectQuantity];
	Object3D->HitBBRadius2 = new float[Object3D->DrawObjectQuantity];
	Object3D->HitBBSize = new VECTOR3D[Object3D->DrawObjectQuantity];
	Object3D->HitBB = new VECTOR3D*[Object3D->DrawObjectQuantity];
	for (int i=0; i<Object3D->DrawObjectQuantity; i++)
	{
		Object3D->HitBB[i] = new VECTOR3D[8];
	}
}










#include "SpaceShip/SpaceShip.h"
#include "GroundObject/GroundObject.h"
#include "Projectile/Projectile.h"
#include "SpaceObject/SpaceObject.h"

extern CSpaceShip *StartSpaceShip;
extern CSpaceShip *EndSpaceShip;
extern CProjectile *StartProjectile;
extern CProjectile *EndProjectile;
extern CGroundObject *StartGroundObject;
extern CGroundObject *EndGroundObject;
extern CSpaceObject *StartSpaceObject;
extern CSpaceObject *EndSpaceObject;
float GetProjectileSpeed(int Num);

// направление движения камеры
extern VECTOR3D	GameCameraMovement;
// скорость движения камеры
float GameCameraGetSpeed();


//-----------------------------------------------------------------------------
// Получение угла поворота оружия на врага для космических кораблей
//-----------------------------------------------------------------------------
void GetShipOnTargetOrientateion(
		int			ObjectStatus, // статус объекта, который целится
		VECTOR3D	Location, // положение точки относительно которой будем наводить
		VECTOR3D	CurrentObjectRotation, // текущие углы объекта
		float		MinDistance, // минимальное расстояние, с которого начинаем прицеливание
		float		RotationMatrix[9], // матрица вращения объекта
		VECTOR3D	*NeedAngle,// нужные углы, чтобы получить нужное направление
		float		Width,		// ширина объекта
		bool		NeedCenterOrientation, // нужен доворот на центр
		bool		NeedByWeaponOrientation, // нужно делать доворот с учетом положения орудия
		VECTOR3D	WeponLocation,
		int         WeaponType) 	// тип орудия орудия
{
	// получаем точки для создания плоскости
	VECTOR3D Orientation(0.0f, 0.0f, 1.0f);
	Matrix33CalcPoint(&Orientation, RotationMatrix);
	VECTOR3D PointUp(0.0f, 1.0f, 0.0f);
	Matrix33CalcPoint(&PointUp, RotationMatrix);
	VECTOR3D PointRight = VECTOR3D(1.0f, 0.0f, 0.0f);
	Matrix33CalcPoint(&PointRight, RotationMatrix);

	// находим плоскость, вертикальную
	float A, B, C, D;
	GetPlaneABCD(&A, &B, &C, &D, Location, Location+Orientation, Location+PointUp);


	// получаем вертикальную плоскость 2 (отсечения перед-зад)
	float A2, B2, C2, D2;
	GetPlaneABCD(&A2, &B2, &C2, &D2, Location, Location+PointRight, Location+PointUp);

	// для выбора - точка, куда целимся + расстояние до нее (квадрат расстояния)
	VECTOR3D TargetLocation = Location;
	VECTOR3D TargetAngle;
	//float TargetAngleYMin = 180.0f;
	float Tdist = 1000.0f*1000.0f;

	// тип, кто заблокировал... чтобы не сбить с активных
	int TType = 0;

	bool TargetLocked = false;

	// нам нужна только половина ширины
	float Width2 = Width/2.0f;


	CSpaceShip *tmp = StartSpaceShip;
	while (tmp!=0)
	{
		CSpaceShip *tmpShip2 = tmp->Next;
		// проверка, чтобы не считать свой корабль
		if (tmp->ID != 111111)
		// если по этому надо стрелять
		if (NeedCheckCollision(tmp))
		// выбираем врага, по своим не целимся
		if ((ObjectStatus == 1 && tmp->ObjectStatus>1) ||
			(ObjectStatus > 1 && tmp->ObjectStatus==1))

		{
		    // находим настоящую точку попадания с учетом скорости объекта и пули... если надо
		    VECTOR3D RealLocation = tmp->Location + (tmp->AABB[0]+tmp->AABB[6])/2.0f;

		    if (tmp->Speed != 0.0f)
		    if (WeaponType != 0 &&
                // это не лучевое оружие, которое бьет сразу
                WeaponType != 11 && WeaponType != 12 && WeaponType != 14 &&
                // это не ракеты...
                WeaponType != 16 && WeaponType != 17 && WeaponType != 18 && WeaponType != 19)
		    {

		        // находим, за какое время снаряд долетит до объекта сейчас
				VECTOR3D TTT = WeponLocation-RealLocation;
		        float ProjectileSpeed = GetProjectileSpeed(WeaponType);
		        float CurrentDist = TTT.Length();
		        float ObjCurrentTime = CurrentDist/ProjectileSpeed;

                // находим где будет объект, когда пройдет это время
                VECTOR3D FutureLocation = tmp->Orientation^(tmp->Speed*ObjCurrentTime);

                // находи точку по середине... это нам и надо... туда целимся...
                RealLocation = RealLocation + FutureLocation;
		    }



			// проверяем, если с одной стороны все точки - значит мимо, если нет - попали :)
			// + учитываем тут Width
			float tmp1 = A * (RealLocation.x+tmp->OBB[0].x)  + B * (RealLocation.y+tmp->OBB[0].y)  + C * (RealLocation.z+tmp->OBB[0].z)  + D;
			float tmp2 = A * (RealLocation.x+tmp->OBB[1].x)  + B * (RealLocation.y+tmp->OBB[1].y)  + C * (RealLocation.z+tmp->OBB[1].z)  + D;
			float tmp3 = A * (RealLocation.x+tmp->OBB[2].x)  + B * (RealLocation.y+tmp->OBB[2].y)  + C * (RealLocation.z+tmp->OBB[2].z)  + D;
			float tmp4 = A * (RealLocation.x+tmp->OBB[3].x)  + B * (RealLocation.y+tmp->OBB[3].y)  + C * (RealLocation.z+tmp->OBB[3].z)  + D;
			float tmp5 = A * (RealLocation.x+tmp->OBB[4].x)  + B * (RealLocation.y+tmp->OBB[4].y)  + C * (RealLocation.z+tmp->OBB[4].z)  + D;
			float tmp6 = A * (RealLocation.x+tmp->OBB[5].x)  + B * (RealLocation.y+tmp->OBB[5].y)  + C * (RealLocation.z+tmp->OBB[5].z)  + D;
			float tmp7 = A * (RealLocation.x+tmp->OBB[6].x)  + B * (RealLocation.y+tmp->OBB[6].y)  + C * (RealLocation.z+tmp->OBB[6].z)  + D;
			float tmp8 = A * (RealLocation.x+tmp->OBB[7].x)  + B * (RealLocation.y+tmp->OBB[7].y)  + C * (RealLocation.z+tmp->OBB[7].z)  + D;


			if (!((tmp1>Width2 && tmp2>Width2 && tmp3>Width2 && tmp4>Width2 && tmp5>Width2 &&
				tmp6>Width2 && tmp7>Width2 && tmp8>Width2) ||
				(tmp1<-Width2 && tmp2<-Width2 && tmp3<-Width2 && tmp4<-Width2 && tmp5<-Width2 &&
				tmp6<-Width2 && tmp7<-Width2 && tmp8<-Width2)))
			{
				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (RealLocation.x+tmp->OBB[0].x)  + B2 * (RealLocation.y+tmp->OBB[0].y)  + C2 * (RealLocation.z+tmp->OBB[0].z)  + D2;
				tmp2 = A2 * (RealLocation.x+tmp->OBB[1].x)  + B2 * (RealLocation.y+tmp->OBB[1].y)  + C2 * (RealLocation.z+tmp->OBB[1].z)  + D2;
				tmp3 = A2 * (RealLocation.x+tmp->OBB[2].x)  + B2 * (RealLocation.y+tmp->OBB[2].y)  + C2 * (RealLocation.z+tmp->OBB[2].z)  + D2;
				tmp4 = A2 * (RealLocation.x+tmp->OBB[3].x)  + B2 * (RealLocation.y+tmp->OBB[3].y)  + C2 * (RealLocation.z+tmp->OBB[3].z)  + D2;
				tmp5 = A2 * (RealLocation.x+tmp->OBB[4].x)  + B2 * (RealLocation.y+tmp->OBB[4].y)  + C2 * (RealLocation.z+tmp->OBB[4].z)  + D2;
				tmp6 = A2 * (RealLocation.x+tmp->OBB[5].x)  + B2 * (RealLocation.y+tmp->OBB[5].y)  + C2 * (RealLocation.z+tmp->OBB[5].z)  + D2;
				tmp7 = A2 * (RealLocation.x+tmp->OBB[6].x)  + B2 * (RealLocation.y+tmp->OBB[6].y)  + C2 * (RealLocation.z+tmp->OBB[6].z)  + D2;
				tmp8 = A2 * (RealLocation.x+tmp->OBB[7].x)  + B2 * (RealLocation.y+tmp->OBB[7].y)  + C2 * (RealLocation.z+tmp->OBB[7].z)  + D2;

				if (tmp1>0.0f && tmp2>0.0f && tmp3>0.0f && tmp4>0.0f && tmp5>0.0f &&
					tmp6>0.0f && tmp7>0.0f && tmp8>0.0f)
				{

					float TargetDist2TMP = A2 * (RealLocation.x)  + B2 * (RealLocation.y)  + C2 * (RealLocation.z)  + D2;

					// проверяем, чтобы объект находился не ближе чем MinDistance
					if (MinDistance<TargetDist2TMP)
					{
						// выбираем объект, так, чтобы он был наиболее длижайшим,
						// идущим по нашему курсу...

						VECTOR3D TargetAngleTMP;
						TargetLocation = RealLocation;

						// находим угол между плоскостью и прямой
						float A3, B3, C3, D3;
						GetPlaneABCD(&A3, &B3, &C3, &D3, Location, Location+Orientation, Location+PointRight);

						float m = TargetLocation.x-Location.x;
						float n = TargetLocation.y-Location.y;
						float p = TargetLocation.z-Location.z;
						if (NeedByWeaponOrientation)
						{
							m = TargetLocation.x-WeponLocation.x;
							n = TargetLocation.y-WeponLocation.y;
							p = TargetLocation.z-WeponLocation.z;
						}

						// поправки к существующим углам поворота оружия
/*
						if (vw_sqrtf(m*m+n*n+p*p) != 0 && vw_sqrtf(A3*A3+B3*B3+C3*C3) != 0)
							TargetAngleTMP.x = CurrentObjectRotation.x + asinf((A3*m+B3*n+C3*p)/(vw_sqrtf(m*m+n*n+p*p) * vw_sqrtf(A3*A3+B3*B3+C3*C3))) * 57.32f;
						else
							TargetAngleTMP.x = CurrentObjectRotation.x;

						// если нужно наведение в цетнр врага...
						if (!NeedCenterOrientation)
							TargetAngleTMP.y = CurrentObjectRotation.y;
						else
							if (vw_sqrtf(m*m+n*n+p*p) != 0 && vw_sqrtf(A*A+B*B+C*C) != 0)
								TargetAngleTMP.y = CurrentObjectRotation.y + asinf((A*m+B*n+C*p)/(vw_sqrtf(m*m+n*n+p*p) * vw_sqrtf(A*A+B*B+C*C))) * 57.32f;
							else
								TargetAngleTMP.y = CurrentObjectRotation.y;
						TargetAngleTMP.z = CurrentObjectRotation.z;
*/

						// поправки к существующим углам поворота оружия
                        float sss1 = vw_sqrtf(m*m+n*n+p*p);
                        float sss2 = vw_sqrtf(A3*A3+B3*B3+C3*C3);
                        TargetAngleTMP.x = CurrentObjectRotation.x;
						if (sss1 != 0 && sss2 != 0)
						{
						    float sss3 = (A3*m+B3*n+C3*p)/(sss1 * sss2);
						    if (sss3 >= -1.0f && sss3 <= 1.0f)
                                TargetAngleTMP.x = CurrentObjectRotation.x + asinf(sss3) * 57.32f;
						}

						float sss4 = vw_sqrtf(A*A+B*B+C*C);
						TargetAngleTMP.y = CurrentObjectRotation.y;
						if (NeedCenterOrientation)
						if (sss1 != 0 && sss4 != 0)
						{
						    float sss5 = (A*m+B*n+C*p)/(sss1 * sss4);
						    if (sss5 >= -1.0f && sss5 <= 1.0f)
                                TargetAngleTMP.y = CurrentObjectRotation.y + asinf(sss5) * 57.32f;
						}

						TargetAngleTMP.z = CurrentObjectRotation.z;


					/*	if (!TargetLocked)
						{
							TargetAngle = TargetAngleTMP;
							TargetLocked = true;
						}
						else
						{
							VECTOR3D TMP1 = TargetAngleTMP - CurrentObjectRotation;
							VECTOR3D TMP2 = TargetAngle - CurrentObjectRotation;
							if (TMP1.Length() < TargetAngle.Length())*/
	/// !!!! ставим по высоте в 5 раза!!!
							//if (TargetAngleYMin > fabsf(TargetAngleTMP.y - NeedAngle->y) || TargetAngleYMin == 180.0f)
							{
								if (Tdist > m*m+n*n*5+p*p && fabsf(TargetAngleTMP.x)<45.0f)
								{

									//TargetAngleYMin = fabsf(TargetAngleTMP.y - NeedAngle->y);

									TargetAngle = TargetAngleTMP;
									Tdist = m*m+n*n*5+p*p;
									TargetLocked = true;
									TType = 1;
								}

							}
					//	}
					}
				}
			}
		}

		tmp = tmpShip2;
	}






	// проверка по наземным объектам
	// не стрелять по "мирным" постойкам
	// !!! ВАЖНО
	// у всех наземных объектов ноль на уровне пола...
	CGroundObject *tmpG = StartGroundObject;
	while (tmpG!=0)
	{
		CGroundObject *tmpGround2 = tmpG->Next;

		// если по этому надо стрелять
		if (NeedCheckCollision(tmpG))
		// выбираем врага, по своим не целимся
		if ((ObjectStatus == 1 && tmpG->ObjectStatus>1) ||
			(ObjectStatus > 1 && tmpG->ObjectStatus==1))
		{

		    // из-за длинного ствола, точка может быть вынесена далеко, и тогда ракета промажет
			// если делать через (tmpG->AABB[0]+tmpG->AABB[6])/2.0f;
			VECTOR3D tmp(0.0f, tmpG->Height/2.0f, 0.0f);
			Matrix33CalcPoint(&tmp, tmpG->CurrentRotationMat); // поворачиваем в плоскость объекта
			VECTOR3D RealLocation = tmpG->Location+tmp;

		    if (tmpG->Speed != 0.0f)
		    if (WeaponType != 0 &&
                // это не лучевое оружие, которое бьет сразу
                WeaponType != 11 && WeaponType != 12 && WeaponType != 14 &&
                // это не ракеты...
                WeaponType != 16 && WeaponType != 17 && WeaponType != 18 && WeaponType != 19)
		    {

		        // находим, за какое время снаряд долетит до объекта сейчас
		        VECTOR3D TTT = WeponLocation-RealLocation;
		        float ProjectileSpeed = GetProjectileSpeed(WeaponType);
		        float CurrentDist = TTT.Length();
		        float ObjCurrentTime = CurrentDist/ProjectileSpeed;

                // находим где будет объект, когда пройдет это время (+ сразу половину считаем!)
                VECTOR3D FutureLocation = tmpG->Orientation^(tmpG->Speed*ObjCurrentTime);

                // находи точку по середине... это нам и надо... туда целимся...
                RealLocation = RealLocation + FutureLocation;

		    }


			// проверяем, если с одной стороны все точки - значит мимо, если нет - попали :)
			float tmp1 = A * (tmpG->Location.x+tmpG->OBB[0].x)  + B * (tmpG->Location.y+tmpG->OBB[0].y)  + C * (tmpG->Location.z+tmpG->OBB[0].z)  + D;
			float tmp2 = A * (tmpG->Location.x+tmpG->OBB[1].x)  + B * (tmpG->Location.y+tmpG->OBB[1].y)  + C * (tmpG->Location.z+tmpG->OBB[1].z)  + D;
			float tmp3 = A * (tmpG->Location.x+tmpG->OBB[2].x)  + B * (tmpG->Location.y+tmpG->OBB[2].y)  + C * (tmpG->Location.z+tmpG->OBB[2].z)  + D;
			float tmp4 = A * (tmpG->Location.x+tmpG->OBB[3].x)  + B * (tmpG->Location.y+tmpG->OBB[3].y)  + C * (tmpG->Location.z+tmpG->OBB[3].z)  + D;
			float tmp5 = A * (tmpG->Location.x+tmpG->OBB[4].x)  + B * (tmpG->Location.y+tmpG->OBB[4].y)  + C * (tmpG->Location.z+tmpG->OBB[4].z)  + D;
			float tmp6 = A * (tmpG->Location.x+tmpG->OBB[5].x)  + B * (tmpG->Location.y+tmpG->OBB[5].y)  + C * (tmpG->Location.z+tmpG->OBB[5].z)  + D;
			float tmp7 = A * (tmpG->Location.x+tmpG->OBB[6].x)  + B * (tmpG->Location.y+tmpG->OBB[6].y)  + C * (tmpG->Location.z+tmpG->OBB[6].z)  + D;
			float tmp8 = A * (tmpG->Location.x+tmpG->OBB[7].x)  + B * (tmpG->Location.y+tmpG->OBB[7].y)  + C * (tmpG->Location.z+tmpG->OBB[7].z)  + D;

			if (!((tmp1>Width2 && tmp2>Width2 && tmp3>Width2 && tmp4>Width2 && tmp5>Width2 &&
				tmp6>Width2 && tmp7>Width2 && tmp8>Width2) ||
				(tmp1<-Width2 && tmp2<-Width2 && tmp3<-Width2 && tmp4<-Width2 && tmp5<-Width2 &&
				tmp6<-Width2 && tmp7<-Width2 && tmp8<-Width2)))
			{
				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (tmpG->Location.x+tmpG->OBB[0].x)  + B2 * (tmpG->Location.y+tmpG->OBB[0].y)  + C2 * (tmpG->Location.z+tmpG->OBB[0].z)  + D2;
				tmp2 = A2 * (tmpG->Location.x+tmpG->OBB[1].x)  + B2 * (tmpG->Location.y+tmpG->OBB[1].y)  + C2 * (tmpG->Location.z+tmpG->OBB[1].z)  + D2;
				tmp3 = A2 * (tmpG->Location.x+tmpG->OBB[2].x)  + B2 * (tmpG->Location.y+tmpG->OBB[2].y)  + C2 * (tmpG->Location.z+tmpG->OBB[2].z)  + D2;
				tmp4 = A2 * (tmpG->Location.x+tmpG->OBB[3].x)  + B2 * (tmpG->Location.y+tmpG->OBB[3].y)  + C2 * (tmpG->Location.z+tmpG->OBB[3].z)  + D2;
				tmp5 = A2 * (tmpG->Location.x+tmpG->OBB[4].x)  + B2 * (tmpG->Location.y+tmpG->OBB[4].y)  + C2 * (tmpG->Location.z+tmpG->OBB[4].z)  + D2;
				tmp6 = A2 * (tmpG->Location.x+tmpG->OBB[5].x)  + B2 * (tmpG->Location.y+tmpG->OBB[5].y)  + C2 * (tmpG->Location.z+tmpG->OBB[5].z)  + D2;
				tmp7 = A2 * (tmpG->Location.x+tmpG->OBB[6].x)  + B2 * (tmpG->Location.y+tmpG->OBB[6].y)  + C2 * (tmpG->Location.z+tmpG->OBB[6].z)  + D2;
				tmp8 = A2 * (tmpG->Location.x+tmpG->OBB[7].x)  + B2 * (tmpG->Location.y+tmpG->OBB[7].y)  + C2 * (tmpG->Location.z+tmpG->OBB[7].z)  + D2;

				if (tmp1>0.0f && tmp2>0.0f && tmp3>0.0f && tmp4>0.0f && tmp5>0.0f &&
					tmp6>0.0f && tmp7>0.0f && tmp8>0.0f)
				{

					float TargetDist2TMP = A2 * (RealLocation.x)  + B2 * (RealLocation.y)  + C2 * (RealLocation.z)  + D2;

					// проверяем, чтобы объект находился не ближе чем MinDistance
					if (MinDistance<TargetDist2TMP)
					{
						// выбираем объект, так, чтобы он был наиболее длижайшим,
						// идущим по нашему курсу...
						VECTOR3D TargetAngleTMP;
						TargetLocation = RealLocation;

						// находим угол между плоскостью и прямой
						float A3, B3, C3, D3;
						GetPlaneABCD(&A3, &B3, &C3, &D3, Location, Location+Orientation, Location+PointRight);

						float m = TargetLocation.x-Location.x;
						float n = TargetLocation.y-Location.y;
						float p = TargetLocation.z-Location.z;
						if (NeedByWeaponOrientation)
						{
							m = TargetLocation.x-WeponLocation.x;
							n = TargetLocation.y-WeponLocation.y;
							p = TargetLocation.z-WeponLocation.z;
						}

						// поправки к существующим углам поворота оружия
/*
						if (vw_sqrtf(m*m+n*n+p*p) != 0 && vw_sqrtf(A3*A3+B3*B3+C3*C3) != 0)
							TargetAngleTMP.x = CurrentObjectRotation.x + asinf((A3*m+B3*n+C3*p)/(vw_sqrtf(m*m+n*n+p*p) * vw_sqrtf(A3*A3+B3*B3+C3*C3))) * 57.32f;
						else
							TargetAngleTMP.x = CurrentObjectRotation.x;

						// если нужно наведение в цетнр врага...
						if (!NeedCenterOrientation)
							TargetAngleTMP.y = CurrentObjectRotation.y;
						else
							if (vw_sqrtf(m*m+n*n+p*p) != 0 && vw_sqrtf(A*A+B*B+C*C) != 0)
								TargetAngleTMP.y = CurrentObjectRotation.y + asinf((A*m+B*n+C*p)/(vw_sqrtf(m*m+n*n+p*p) * vw_sqrtf(A*A+B*B+C*C))) * 57.32f;
							else
								TargetAngleTMP.y = CurrentObjectRotation.y;

						TargetAngleTMP.z = CurrentObjectRotation.z;
*/


						// поправки к существующим углам поворота оружия
                        float sss1 = vw_sqrtf(m*m+n*n+p*p);
                        float sss2 = vw_sqrtf(A3*A3+B3*B3+C3*C3);
                        TargetAngleTMP.x = CurrentObjectRotation.x;
						if (sss1 != 0 && sss2 != 0)
						{
						    float sss3 = (A3*m+B3*n+C3*p)/(sss1 * sss2);
						    if (sss3 >= -1.0f && sss3 <= 1.0f)
                                TargetAngleTMP.x = CurrentObjectRotation.x + asinf(sss3) * 57.32f;
						}

						float sss4 = vw_sqrtf(A*A+B*B+C*C);
						TargetAngleTMP.y = CurrentObjectRotation.y;
						if (NeedCenterOrientation)
						if (sss1 != 0 && sss4 != 0)
						{
						    float sss5 = (A*m+B*n+C*p)/(sss1 * sss4);
						    if (sss5 >= -1.0f && sss5 <= 1.0f)
                                TargetAngleTMP.y = CurrentObjectRotation.y + asinf(sss5) * 57.32f;
						}

						TargetAngleTMP.z = CurrentObjectRotation.z;



					/*	if (!TargetLocked)
						{
							TargetAngle = TargetAngleTMP;
							TargetLocked = true;
						}
						else
						{
							VECTOR3D TMP1 = TargetAngleTMP - CurrentObjectRotation;
							VECTOR3D TMP2 = TargetAngle - CurrentObjectRotation;
							if (TMP1.Length() < TargetAngle.Length())*/
							if (TType < 2 && TargetLocked)
							{
								//if (TargetAngleYMin > fabsf(TargetAngleTMP.y - NeedAngle->y) || TargetAngleYMin == 180.0f)
								{
									// только если в 5 раза ближе
									if (Tdist > m*m+n*n*5+p*p && fabsf(TargetAngleTMP.x)<45.0f)
									{
									//	TargetAngleYMin = fabsf(TargetAngleTMP.y - NeedAngle->y);

										TargetAngle = TargetAngleTMP;
										Tdist = m*m+n*n+p*p;
										TargetLocked = true;
										TType = 2;
									}
                                }
							}
							else
								//if (TargetAngleYMin > fabsf(TargetAngleTMP.y - NeedAngle->y) || TargetAngleYMin == 180.0f)
								{
									if (Tdist > m*m+n*n+p*p && fabsf(TargetAngleTMP.x)<45.0f)
									{

									//	TargetAngleYMin = fabsf(TargetAngleTMP.y - NeedAngle->y);

										TargetAngle = TargetAngleTMP;
										Tdist = m*m+n*n+p*p;
										TargetLocked = true;
										TType = 2;
									}
                                }
						//}
					}
				}
			}
		}

		tmpG = tmpGround2;
	}





	// проверка по космическим объектам
	CSpaceObject *tmpS = StartSpaceObject;
	while (tmpS!=0)
	{
		CSpaceObject *tmpSpace2 = tmpS->Next;

		// если по этому надо стрелять
		if (NeedCheckCollision(tmpS))
		// выбираем врага, по своим не целимся
		if ((ObjectStatus == 1 && tmpS->ObjectStatus>1) ||
			(ObjectStatus > 1 && tmpS->ObjectStatus==1))
		{


		    // находим настоящую точку попадания с учетом скорости объекта и пули... если надо
		    VECTOR3D RealLocation = tmpS->Location + (tmpS->AABB[0]+tmpS->AABB[6])/2.0f;

		    // если нужно проверить
		    if (tmpS->Speed != 0.0f)
		    if (WeaponType != 0 &&
                // это не лучевое оружие, которое бьет сразу
                WeaponType != 11 && WeaponType != 12 && WeaponType != 14 &&
                // это не ракеты...
                WeaponType != 16 && WeaponType != 17 && WeaponType != 18 && WeaponType != 19)
		    {

		        // находим, за какое время снаряд долетит до объекта сейчас
		        VECTOR3D TTT = WeponLocation-RealLocation;
		        float ProjectileSpeed = GetProjectileSpeed(WeaponType);
		        float CurrentDist = TTT.Length();
		        float ObjCurrentTime = CurrentDist/ProjectileSpeed;

                // находим где будет объект, когда пройдет это время (+ сразу половину считаем!)
                VECTOR3D FutureLocation = tmpS->Orientation^(tmpS->Speed*ObjCurrentTime);

                // находи точку по середине... это нам и надо... туда целимся...
                RealLocation = RealLocation + FutureLocation;

		    }



			// проверяем, если с одной стороны все точки - значит мимо, если нет - попали :)
			float tmp1 = A * (RealLocation.x+tmpS->OBB[0].x)  + B * (RealLocation.y+tmpS->OBB[0].y)  + C * (RealLocation.z+tmpS->OBB[0].z)  + D;
			float tmp2 = A * (RealLocation.x+tmpS->OBB[1].x)  + B * (RealLocation.y+tmpS->OBB[1].y)  + C * (RealLocation.z+tmpS->OBB[1].z)  + D;
			float tmp3 = A * (RealLocation.x+tmpS->OBB[2].x)  + B * (RealLocation.y+tmpS->OBB[2].y)  + C * (RealLocation.z+tmpS->OBB[2].z)  + D;
			float tmp4 = A * (RealLocation.x+tmpS->OBB[3].x)  + B * (RealLocation.y+tmpS->OBB[3].y)  + C * (RealLocation.z+tmpS->OBB[3].z)  + D;
			float tmp5 = A * (RealLocation.x+tmpS->OBB[4].x)  + B * (RealLocation.y+tmpS->OBB[4].y)  + C * (RealLocation.z+tmpS->OBB[4].z)  + D;
			float tmp6 = A * (RealLocation.x+tmpS->OBB[5].x)  + B * (RealLocation.y+tmpS->OBB[5].y)  + C * (RealLocation.z+tmpS->OBB[5].z)  + D;
			float tmp7 = A * (RealLocation.x+tmpS->OBB[6].x)  + B * (RealLocation.y+tmpS->OBB[6].y)  + C * (RealLocation.z+tmpS->OBB[6].z)  + D;
			float tmp8 = A * (RealLocation.x+tmpS->OBB[7].x)  + B * (RealLocation.y+tmpS->OBB[7].y)  + C * (RealLocation.z+tmpS->OBB[7].z)  + D;

			if (!((tmp1>Width2 && tmp2>Width2 && tmp3>Width2 && tmp4>Width2 && tmp5>Width2 &&
				tmp6>Width2 && tmp7>Width2 && tmp8>Width2) ||
				(tmp1<-Width2 && tmp2<-Width2 && tmp3<-Width2 && tmp4<-Width2 && tmp5<-Width2 &&
				tmp6<-Width2 && tmp7<-Width2 && tmp8<-Width2)))
			{
				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (RealLocation.x+tmpS->OBB[0].x)  + B2 * (RealLocation.y+tmpS->OBB[0].y)  + C2 * (RealLocation.z+tmpS->OBB[0].z)  + D2;
				tmp2 = A2 * (RealLocation.x+tmpS->OBB[1].x)  + B2 * (RealLocation.y+tmpS->OBB[1].y)  + C2 * (RealLocation.z+tmpS->OBB[1].z)  + D2;
				tmp3 = A2 * (RealLocation.x+tmpS->OBB[2].x)  + B2 * (RealLocation.y+tmpS->OBB[2].y)  + C2 * (RealLocation.z+tmpS->OBB[2].z)  + D2;
				tmp4 = A2 * (RealLocation.x+tmpS->OBB[3].x)  + B2 * (RealLocation.y+tmpS->OBB[3].y)  + C2 * (RealLocation.z+tmpS->OBB[3].z)  + D2;
				tmp5 = A2 * (RealLocation.x+tmpS->OBB[4].x)  + B2 * (RealLocation.y+tmpS->OBB[4].y)  + C2 * (RealLocation.z+tmpS->OBB[4].z)  + D2;
				tmp6 = A2 * (RealLocation.x+tmpS->OBB[5].x)  + B2 * (RealLocation.y+tmpS->OBB[5].y)  + C2 * (RealLocation.z+tmpS->OBB[5].z)  + D2;
				tmp7 = A2 * (RealLocation.x+tmpS->OBB[6].x)  + B2 * (RealLocation.y+tmpS->OBB[6].y)  + C2 * (RealLocation.z+tmpS->OBB[6].z)  + D2;
				tmp8 = A2 * (RealLocation.x+tmpS->OBB[7].x)  + B2 * (RealLocation.y+tmpS->OBB[7].y)  + C2 * (RealLocation.z+tmpS->OBB[7].z)  + D2;

				if (tmp1>0.0f && tmp2>0.0f && tmp3>0.0f && tmp4>0.0f && tmp5>0.0f &&
					tmp6>0.0f && tmp7>0.0f && tmp8>0.0f)
				{

					float TargetDist2TMP = A2 * (RealLocation.x)  + B2 * (RealLocation.y)  + C2 * (RealLocation.z)  + D2;

					// проверяем, чтобы объект находился не ближе чем MinDistance
					if (MinDistance<TargetDist2TMP)
					{
						// выбираем объект, так, чтобы он был наиболее длижайшим,
						// идущим по нашему курсу...
						VECTOR3D TargetAngleTMP;
						TargetLocation = RealLocation;

						// находим угол между плоскостью и прямой
						float A3, B3, C3, D3;
						GetPlaneABCD(&A3, &B3, &C3, &D3, Location, Location+Orientation, Location+PointRight);

						float m = TargetLocation.x-Location.x;
						float n = TargetLocation.y-Location.y;
						float p = TargetLocation.z-Location.z;
						if (NeedByWeaponOrientation)
						{
							m = TargetLocation.x-WeponLocation.x;
							n = TargetLocation.y-WeponLocation.y;
							p = TargetLocation.z-WeponLocation.z;
						}

						// поправки к существующим углам поворота оружия
/*
						if (vw_sqrtf(m*m+n*n+p*p) != 0 && vw_sqrtf(A3*A3+B3*B3+C3*C3) != 0)
							TargetAngleTMP.x = CurrentObjectRotation.x + asinf((A3*m+B3*n+C3*p)/(vw_sqrtf(m*m+n*n+p*p) * vw_sqrtf(A3*A3+B3*B3+C3*C3))) * 57.32f;
						else
							TargetAngleTMP.x = CurrentObjectRotation.x;

						// если нужно наведение в цетнр врага...
						if (!NeedCenterOrientation)
							TargetAngleTMP.y = CurrentObjectRotation.y;
						else
							if (vw_sqrtf(m*m+n*n+p*p) != 0 && vw_sqrtf(A*A+B*B+C*C) != 0)
								TargetAngleTMP.y = CurrentObjectRotation.y + asinf((A*m+B*n+C*p)/(vw_sqrtf(m*m+n*n+p*p) * vw_sqrtf(A*A+B*B+C*C))) * 57.32f;
							else
								TargetAngleTMP.y = CurrentObjectRotation.y;

						TargetAngleTMP.z = CurrentObjectRotation.z;
*/


						// поправки к существующим углам поворота оружия
                        float sss1 = vw_sqrtf(m*m+n*n+p*p);
                        float sss2 = vw_sqrtf(A3*A3+B3*B3+C3*C3);
                        TargetAngleTMP.x = CurrentObjectRotation.x;
						if (sss1 != 0 && sss2 != 0)
						{
						    float sss3 = (A3*m+B3*n+C3*p)/(sss1 * sss2);
						    if (sss3 >= -1.0f && sss3 <= 1.0f)
                                TargetAngleTMP.x = CurrentObjectRotation.x + asinf(sss3) * 57.32f;
						}

						float sss4 = vw_sqrtf(A*A+B*B+C*C);
						TargetAngleTMP.y = CurrentObjectRotation.y;
						if (NeedCenterOrientation)
						if (sss1 != 0 && sss4 != 0)
						{
						    float sss5 = (A*m+B*n+C*p)/(sss1 * sss4);
						    if (sss5 >= -1.0f && sss5 <= 1.0f)
                                TargetAngleTMP.y = CurrentObjectRotation.y + asinf(sss5) * 57.32f;
						}

						TargetAngleTMP.z = CurrentObjectRotation.z;



					/*	if (!TargetLocked)
						{
							TargetAngle = TargetAngleTMP;
							TargetLocked = true;
						}
						else
						{
							VECTOR3D TMP1 = TargetAngleTMP - CurrentObjectRotation;
							VECTOR3D TMP2 = TargetAngle - CurrentObjectRotation;
							if (TMP1.Length() < TargetAngle.Length())*/
							if (TType < 3 && TargetLocked)
							{
								//if (TargetAngleYMin > fabsf(TargetAngleTMP.y - NeedAngle->y) || TargetAngleYMin == 180.0f)
								{
									// только если в 10 раза ближе
									if (Tdist/10.0f > m*m+n*n+p*p && fabsf(TargetAngleTMP.x)<45.0f)
									{
									//	TargetAngleYMin = fabsf(TargetAngleTMP.y - NeedAngle->y);

										TargetAngle = TargetAngleTMP;
										Tdist = m*m+n*n+p*p;
										TargetLocked = true;
										TType = 3;
									}
                                }
							}
							else
								//if (TargetAngleYMin > fabsf(TargetAngleTMP.y - NeedAngle->y) || TargetAngleYMin == 180.0f)
								{
									if (Tdist > m*m+n*n+p*p && fabsf(TargetAngleTMP.x)<45.0f)
									{

									//	TargetAngleYMin = fabsf(TargetAngleTMP.y - NeedAngle->y);

										TargetAngle = TargetAngleTMP;
										Tdist = m*m+n*n+p*p;
										TargetLocked = true;
										TType = 3;
									}
                                }
						//}
					}
				}
			}
		}

		tmpS = tmpSpace2;
	}




	// находим направление и углы нацеливания на цель, если нужно
	if (TargetLocked)
	{
		(*NeedAngle) = TargetAngle;
	}


}











//-----------------------------------------------------------------------------
// Получение угла поворота оружия на врага для космических кораблей противника
// !! почти полная копия как наведение на у турелей, но без учета выше-ниже противника
//-----------------------------------------------------------------------------
void GetEnemyShipOnTargetOrientateion(
		int			ObjectStatus, // статус объекта, который целится
		VECTOR3D	Location, // положение точки относительно которой будем наводить
		VECTOR3D	CurrentObjectRotation, // текущие углы объекта
		float		RotationMatrix[9], // матрица вращения объекта
		VECTOR3D	*NeedAngle, // нужные углы, чтобы получить нужное направление
		int			WeaponType, // номер оружия
		VECTOR3D	FirePos) // положение оружия
{


	// получаем точки для создания плоскости
	VECTOR3D Orientation(0.0f, 0.0f, 1.0f);
	Matrix33CalcPoint(&Orientation, RotationMatrix);
	VECTOR3D PointRight(1.0f, 0.0f, 0.0f);
	Matrix33CalcPoint(&PointRight, RotationMatrix);
	VECTOR3D PointUp(0.0f, 1.0f, 0.0f);
	Matrix33CalcPoint(&PointUp, RotationMatrix);

	// находим плоскость, горизонтальную
	float A, B, C, D;
	GetPlaneABCD(&A, &B, &C, &D, Location, Location+Orientation, Location+PointRight);


	// для выбора - точка, куда целимся + расстояние до нее (квадрат расстояния)
	VECTOR3D TargetLocation = Location;
	float TargetDist2 = 1000000.0f;
	bool TargetLocked = false;




	CSpaceShip *tmp = StartSpaceShip;
	while (tmp!=0)
	{
		CSpaceShip *tmpShip2 = tmp->Next;
		// если по этому надо стрелять
		if (NeedCheckCollision(tmp))
		// выбираем врага, по своим не целимся
		if ((ObjectStatus == 1 && tmp->ObjectStatus>1) ||
			(ObjectStatus > 1 && tmp->ObjectStatus==1))
		{

			// находим настоящую точку попадания с учетом скорости объекта и пули... если надо
		    VECTOR3D RealLocation = tmp->Location + (tmp->AABB[0]+tmp->AABB[6])/2.0f;

			// учитываем, если лазер - наводить не надо
			if (WeaponType != 110)
			{
                // находим, за какое время снаряд долетит до объекта сейчас
				VECTOR3D TTT = Location - RealLocation;
		        float ProjectileSpeed = GetProjectileSpeed(WeaponType);
				if (ObjectStatus!=3) ProjectileSpeed = ProjectileSpeed/GameNPCWeaponPenalty;
		        float CurrentDist = TTT.Length();
		        float ObjCurrentTime = CurrentDist/ProjectileSpeed;

                // находим где будет объект, когда пройдет это время (+ сразу половину считаем!)
                VECTOR3D FutureLocation = tmp->Orientation^(tmp->Speed*ObjCurrentTime);
                // учитываем камеру...
                VECTOR3D CamPosTTT(0.0f,0.0f,0.0f);
                if (tmp->ObjectStatus==3) CamPosTTT = GameCameraMovement^(GameCameraGetSpeed()*ObjCurrentTime);

                // находи точку по середине... это нам и надо... туда целимся...
                VECTOR3D PossibleRealLocation = RealLocation + FutureLocation + CamPosTTT;

                TTT = Location - PossibleRealLocation;
                float PossibleDist = TTT.Length();
		        float PossibleObjTime = PossibleDist/ProjectileSpeed;

				float PoprTime = PossibleObjTime;


				FutureLocation = tmp->Orientation^(tmp->Speed*PoprTime);
                // учитываем камеру...
				CamPosTTT = VECTOR3D(0.0f,0.0f,0.0f);
				if (tmp->ObjectStatus==3) CamPosTTT = GameCameraMovement^(GameCameraGetSpeed()*PoprTime);

				RealLocation = RealLocation + FutureLocation + CamPosTTT;
			}



				float TargetDist2TMP = (Location.x - RealLocation.x)*(Location.x - RealLocation.x)
					+ (Location.y - RealLocation.y)*(Location.y - RealLocation.y)
					+ (Location.z - RealLocation.z)*(Location.z - RealLocation.z);

				// проверяем, чтобы объект находился не ближе чем MinDistance
				if (TargetDist2>TargetDist2TMP)
				{
					// запоминаем эти данные
					TargetLocation = RealLocation;
					TargetDist2 = TargetDist2TMP;
					TargetLocked = true;
				}

		}

		tmp = tmpShip2;
	}









	// находим направление и углы нацеливания на цель, если нужно
	if (TargetLocked)
	{
		// находим угол между плоскостью и прямой
		float m = TargetLocation.x-Location.x;
		float n = TargetLocation.y-Location.y;
		float p = TargetLocation.z-Location.z;

		// поправки к существующим углам поворота оружия
		float sss1 = m*m+n*n+p*p;
		float sss2 = A*A+B*B+C*C;
		if (sss1 != 0.0f && sss2 != 0.0f)
		{
			float ttt = (A*m+B*n+C*p)/(vw_sqrtf(sss1) * vw_sqrtf(sss2));
			if (ttt >= -1.0f && ttt <= 1.0f)
				(*NeedAngle).x = CurrentObjectRotation.x + asinf(ttt) * 57.32f;
		}

		(*NeedAngle).z = CurrentObjectRotation.z;




		// нужно найти точку на плоскости, образованную перпендикуляром с точки TargetLocation
		// иначе не правильно будем ориентировать
		if (sss2 != 0)
		{
			float t = (-D - A*TargetLocation.x - B*TargetLocation.y - C*TargetLocation.z)/(A*A+B*B+C*C);
			TargetLocation.x = t*A + TargetLocation.x;
			TargetLocation.y = t*B + TargetLocation.y;
			TargetLocation.z = t*C + TargetLocation.z;
			m = TargetLocation.x-Location.x;
			n = TargetLocation.y-Location.y;
			p = TargetLocation.z-Location.z;



			// находим плоскость, вертикальную
			float A2, B2, C2, D2;
			GetPlaneABCD(&A2, &B2, &C2, &D2, Location, Location+PointUp, Location+PointRight);

			// смотрим в какой полуплоскости
			float tmp1_1 = A2 * TargetLocation.x + B2 * TargetLocation.y + C2 * TargetLocation.z + D2;

			if (tmp1_1 >= 0.0f)
			{
				GetPlaneABCD(&A2, &B2, &C2, &D2, Location, Location+Orientation, Location+PointUp);

				// находим угол поворота
				sss1 = vw_sqrtf(m*m+n*n+p*p);
				float sss3 = vw_sqrtf(A2*A2+B2*B2+C2*C2);
				if (sss1 != 0.0f && sss3 != 0.0f)
				{
					float ttt = (A2*m+B2*n+C2*p)/(sss1 * sss3);
					if (ttt >= -1.0f && ttt <= 1.0f)
						(*NeedAngle).y = 180.0f-asinf(ttt) * 57.32f;
				}
			}
			else
			{
				GetPlaneABCD(&A2, &B2, &C2, &D2, Location, Location+(Orientation^(-1.0f)), Location+PointUp);

				// находим угол поворота
				sss1 = vw_sqrtf(m*m+n*n+p*p);
				float sss3 = vw_sqrtf(A2*A2+B2*B2+C2*C2);
				if (sss1 != 0.0f && sss3 != 0.0f)
				{
					float ttt = (A2*m+B2*n+C2*p)/(sss1 * sss3);
					if (ttt >= -1.0f && ttt <= 1.0f)
					{
						(*NeedAngle).y = -asinf(ttt) * 57.32f;
						if ((*NeedAngle).y < 0.0f) (*NeedAngle).y +=360.0f;
					}
				}
			}
		}
	}


}










//-----------------------------------------------------------------------------
// Получение угла поворота оружия на врага для турелей наземных объектов
//-----------------------------------------------------------------------------
bool GetTurretOnTargetOrientateion(
		int			ObjectStatus, // статус объекта, который целится
		VECTOR3D	Location, // положение точки относительно которой будем наводить
		VECTOR3D	CurrentObjectRotation, // текущие углы объекта
		float		RotationMatrix[9], // матрица вращения объекта
		VECTOR3D	*NeedAngle, // нужные углы, чтобы получить нужное направление
		int			WeaponType, // номер оружия
		VECTOR3D	FirePos) // положение оружия
{


	// получаем точки для создания плоскости
	VECTOR3D Orientation(0.0f, 0.0f, 1.0f);
	Matrix33CalcPoint(&Orientation, RotationMatrix);
	VECTOR3D PointRight(1.0f, 0.0f, 0.0f);
	Matrix33CalcPoint(&PointRight, RotationMatrix);
	VECTOR3D PointUp(0.0f, 1.0f, 0.0f);
	Matrix33CalcPoint(&PointUp, RotationMatrix);

	// находим плоскость, горизонтальную
	float A, B, C, D;
	GetPlaneABCD(&A, &B, &C, &D, Location, Location+Orientation, Location+PointRight);


	// для выбора - точка, куда целимся + расстояние до нее (квадрат расстояния)
	VECTOR3D TargetLocation = Location;
	float TargetDist2 = 1000000.0f;
	bool TargetLocked = false;




	CSpaceShip *tmp = StartSpaceShip;
	while (tmp!=0)
	{
		CSpaceShip *tmpShip2 = tmp->Next;
		// если по этому надо стрелять
		if (NeedCheckCollision(tmp))
		// выбираем врага, по своим не целимся
		if ((ObjectStatus == 1 && tmp->ObjectStatus>1) ||
			(ObjectStatus > 1 && tmp->ObjectStatus==1))
		{

			// находим настоящую точку попадания с учетом скорости объекта и пули... если надо
		    VECTOR3D RealLocation = tmp->Location + (tmp->AABB[0]+tmp->AABB[6])/2.0f;


                // находим, за какое время снаряд долетит до объекта сейчас
				VECTOR3D TTT = Location - RealLocation;
		        float ProjectileSpeed = GetProjectileSpeed(WeaponType);
				if (ObjectStatus!=3) ProjectileSpeed = ProjectileSpeed/GameNPCWeaponPenalty;
		        float CurrentDist = TTT.Length();
		        float ObjCurrentTime = CurrentDist/ProjectileSpeed;

                // находим где будет объект, когда пройдет это время (+ сразу половину считаем!)
                VECTOR3D FutureLocation = tmp->Orientation^(tmp->Speed*ObjCurrentTime);
                // учитываем камеру...
                VECTOR3D CamPosTTT(0.0f,0.0f,0.0f);
                if (tmp->ObjectStatus==3) CamPosTTT = GameCameraMovement^(GameCameraGetSpeed()*ObjCurrentTime);

                // находи точку по середине... это нам и надо... туда целимся...
                VECTOR3D PossibleRealLocation = RealLocation + FutureLocation + CamPosTTT;

                TTT = Location - PossibleRealLocation;
                float PossibleDist = TTT.Length();
		        float PossibleObjTime = PossibleDist/ProjectileSpeed;

				float PoprTime = PossibleObjTime;


				FutureLocation = tmp->Orientation^(tmp->Speed*PoprTime);
                // учитываем камеру...
				CamPosTTT = VECTOR3D(0.0f,0.0f,0.0f);
				if (tmp->ObjectStatus==3) CamPosTTT = GameCameraMovement^(GameCameraGetSpeed()*PoprTime);

				RealLocation = RealLocation + FutureLocation + CamPosTTT;





			// проверяем, если все точки выше
			float tmp1 = A * (RealLocation.x+tmp->OBB[0].x)  + B * (RealLocation.y+tmp->OBB[0].y)  + C * (RealLocation.z+tmp->OBB[0].z)  + D;
			float tmp2 = A * (RealLocation.x+tmp->OBB[1].x)  + B * (RealLocation.y+tmp->OBB[1].y)  + C * (RealLocation.z+tmp->OBB[1].z)  + D;
			float tmp3 = A * (RealLocation.x+tmp->OBB[2].x)  + B * (RealLocation.y+tmp->OBB[2].y)  + C * (RealLocation.z+tmp->OBB[2].z)  + D;
			float tmp4 = A * (RealLocation.x+tmp->OBB[3].x)  + B * (RealLocation.y+tmp->OBB[3].y)  + C * (RealLocation.z+tmp->OBB[3].z)  + D;
			float tmp5 = A * (RealLocation.x+tmp->OBB[4].x)  + B * (RealLocation.y+tmp->OBB[4].y)  + C * (RealLocation.z+tmp->OBB[4].z)  + D;
			float tmp6 = A * (RealLocation.x+tmp->OBB[5].x)  + B * (RealLocation.y+tmp->OBB[5].y)  + C * (RealLocation.z+tmp->OBB[5].z)  + D;
			float tmp7 = A * (RealLocation.x+tmp->OBB[6].x)  + B * (RealLocation.y+tmp->OBB[6].y)  + C * (RealLocation.z+tmp->OBB[6].z)  + D;
			float tmp8 = A * (RealLocation.x+tmp->OBB[7].x)  + B * (RealLocation.y+tmp->OBB[7].y)  + C * (RealLocation.z+tmp->OBB[7].z)  + D;

			if (tmp1>0.0f && tmp2>0.0f && tmp3>0.0f && tmp4>0.0f && tmp5>0.0f &&
				tmp6>0.0f && tmp7>0.0f && tmp8>0.0f)
			{

				float TargetDist2TMP = (Location.x - RealLocation.x)*(Location.x - RealLocation.x)
					+ (Location.y - RealLocation.y)*(Location.y - RealLocation.y)
					+ (Location.z - RealLocation.z)*(Location.z - RealLocation.z);

				// проверяем, чтобы объект находился не ближе чем MinDistance
				if (TargetDist2>TargetDist2TMP)
				{
					// запоминаем эти данные
					TargetLocation = RealLocation;
					TargetDist2 = TargetDist2TMP;
					TargetLocked = true;
				}
			}
		}

		tmp = tmpShip2;
	}









	// находим направление и углы нацеливания на цель, если нужно
	if (TargetLocked)
	{
		// находим угол между плоскостью и прямой
		float m = TargetLocation.x-Location.x;
		float n = TargetLocation.y-Location.y;
		float p = TargetLocation.z-Location.z;

		// поправки к существующим углам поворота оружия
		float sss1 = m*m+n*n+p*p;
		float sss2 = A*A+B*B+C*C;
		if (sss1 != 0.0f && sss2 != 0.0f)
		{
			float ttt = (A*m+B*n+C*p)/(vw_sqrtf(sss1) * vw_sqrtf(sss2));
			if (ttt >= -1.0f && ttt <= 1.0f)
				(*NeedAngle).x = CurrentObjectRotation.x + asinf(ttt) * 57.32f;
		}

		(*NeedAngle).z = CurrentObjectRotation.z;




		// нужно найти точку на плоскости, образованную перпендикуляром с точки TargetLocation
		// иначе не правильно будем ориентировать
		if (sss2 != 0)
		{
			float t = (-D - A*TargetLocation.x - B*TargetLocation.y - C*TargetLocation.z)/(A*A+B*B+C*C);
			TargetLocation.x = t*A + TargetLocation.x;
			TargetLocation.y = t*B + TargetLocation.y;
			TargetLocation.z = t*C + TargetLocation.z;
			m = TargetLocation.x-Location.x;
			n = TargetLocation.y-Location.y;
			p = TargetLocation.z-Location.z;



			// находим плоскость, вертикальную
			float A2, B2, C2, D2;
			GetPlaneABCD(&A2, &B2, &C2, &D2, Location, Location+PointUp, Location+PointRight);

			// смотрим в какой полуплоскости
			float tmp1_1 = A2 * TargetLocation.x + B2 * TargetLocation.y + C2 * TargetLocation.z + D2;

			if (tmp1_1 >= 0.0f)
			{
				GetPlaneABCD(&A2, &B2, &C2, &D2, Location, Location+Orientation, Location+PointUp);

				// находим угол поворота
				sss1 = vw_sqrtf(m*m+n*n+p*p);
				float sss3 = vw_sqrtf(A2*A2+B2*B2+C2*C2);
				if (sss1 != 0.0f && sss3 != 0.0f)
				{
					float ttt = (A2*m+B2*n+C2*p)/(sss1 * sss3);
					if (ttt >= -1.0f && ttt <= 1.0f)
						(*NeedAngle).y = 180.0f-asinf(ttt) * 57.32f;
				}
			}
			else
			{
				GetPlaneABCD(&A2, &B2, &C2, &D2, Location, Location+(Orientation^(-1.0f)), Location+PointUp);

				// находим угол поворота
				sss1 = vw_sqrtf(m*m+n*n+p*p);
				float sss3 = vw_sqrtf(A2*A2+B2*B2+C2*C2);
				if (sss1 != 0.0f && sss3 != 0.0f)
				{
					float ttt = (A2*m+B2*n+C2*p)/(sss1 * sss3);
					if (ttt >= -1.0f && ttt <= 1.0f)
					{
						(*NeedAngle).y = -asinf(ttt) * 57.32f;
						if ((*NeedAngle).y < 0.0f) (*NeedAngle).y +=360.0f;
					}
				}
			}
		}
	}

	// передаем навелись или нет (нет врагов)
	return TargetLocked;
}









//-----------------------------------------------------------------------------
// Получение угла поворота ракеты, торпеды или бомбы
//-----------------------------------------------------------------------------
CObject3D *GetMissileOnTargetOrientateion(
		int			ObjectStatus, // статус объекта, который целится
		VECTOR3D	Location, // положение точки относительно которой будем наводить
		VECTOR3D	CurrentObjectRotation, // текущие углы объекта
		float		RotationMatrix[9], // матрица вращения объекта
		VECTOR3D	*NeedAngle)// нужные углы, чтобы получить нужное направление
{
	// получаем точки для создания плоскости
	VECTOR3D Orientation(0.0f, 0.0f, 1.0f);
	Matrix33CalcPoint(&Orientation, RotationMatrix);
	VECTOR3D PointUp(0.0f, 1.0f, 0.0f);
	Matrix33CalcPoint(&PointUp, RotationMatrix);
	VECTOR3D PointRight = VECTOR3D(1.0f, 0.0f, 0.0f);
	Matrix33CalcPoint(&PointRight, RotationMatrix);

	// находим плоскость, вертикальную
	float A, B, C, D;
	GetPlaneABCD(&A, &B, &C, &D, Location, Location+Orientation, Location+PointUp);


	// получаем вертикальную плоскость 2 (отсечения перед-зад)
	float A2, B2, C2, D2;
	GetPlaneABCD(&A2, &B2, &C2, &D2, Location, Location+PointRight, Location+PointUp);

	// для выбора - точка, куда целимся + расстояние до нее (квадрат расстояния)
	VECTOR3D TargetLocation = Location;
	VECTOR3D TargetAngle = CurrentObjectRotation;
	float Tdist = 1000.0f*1000.0f;

    // тип, кто заблокировал... чтобы не сбить с активных
	int TType = 0;
	bool TargetLocked = false;

	float tmp1, tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8;

	float MinDistance = 0.0f;


	// цель
	CObject3D *Target = 0;







	// проверка по снарядам, фларес
	CProjectile *tmpProjectile = StartProjectile;
	while (tmpProjectile!=0)
	{
		CProjectile *tmpProjectile2 = tmpProjectile->Next;
		// только фларес
		if (tmpProjectile->ProjectileType == 3)
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpProjectile))
		// выбираем врага, по своим не целимся
		if ((ObjectStatus == 1 && tmpProjectile->ObjectStatus>1) ||
			(ObjectStatus > 1 && tmpProjectile->ObjectStatus==1))
		{

				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (tmpProjectile->Location.x)  + B2 * (tmpProjectile->Location.y)  + C2 * (tmpProjectile->Location.z)  + D2;
				if (tmp1>0.0f)
				{

					float TargetDist2TMP = A2 * (tmpProjectile->Location.x)  + B2 * (tmpProjectile->Location.y)  + C2 * (tmpProjectile->Location.z)  + D2;

					// проверяем, чтобы объект находился не ближе чем MinDistance
					if (MinDistance<TargetDist2TMP)
					{
						// выбираем объект, так, чтобы он был наиболее длижайшим,
						// идущим по нашему курсу...

						VECTOR3D TargetAngleTMP;
						TargetLocation = tmpProjectile->Location;

						// находим угол между плоскостью и прямой
						float A3, B3, C3, D3;
						GetPlaneABCD(&A3, &B3, &C3, &D3, Location, Location+Orientation, Location+PointRight);

						float m = TargetLocation.x-Location.x;
						float n = TargetLocation.y-Location.y;
						float p = TargetLocation.z-Location.z;

						// поправки к существующим углам поворота оружия
                        float sss1 = vw_sqrtf(m*m+n*n+p*p);
                        float sss2 = vw_sqrtf(A3*A3+B3*B3+C3*C3);
                        TargetAngleTMP.x = CurrentObjectRotation.x;
						if (sss1 != 0 && sss2 != 0)
						{
						    float sss3 = (A3*m+B3*n+C3*p)/(sss1 * sss2);
						    if (sss3 >= -1.0f && sss3 <= 1.0f)
                                TargetAngleTMP.x = CurrentObjectRotation.x + asinf(sss3) * 57.32f;
						}

						float sss4 = vw_sqrtf(A*A+B*B+C*C);
						TargetAngleTMP.y = CurrentObjectRotation.y;
						if (sss1 != 0 && sss4 != 0)
						{
						    float sss5 = (A*m+B*n+C*p)/(sss1 * sss4);
						    if (sss5 >= -1.0f && sss5 <= 1.0f)
                                TargetAngleTMP.y = CurrentObjectRotation.y + asinf(sss5) * 57.32f;
						}

						TargetAngleTMP.z = CurrentObjectRotation.z;


					/*	if (!TargetLocked)
						{
							TargetAngle = TargetAngleTMP;
							TargetLocked = true;
							Tdist = m*m+n*n+p*p;
						}
						else
						{*/
							if (Tdist > m*m+n*n+p*p)
							{
								TargetAngle = TargetAngleTMP;
								Tdist = m*m+n*n+p*p;
								TargetLocked = true;
								TType = 1;
								Target = tmpProjectile;
							}
					//	}
					}
				}

		}

		tmpProjectile = tmpProjectile2;
	}





	// проверка по наземным объектам
	// не стрелять по "мирным" постойкам
	CGroundObject *tmpG = StartGroundObject;
	while (tmpG!=0)
	{
		CGroundObject *tmpGround2 = tmpG->Next;
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpG))
		// выбираем врага, по своим не целимся
		if ((ObjectStatus == 1 && tmpG->ObjectStatus>1) ||
			(ObjectStatus > 1 && tmpG->ObjectStatus==1))
		{
				// из-за длинного ствола, точка может быть вынесена далеко, и тогда ракета промажет
				// если делать через (tmpG->AABB[0]+tmpG->AABB[6])/2.0f;
				VECTOR3D tmp(0.0f, tmpG->Height/2.0f, 0.0f);
				Matrix33CalcPoint(&tmp, tmpG->CurrentRotationMat); // поворачиваем в плоскость объекта
				TargetLocation = tmpG->Location+tmp;

				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (TargetLocation.x)  + B2 * (TargetLocation.y)  + C2 * (TargetLocation.z)  + D2;
				if (tmp1>0.0f)
				{

					float TargetDist2TMP = A2 * (TargetLocation.x)  + B2 * (TargetLocation.y)  + C2 * (TargetLocation.z)  + D2;

					// проверяем, чтобы объект находился не ближе чем MinDistance
					if (MinDistance<TargetDist2TMP)
					{
						// выбираем объект, так, чтобы он был наиболее длижайшим,
						// идущим по нашему курсу...
						VECTOR3D TargetAngleTMP;

						// находим угол между плоскостью и прямой
						float A3, B3, C3, D3;
						GetPlaneABCD(&A3, &B3, &C3, &D3, Location, Location+Orientation, Location+PointRight);

						float m = TargetLocation.x-Location.x;
						float n = TargetLocation.y-Location.y;
						float p = TargetLocation.z-Location.z;

						// поправки к существующим углам поворота оружия
                        float sss1 = vw_sqrtf(m*m+n*n+p*p);
                        float sss2 = vw_sqrtf(A3*A3+B3*B3+C3*C3);
                        TargetAngleTMP.x = CurrentObjectRotation.x;
						if (sss1 != 0 && sss2 != 0)
						{
						    float sss3 = (A3*m+B3*n+C3*p)/(sss1 * sss2);
						    if (sss3 >= -1.0f && sss3 <= 1.0f)
                                TargetAngleTMP.x = CurrentObjectRotation.x + asinf(sss3) * 57.32f;
						}

						float sss4 = vw_sqrtf(A*A+B*B+C*C);
						TargetAngleTMP.y = CurrentObjectRotation.y;
						if (sss1 != 0 && sss4 != 0)
						{
						    float sss5 = (A*m+B*n+C*p)/(sss1 * sss4);
						    if (sss5 >= -1.0f && sss5 <= 1.0f)
                                TargetAngleTMP.y = CurrentObjectRotation.y + asinf(sss5) * 57.32f;
						}

						TargetAngleTMP.z = CurrentObjectRotation.z;



							if (TType < 2 && TargetLocked)
							{
                                if (Tdist/3.0f > m*m+n*n+p*p && fabsf(TargetAngleTMP.x-CurrentObjectRotation.x)<45.0f)
                                {
                                    TargetAngle = TargetAngleTMP;
                                    Tdist = m*m+n*n+p*p;
                                    TargetLocked = true;
                                    TType = 2;
                                    Target = tmpG;
                                }
							}
							else
                                if (Tdist > m*m+n*n+p*p && fabsf(TargetAngleTMP.x-CurrentObjectRotation.x)<45.0f)
                                {
                                    TargetAngle = TargetAngleTMP;
                                    Tdist = m*m+n*n+p*p;
                                    TargetLocked = true;
                                    TType = 2;
                                    Target = tmpG;
                                }
					}
				}

		}

		tmpG = tmpGround2;
	}




	CSpaceShip *tmp = StartSpaceShip;
	while (tmp!=0)
	{
		CSpaceShip *tmpShip2 = tmp->Next;
		// проверка, чтобы не считать свой корабль
		if (tmp->ID != 111111)
		// если по этому надо стрелять
		if (NeedCheckCollision(tmp))
		// выбираем врага, по своим не целимся
		if ((ObjectStatus == 1 && tmp->ObjectStatus>1) ||
			(ObjectStatus > 1 && tmp->ObjectStatus==1))
		{

				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (tmp->Location.x)  + B2 * (tmp->Location.y)  + C2 * (tmp->Location.z)  + D2;
				if (tmp1>0.0f)
				{

					float TargetDist2TMP = A2 * (tmp->Location.x)  + B2 * (tmp->Location.y)  + C2 * (tmp->Location.z)  + D2;

					// проверяем, чтобы объект находился не ближе чем MinDistance
					if (MinDistance<TargetDist2TMP)
					{
						// выбираем объект, так, чтобы он был наиболее длижайшим,
						// идущим по нашему курсу...

						VECTOR3D TargetAngleTMP;
						TargetLocation = tmp->Location;

						// находим угол между плоскостью и прямой
						float A3, B3, C3, D3;
						GetPlaneABCD(&A3, &B3, &C3, &D3, Location, Location+Orientation, Location+PointRight);

						float m = TargetLocation.x-Location.x;
						float n = TargetLocation.y-Location.y;
						float p = TargetLocation.z-Location.z;

						// поправки к существующим углам поворота оружия
                        float sss1 = vw_sqrtf(m*m+n*n+p*p);
                        float sss2 = vw_sqrtf(A3*A3+B3*B3+C3*C3);
                        TargetAngleTMP.x = CurrentObjectRotation.x;
						if (sss1 != 0 && sss2 != 0)
						{
						    float sss3 = (A3*m+B3*n+C3*p)/(sss1 * sss2);
						    if (sss3 >= -1.0f && sss3 <= 1.0f)
                                TargetAngleTMP.x = CurrentObjectRotation.x + asinf(sss3) * 57.32f;
						}

						float sss4 = vw_sqrtf(A*A+B*B+C*C);
						TargetAngleTMP.y = CurrentObjectRotation.y;
						if (sss1 != 0 && sss4 != 0)
						{
						    float sss5 = (A*m+B*n+C*p)/(sss1 * sss4);
						    if (sss5 >= -1.0f && sss5 <= 1.0f)
                                TargetAngleTMP.y = CurrentObjectRotation.y + asinf(sss5) * 57.32f;
						}

						TargetAngleTMP.z = CurrentObjectRotation.z;


							if (TType < 3 && TargetLocked)
							{
							    // только если в 6 раза ближе
                                if (Tdist/6.0f > m*m+n*n+p*p && fabsf(TargetAngleTMP.x-CurrentObjectRotation.x)<45.0f)
                                {
                                    TargetAngle = TargetAngleTMP;
                                    Tdist = m*m+n*n+p*p;
                                    TargetLocked = true;
                                    TType = 3;
                                    Target = tmp;
                                }
							}
							else
                                if (Tdist > m*m+n*n+p*p && fabsf(TargetAngleTMP.x-CurrentObjectRotation.x)<45.0f)
                                {
                                    TargetAngle = TargetAngleTMP;
                                    Tdist = m*m+n*n+p*p;
                                    TargetLocked = true;
                                    TType = 3;
                                    Target = tmp;
                                }
					}
				}

		}

		tmp = tmpShip2;
	}







	// проверка по космическим объектам
	CSpaceObject *tmpS = StartSpaceObject;
	while (tmpS!=0)
	{
		CSpaceObject *tmpSpace2 = tmpS->Next;
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpS))
		// выбираем врага, по своим не целимся
		if ((ObjectStatus == 1 && tmpS->ObjectStatus>1) ||
			(ObjectStatus > 1 && tmpS->ObjectStatus==1))
		if (tmpS->ObjectType != 8) // если это не части кораблей
		{
				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (tmpS->Location.x)  + B2 * (tmpS->Location.y)  + C2 * (tmpS->Location.z)  + D2;
				if (tmp1>0.0f)
				{

					float TargetDist2TMP = A2 * (tmpS->Location.x)  + B2 * (tmpS->Location.y)  + C2 * (tmpS->Location.z)  + D2;

					// проверяем, чтобы объект находился не ближе чем MinDistance
					if (MinDistance<TargetDist2TMP)
					{
						// выбираем объект, так, чтобы он был наиболее длижайшим,
						// идущим по нашему курсу...
						VECTOR3D TargetAngleTMP;
						TargetLocation = tmpS->Location;

						// находим угол между плоскостью и прямой
						float A3, B3, C3, D3;
						GetPlaneABCD(&A3, &B3, &C3, &D3, Location, Location+Orientation, Location+PointRight);

						float m = TargetLocation.x-Location.x;
						float n = TargetLocation.y-Location.y;
						float p = TargetLocation.z-Location.z;

						// поправки к существующим углам поворота оружия
                        float sss1 = vw_sqrtf(m*m+n*n+p*p);
                        float sss2 = vw_sqrtf(A3*A3+B3*B3+C3*C3);
                        TargetAngleTMP.x = CurrentObjectRotation.x;
						if (sss1 != 0 && sss2 != 0)
						{
						    float sss3 = (A3*m+B3*n+C3*p)/(sss1 * sss2);
						    if (sss3 >= -1.0f && sss3 <= 1.0f)
                                TargetAngleTMP.x = CurrentObjectRotation.x + asinf(sss3) * 57.32f;
						}

						float sss4 = vw_sqrtf(A*A+B*B+C*C);
						TargetAngleTMP.y = CurrentObjectRotation.y;
						if (sss1 != 0 && sss4 != 0)
						{
						    float sss5 = (A*m+B*n+C*p)/(sss1 * sss4);
						    if (sss5 >= -1.0f && sss5 <= 1.0f)
                                TargetAngleTMP.y = CurrentObjectRotation.y + asinf(sss5) * 57.32f;
						}

						TargetAngleTMP.z = CurrentObjectRotation.z;

				/*		if (!TargetLocked)
						{
							TargetAngle = TargetAngleTMP;
							TargetLocked = true;
							Tdist = m*m+n*n+p*p;
						}
						else
						{*/
							if (TType < 4 && TargetLocked)
							{
							    // только если в 10 раза ближе
                                if (Tdist/10.0f > m*m+n*n+p*p && fabsf(TargetAngleTMP.x-CurrentObjectRotation.x)<45.0f)
                                {
                                    TargetAngle = TargetAngleTMP;
                                    Tdist = m*m+n*n+p*p;
                                    TargetLocked = true;
                                    TType = 4;
                                    Target = tmpS;
                                }
							}
							else
                                if (Tdist > m*m+n*n+p*p && fabsf(TargetAngleTMP.x-CurrentObjectRotation.x)<45.0f)
                                {
                                    TargetAngle = TargetAngleTMP;
                                    Tdist = m*m+n*n+p*p;
                                    TargetLocked = true;
                                    TType = 4;
                                    Target = tmpS;
                                }
						//}
					}
				}

		}

		tmpS = tmpSpace2;
	}



	// находим направление и углы нацеливания на цель, если нужно
	if (TargetLocked)
	{
		(*NeedAngle) = TargetAngle;
	}

	return Target;
}







//-----------------------------------------------------------------------------
// Получение положения ближайшего врага, для мин
//-----------------------------------------------------------------------------
CObject3D *GetCloserTargetPosition(
		int			ObjectStatus, // статус объекта, который целится
		VECTOR3D	Location) // положение точки относительно которой будем наводить
{

	// результат
	CObject3D *Res = 0;
	// пока ставим отрицательный, т.е. вообще ничего нет
	float MinRatius2 = -1.0f;



	// перебираем только корабли...
	CSpaceShip *tmp = StartSpaceShip;
	while (tmp!=0)
	{
		CSpaceShip *tmpShip2 = tmp->Next;

		// если по этому надо стрелять
		if (NeedCheckCollision(tmp))
		// выбираем врага, по своим не целимся
		if ((ObjectStatus == 1 && tmp->ObjectStatus>1) ||
			(ObjectStatus > 1 && tmp->ObjectStatus==1))
		{
			// получаем квадрат радиуса
			float MinRatius2TMP = (tmp->Location.x-Location.x)*(tmp->Location.x-Location.x) +
								(tmp->Location.y-Location.y)*(tmp->Location.y-Location.y) +
								(tmp->Location.z-Location.z)*(tmp->Location.z-Location.z);

			if (MinRatius2 < 0.0f)
			{
				MinRatius2 = MinRatius2TMP;
				Res = tmp;
			}
			else
			{
				if (MinRatius2TMP < MinRatius2)
				{
					MinRatius2 = MinRatius2TMP;
					Res = tmp;
				}
			}
		}

		tmp = tmpShip2;
	}



	return Res;
}
