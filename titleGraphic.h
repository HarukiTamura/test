/*******************************************************************************
* �^�C�g��	:�^�C�g���O���t�B�b�N.h
* �t�@�C����:TitleGraphic.h
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/
#ifndef _TITLE_GRAPHIC_H_
#define _TITLE_GRAPHIC_H_
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"

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
* �֐���	:HRESULT InitTitleGraphic( void )
*		�^�C�g���O���t�B�b�N�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitTitleGraphic( void );

/*******************************************************************************
* �֐���	:void UninitTitleGraphic( void )
*		�^�C�g���O���t�B�b�N�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitTitleGraphic( void );

/*******************************************************************************
* �֐���	:void UpdateTitleGraphic( void )
*		�^�C�g���O���t�B�b�N�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateTitleGraphic( void );

/*******************************************************************************
* �֐���	:void DrawTitleGraphic( void )
*		�^�C�g���O���t�B�b�N�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawTitleGraphic( void );

#endif