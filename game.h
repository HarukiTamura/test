/*******************************************************************************
* タイトル	:ゲーム.h
* ファイル名:game.h
* 作成者	:田村春樹
* 作成日	:2015/05/29
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/
#ifndef _GAME_H_
#define _GAME_H_
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "input.h"
#include "sound.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

/*フィールドサイズ：主にゲームが進行するフィールドの幅を定義します。*/

/*float版*/
#define FIELD_WIDTH		( SCREEN_WIDTH * 0.70 )	/*フィールドの幅*/
#define FIELD_HEIGHT	( SCREEN_HEIGHT )		/*フィールドの高さ*/
												  
#define FIELD_START_X	( SCREEN_WIDTH * 0.05 )	/*フィールドの始点X*/
#define FIELD_START_Y	( SCREEN_TOP )			/*フィールドの始点Y*/

#define FIELD_TOP		( FIELD_START_Y )					/*フィールドの上限界*/
#define FIELD_BOTTOM	( FIELD_START_Y + FIELD_HEIGHT )	/*フィールドの下限界*/
#define FIELD_LEFT		( FIELD_START_X )					/*フィールドの左限界*/
#define FIELD_RIGHT		( FIELD_START_X + FIELD_WIDTH )		/*フィールドの右限界*/

/*処理軽減int版*/
#define INT_FIELD_WIDTH		( 1000 * FIELD_WIDTH )		/*フィールドの幅：(画面の幅の0.5割*1000=50)*/
#define INT_FIELD_HEIGHT	( 1000 * FIELD_HEIGHT )		/*フィールドの高さ*/
									 			
#define INT_FIELD_START_X	( 1000 * FIELD_START_X )	/*フィールドの始点X*/
#define INT_FIELD_START_Y	( 1000 * FIELD_START_Y )	/*フィールドの始点Y*/
		
#define INT_FIELD_TOP		( 1000 * ( FIELD_START_Y ) )					/*フィールドの上限界*/
#define INT_FIELD_BOTTOM	( 1000 * ( FIELD_START_Y + FIELD_HEIGHT ) )	/*フィールドの下限界*/
#define INT_FIELD_LEFT		( 1000 * ( FIELD_START_X ) )					/*フィールドの左限界*/
#define INT_FIELD_RIGHT		( 1000 * ( FIELD_START_X + FIELD_WIDTH ) )	/*フィールドの右限界*/
#define INT_FIELD_CENTER_X	( 1000 * ( FIELD_START_X + ( FIELD_WIDTH / 2 ) ) )	/*フィールドのX中央*/
#define INT_FIELD_CENTER_Y	( 1000 * ( FIELD_START_Y + ( FIELD_HEIGHT / 2 ) ) )	/*フィールドのY中央*/

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
	CATEGORY_PLAYERHYPER,	//独自要素：ハイパーモード弾
	CATEGORY_ENEMY,
	CATEGORY_NEUTRAL,
	CATEGORY_ANTIBREAK,	//エフェクト等非破壊種別
	CATEGORY_LIAR,		//独自要素：嘘属性
	CATEGORY_MAX
}CATEGORY;
/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* 関数名	:HRESULT InitGame( void )
*		ゲーム初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitGame( void );

/*******************************************************************************
* 関数名	:void UninitGame( void )
*		ゲーム終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitGame( void );

/*******************************************************************************
* 関数名	:void UpdateGame( void )
*		ゲーム更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateGame( void );

/*******************************************************************************
* 関数名	:void DrawGame( void )
*		ゲーム描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawGame( void );

#endif