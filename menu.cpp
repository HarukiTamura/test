/*******************************************************************************
* タイトル	:メニュー.cpp
* ファイル名:Menu.cpp
* 作成者	:田村春樹
* 作成日	:2015/05/29
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/

/*******************************************************************************
* インクルードファイル
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
* マクロ定義
*******************************************************************************/

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static int g_menuCommand;
static int g_nKeyPressFps;
/*******************************************************************************
* 関数名	:HRESULT InitMenu( void )
*		メニュー初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitMenu( void )
{
	PlaySound(SOUND_LABEL_BGM_MENU);

	setRepeatFps( 60 );
	g_nKeyPressFps = 0;

	g_menuCommand = MENU_ARCADE;
	/*背景の設定*/
	if( InitMenuBackGround() != S_OK )
	{
		Error("背景データの初期化に失敗しました。");
	}
	/*メニューテキストの設定*/
	if( InitMenuTxt() != S_OK )
	{
		Error("メニューテキストの初期化に失敗しました。");
	}
	/*メニューカーソルの設定*/
	if( InitMenuCursor() != S_OK )
	{
		Error("メニューカーソルの初期化に失敗しました。");
	}
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitMenu( void )
*		メニュー終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitMenu( void )
{
	setRepeatFps();

	/*背景の開放*/
	UninitMenuBackGround();

	/*メニューカーソルの開放*/
	UninitMenuCursor();

	/*メニューテキストの開放*/
	UninitMenuTxt();
}

/*******************************************************************************
* 関数名	:void UpdateMenu( void )
*		メニュー更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
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
	
	/*背景の更新*/
	UpdateMenuBackGround();
	
	/*メニューテキストの更新*/
	UpdateMenuTxt( g_menuCommand );

	/*メニューカーソルの更新*/
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
					//YESNOの形だとバグって無限にウィンドウが出続けたので強硬手段。
#ifdef _DEBUG					
					Uninit();
					MessageBox( GetWnd() ,"お疲れ様でした。","ゲームを終了します。", MB_OK );
					DestroyWindow( GetWnd() );	/*ウィンドウを破棄する*/
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
					Error("予期せぬモードが選択されました。");
					break;
				}
		}
	}
	
	//if( exitFlag == true )
	//{	
	//	nID = MessageBox( GetWnd() ,"アプリケーションを終了しますか。","終了確認", MB_YESNO );
	//	if( nID == IDYES )
	//	{
	//		DestroyWindow( GetWnd() );	/*ウィンドウを破棄する*/
	//	}
	//	else
	//	{
	//		exitFlag = false;
	//	}
	//}
}

/*******************************************************************************
* 関数名	:void DrawMenu( void )
*		メニュー描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawMenu( void )
{
		/*背景の描画:遠景順*/
		DrawMenuBackGround();
	
		/*メニューカーソルの描画*/
		DrawMenuCursor();	

		/*メニューテキストの描画*/
		DrawMenuTxt();
}