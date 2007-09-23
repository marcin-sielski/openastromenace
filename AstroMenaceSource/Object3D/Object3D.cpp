/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Object3D.cpp

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

extern GameSetup Setup;
extern eDevCaps CAPS;
void DrawFont3D(float X, float Y, float Z, const char *Text, ...);

// флаг, показывать боксы или нет (1>AABB, 2>OBB, 3>HitBB)
int NeedShowBB = 0;




//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
CObject3D::CObject3D(void)
{
	// друг
	ObjectStatus = 2;

	// типа пока нет
	ObjectType = 0;
	ObjectCreationType = 0;

	// не задействуем
	ShowDeleteOnHide = -1;

	// идентификатора объекта - нет
	ID = 0;

	// по умолчанию - рисуем с оптимизацией
	NeedCullFaces = true;


	// лист пуст, и никуда не указывает
	DrawObjectQuantity = 0;
	DrawObjectList = 0;
	PromptDrawDist2 = -1.0f;
	InternalLights = 0;

	// начальныя установка коробок
	AABB[0]=AABB[1]=AABB[2]=AABB[3]=AABB[4]=AABB[5]=AABB[6]=AABB[7]= VECTOR3D(0.0f, 0.0f, 0.0f);
	OBBLocation = OBB[0]=OBB[1]=OBB[2]=OBB[3]=OBB[4]=OBB[5]=OBB[6]=OBB[7]= VECTOR3D(0.0f, 0.0f, 0.0f);
	HitBB = 0;
	HitBBLocation = 0;
	HitBBRadius2 = 0;
	HitBBSize = 0;

	// болтание объекта
	DeviationOn = false;
	DeviationObjQuantity = 0;
	Deviation = 0;
	NeedDeviation = 0;
	CurentDeviation = 0;
	CurentDeviationSum = 0;
	DeviationObjNum = 0;


	// у нас нет текстур для этой модели
	Texture = 0;
	TextureIllum = 0;

	Object3DGLSL[0][0] = vw_FindShaderByName("ObjectLight10");
	Object3DGLSL[0][1] = vw_FindShaderByName("ObjectLight11");
	Object3DGLSL[0][2] = vw_FindShaderByName("ObjectLight12");
	Object3DGLSL[0][3] = vw_FindShaderByName("ObjectLight13");
	Object3DGLSL[0][4] = vw_FindShaderByName("ObjectLight14");
	Object3DGLSL[0][5] = vw_FindShaderByName("ObjectLight15");
	Object3DGLSL[0][6] = vw_FindShaderByName("ObjectLight16");
	Object3DGLSL[1][0] = vw_FindShaderByName("ObjectLight20");
	Object3DGLSL[1][1] = vw_FindShaderByName("ObjectLight21");
	Object3DGLSL[1][2] = vw_FindShaderByName("ObjectLight22");
	Object3DGLSL[1][3] = vw_FindShaderByName("ObjectLight23");
	Object3DGLSL[1][4] = vw_FindShaderByName("ObjectLight24");
	Object3DGLSL[1][5] = vw_FindShaderByName("ObjectLight25");
	Object3DGLSL[1][6] = vw_FindShaderByName("ObjectLight26");
	ShaderType = 0;

	// мы не знаем как ориентирован объект и где он находится
	Location = PrevLocation = Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
	Orientation = VECTOR3D(0.0f, 0.0f, 1.0f);

	Radius = 0.0f;

	// объект еще не проверяли
	TimeLastUpdate = -1.0f;
	// время между апдейтами
	TimeDelta = 0.0f;

	// время жизни не установлено, объект не проверяется по времени
	Lifetime = -1.0f;

	// габаритов и других данных пока нет
	Weight = Width = Length = Height = 1.0f;
	Strength = StrengthStart = 0.0f;
	ShieldStrength = ShieldStrengthStart = ShieldRecharge = 0.0f;
	ShowStrength = true;
	NeedShowStrengthNow = false;

	// нет ослабления
	ResistanceHull = ResistanceSystems = 1.0f;

	// начальные установки для мартиц поворотов
	Matrix33Identity(CurrentRotationMat);
	Matrix33Identity(OldInvRotationMat);


	// пока базовые установки материала
	Diffuse[0] = Diffuse[1] = Diffuse[2] = Diffuse[3] = 1.0f;
	Specular[0] = Specular[1] = Specular[2] = Specular[3] = 1.0f;
	Ambient[0] = Ambient[1] = Ambient[2] = Ambient[3] = 0.1f;
	Power[0] = 64.0f;//128.0f;


	// изночально, ничего не выводим
	DebugInfo = 0;

	StartTimeSheet = 0;
	EndTimeSheet = 0;
}



//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
CObject3D::~CObject3D(void)
{
	// освобождаем память, выделенную для геометрии
	if (DrawObjectList != 0){delete [] DrawObjectList; DrawObjectList = 0;}

	if (HitBB != 0)
	{
		for (int i=0; i<DrawObjectQuantity; i++)
		{
			if (HitBB[i] != 0){delete [] HitBB[i]; HitBB[i] = 0;}
		}
		delete [] HitBB; HitBB = 0;
	}
	if (HitBBLocation != 0){delete [] HitBBLocation; HitBBLocation = 0;}
	if (HitBBRadius2 != 0){delete [] HitBBRadius2; HitBBRadius2 = 0;}
	if (HitBBSize != 0){delete [] HitBBSize; HitBBSize = 0;}

	if (DeviationObjQuantity != 0)
	{
		if (Deviation != 0){delete [] Deviation; Deviation = 0;}
		if (NeedDeviation != 0){delete [] NeedDeviation; NeedDeviation = 0;}
		if (CurentDeviation != 0){delete [] CurentDeviation; CurentDeviation = 0;}
		if (CurentDeviationSum != 0){delete [] CurentDeviationSum; CurentDeviationSum = 0;}
		if (DeviationObjNum != 0){delete [] DeviationObjNum; DeviationObjNum = 0;}
	}

	if (Texture != 0) {delete [] Texture; Texture = 0;}
	if (TextureIllum != 0) {delete [] TextureIllum; TextureIllum = 0;}

	if (DebugInfo != 0){delete [] DebugInfo; DebugInfo = 0;}

	// удаляем все записи, если они есть
	while (StartTimeSheet!=0)
	{
		DetachTimeSheet(StartTimeSheet);
	}

}






