/*******************************************************************************
* �^�C�g��	:�^�C�g���C���^�[�t�F�[�X.h
* �t�@�C����:titleGui.h
* �쐬��	:�c���t��
* �쐬��	:2015/06/24
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/
#ifndef _TITLEGUI_H_
#define _TITLEGUI_H_
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
* �֐���	:HRESULT InitTitleGui( void )
*		�^�C�g���C���^�[�t�F�[�X�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitTitleGui( void );

/*******************************************************************************
* �֐���	:void UninitTitleGui( void )
*		�^�C�g���C���^�[�t�F�[�X�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitTitleGui( void );

/*******************************************************************************
* �֐���	:void UpdateTitleGui( void )
*		�^�C�g���C���^�[�t�F�[�X�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateTitleGui( void );

/*******************************************************************************
* �֐���	:void DrawTitleGui( void )
*		�^�C�g���C���^�[�t�F�[�X�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawTitleGui( void );

#endif