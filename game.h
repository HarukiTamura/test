/*******************************************************************************
* �^�C�g��	:�Q�[��.h
* �t�@�C����:game.h
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/
#ifndef _GAME_H_
#define _GAME_H_
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "input.h"
#include "sound.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*�t�B�[���h�T�C�Y�F��ɃQ�[�����i�s����t�B�[���h�̕����`���܂��B*/

/*float��*/
#define FIELD_WIDTH		( SCREEN_WIDTH * 0.70 )	/*�t�B�[���h�̕�*/
#define FIELD_HEIGHT	( SCREEN_HEIGHT )		/*�t�B�[���h�̍���*/
												  
#define FIELD_START_X	( SCREEN_WIDTH * 0.05 )	/*�t�B�[���h�̎n�_X*/
#define FIELD_START_Y	( SCREEN_TOP )			/*�t�B�[���h�̎n�_Y*/

#define FIELD_TOP		( FIELD_START_Y )					/*�t�B�[���h�̏���E*/
#define FIELD_BOTTOM	( FIELD_START_Y + FIELD_HEIGHT )	/*�t�B�[���h�̉����E*/
#define FIELD_LEFT		( FIELD_START_X )					/*�t�B�[���h�̍����E*/
#define FIELD_RIGHT		( FIELD_START_X + FIELD_WIDTH )		/*�t�B�[���h�̉E���E*/

/*�����y��int��*/
#define INT_FIELD_WIDTH		( 1000 * FIELD_WIDTH )		/*�t�B�[���h�̕��F(��ʂ̕���0.5��*1000=50)*/
#define INT_FIELD_HEIGHT	( 1000 * FIELD_HEIGHT )		/*�t�B�[���h�̍���*/
									 			
#define INT_FIELD_START_X	( 1000 * FIELD_START_X )	/*�t�B�[���h�̎n�_X*/
#define INT_FIELD_START_Y	( 1000 * FIELD_START_Y )	/*�t�B�[���h�̎n�_Y*/
		
#define INT_FIELD_TOP		( 1000 * ( FIELD_START_Y ) )					/*�t�B�[���h�̏���E*/
#define INT_FIELD_BOTTOM	( 1000 * ( FIELD_START_Y + FIELD_HEIGHT ) )	/*�t�B�[���h�̉����E*/
#define INT_FIELD_LEFT		( 1000 * ( FIELD_START_X ) )					/*�t�B�[���h�̍����E*/
#define INT_FIELD_RIGHT		( 1000 * ( FIELD_START_X + FIELD_WIDTH ) )	/*�t�B�[���h�̉E���E*/
#define INT_FIELD_CENTER_X	( 1000 * ( FIELD_START_X + ( FIELD_WIDTH / 2 ) ) )	/*�t�B�[���h��X����*/
#define INT_FIELD_CENTER_Y	( 1000 * ( FIELD_START_Y + ( FIELD_HEIGHT / 2 ) ) )	/*�t�B�[���h��Y����*/

typedef enum
{
	SEQUENCE_PLAY = 0,
	SEQUENCE_COUNTDOWN,
	SEQUENCE_PAUSE,
	SEQUENCE_CONTINUE,
	SEQUENCE_GAMEOVER
}GAME_SEQUENCE;

typedef enum
{
	CATEGORY_PLAYER,
	CATEGORY_PLAYERHYPER,	//�Ǝ��v�f�F�n�C�p�[���[�h�e
	CATEGORY_ENEMY,
	CATEGORY_NEUTRAL,
	CATEGORY_ANTIBREAK,	//�G�t�F�N�g����j����
	CATEGORY_LIAR,		//�Ǝ��v�f�F�R����
	CATEGORY_MAX
}CATEGORY;
/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �֐���	:HRESULT InitGame( void )
*		�Q�[���������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitGame( void );

/*******************************************************************************
* �֐���	:void UninitGame( void )
*		�Q�[���I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitGame( void );

/*******************************************************************************
* �֐���	:void UpdateGame( void )
*		�Q�[���X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateGame( void );

/*******************************************************************************
* �֐���	:void DrawGame( void )
*		�Q�[���`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawGame( void );

#endif