//-----------------------------------------------------------------------------
// Установка AABB, OBB и габаритов по геометрии объекта
//-----------------------------------------------------------------------------
void CObject3D::InitByDrawObjectList()
{
	// нет самой геометрии, работать не можем
	if (DrawObjectList == 0) return;

	// создаем HitBB
	for (int i=0; i<DrawObjectQuantity; i++)
	{
		float Matrix[9];
		Matrix33CreateRotate(Matrix, DrawObjectList[i].Rotation);


		float MinX = 10000.0f;
		float MaxX = -10000.0f;
		float MinY = 10000.0f;
		float MaxY = -10000.0f;
		float MinZ = 10000.0f;
		float MaxZ = -10000.0f;
		if (DrawObjectList[i].VertexCount > 0)
		{
			int j = 0;
			int j2;
			if (DrawObjectList[i].IndexBuffer != 0)
				j2 = DrawObjectList[i].IndexBuffer[DrawObjectList[i].RangeStart+j]*DrawObjectList[i].Stride;
			else
				j2 = (DrawObjectList[i].RangeStart+j)*DrawObjectList[i].Stride;

			VECTOR3D tmp;
			tmp.x = DrawObjectList[i].VertexBuffer[j2 + 0];
			tmp.y = DrawObjectList[i].VertexBuffer[j2 + 1];
			tmp.z = DrawObjectList[i].VertexBuffer[j2 + 2];
			Matrix33CalcPoint(&tmp, Matrix);
			MinX = MaxX = tmp.x;
			MinY = MaxY = tmp.y;
			MinZ = MaxZ = tmp.z;
		}
		else
		{	// пустой объект
			MinX = MaxX = 0.0f;
			MinY = MaxY = 0.0f;
			MinZ = MaxZ = 0.0f;
		}

		for (int j=0; j<DrawObjectList[i].VertexCount; j++)
		{
			int j2;
			if (DrawObjectList[i].IndexBuffer != 0)
				j2 = DrawObjectList[i].IndexBuffer[DrawObjectList[i].RangeStart+j]*DrawObjectList[i].Stride;
			else
				j2 = (DrawObjectList[i].RangeStart+j)*DrawObjectList[i].Stride;

			VECTOR3D v;
			v.x = DrawObjectList[i].VertexBuffer[j2];
			v.y = DrawObjectList[i].VertexBuffer[j2+1];
			v.z = DrawObjectList[i].VertexBuffer[j2+2];
			Matrix33CalcPoint(&v, Matrix);
			if (MinX > v.x) MinX = v.x;
			if (MinY > v.y) MinY = v.y;
			if (MinZ > v.z) MinZ = v.z;
			if (MaxX < v.x) MaxX = v.x;
			if (MaxY < v.y) MaxY = v.y;
			if (MaxZ < v.z) MaxZ = v.z;
		}

		// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
		HitBB[i][0] = VECTOR3D(MaxX, MaxY, MaxZ) + DrawObjectList[i].Location;
		HitBB[i][1] = VECTOR3D(MinX, MaxY, MaxZ) + DrawObjectList[i].Location;
		HitBB[i][2] = VECTOR3D(MinX, MaxY, MinZ) + DrawObjectList[i].Location;
		HitBB[i][3] = VECTOR3D(MaxX, MaxY, MinZ) + DrawObjectList[i].Location;
		HitBB[i][4] = VECTOR3D(MaxX, MinY, MaxZ) + DrawObjectList[i].Location;
		HitBB[i][5] = VECTOR3D(MinX, MinY, MaxZ) + DrawObjectList[i].Location;
		HitBB[i][6] = VECTOR3D(MinX, MinY, MinZ) + DrawObjectList[i].Location;
		HitBB[i][7] = VECTOR3D(MaxX, MinY, MinZ) + DrawObjectList[i].Location;

		// находим координаты центра HitBB относительно координат модели
		HitBBLocation[i].x = (MaxX + MinX)/2.0f;
		HitBBLocation[i].y = (MaxY + MinY)/2.0f;
		HitBBLocation[i].z = (MaxZ + MinZ)/2.0f;

		// находим размеры коробки
		HitBBSize[i].x = fabsf(MaxX-MinX);
		HitBBSize[i].y = fabsf(MaxY-MinY);
		HitBBSize[i].z = fabsf(MaxZ-MinZ);

		// находим квадрат радиуса
		HitBBRadius2[i] = (HitBBSize[i].x/2.0f) * (HitBBSize[i].x/2.0f) +
				(HitBBSize[i].y/2.0f) * (HitBBSize[i].y/2.0f) +
				(HitBBSize[i].z/2.0f) * (HitBBSize[i].z/2.0f);

		// переносим данные HitBB чтобы центр стал центром
		for (int k=0; k<8; k++)
			HitBB[i][k] -= HitBBLocation[i];
	}



	// находим AABB, OBB... первоначально без учета текущего положения и поворота - их нет
	// 1-й HitBB всегда есть... фактически это OBB
	float MinX = HitBB[0][6].x + HitBBLocation[0].x;
	float MaxX = HitBB[0][0].x + HitBBLocation[0].x;
	float MinY = HitBB[0][6].y + HitBBLocation[0].y;
	float MaxY = HitBB[0][0].y + HitBBLocation[0].y;
	float MinZ = HitBB[0][6].z + HitBBLocation[0].z;
	float MaxZ = HitBB[0][0].z + HitBBLocation[0].z;

	// берем то по HitBB, причем со 2-го сразу, т.к. первый учли выше
	for (int i=1; i<DrawObjectQuantity; i++)
	{
		if (MinX > HitBB[i][6].x + HitBBLocation[i].x) MinX = HitBB[i][6].x + HitBBLocation[i].x;
		if (MaxX < HitBB[i][0].x + HitBBLocation[i].x) MaxX = HitBB[i][0].x + HitBBLocation[i].x;
		if (MinY > HitBB[i][6].y + HitBBLocation[i].y) MinY = HitBB[i][6].y + HitBBLocation[i].y;
		if (MaxY < HitBB[i][0].y + HitBBLocation[i].y) MaxY = HitBB[i][0].y + HitBBLocation[i].y;
		if (MinZ > HitBB[i][6].z + HitBBLocation[i].z) MinZ = HitBB[i][6].z + HitBBLocation[i].z;
		if (MaxZ < HitBB[i][0].z + HitBBLocation[i].z) MaxZ = HitBB[i][0].z + HitBBLocation[i].z;
	}

	// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
	OBB[0] = AABB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
	OBB[1] = AABB[1] = VECTOR3D(MinX, MaxY, MaxZ);
	OBB[2] = AABB[2] = VECTOR3D(MinX, MaxY, MinZ);
	OBB[3] = AABB[3] = VECTOR3D(MaxX, MaxY, MinZ);
	OBB[4] = AABB[4] = VECTOR3D(MaxX, MinY, MaxZ);
	OBB[5] = AABB[5] = VECTOR3D(MinX, MinY, MaxZ);
	OBB[6] = AABB[6] = VECTOR3D(MinX, MinY, MinZ);
	OBB[7] = AABB[7] = VECTOR3D(MaxX, MinY, MinZ);

	// находим координаты центра OBB относительно координат модели
	OBBLocation.x = (MaxX + MinX)/2.0f;
	OBBLocation.y = (MaxY + MinY)/2.0f;
	OBBLocation.z = (MaxZ + MinZ)/2.0f;

	// переносим данные OBB чтобы центр стал центром
	for (int k=0; k<8; k++)
	{
		OBB[k] -= OBBLocation;
	}


	// габариты
	Width = fabsf(MaxX-MinX);
	Height = fabsf(MaxY-MinY);
	Length = fabsf(MaxZ-MinZ);

	float Width2 = Width/2.0f;
	float Length2 = Length/2.0f;
	float Height2 = Height/2.0f;
	// находим радиус
	Radius = vw_sqrtf(Width2*Width2+Length2*Length2+Height2*Height2);

}






