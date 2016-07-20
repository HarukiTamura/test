/*******************************************************************************
* �^�C�g��	:�J�[�\��.h
* �t�@�C����:AfCursor.h
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/
#ifndef _AF_CURSOR_H_
#define _AF_CURSOR_H_
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "afPointConvert.h"
/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define CURSOR_RADIUS ( 5000 )	//�����蔻��

typedef enum
{
	CURSOR_MODE_NORMAL = 0,
	CURSOR_MODE_LOCKON,
	CURSOR_MODE_MAX
}CURSOR_MODE;

/*******************************************************************************
* �\���̒�`
*******************************************************************************/
typedef struct
{
	AF_OBJECT_STATUS posStatus;
	int intPosX;
	int intPosY;
	CURSOR_MODE mode;
}CURSOR;

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �֐���	:HRESULT InitAfCursor( void )
*		�J�[�\���������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfCursor( void );

/*******************************************************************************
* �֐���	:HRESULT InitDirectAfCursor( void )
*		�J�[�\���������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitDirectAfCursor( void );

/*******************************************************************************
* �֐���	:HRESULT InitValueAfCursor( void )
*		�J�[�\���������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void InitValueAfCursor( void );

/*******************************************************************************
* �֐���	:void UninitAfCursor( void )
*		�J�[�\���I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfCursor( void );

/*******************************************************************************
* �֐���	:void UpdateAfCursor( void )
*		�J�[�\���X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfCursor( void );

/*******************************************************************************
* �֐���	:void DrawAfCursor( void )
*		�J�[�\���`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfCursor( void );

/*******************************************************************************
* �֐���	:void DrawAfCursorcore( void )
*		�J�[�\���`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfCursorcore( void );

/*******************************************************************************
* �֐���	:CURSOR* GetCursorState( void )
*		�J�[�\���̃X�e�[�^�X���擾���܂��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�J�[�\���̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
CURSOR* GetCursorState( void );

#endif