/*******************************************************************************
* �^�C�g��	:�w�i.cpp
* �t�@�C����:afPointConvert.cpp
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
#include "input.h"
#include "unoDirectX.h"
#include "unoHitChecker.h"
#include "afPointConvert.h"
#include "AfPlayer.h"
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
* �O���[�o���ϐ�
*******************************************************************************/
//�`���ʒu
static AF_VIEW_STATUS g_NowViewStatus;
/*******************************************************************************
* �֐���	:HRESULT InitAfPointConvert( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfPointConvert( void )
{
	InitValueAfPointConvert();
	return S_OK;
}

/*******************************************************************************
* �֐���	:HRESULT InitValueAfPointConvert( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void InitValueAfPointConvert( void )
{
	g_NowViewStatus.centerPosX = GetPlayerState()->posStatus.DrawPosX;
	g_NowViewStatus.centerPosY = GetPlayerState()->posStatus.DrawPosY;
	g_NowViewStatus.centerVector = GetPlayerState()->posStatus.DrawVector;
}

/*******************************************************************************
* �֐���	:void UninitAfPointConvert( void )
*		�w�i�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfPointConvert( void )
{
}

/*******************************************************************************
* �֐���	:void UpdateAfPointConvert( void )
*		�w�i�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfPointConvert( void )
{
	g_NowViewStatus.centerPosX = GetPlayerState()->posStatus.DrawPosX;
	g_NowViewStatus.centerPosY = GetPlayerState()->posStatus.DrawPosY;
	g_NowViewStatus.centerVector = GetPlayerState()->posStatus.DrawVector;
}

/*******************************************************************************
* �֐���	:void DrawAfPointConvert( void )
*		�w�i�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfPointConvert( void )
{
#ifdef _DEBUG
	/*char txt[60];
	RECT rectW = { ( LONG )( 0 ) , ( LONG )( 100 ) , WINDOW_WIDTH , WINDOW_HEIGHT };
	sprintf( &txt[0] , "veiwX:%3.6f \nviewY:%3.6f \nviewV:%3.6f", 
		g_NowViewStatus.centerPosX , g_NowViewStatus.centerPosY , g_NowViewStatus.centerVector );
	getFont()->DrawText( NULL , &txt[0] , -1 , &rectW , DT_LEFT , D3DCOLOR_RGBA( 255 , 222 , 222 , 255 ) );
	*/
#endif
}


/*******************************************************************************
* �֐���	:void DrawAfPointConvert( void )
*		�w�i�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void AfPointConvert( AF_OBJECT_STATUS convertStatus )
{
	/*convertStatus.posX =
	convertStatus.
	convertStatus.*/
}

/*******************************************************************************
* �֐���	:AF_VIEW_STATUS* getViewStatus( void )
*		�r���[���\���̂�n�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:AF_VIEW_STATUS*�@g_NowViewStatus�̃A�h���X
* ����		:�r���[���\���̂�n���֐��ł��B
*******************************************************************************/
AF_VIEW_STATUS* getViewStatus( void )
{
	return &g_NowViewStatus;
}