//-----------------------------------------------------------------------------
// Установка положения 1 объекта модели
//-----------------------------------------------------------------------------
void CObject3D::SetObjectLocation(VECTOR3D NewLocation, int ObjectNum)
{

	// пересчет HitBB
	if (HitBB != 0)
	{
		HitBBLocation[ObjectNum] -= DrawObjectList[ObjectNum].Location - NewLocation;

		// нужно подкорректировать OBB и ABB

		float MinX = 10000.0f;
		float MaxX = -10000.0f;
		float MinY = 10000.0f;
		float MaxY = -10000.0f;
		float MinZ = 10000.0f;
		float MaxZ = -10000.0f;

		// делаем временную обратную матрицу модели
		float OldInvRotationMatTmp[9];
		memcpy(OldInvRotationMatTmp, CurrentRotationMat, 9*sizeof(float));
		Matrix33InverseRotate(OldInvRotationMatTmp);

		// проверяем данные
		for (int i=0; i<DrawObjectQuantity; i++)
		{
			Matrix33CalcPoint(&HitBBLocation[i], OldInvRotationMatTmp);

			for (int j=0; j<8; j++)
			{
				Matrix33CalcPoint(&HitBB[i][j], OldInvRotationMatTmp);

				if (MinX > HitBB[i][j].x + HitBBLocation[i].x) MinX = HitBB[i][j].x + HitBBLocation[i].x;
				if (MaxX < HitBB[i][j].x + HitBBLocation[i].x) MaxX = HitBB[i][j].x + HitBBLocation[i].x;
				if (MinY > HitBB[i][j].y + HitBBLocation[i].y) MinY = HitBB[i][j].y + HitBBLocation[i].y;
				if (MaxY < HitBB[i][j].y + HitBBLocation[i].y) MaxY = HitBB[i][j].y + HitBBLocation[i].y;
				if (MinZ > HitBB[i][j].z + HitBBLocation[i].z) MinZ = HitBB[i][j].z + HitBBLocation[i].z;
				if (MaxZ < HitBB[i][j].z + HitBBLocation[i].z) MaxZ = HitBB[i][j].z + HitBBLocation[i].z;

				Matrix33CalcPoint(&HitBB[i][j], CurrentRotationMat);
			}
			Matrix33CalcPoint(&HitBBLocation[i], CurrentRotationMat);
		}

		// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
		OBB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
		OBB[1] = VECTOR3D(MinX, MaxY, MaxZ);
		OBB[2] = VECTOR3D(MinX, MaxY, MinZ);
		OBB[3] = VECTOR3D(MaxX, MaxY, MinZ);
		OBB[4] = VECTOR3D(MaxX, MinY, MaxZ);
		OBB[5] = VECTOR3D(MinX, MinY, MaxZ);
		OBB[6] = VECTOR3D(MinX, MinY, MinZ);
		OBB[7] = VECTOR3D(MaxX, MinY, MinZ);


		// габариты, пересчет именно тут, т.к. нужны данные OBB
		Width = fabsf(MaxX-MinX);
		Height = fabsf(MaxY-MinY);
		Length = fabsf(MaxZ-MinZ);

		float Width2 = Width/2.0f;
		float Length2 = Length/2.0f;
		float Height2 = Height/2.0f;
		// находим радиус
		Radius = vw_sqrtf(Width2*Width2+Length2*Length2+Height2*Height2);


		// находим координаты центра OBB относительно координат модели
		OBBLocation.x = (MaxX + MinX)/2.0f;
		OBBLocation.y = (MaxY + MinY)/2.0f;
		OBBLocation.z = (MaxZ + MinZ)/2.0f;

		// переносим данные OBB чтобы центр стал центром
		for (int k=0; k<8; k++)
		{
			OBB[k] -= OBBLocation;
			Matrix33CalcPoint(&OBB[k], CurrentRotationMat);
		}
		Matrix33CalcPoint(&OBBLocation, CurrentRotationMat);

		// по OBB находим AABB, это не совсем AABB (он будет больше), но зато быстро
		MinX = MaxX = OBB[0].x + OBBLocation.x;
		MinY = MaxY = OBB[0].y + OBBLocation.y;
		MinZ = MaxZ = OBB[0].z + OBBLocation.z;
		// берем и проверяем
		for (int j=0; j<8; j++)
		{
			if (MinX > OBB[j].x + OBBLocation.x) MinX = OBB[j].x + OBBLocation.x;
			if (MinY > OBB[j].y + OBBLocation.y) MinY = OBB[j].y + OBBLocation.y;
			if (MinZ > OBB[j].z + OBBLocation.z) MinZ = OBB[j].z + OBBLocation.z;
			if (MaxX < OBB[j].x + OBBLocation.x) MaxX = OBB[j].x + OBBLocation.x;
			if (MaxY < OBB[j].y + OBBLocation.y) MaxY = OBB[j].y + OBBLocation.y;
			if (MaxZ < OBB[j].z + OBBLocation.z) MaxZ = OBB[j].z + OBBLocation.z;
		}
		AABB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
		AABB[1] = VECTOR3D(MinX, MaxY, MaxZ);
		AABB[2] = VECTOR3D(MinX, MaxY, MinZ);
		AABB[3] = VECTOR3D(MaxX, MaxY, MinZ);
		AABB[4] = VECTOR3D(MaxX, MinY, MaxZ);
		AABB[5] = VECTOR3D(MinX, MinY, MaxZ);
		AABB[6] = VECTOR3D(MinX, MinY, MinZ);
		AABB[7] = VECTOR3D(MaxX, MinY, MinZ);
	}

	// собственно меняем данные в геометрии
	DrawObjectList[ObjectNum].Location = NewLocation;
}




