/*******************************************************************************
* �^�C�g��	:���j���[.cpp
* �t�@�C����:Menu.cpp
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
#include "menu.h"
#include "input.h"
#include "fade.h"

#include "menuBackGround.h"
#include "menuTxt.h"
#include "menuCursor.h"

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
static int g_menuCommand;
static int g_nKeyPressFps;
/*******************************************************************************
* �֐���	:HRESULT InitMenu( void )
*		���j���[�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitMenu( void )
{
	PlaySound(SOUND_LABEL_BGM_MENU);

	setRepeatFps( 60 );
	g_nKeyPressFps = 0;

	g_menuCommand = MENU_ARCADE;
	/*�w�i�̐ݒ�*/
	if( InitMenuBackGround() != S_OK )
	{
		Error("�w�i�f�[�^�̏������Ɏ��s���܂����B");
	}
	/*���j���[�e�L�X�g�̐ݒ�*/
	if( InitMenuTxt() != S_OK )
	{
		Error("���j���[�e�L�X�g�̏������Ɏ��s���܂����B");
	}
	/*���j���[�J�[�\���̐ݒ�*/
	if( InitMenuCursor() != S_OK )
	{
		Error("���j���[�J�[�\���̏������Ɏ��s���܂����B");
	}
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitMenu( void )
*		���j���[�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitMenu( void )
{
	setRepeatFps();

	/*�w�i�̊J��*/
	UninitMenuBackGround();

	/*���j���[�J�[�\���̊J��*/
	UninitMenuCursor();

	/*���j���[�e�L�X�g�̊J��*/
	UninitMenuTxt();
}

/*******************************************************************************
* �֐���	:void UpdateMenu( void )
*		���j���[�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateMenu( void )
{
	int nID = 0;
	bool exitFlag = false;
	
	if( GetKeyboardRepeat( DIK_DOWN ) || GetKeyboardRepeat( DIK_S ) || GetKeyboardRepeat( DIK_UP ) || GetKeyboardRepeat( DIK_W ) ||
		GetGamePadAngleRepeat( GP_ANGLE_UP ) || GetGamePadAngleRepeat( GP_ANGLE_DOWN ) )
	{	
		if( g_nKeyPressFps % 10 == 0 )
		{
			if( GetKeyboardPress( DIK_DOWN ) || GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
			{
				g_menuCommand = ( g_menuCommand + 1 >= MENU_MAX ? 0 : g_menuCommand + 1 );
			}
			if( GetKeyboardPress( DIK_UP ) || GetKeyboardPress( DIK_W ) || GetGamePadAnglePress( GP_ANGLE_UP ) )
			{
				g_menuCommand = ( g_menuCommand - 1 < 0 ? MENU_MAX - 1 : g_menuCommand - 1 );
			}
		}
		g_nKeyPressFps = ( g_nKeyPressFps + 1 == 60 ? 0 : g_nKeyPressFps + 1 );
	}
	else
	{	
		if( GetKeyboardTrigger( DIK_DOWN ) || GetKeyboardTrigger( DIK_S ) || GetGamePadAngleTrigger( GP_ANGLE_DOWN ) )
		{
			g_menuCommand = ( g_menuCommand + 1 >= MENU_MAX ? 0 : g_menuCommand + 1 );
		}
		else if( GetKeyboardTrigger( DIK_UP ) || GetKeyboardTrigger( DIK_W ) || GetGamePadAngleTrigger( GP_ANGLE_UP ) )
		{
			g_menuCommand = ( g_menuCommand - 1 < 0 ? MENU_MAX - 1 : g_menuCommand - 1 );
		}
		g_nKeyPressFps = 1;
	}
	
	/*�w�i�̍X�V*/
	UpdateMenuBackGround();
	
	/*���j���[�e�L�X�g�̍X�V*/
	UpdateMenuTxt( g_menuCommand );

	/*���j���[�J�[�\���̍X�V*/
	UpdateMenuCursor( g_menuCommand );

	if( exitFlag != true && GetFade() == FADE_NONE && ( GetKeyboardTrigger( DIK_RETURN ) || GetGamePadTrigger( 0 ) ) )
	{
		PlaySound(SOUND_LABEL_SE_ENTER);
		switch( g_menuCommand )
		{
			case MENU_ARCADE:
				{
					SetFade( FADE_OUT , MODE_AF_GAME ); 
					break;
				}
			case MENU_STORY:
				{
					SetFade( FADE_OUT , MODE_GAME ); 
					break;
				}
			case MENU_OPTION:
				{
					
					break;
				}
			case MENU_EXIT:
				{
					//YESNO�̌`���ƃo�O���Ė����ɃE�B���h�E���o�������̂ŋ��d��i�B
#ifdef _DEBUG					
					Uninit();
					MessageBox( GetWnd() ,"�����l�ł����B","�Q�[�����I�����܂��B", MB_OK );
					DestroyWindow( GetWnd() );	/*�E�B���h�E��j������*/
					PostQuitMessage( 0 );
					exitFlag = true;
#endif
#ifndef _DEBUG			
					SetFade( FADE_OUT , MODE_TITLE ); 
#endif
					break;
				}
			default:
				{
					Error("�\�����ʃ��[�h���I������܂����B");
					break;
				}
		}
	}
	
	//if( exitFlag == true )
	//{	
	//	nID = MessageBox( GetWnd() ,"�A�v���P�[�V�������I�����܂����B","�I���m�F", MB_YESNO );
	//	if( nID == IDYES )
	//	{
	//		DestroyWindow( GetWnd() );	/*�E�B���h�E��j������*/
	//	}
	//	else
	//	{
	//		exitFlag = false;
	//	}
	//}
}

/*******************************************************************************
* �֐���	:void DrawMenu( void )
*		���j���[�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawMenu( void )
{
		/*�w�i�̕`��:���i��*/
		DrawMenuBackGround();
	
		/*���j���[�J�[�\���̕`��*/
		DrawMenuCursor();	

		/*���j���[�e�L�X�g�̕`��*/
		DrawMenuTxt();
}