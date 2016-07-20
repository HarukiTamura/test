/*******************************************************************************
* �^�C�g��	:�X�R�A.h
* �t�@�C����:afAfScore.h
* �쐬��	:�c���t��
* �쐬��	:2015/06/24
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/
#ifndef _AF_SCORE2_H_
#define _AF_SCORE2_H_
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "game.h"

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
* �֐���	:HRESULT InitAfScore2( void )
*		�X�R�A�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfScore2( void );

/*******************************************************************************
* �֐���	:void UninitAfScore2( void )
*		�X�R�A�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfScore2( void );

/*******************************************************************************
* �֐���	:void UpdateAfScore2( void )
*		�X�R�A�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfScore2( void );

/*******************************************************************************
* �֐���	:void DrawAfScore2( void )
*		�X�R�A�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfScore2( void );

/*******************************************************************************
* �֐���	:void PlusAfScore2( unsigned long plusNum )
*		�X�R�A���Z�֐��ł��B
* ����		:unsigned long plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z���܂��B
*******************************************************************************/
void PlusAfScore2( unsigned long plusNum );

/*******************************************************************************
* �֐���	:resetAfScore2( unsigned long resetNum )
*		�X�R�A���Z�b�g�֐��ł��B
* ����		:unsigned long resetNum	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z�b�g���܂��B
*******************************************************************************/
void resetAfScore2( unsigned long resetNum );

#endif