//-----------------------------------------------------------------------------
// Установка углов поворота 1 объекта модели
//-----------------------------------------------------------------------------
void CObject3D::SetObjectRotation(VECTOR3D NewRotation, int ObjectNum)
{
	// пересчет HitBB
	if (HitBB != 0)
	{
		// создаем матрицу поворота объекта
		float CurrentRotationMatTmp2[9];
		Matrix33CreateRotate(CurrentRotationMatTmp2, NewRotation);

		float OldInvRotationMatTmp2[9];
		Matrix33CreateRotate(OldInvRotationMatTmp2, DrawObjectList[ObjectNum].Rotation);
		Matrix33InverseRotate(OldInvRotationMatTmp2);

		// делаем временную обратную матрицу модели
		float OldInvRotationMatTmp[9];
		memcpy(OldInvRotationMatTmp, CurrentRotationMat, 9*sizeof(float));
		Matrix33InverseRotate(OldInvRotationMatTmp);

		// собственно меняем данные в геометрии
		Matrix33CalcPoint(&HitBBLocation[ObjectNum], OldInvRotationMatTmp);
		HitBBLocation[ObjectNum] -= DrawObjectList[ObjectNum].Location;
		Matrix33CalcPoint(&HitBBLocation[ObjectNum], OldInvRotationMatTmp2);
		Matrix33CalcPoint(&HitBBLocation[ObjectNum], CurrentRotationMatTmp2);
		HitBBLocation[ObjectNum] += DrawObjectList[ObjectNum].Location;
		Matrix33CalcPoint(&HitBBLocation[ObjectNum], CurrentRotationMat);
		for (int j=0; j<8; j++)
		{
			Matrix33CalcPoint(&HitBB[ObjectNum][j], OldInvRotationMatTmp);
			Matrix33CalcPoint(&HitBB[ObjectNum][j], OldInvRotationMatTmp2);
			Matrix33CalcPoint(&HitBB[ObjectNum][j], CurrentRotationMatTmp2);
			Matrix33CalcPoint(&HitBB[ObjectNum][j], CurrentRotationMat);
		}


		// нужно подкорректировать OBB и ABB

		float MinX = 10000.0f;
		float MaxX = -10000.0f;
		float MinY = 10000.0f;
		float MaxY = -10000.0f;
		float MinZ = 10000.0f;
		float MaxZ = -10000.0f;

		// проверяем данные
		for (int i=0; i<DrawObjectQuantity; i++)
		{
			Matrix33CalcPoint(&HitBBLocation[i], OldInvRotationMatTmp);

			for (int j=0; j<8; j++)
			{
				Matrix33CalcPoint(&HitBB[i][j], OldInvRotationMatTmp);

				if (MinX > HitBB[i][j].x + HitBBLocation[i].x) MinX = HitBB[i][j].x + HitBBLocation[i].x;
				if (MaxX < HitBB[i][j].x + HitBBLocation[i].x) MaxX = HitBB[i][j].x + HitBBLocation[i].x;
				if (MinY > HitBB[i][j].y + HitBBLocation[i].y) MinY = HitBB[i][j].y + HitBBLocation[i].y;
				if (MaxY < HitBB[i][j].y + HitBBLocation[i].y) MaxY = HitBB[i][j].y + HitBBLocation[i].y;
				if (MinZ > HitBB[i][j].z + HitBBLocation[i].z) MinZ = HitBB[i][j].z + HitBBLocation[i].z;
				if (MaxZ < HitBB[i][j].z + HitBBLocation[i].z) MaxZ = HitBB[i][j].z + HitBBLocation[i].z;

				Matrix33CalcPoint(&HitBB[i][j], CurrentRotationMat);
			}
			Matrix33CalcPoint(&HitBBLocation[i], CurrentRotationMat);
		}

		// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
		OBB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
		OBB[1] = VECTOR3D(MinX, MaxY, MaxZ);
		OBB[2] = VECTOR3D(MinX, MaxY, MinZ);
		OBB[3] = VECTOR3D(MaxX, MaxY, MinZ);
		OBB[4] = VECTOR3D(MaxX, MinY, MaxZ);
		OBB[5] = VECTOR3D(MinX, MinY, MaxZ);
		OBB[6] = VECTOR3D(MinX, MinY, MinZ);
		OBB[7] = VECTOR3D(MaxX, MinY, MinZ);


		// габариты, пересчет именно тут, т.к. нужны данные OBB
		Width = fabsf(MaxX-MinX);
		Height = fabsf(MaxY-MinY);
		Length = fabsf(MaxZ-MinZ);

		float Width2 = Width/2.0f;
		float Length2 = Length/2.0f;
		float Height2 = Height/2.0f;
		// находим радиус
		Radius = vw_sqrtf(Width2*Width2+Length2*Length2+Height2*Height2);


		// находим координаты центра OBB относительно координат модели
		OBBLocation.x = (MaxX + MinX)/2.0f;
		OBBLocation.y = (MaxY + MinY)/2.0f;
		OBBLocation.z = (MaxZ + MinZ)/2.0f;

		// переносим данные OBB чтобы центр стал центром
		for (int k=0; k<8; k++)
		{
			OBB[k] -= OBBLocation;
			Matrix33CalcPoint(&OBB[k], CurrentRotationMat);
		}
		Matrix33CalcPoint(&OBBLocation, CurrentRotationMat);

		// по OBB находим AABB, это не совсем AABB (он будет больше), но зато быстро
		MinX = MaxX = OBB[0].x + OBBLocation.x;
		MinY = MaxY = OBB[0].y + OBBLocation.y;
		MinZ = MaxZ = OBB[0].z + OBBLocation.z;
		// берем и проверяем
		for (int j=0; j<8; j++)
		{
			if (MinX > OBB[j].x + OBBLocation.x) MinX = OBB[j].x + OBBLocation.x;
			if (MinY > OBB[j].y + OBBLocation.y) MinY = OBB[j].y + OBBLocation.y;
			if (MinZ > OBB[j].z + OBBLocation.z) MinZ = OBB[j].z + OBBLocation.z;
			if (MaxX < OBB[j].x + OBBLocation.x) MaxX = OBB[j].x + OBBLocation.x;
			if (MaxY < OBB[j].y + OBBLocation.y) MaxY = OBB[j].y + OBBLocation.y;
			if (MaxZ < OBB[j].z + OBBLocation.z) MaxZ = OBB[j].z + OBBLocation.z;
		}
		AABB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
		AABB[1] = VECTOR3D(MinX, MaxY, MaxZ);
		AABB[2] = VECTOR3D(MinX, MaxY, MinZ);
		AABB[3] = VECTOR3D(MaxX, MaxY, MinZ);
		AABB[4] = VECTOR3D(MaxX, MinY, MaxZ);
		AABB[5] = VECTOR3D(MinX, MinY, MaxZ);
		AABB[6] = VECTOR3D(MinX, MinY, MinZ);
		AABB[7] = VECTOR3D(MaxX, MinY, MinZ);
	}


	// собственно меняем данные в геометрии
	DrawObjectList[ObjectNum].Rotation = NewRotation;
}






//-----------------------------------------------------------------------------
// Установка положения модели
//-----------------------------------------------------------------------------
void CObject3D::SetLocation(VECTOR3D NewLocation)
{
	PrevLocation = Location;
	// новое положение объекта
	Location = NewLocation;
}




