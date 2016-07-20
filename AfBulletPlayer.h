/*******************************************************************************
* �^�C�g��	:���e.h
* �t�@�C����:AfBulletPlayer.h
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/
#ifndef _AF_BULLET_PLAYER_H_
#define _AF_BULLET_PLAYER_H_
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "afPointConvert.h"
/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define BULLET_PLAYER_MAX_NUM	( 100 )	//���e�̐�
#define BULLET_PLAYER_RADIUS	( 10 )	//�����蔻��

typedef enum
{
	BULLET_PLAYER_MODE_NORMAL = 0,
	BULLET_PLAYER_MODE_DAMAGE,
	BULLET_PLAYER_MODE_HYPERMODE,
	BULLET_PLAYER_MODE_OVERHEAT,
	BULLET_PLAYER_MODE_START,
	BULLET_PLAYER_MODE_MAX
}BULLET_PLAYER_MODE;


/*******************************************************************************
* �\���̒�`
*******************************************************************************/
typedef struct
{
	bool bUse;
	AF_OBJECT_STATUS posStatus;
	float moveZ;
	float posZ;
	int intPosX;
	int intPosY;
	int nLife;
}BULLET_PLAYER_STATUS;

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �֐���	:HRESULT InitAfBulletPlayer( void )
*		���e�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfBulletPlayer( void );

/*******************************************************************************
* �֐���	:HRESULT InitDirectAfBulletPlayer( void )
*		���e�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitDirectAfBulletPlayer( void );

/*******************************************************************************
* �֐���	:HRESULT InitValueAfBulletPlayer( void )
*		���e�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void InitValueAfBulletPlayer( void );

/*******************************************************************************
* �֐���	:void UninitAfBulletPlayer( void )
*		���e�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfBulletPlayer( void );

/*******************************************************************************
* �֐���	:void UpdateAfBulletPlayer( void )
*		���e�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfBulletPlayer( void );

/*******************************************************************************
* �֐���	:void DrawAfBulletPlayer( void )
*		���e�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfBulletPlayer( void );

/*******************************************************************************
* �֐���	:void DrawAfBulletPlayercore( void )
*		���e�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfBulletPlayercore( void );

/*******************************************************************************
* �֐���	:BULLET_PLAYER_STATUS* GetBulletPlayerState( int num )
*		���e�̃X�e�[�^�X���擾���܂��B
* ����		:int num	���肷��\���̂̔ԍ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:���e�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
BULLET_PLAYER_STATUS* GetBulletPlayerState( int num );

/*******************************************************************************
* �֐���	:BULLET_PLAYER_STATUS* GetBulletPlayerState( int num )
*		���e�̃X�e�[�^�X���擾���܂��B
* ����		:int num	���肷��\���̂̔ԍ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:���e�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
void SetBulletPlayerState( float posX , float posY , float posZ , float moveZ );

#endif