/*******************************************************************************
* タイトル	:背景.cpp
* ファイル名:afPointConvert.cpp
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
#include "input.h"
#include "unoDirectX.h"
#include "unoHitChecker.h"
#include "afPointConvert.h"
#include "AfPlayer.h"
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
//描画基準位置
static AF_VIEW_STATUS g_NowViewStatus;
/*******************************************************************************
* 関数名	:HRESULT InitAfPointConvert( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfPointConvert( void )
{
	InitValueAfPointConvert();
	return S_OK;
}

/*******************************************************************************
* 関数名	:HRESULT InitValueAfPointConvert( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void InitValueAfPointConvert( void )
{
	g_NowViewStatus.centerPosX = GetPlayerState()->posStatus.DrawPosX;
	g_NowViewStatus.centerPosY = GetPlayerState()->posStatus.DrawPosY;
	g_NowViewStatus.centerVector = GetPlayerState()->posStatus.DrawVector;
}

/*******************************************************************************
* 関数名	:void UninitAfPointConvert( void )
*		背景終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfPointConvert( void )
{
}

/*******************************************************************************
* 関数名	:void UpdateAfPointConvert( void )
*		背景更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfPointConvert( void )
{
	g_NowViewStatus.centerPosX = GetPlayerState()->posStatus.DrawPosX;
	g_NowViewStatus.centerPosY = GetPlayerState()->posStatus.DrawPosY;
	g_NowViewStatus.centerVector = GetPlayerState()->posStatus.DrawVector;
}

/*******************************************************************************
* 関数名	:void DrawAfPointConvert( void )
*		背景描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
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
* 関数名	:void DrawAfPointConvert( void )
*		背景描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void AfPointConvert( AF_OBJECT_STATUS convertStatus )
{
	/*convertStatus.posX =
	convertStatus.
	convertStatus.*/
}

/*******************************************************************************
* 関数名	:AF_VIEW_STATUS* getViewStatus( void )
*		ビュー情報構造体を渡す処理関数です。
* 引数		:void 引数なし
* 戻り値	:AF_VIEW_STATUS*　g_NowViewStatusのアドレス
* 説明		:ビュー情報構造体を渡す関数です。
*******************************************************************************/
AF_VIEW_STATUS* getViewStatus( void )
{
	return &g_NowViewStatus;
}