//-----------------------------------------------------------------------------
// Установка углов поворота модели
//-----------------------------------------------------------------------------
void CObject3D::SetRotation(VECTOR3D NewRotation)
{
	// Записываем данные в Rotation
	OldRotationInv = VECTOR3D(-Rotation.x,-Rotation.y,-Rotation.z);
	Rotation += NewRotation;


	// сохраняем старые значения + пересчет новых
	memcpy(OldInvRotationMat, CurrentRotationMat, 9*sizeof(float));
	// делаем инверсную старую матрицу
	Matrix33InverseRotate(OldInvRotationMat);
	Matrix33CreateRotate(CurrentRotationMat, Rotation);



	// По углам находим новые значения вектора Orientation
	Matrix33CalcPoint(&Orientation, OldInvRotationMat);
	Matrix33CalcPoint(&Orientation, CurrentRotationMat);


	// пересчет HitBB
	if (HitBB != 0)
	for (int i=0; i<DrawObjectQuantity; i++)
	{
		Matrix33CalcPoint(&HitBBLocation[i], OldInvRotationMat);
		Matrix33CalcPoint(&HitBBLocation[i], CurrentRotationMat);

		for (int j=0; j<8; j++)
		{
			Matrix33CalcPoint(&HitBB[i][j], OldInvRotationMat);
			Matrix33CalcPoint(&HitBB[i][j], CurrentRotationMat);
		}
	}

	// Пересчитываем OBB, просто поворачиваем его как и модель
	Matrix33CalcPoint(&OBBLocation, OldInvRotationMat);
	Matrix33CalcPoint(&OBBLocation, CurrentRotationMat);
	for (int j=0; j<8; j++)
	{
		Matrix33CalcPoint(&OBB[j], OldInvRotationMat);
		Matrix33CalcPoint(&OBB[j], CurrentRotationMat);
	}


	// по OBB находим AABB, это не совсем AABB (он будет больше), но зато быстро
	float MinX = 10000.0f;
	float MaxX = -10000.0f;
	float MinY = 10000.0f;
	float MaxY = -10000.0f;
	float MinZ = 10000.0f;
	float MaxZ = -10000.0f;
	// берем и проверяем
	for (int j=0; j<8; j++)
	{
		if (MinX > OBB[j].x + OBBLocation.x) MinX = OBB[j].x + OBBLocation.x;
		if (MinY > OBB[j].y + OBBLocation.y) MinY = OBB[j].y + OBBLocation.y;
		if (MinZ > OBB[j].z + OBBLocation.z) MinZ = OBB[j].z + OBBLocation.z;
		if (MaxX < OBB[j].x + OBBLocation.x) MaxX = OBB[j].x + OBBLocation.x;
		if (MaxY < OBB[j].y + OBBLocation.y) MaxY = OBB[j].y + OBBLocation.y;
		if (MaxZ < OBB[j].z + OBBLocation.z) MaxZ = OBB[j].z + OBBLocation.z;
	}
	AABB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
	AABB[1] = VECTOR3D(MinX, MaxY, MaxZ);
	AABB[2] = VECTOR3D(MinX, MaxY, MinZ);
	AABB[3] = VECTOR3D(MaxX, MaxY, MinZ);
	AABB[4] = VECTOR3D(MaxX, MinY, MaxZ);
	AABB[5] = VECTOR3D(MinX, MinY, MaxZ);
	AABB[6] = VECTOR3D(MinX, MinY, MinZ);
	AABB[7] = VECTOR3D(MaxX, MinY, MinZ);

}








//-----------------------------------------------------------------------------
// Прорисовка одной линии
//-----------------------------------------------------------------------------
void DrawLine(VECTOR3D Point1, VECTOR3D Point2, float ColorR, float ColorG, float ColorB, float ColorA)
{
	// буфер для последовательности RI_LINES
	// войдет RI_3f_XYZ | RI_4f_COLOR
	float *tmpDATA = 0;
	tmpDATA = new float[2*(3+4)];
	int k=0;

	tmpDATA[k++] = Point1.x;		// X
	tmpDATA[k++] = Point1.y;		// Y
	tmpDATA[k++] = Point1.z;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = Point2.x;		// X
	tmpDATA[k++] = Point2.y;		// Y
	tmpDATA[k++] = Point2.z;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;


	vw_SendVertices(RI_LINES, 2, RI_3f_XYZ | RI_4f_COLOR, tmpDATA, 7*sizeof(float));

	if (tmpDATA != 0){delete [] tmpDATA; tmpDATA = 0;}
}
//-----------------------------------------------------------------------------
// Прорисовка сетки бокса
//-----------------------------------------------------------------------------
void DrawBoxLines(VECTOR3D Point[8], VECTOR3D LocalLocation, float ColorR, float ColorG, float ColorB, float ColorA)
{
	vw_PushMatrix();
	vw_Translate(LocalLocation);


	DrawLine(Point[0], Point[1], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[1], Point[2], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[2], Point[3], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[3], Point[0], ColorR, ColorG, ColorB, ColorA);

	DrawLine(Point[4], Point[5], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[5], Point[6], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[6], Point[7], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[7], Point[4], ColorR, ColorG, ColorB, ColorA);

	DrawLine(Point[0], Point[4], ColorR, ColorG, ColorB, ColorA);

	DrawLine(Point[1], Point[5], ColorR, ColorG, ColorB, ColorA);

	DrawLine(Point[2], Point[6], ColorR, ColorG, ColorB, ColorA);

	DrawLine(Point[3], Point[7], ColorR, ColorG, ColorB, ColorA);


	vw_PopMatrix();
}







