/*******************************************************************************
* �^�C�g��	:���j���[.h
* �t�@�C����:menu.h
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/
#ifndef _MENU_H_
#define _MENU_H_
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "input.h"
#include "sound.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/
typedef enum{
	MENU_ARCADE,
	MENU_STORY,
	MENU_OPTION,
	MENU_EXIT,
	MENU_MAX
}MENU_COMMAND;
/*******************************************************************************
* �֐���	:HRESULT InitMenu( void )
*		���j���[�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitMenu( void );

/*******************************************************************************
* �֐���	:void UninitMenu( void )
*		���j���[�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitMenu( void );

/*******************************************************************************
* �֐���	:void UpdateMenu( void )
*		���j���[�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateMenu( void );

/*******************************************************************************
* �֐���	:void DrawMenu( void )
*		���j���[�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawMenu( void );

#endif