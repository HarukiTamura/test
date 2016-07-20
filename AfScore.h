/*******************************************************************************
* �^�C�g��	:�X�R�A.h
* �t�@�C����:afScore.h
* �쐬��	:�c���t��
* �쐬��	:2015/06/24
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/
#ifndef _AF_SCORE_H_
#define _AF_SCORE_H_
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
* �֐���	:HRESULT InitAfScore( void )
*		�X�R�A�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfScore( void );

/*******************************************************************************
* �֐���	:void UninitAfScore( void )
*		�X�R�A�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfScore( void );

/*******************************************************************************
* �֐���	:void UpdateAfScore( void )
*		�X�R�A�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfScore( void );

/*******************************************************************************
* �֐���	:void DrawAfScore( void )
*		�X�R�A�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfScore( void );

/*******************************************************************************
* �֐���	:void PlusAfScore( long plusNum )
*		�X�R�A���Z�֐��ł��B
* ����		:long plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z���܂��B
*******************************************************************************/
void PlusAfScore( long plusNum );

/*******************************************************************************
* �֐���	:resetAfScore( unsigned long resetNum )
*		�X�R�A���Z�b�g�֐��ł��B
* ����		:unsigned long	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z�b�g���܂��B
*******************************************************************************/
void resetAfScore( unsigned long resetNum );

/*******************************************************************************
* �֐���	:unsigned long  getAfScore( void )
*		�X�R�A�Q�b�g�֐��ł��B
* ����		:void	�����Ȃ�
* �߂�l	:unsigned long nowAfScore	���݂̃X�R�A
* ����		:�X�R�A���擾���܂��B
*******************************************************************************/
unsigned long getAfScore( void );

#endif