//-----------------------------------------------------------------------------
// Прорисовка объектa Object3D
//-----------------------------------------------------------------------------
void CObject3D::Draw()
{
	// если нечего рисовать - выходим
	if (DrawObjectList == 0) return;


	// оптимизация, если не в фруструме - соотв. и не видем его
	if (!vw_BoxInFrustum(Location + AABB[6], Location + AABB[0]))
	{
		// если показали а сейчас нет - установка флага
		if (ShowDeleteOnHide == 1) ShowDeleteOnHide = 2;


		return;
	}



	// показываем - нужно установить флаг
	if (ShowDeleteOnHide == 0) ShowDeleteOnHide = 1;
	// уже включили обратный отсчет на удаление - нужно его выключить
	if (ShowDeleteOnHide == 3)
	{
		ShowDeleteOnHide = 1;
		Lifetime = -1.0f;
	}



	// текущая текстура, чтобы не переставлять по 10 раз одно и то же
	eTexture *CurrentText = 0;
	// текущий шейдер, чтобы не ставить лишний раз
	eGLSL *CurrentGLSL = 0;
	// получаем матрицу, до всех преобразований
	float Matrix[16];
	vw_GetMatrix(RI_MODELVIEW_MATRIX, Matrix);

	vw_PushMatrix();

	// переносим и двигаем уже по данным всей модели
	vw_Translate(Location);
	vw_Rotate(0.0f, 0.0f, -Rotation.z);
	vw_Rotate(0.0f, -Rotation.y, 0.0f);
	vw_Rotate(-Rotation.x, 0.0f, 0.0f);


	// для корректной прорисовки на всех видеокартах атмосферы планеты ...
	bool N1 = false;
	for (int i=0; i<DrawObjectQuantity; i++)
	{
		if (DrawObjectList[i].DrawType == 1) N1 = true;
	}






	// если есть установка, нужно получить квадрат расстояния до камеры
	bool NeedPromptDraw = false;
	if (PromptDrawDist2 >= 0.0f)
	{
		VECTOR3D CurrentCameraLocation;
		vw_GetCameraLocation(&CurrentCameraLocation);
		float PromptDrawRealDist2 = (Location.x-CurrentCameraLocation.x)*(Location.x-CurrentCameraLocation.x)+(Location.y-CurrentCameraLocation.y)*(Location.y-CurrentCameraLocation.y)+(Location.z-CurrentCameraLocation.z)*(Location.z-CurrentCameraLocation.z);

		// получаем кол-во точечных источников, воздействующих на объект
		int LightsCount = vw_CheckAllPointLights(Location, Radius*Radius);

		// если дальше - смотрим сколько воздействует источников света
		if (PromptDrawRealDist2 > PromptDrawDist2)
		{
			// если больше заданного кол-ва
			if (LightsCount <= Setup.MaxPointLights) NeedPromptDraw = true;
			else NeedPromptDraw = false;

			// если это двигатели - не надо переходить
			if (InternalLights >= LightsCount) NeedPromptDraw = true;
		}
		else
		{
			// находимся близко, но нужно посмотреть, если кол-во источников ниже макс, надо перейти в упращенный режим
			if (LightsCount <= Setup.MaxPointLights) NeedPromptDraw = true;
		}
	}


	// Устанавливаем данные для поверхности объекта
	vw_MaterialV(RI_DIFFUSE, Diffuse);
	vw_MaterialV(RI_AMBIENT, Ambient);
	vw_MaterialV(RI_SPECULAR, Specular);
	vw_MaterialV(RI_SHININESS, Power);



	// если надо рисовать
	if (NeedPromptDraw)
	{
		vw_SetTextureDef(0);
		// если у нас есть 2-я, нужно скинуть установки
		if (TextureIllum!=0)
			if (TextureIllum[0]!=0)
				vw_SetTextureDef(1);

		vw_SetTextureT(0, Texture[0], Setup.AnisotropyLevel);

		// включаем вторую текстуру
		if (TextureIllum!=0)
		if (TextureIllum[0]!=0)
		{
			// свечение
			vw_SetTextureT(1, TextureIllum[0], Setup.AnisotropyLevel);
			vw_SetTextureBlendMode(1, RI_TBLEND_COLOROP, RI_TBLEND_ADD);
			vw_SetTextureBlendMode(1, RI_TBLEND_COLORARG1,  RI_TBLEND_CURRENT);
			vw_SetTextureBlendMode(1, RI_TBLEND_COLORARG2,  RI_TBLEND_TEXTURE);
		}

		// по умолчанию всегда трилинейная фильтрация

		// нужно ставить билинейную
		if (Setup.TextureFilteringMode == 1)
		{
			if (Texture[0] != 0)
				vw_SetTexFiltering(0, RI_TEXTURE_BILINEAR, Setup.AnisotropyLevel);
			if (TextureIllum != 0)
				if (TextureIllum[0] != 0)
					vw_SetTexFiltering(1, RI_TEXTURE_BILINEAR, Setup.AnisotropyLevel);
		}


		if (!NeedCullFaces) vw_CullFace(RI_NONE);


		int LightType1, LightType2;
		// включаем источники света, максимальное кол-во 4
		int LightsCount = vw_CheckAndActivateAllLights(&LightType1, &LightType2, Location, Radius*Radius, 2, Setup.MaxPointLights, Matrix);

		if (Setup.UseGLSL && ShaderType >= 0)
		{
			eGLSL *CurrentObject3DGLSL = 0;
			// ставим нужный шейдер
			// LightType1 1-2; LightType2 0-6
			CurrentObject3DGLSL = Object3DGLSL[LightType1-1][LightType2];

			if (CurrentObject3DGLSL != 0)
			{
				vw_UseShaderProgram(CurrentObject3DGLSL);

				vw_Uniform1i(CurrentObject3DGLSL, "tex1", 0);
				vw_Uniform1i(CurrentObject3DGLSL, "tex2", 1);
				if (TextureIllum != 0)
				{
					if (TextureIllum[0] != 0) vw_Uniform1i(CurrentObject3DGLSL, "NeedMultitexture", 1);
					else vw_Uniform1i(CurrentObject3DGLSL, "NeedMultitexture", 0);
				}
				else vw_Uniform1i(CurrentObject3DGLSL, "NeedMultitexture", 0);
			}
		}


		int GlobalVertexCount = 0;
		for (int i=0; i<DrawObjectQuantity; i++)
		{
			GlobalVertexCount += DrawObjectList[i].VertexCount;
		}

		vw_SendVertices(RI_TRIANGLES, GlobalVertexCount, DrawObjectList[0].FVF_Format, DrawObjectList[0].VertexBuffer,
						DrawObjectList[0].Stride*sizeof(float), DrawObjectList[0].VertexBufferVBO, 0,
						DrawObjectList[0].IndexBuffer, DrawObjectList[0].IndexBufferVBO);

		vw_DeActivateAllLights();
	}
	else
	{

		// установка текстур и подхотовка к прорисовке
		for (int i=0; i<DrawObjectQuantity; i++)
		{

			// небольшая проверка для конкретной части
			if (HitBB != 0 && HitBBLocation != 0)
			{
				VECTOR3D Min, Max;
				Min.x = Max.x = HitBB[i][0].x + HitBBLocation[i].x;
				Min.y = Max.y = HitBB[i][0].y + HitBBLocation[i].y;
				Min.z = Max.z = HitBB[i][0].z + HitBBLocation[i].z;
				// берем и проверяем
				for (int j=0; j<8; j++)
				{
					if (Min.x > HitBB[i][j].x + HitBBLocation[i].x) Min.x = HitBB[i][j].x + HitBBLocation[i].x;
					if (Min.y > HitBB[i][j].y + HitBBLocation[i].y) Min.y = HitBB[i][j].y + HitBBLocation[i].y;
					if (Min.z > HitBB[i][j].z + HitBBLocation[i].z) Min.z = HitBB[i][j].z + HitBBLocation[i].z;
					if (Max.x < HitBB[i][j].x + HitBBLocation[i].x) Max.x = HitBB[i][j].x + HitBBLocation[i].x;
					if (Max.y < HitBB[i][j].y + HitBBLocation[i].y) Max.y = HitBB[i][j].y + HitBBLocation[i].y;
					if (Max.z < HitBB[i][j].z + HitBBLocation[i].z) Max.z = HitBB[i][j].z + HitBBLocation[i].z;
				}

				if (!vw_BoxInFrustum(Location + Min, Location + Max))
				{
					continue;
				}
			}


			// работа с текстурами
			if (CurrentText != Texture[i])
			{
				vw_SetTextureDef(0);
				// если у нас есть 2-я, нужно скинуть установки
				if (TextureIllum!=0)
					if (TextureIllum[i]!=0)
						vw_SetTextureDef(1);

				vw_SetTextureT(0, Texture[i], Setup.AnisotropyLevel);

				// включаем вторую текстуру
				if (TextureIllum!=0)
				if (TextureIllum[i]!=0)
				{
					// свечение
					vw_SetTextureT(1, TextureIllum[i], Setup.AnisotropyLevel);
					vw_SetTextureBlendMode(1, RI_TBLEND_COLOROP, RI_TBLEND_ADD);
					vw_SetTextureBlendMode(1, RI_TBLEND_COLORARG1,  RI_TBLEND_CURRENT);
					vw_SetTextureBlendMode(1, RI_TBLEND_COLORARG2,  RI_TBLEND_TEXTURE);
				}
				// по умолчанию всегда трилинейная фильтрация

				// нужно ставить билинейную
				if (Setup.TextureFilteringMode == 1)
				{
					if (Texture[i] != 0)
						vw_SetTexFiltering(0, RI_TEXTURE_BILINEAR, Setup.AnisotropyLevel);
					if (TextureIllum != 0)
						if (TextureIllum[i] != 0)
							vw_SetTexFiltering(1, RI_TEXTURE_BILINEAR, Setup.AnisotropyLevel);
				}

				CurrentText = Texture[i];
			}



			vw_PushMatrix();

			// сдвигаем его в нужное место
			vw_Translate(DrawObjectList[i].Location);
			// поворачиваем объект
			vw_Rotate(0.0f, 0.0f, -DrawObjectList[i].Rotation.z);
			vw_Rotate(0.0f, -DrawObjectList[i].Rotation.y, 0.0f);
			vw_Rotate(-DrawObjectList[i].Rotation.x, 0.0f, 0.0f);


			if (!NeedCullFaces) vw_CullFace(RI_NONE);


			int LightType1, LightType2;
			// включаем источники света, максимальное кол-во 4
			int LightsCount;
			if (HitBB != 0)
				LightsCount = vw_CheckAndActivateAllLights(&LightType1, &LightType2, Location + HitBBLocation[i], HitBBRadius2[i], 2, Setup.MaxPointLights, Matrix);
			else
				LightsCount = vw_CheckAndActivateAllLights(&LightType1, &LightType2, Location, Radius*Radius, 2, Setup.MaxPointLights, Matrix);


			// если нужно рисовать прозрачным
			// для корректной прорисовки на всех видеокартах атмосферы планеты ...
			if (N1)
			{
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(2.0f, 2.0f);
			}
			if (DrawObjectList[i].DrawType == 1)
			{
				vw_SetTexAlpha(true, 0.01f);
				vw_SetTexBlend(RI_BLEND_SRCALPHA, RI_BLEND_ONE);
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0f, 1.0f);
			}



			if (Setup.UseGLSL && ShaderType >= 0)
			{
				eGLSL *CurrentObject3DGLSL = 0;
				// ставим нужный шейдер
				// LightType1 1-2; LightType2 0-6
				CurrentObject3DGLSL = Object3DGLSL[LightType1-1][LightType2];


				if (CurrentGLSL != CurrentObject3DGLSL)
				{
					if (CurrentGLSL != 0)
					{
						vw_StopShaderProgram();
						CurrentGLSL = 0;
					}

					CurrentGLSL == CurrentObject3DGLSL;

					if (CurrentObject3DGLSL != 0) vw_UseShaderProgram(CurrentObject3DGLSL);
				}
				// данные ставим каждый раз, т.к. может что-то поменяться
				if (CurrentObject3DGLSL != 0)
				{
					vw_Uniform1i(CurrentObject3DGLSL, "tex1", 0);
					vw_Uniform1i(CurrentObject3DGLSL, "tex2", 1);
					if (TextureIllum != 0)
					{
						if (TextureIllum[i] != 0) vw_Uniform1i(CurrentObject3DGLSL, "NeedMultitexture", 1);
						else vw_Uniform1i(CurrentObject3DGLSL, "NeedMultitexture", 0);
					}
					else vw_Uniform1i(CurrentObject3DGLSL, "NeedMultitexture", 0);

					// шейдеры взрывов
					if (ShaderType == 2)
					{
						vw_Uniform1f(CurrentObject3DGLSL, "SpeedData1", ShaderData[0]);
						vw_Uniform1f(CurrentObject3DGLSL, "SpeedData2", ShaderData[1]);
					}
				}

			}


			vw_SendVertices(RI_TRIANGLES, DrawObjectList[i].VertexCount, DrawObjectList[i].FVF_Format, DrawObjectList[i].VertexBuffer,
							DrawObjectList[i].Stride*sizeof(float), DrawObjectList[i].VertexBufferVBO,
							DrawObjectList[i].RangeStart, DrawObjectList[i].IndexBuffer, DrawObjectList[i].IndexBufferVBO);



			if (DrawObjectList[i].DrawType == 1)
			{
				glDisable(GL_POLYGON_OFFSET_FILL);
				// как было ставим
				glPolygonOffset(1.0f, 2.0f);
			}


			vw_DeActivateAllLights();

			vw_PopMatrix();
		}
	}


	// останавливаем шейдер, если был запущен
	if (Setup.UseGLSL)
	{
		vw_StopShaderProgram();
		CurrentGLSL = 0;
	}
	// установка параметров текстур в исходное состояние
	vw_SetTextureDef(1);
	vw_SetTextureDef(0);
	if (!NeedCullFaces) vw_CullFace(RI_BACK);
	vw_PopMatrix();







	// вывод отладочной информации, если она есть
	if (DebugInfo != 0)
	{
		DrawFont3D(Location.x, Location.y+AABB[0].y, Location.z, DebugInfo);
	}







