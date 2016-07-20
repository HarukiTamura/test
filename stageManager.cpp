/*******************************************************************************
* �^�C�g��	:�X�e�[�W�}�l�[�W���[.cpp
* �t�@�C����:StageManager.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "stageManager.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

#include "backGround.h"
#include "backGround2.h"
#include "backGround3.h"
#include "backGround4.h"
#include "backGround5.h"
#include "player01.h"
#include "barrier.h"

#include "explosion.h"
#include "enemy.h"
#include "bullet.h"
#include "timer.h"

#include "hystericGauge.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define FPS_MAX	( 99999999 )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static long long nowFps = 0;
static bool clearFlag = false;

/*******************************************************************************
* �֐���	:HRESULT InitStageManager( void )
*		�X�e�[�W�}�l�[�W���[�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitStageManager( void )
{
	clearFlag = false;
	resetTimer( 0 );
	nowFps = 0;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitStageManager( void )
*		�X�e�[�W�}�l�[�W���[�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitStageManager( void )
{
	
}

/*******************************************************************************
* �֐���	:bool UpdateStageManager( void )
*		�X�e�[�W�}�l�[�W���[�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:bool clearFlag	�X�e�[�W�N���A�������ǂ����̃t���O
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
bool UpdateStageManager( void )
{
	nowFps = ( nowFps + 1 >= FPS_MAX ? FPS_MAX : nowFps + 1 );
	if( nowFps % 175 == 12 )
	{
		SetEnemy( 400000 - ( rand() % 2000 ) , 100000 - ( rand() % 2000 ) , 0 , 0 , 0 , 0 , 20 , CATEGORY_ENEMY , ENEMY_002 , ENEMY_AI_0004 );
	}
	if( nowFps % 175 == 4 )
	{
		SetEnemy( 200000 - ( rand() % 2000 ) , 100000 - ( rand() % 2000 ) , 0 , 0 , 500 , 0 , 20 , CATEGORY_ENEMY , ENEMY_002 , ENEMY_AI_0001 );
	}
	if( nowFps >= 1800 )//����3600�b��clear�ɂ��Ă����B
	{
		clearFlag = true;
	}

#ifdef _DEBUG
	if( GetKeyboardTrigger( DIK_E) )
	{
		SetBulletPlayer( 500000 , 12 , 0 , 0 , -10000 , 0 , 12 );
	}
	if( GetKeyboardTrigger( DIK_R ) )
	{
		SetBulletLiar( 500000 ,12, 0 , 0 , -10000 , 0 , 12 , BULLET_HOMING );
	}
	if( GetKeyboardTrigger( DIK_1 ) )
	{
		SetEnemy( 400000 , 100000 , 0 ,0 , 0 , 0 , 50 , CATEGORY_ENEMY , ENEMY_002 );
		//	SetEnemyEnemy( 400000 , 100000 , 0 ,0 , 0 , 0 , 12 );
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
	}
	
	if( GetKeyboardTrigger( DIK_2 ) )
	{
		SetEnemy( 400000 , 100000 , 0 ,0 , 0 , 0 , 50 , CATEGORY_ENEMY , ENEMY_002 );
		//	SetEnemyEnemy( 400000 , 100000 , 0 ,0 , 100 , 0 , 12 );
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
	}
	
	if( GetKeyboardPress( DIK_T ) )
	{
		for( int i = 0; i<= 40; i++ )
		{
			SetBulletEnemy( 400000 , 100 , 0 , 5900 * sin( D3DX_PI * 0.05 * i + 0.01 ) , 5900  * cos( D3DX_PI * 0.05 * i + 0.01 ) , 0 , 12 );
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
		}
	}
	if( GetKeyboardPress( DIK_Y ) )
	{
		for( int i = 0; i<= 40; i++ )
		{
			SetBulletLiar( 400000 , 30000 , 0 , 5900 * sin( D3DX_PI * 0.05 * i + 0.01 ) , 5900  * cos( D3DX_PI * 0.05 * i + 0.01 ) , 0 , 12 , BULLET_NORMAL);
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
		}
	}
#endif
	return clearFlag;
}

/*******************************************************************************
* �֐���	:void DrawStageManager( void )
*		�X�e�[�W�}�l�[�W���[�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawStageManager( void )
{
	
}

/*******************************************************************************
* �֐���	:int getStageFps( void )
*		�o��FPS�Q�b�g�֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:int nowFps		���݂̌o��FPS
* ����		:�o��FPS���Q�b�g���܂��B
*******************************************************************************/
int getStageFps( void )
{
	return nowFps;
}