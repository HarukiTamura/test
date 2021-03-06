/*******************************************************************************
* タイトル	:
* ファイル名:main.h
* 作成者	:田村春樹
* 作成日	:2015/05/22
********************************************************************************
* 更新履歴	ver 
*******************************************************************************/
#ifndef _MAIN_H_
#define _MAIN_H_

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include <windows.h>
#include <d3dx9.h>	/*描画処理に必要*/
#include <stdio.h>

#define DIRECTINPUT_VERSION ( 0x0800 )
#include "dinput.h"
#include "xaudio2.h"

/*ライブラリのリンク*/
#pragma comment( lib , "d3d9.lib" )		/*描画処理に必要*/
#pragma comment( lib , "d3dx9.lib" )	/*dxd9拡張ライブラリ*/
#pragma comment( lib , "dxguid.lib" )	/*DirectXコンポーネントに必要*/
#pragma comment( lib , "winmm.lib" )	/*windowsとの連携に必要*/
#pragma comment( lib , "dinput8.lib" )	/*DirectXコンポーネントに必要*/

/*******************************************************************************
* マクロ定義
*******************************************************************************/
/*ウィンドウ・フルスクリーン指定*/
#define SCREEN_TYPE ( true )	/* true：ウィンドウ false：フルスクリーン*/

/*FPSレート*/
#define FPS_RATE ( 60 )

/*ウィンドウサイズ：クライアント領域そのものの大きさ*/  
#define WINDOW_WIDTH	( 800 )
#define WINDOW_HEIGHT	( 600 )

/*画面サイズ：クライアント領域内部の大きさ*/
#define SCREEN_WIDTH	( WINDOW_WIDTH )
#define SCREEN_HEIGHT	( WINDOW_HEIGHT )

#define SCREEN_TOP		( 0 )				/*画面の上限界*/
#define SCREEN_BOTTOM	( WINDOW_HEIGHT )	/*画面の下限界*/
#define SCREEN_LEFT		( 0 )				/*画面の左限界*/
#define SCREEN_RIGHT	( WINDOW_WIDTH )	/*画面の右限界*/

#define INT_SCREEN_TOP		( 0 )					/*画面の上限界*/
#define INT_SCREEN_BOTTOM	( WINDOW_HEIGHT * 100 )	/*画面の下限界*/
#define INT_SCREEN_LEFT		( 0 )					/*画面の左限界*/
#define INT_SCREEN_RIGHT	( WINDOW_WIDTH * 100 )	/*画面の右限界*/

/*DirectX描画背景色*/
#define D3DCOLOR_RGBA_BACK	( D3DCOLOR_RGBA( 30 , 30 , 10 , 0 ) )

/*ポリゴンフォーマットの指定*/
#define FVF_VERTEX_2D	( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

/*モードの種類*/
typedef enum
{
	MODE_TITLE = 0,	//タイトル画面
	MODE_MENU,		//メニュー画面
	MODE_GAME,		//ゲーム画面
	MODE_RESULT,	//リザルト画面
	MODE_AF_GAME,	//(アフターバーナー風)ゲーム画面
	MODE_AF_RESULT,	//(アフターバーナー風)リザルト画面
	MODE_MAX		//項目数
}MODE;


/*******************************************************************************
* 構造体定義
*******************************************************************************/
/*ポリゴンフォーマット構造体*/
typedef struct
{
	D3DXVECTOR3 pos;	/*頂点座標*/
	float rhw;			/*座標変換用係数*/
	D3DCOLOR col;		/*頂点カラー*/
	D3DXVECTOR2 tex;	/*テクスチャ座標*/
}VERTEX_2D;

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* 関数名	:void Uninit( void )
*		全体終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void Uninit( void );

/*******************************************************************************
* 関数名	:void SetMode( MODE mode )
*		モードの設定と初期化を行います。
* 引数		:MODE mode	変更先のモード
* 戻り値	:void 戻り値なし
* 説明		:モードの設定と初期化を行います。
*******************************************************************************/
void SetMode( MODE mode );

/*******************************************************************************
* 関数名	:MODE GetMode( void )
*		現在実行中のモードIDを参照を行います。
* 引数		:void		引数なし
* 戻り値	:MODE mode	現在のモード
* 説明		:現在実行中のモードIDを参照を行います。
*******************************************************************************/
MODE GetMode( void );

/*******************************************************************************
* 関数名	:LPDIRECT3DDEVICE9 GetDevice( void )
*		Direct3Dデバイスを返す関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:Direct3Dデバイスを他のファイルから呼ぶ際に使用します。
*******************************************************************************/
LPDIRECT3DDEVICE9 GetDevice( void );

/*******************************************************************************
* 関数名	:HWND GetWnd( void )
*		hWndのミラーを返す関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:hWndを他のファイルから参照する際に使用します。
*******************************************************************************/
HWND GetWnd( void );

/*******************************************************************************
* 関数名	:void Error( void )
*		Errorメッセージボックス表示関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:Errorを示すメッセージボックスを表示する関数です。
*******************************************************************************/
void Error( char *errorTxt );

/*******************************************************************************
* 関数名	:void DebugMessage( char *errorTxt , float x , float y )
*		デバッグメッセージ表示関数です。
* 引数		:char *errorTxt	表示するテキスト
*			 float x	表示するX座標
*			 float y	表示するY座標
* 戻り値	:void 戻り値なし
* 説明		:Errorを示すメッセージボックスを表示する関数です。
*******************************************************************************/
void DebugMessage( char *errorTxt , float x , float y );

/*******************************************************************************
* 関数名	:LPD3DXFONT *getFont( void )
*		フォントの値をゲットする関数です。
* 引数		:void 引数なし
* 戻り値	:LPD3DXFONT*	g_pFontのアドレス
* 説明		:フォントの値をゲットする関数です。
*******************************************************************************/
LPD3DXFONT getFont( void );

#endif