/////////////////////
// рисуем боксы

	if (NeedShowBB>=1)
		// AABB объекта
		DrawBoxLines(AABB, Location, 1.0f, 0.0f, 0.0f, 1.0f);

	if (NeedShowBB>=2)
		// OBB объекта
		DrawBoxLines(OBB, Location+OBBLocation, 0.0f, 1.0f, 0.0f, 1.0f);

	if (NeedShowBB>=3)
		if (HitBB != 0)
		for (int i=0; i<DrawObjectQuantity; i++)
		{
			DrawBoxLines(HitBB[i], Location+HitBBLocation[i], 0.0f, 0.0f, 1.0f, 1.0f);
		}

///////////////////////







	// выводим "жизнь", если нужно
	if (!ShowStrength) return;
	if (StrengthStart <= 0.0f) return;
	if (Strength == StrengthStart && ShieldStrength == ShieldStrengthStart && !NeedShowStrengthNow) return;

	// раз тут, значит показали, и нужно теперь и дальше всегда показывать, даже если щит перезарядили
	NeedShowStrengthNow = true;



	// буфер для последовательности RI_TRIANGLE_STRIP
	// войдет RI_3f_XYZ | RI_4f_COLOR
	float *tmpDATA = 0;
	tmpDATA = new float[4*(3+4)];
	int k=0;

	// рисуем заднюю часть - основу
	float SizeX = Width/2.5f+0.1f;
	float SizeY = 0.35f;
	float ColorR = 0.0f;
	float ColorG = 0.0f;
	float ColorB = 0.0f;
	float ColorA = 1.0f;

	tmpDATA[k++] = -SizeX;		// X
	tmpDATA[k++] = -SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;


	tmpDATA[k++] = -SizeX;		// X
	tmpDATA[k++] = SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = SizeX;		// X
	tmpDATA[k++] = -SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = SizeX;		// X
	tmpDATA[k++] = SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;


	vw_PushMatrix();
	VECTOR3D CurrentCameraRotation;
	vw_GetCameraRotation(&CurrentCameraRotation);
	// поднимаем
	vw_Translate(VECTOR3D(Location.x, Location.y+AABB[0].y+SizeY*2.0f, Location.z));
	// поворачиваем к камере
	vw_Rotate(0, -180+CurrentCameraRotation.y, 0);
	vw_Rotate(-CurrentCameraRotation.x, 0, 0);



	vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR, tmpDATA, 7*sizeof(float));


	// рисуем вывод кол-ва жизни
	k=0;
	float SizeXStart = Width/2.5f - (2.0f*Width/2.5f)*Strength/StrengthStart;
	float SizeXEnd = Width/2.5f;
	SizeY = 0.25f;
	ColorR = 1.0f;
	ColorG = 0.0f;
	ColorB = 0.0f;
	ColorA = 1.0f;

	tmpDATA[k++] = SizeXStart;	// X
	tmpDATA[k++] = -SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = SizeXStart;	// X
	tmpDATA[k++] = SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = SizeXEnd;	// X
	tmpDATA[k++] = -SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = SizeXEnd;	// X
	tmpDATA[k++] = SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR, tmpDATA, 7*sizeof(float));

	vw_PopMatrix();



	// выводим щит, если он есть
	if (ShieldStrengthStart > 0.0f)
	{
		int k=0;

		// рисуем заднюю часть - основу
		float SizeX = Width/2.5f+0.1f;
		float SizeY = 0.35f;
		float ColorR = 0.0f;
		float ColorG = 0.0f;
		float ColorB = 0.0f;
		float ColorA = 1.0f;

		tmpDATA[k++] = -SizeX;		// X
		tmpDATA[k++] = -SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;


		tmpDATA[k++] = -SizeX;		// X
		tmpDATA[k++] = SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;

		tmpDATA[k++] = SizeX;		// X
		tmpDATA[k++] = -SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;

		tmpDATA[k++] = SizeX;		// X
		tmpDATA[k++] = SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;


		vw_PushMatrix();
		VECTOR3D CurrentCameraRotation;
		vw_GetCameraRotation(&CurrentCameraRotation);
		// поднимаем
		vw_Translate(VECTOR3D(Location.x, Location.y+AABB[0].y+SizeY*5.0f, Location.z));
		// поворачиваем к камере
		vw_Rotate(0, -180+CurrentCameraRotation.y, 0);
		vw_Rotate(-CurrentCameraRotation.x, 0, 0);



		vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR, tmpDATA, 7*sizeof(float));


		// рисуем вывод кол-ва жизни
		k=0;
		float SizeXStart = Width/2.5f - (2.0f*Width/2.5f)*ShieldStrength/ShieldStrengthStart;
		float SizeXEnd = Width/2.5f;
		SizeY = 0.25f;
		ColorR = 0.1f;
		ColorG = 0.7f;
		ColorB = 1.0f;
		ColorA = 1.0f;

		tmpDATA[k++] = SizeXStart;	// X
		tmpDATA[k++] = -SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;

		tmpDATA[k++] = SizeXStart;	// X
		tmpDATA[k++] = SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;

		tmpDATA[k++] = SizeXEnd;	// X
		tmpDATA[k++] = -SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;

		tmpDATA[k++] = SizeXEnd;	// X
		tmpDATA[k++] = SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;

		vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR, tmpDATA, 7*sizeof(float));

		vw_PopMatrix();
	}



	if (tmpDATA != 0){delete [] tmpDATA; tmpDATA = 0;}



	vw_SetTextureDef(0);
}





