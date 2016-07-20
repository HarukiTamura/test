/*******************************************************************************
* �^�C�g��	:���쓖���蔻��cpp
* �t�@�C����:unoHitChecker.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/07/05
********************************************************************************
* �X�V����	ver 1.00 �����悻�̊���
*
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "unoHitChecker.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐���	:HitTwinCircle( int posX1 , int posY1 , int radius1 , int posX2 , int posY2 , int radius2 )
*		�~�Ɖ~�̓����蔻����s���܂��B
* ����		:int posX1		�~�P��X���W
*			 int posY1		�~�P��Y���W
*			 int radius1	�~�P�̔��a
*			 int posX2		�~�Q��X���W
*			 int posY2		�~�Q��Y���W
*			 int radius2	�~�Q�̔��a
* �߂�l	:bool true:�ڐG false:��ڐG
* ����		:�~�Ɖ~�̓����蔻����s���܂��B
*******************************************************************************/
bool HitTwinCircle( int posX1 , int posY1 , int radius1 , int posX2 , int posY2 , int radius2 )
{
	double divisionX = 0, divisionY = 0, radiusPlus = 0;
	
	divisionX = posX2 - posX1;
	divisionY = posY2 - posY1;
	radiusPlus = radius1 + radius2;

	if( divisionX * divisionX + divisionY * divisionY <= radiusPlus * radiusPlus )
	{
		return true;
	}
	return false;
}


/*******************************************************************************
* �֐���	:bool HitTwinLine( int pastPosX1 , int pastPosY1 , int posX1 , int posY1 , int pastPosX2 , int pastPosY2 , int posX2 ,int posY2 )
*		�����Ɛ����̓����蔻����s���܂��B
* ����		:int pastPosX1	�����P�̎n�_X���W
*			 int pastPosY1	�����P�̎n�_Y���W
*			 int posX1		�����P�̏I�_X���W
*			 int posY1		�����P�̏I�_Y���W
*			 int pastPosX2	�����Q�̎n�_X���W		
*			 int pastPosY2	�����Q�̎n�_Y���W		
*			 int posX2		�����Q�̏I�_X���W		
*			 int posY2		�����Q�̏I�_Y���W		
* �߂�l	:bool true:�ڐG false:��ڐG
* ����		:�����Ɛ����̓����蔻����s���܂��B
*******************************************************************************/
bool HitTwinLine( int pastPosX1 , int pastPosY1 , int posX1 , int posY1 , int pastPosX2 , int pastPosY2 , int posX2 ,int posY2 )
{
	if( ( ( posX1 - pastPosX1 ) * ( pastPosY2 - pastPosY1 ) - ( posY1 - pastPosY1 ) * ( pastPosX2 - pastPosX1 ) )
	* ( ( posX1 - pastPosX1 ) * ( posY2 - pastPosY1 ) - ( posY1 - pastPosY1 ) * ( posX2 - pastPosX1 ) ) < 0 &&
	( ( posX2 - pastPosX2 ) * ( pastPosY1 - pastPosY2 ) - ( posY2 - pastPosY2 ) * ( pastPosX1 - pastPosX2 ) )
	* ( ( posX2 - pastPosX2 ) * ( posY1 - pastPosY2 ) - ( posY2 - pastPosY2 ) * ( posX1 - pastPosX2 ) ) < 0 )
	{
		return true;
	}
	return false;
}

/*******************************************************************************
* �֐���	:bool HitTwinLine( int pastPosX1 , int pastPosY1 , int posX1 , int posY1 , int pastPosX2 , int pastPosY2 , int posX2 ,int posY2 )
*		�����Ɛ����̓����蔻����s���܂��B
* ����		:int pastPosX1	�����P�̎n�_X���W
*			 int pastPosY1	�����P�̎n�_Y���W
*			 int posX1		�����P�̏I�_X���W
*			 int posY1		�����P�̏I�_Y���W
*			 int pastPosX2	�����Q�̎n�_X���W		
*			 int pastPosY2	�����Q�̎n�_Y���W		
*			 int posX2		�����Q�̏I�_X���W		
*			 int posY2		�����Q�̏I�_Y���W		
* �߂�l	:bool true:�ڐG false:��ڐG
* ����		:�����Ɛ����̓����蔻����s���܂��B
*******************************************************************************/
bool HitPosTwinLine( float posX1 , float posY1 , float posX2 , float posY2 , float posX3 , float posY3 , float posX4 ,float posY4 , float *closeX , float *closeY )
{
	float ksi = 0, eta = 0, delta = 0;
	float ramda = 0, mu = 0;

	ksi = ( posY4 - posY3 ) * ( posX3 - posX1 ) - ( posX4 - posX3 ) * ( posX4 - posX1 );

	eta = ( posX2 - posX1 ) * ( posY4 - posY1 ) - ( posY2 - posY1 ) * ( posX4 - posX1 );

	delta = ( posX2 - posX1 ) * ( posY4 - posY3 ) - ( posY2 - posY1 ) * ( posX4 - posX3 );

	ramda = ksi / delta;

	mu = eta / delta;

	if( ( ramda >= 0 && ramda <= 1 ) && ( mu >= 0 && mu <= 1 ) )
	{
		*closeX = posX1 + ramda * ( posX2 - posX1 );
		*closeY = posY1 + ramda * ( posY2 - posY1 );
		return true;
	}

	return false;
}