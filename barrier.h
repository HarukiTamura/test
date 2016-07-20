/*******************************************************************************
* �^�C�g��	:�o���A�[.h
* �t�@�C����:barrier.h
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/
#ifndef _BARRIER_H_
#define _BARRIER_H_
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define BARRIER_RADIUS	( 150 )		/*���E�g�厞���a*/

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �֐���	:HRESULT InitBarrier( void )
*		�o���A�[�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitBarrier( void );

/*******************************************************************************
* �֐���	:void UninitBarrier( void )
*		�o���A�[�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitBarrier( void );

/*******************************************************************************
* �֐���	:void UpdateBarrier( void )
*		�o���A�[�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateBarrier( void );

/*******************************************************************************
* �֐���	:void DrawBarrier( void )
*		�o���A�[�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawBarrier( void );

/*******************************************************************************
* �֐���	:int GetBarrierRadiusInt( void )
*		�o���A�[���a��n�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:int�@�o���A���aint�ϊ��l
* ����		:�o���A�[�̔��a��n���֐��ł��B
*******************************************************************************/
int GetBarrierRadiusInt( void );

#endif