//-----------------------------------------------------------------------------
// Обновление данных объектa Object3D
//-----------------------------------------------------------------------------
bool CObject3D::Update(float Time)
{

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// начальные проверки-действия
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	// если еще ни разу не обновляли - просто запоминаем время
	if (TimeLastUpdate == -1.0f)
	{
		TimeLastUpdate = Time;
		return true;
	}


	// нужно удалить объект - он вышел из зоны видемости
	if (ShowDeleteOnHide == 2)
	{
		// показываем еще 1 секунду, и удаляем
		Lifetime = 1.0f;
		// если объект еще раз покажем, опять будем показывать 1 секунду с момента ухода со сцены
		ShowDeleteOnHide = 3;
	}


	// находим дельту по времени, и запоминаем время вызова
	TimeDelta = Time - TimeLastUpdate;
	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f) return true;

	TimeLastUpdate = Time;

	// проверяем, сколько объекту жить, если нужно...-1.0f  - проверка не нужна
	if (Lifetime > -1.0f)
	{
		// считаем, сколько осталось жить
		Lifetime -= TimeDelta;
		// если уже ничего не осталось - его нужно уничтожить
		if (Lifetime <= 0.0f) return false;
	}



	// проверка, зарядка щита если он есть
	if (ShieldStrengthStart > 0.0f)
	if (ShieldStrength < ShieldStrengthStart)
	{
		ShieldStrength += ShieldRecharge*TimeDelta;
		if (ShieldStrength > ShieldStrengthStart) ShieldStrength = ShieldStrengthStart;
	}



	// если есть, нужно обработать
	if (StartTimeSheet != 0)
	{

		// если это не вечная запись
		if (StartTimeSheet != 0)
		if (StartTimeSheet->Time > -1.0f)
		{
			StartTimeSheet->Time -= TimeDelta;
			// ставим первую запись
			if (StartTimeSheet->Time <= 0.0f)
			{
				// чтобы не переделать больше чем нужно (особенно при поворотах)
				if (StartTimeSheet->Time < 0.0f) TimeDelta -= StartTimeSheet->Time;

				CTimeSheet* TmpTimeSheet = StartTimeSheet;
				// отсоединяем от списка
				DetachTimeSheet(StartTimeSheet);
				// удаляем
				delete TmpTimeSheet; TmpTimeSheet = 0;
			}
		}


		// нужно проверить, если это упаковынные действия, нужно распаковать
		if (StartTimeSheet != 0)
		if (StartTimeSheet->AI_Mode != 0)
		{
			// передаем на распаковку
			InterAIMode(this, StartTimeSheet);
			// удаляем эту запись, мы ее уже распаковали
			CTimeSheet* TmpTimeSheet = StartTimeSheet;
			DetachTimeSheet(StartTimeSheet);
			delete TmpTimeSheet; TmpTimeSheet = 0;
		}
	}




	// объект в порядке - удалять не нужно
	return true;
}






//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void CObject3D::AttachTimeSheet(CTimeSheet* TimeSheet)
{
	if (TimeSheet == 0) return;

	// первый в списке...
	if (EndTimeSheet == 0)
	{
		TimeSheet->Prev = 0;
		TimeSheet->Next = 0;
		StartTimeSheet = TimeSheet;
		EndTimeSheet = TimeSheet;
	}
	else // продолжаем заполнение...
	{
		TimeSheet->Prev = EndTimeSheet;
		TimeSheet->Next = 0;
		EndTimeSheet->Next = TimeSheet;
		EndTimeSheet = TimeSheet;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void CObject3D::DetachTimeSheet(CTimeSheet* TimeSheet)
{
	if (TimeSheet == 0) return;

	// переустанавливаем указатели...
	if (StartTimeSheet == TimeSheet) StartTimeSheet = TimeSheet->Next;
	if (EndTimeSheet == TimeSheet) EndTimeSheet = TimeSheet->Prev;


	if (TimeSheet->Next != 0) TimeSheet->Next->Prev = TimeSheet->Prev;
		else if (TimeSheet->Prev != 0) TimeSheet->Prev->Next = 0;
	if (TimeSheet->Prev != 0) TimeSheet->Prev->Next = TimeSheet->Next;
		else if (TimeSheet->Next != 0) TimeSheet->Next->Prev = 0;
}



