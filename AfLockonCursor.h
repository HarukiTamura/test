/*******************************************************************************
* タイトル	:自弾.h
* ファイル名:AfLockonCursor.h
* 作成者	:田村春樹
* 作成日	:2015/05/29
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/
#ifndef _AF_LOCKON_CURSOR_H_
#define _AF_LOCKON_CURSOR_H_
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "afPointConvert.h"
#include "AfEnemy.h"
/*******************************************************************************
* マクロ定義
*******************************************************************************/

#define LOCKON_CURSOR_MAX_NUM	( ENEMY_MAX_NUM )	//自弾の数
#define LOCKON_CURSOR_RADIUS	( 10 )	//当たり判定

#define AF_LOCKON_CURSOR_LOST		( 0.80f )	//ロックオン消失

typedef enum
{
	LOCKON_CURSOR_MODE_UNLOCK = 0,
	LOCKON_CURSOR_MODE_LOCK,
	LOCKON_CURSOR_MODE_CHANGE,
	LOCKON_CURSOR_MODE_NONLOCK,
	LOCKON_CURSOR_MODE_MAX
}LOCKON_CURSOR_MODE;


/*******************************************************************************
* 構造体定義
*******************************************************************************/
typedef struct
{
	bool bUse;
	LOCKON_CURSOR_MODE mode;
	AF_OBJECT_STATUS posStatus;
	int intPosX;
	int intPosY;
}LOCKON_CURSOR_STATUS;

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* 関数名	:HRESULT InitAfLockonCursor( void )
*		自弾初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfLockonCursor( void );

/*******************************************************************************
* 関数名	:HRESULT InitDirectAfLockonCursor( void )
*		自弾初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitDirectAfLockonCursor( void );

/*******************************************************************************
* 関数名	:HRESULT InitValueAfLockonCursor( void )
*		自弾初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void InitValueAfLockonCursor( void );

/*******************************************************************************
* 関数名	:void UninitAfLockonCursor( void )
*		自弾終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfLockonCursor( void );

/*******************************************************************************
* 関数名	:void UpdateAfLockonCursor( void )
*		自弾更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfLockonCursor( void );

/*******************************************************************************
* 関数名	:void DrawAfLockonCursor( void )
*		自弾描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfLockonCursor( void );

/*******************************************************************************
* 関数名	:void DrawAfLockonCursorcore( void )
*		自弾描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfLockonCursorcore( void );

/*******************************************************************************
* 関数名	:LOCKON_CURSOR_STATUS* GetLockonCursorState( int num )
*		自弾のステータスを取得します。
* 引数		:int num	入手する構造体の番号
* 戻り値	:void 戻り値なし
* 説明		:自弾のステータスを取得します。。
*******************************************************************************/
LOCKON_CURSOR_STATUS* GetLockonCursorState( int num );

/*******************************************************************************
* 関数名	:LOCKON_CURSOR_STATUS* GetLockonCursorState( int num )
*		自弾のステータスを取得します。
* 引数		:int num	入手する構造体の番号
* 戻り値	:void 戻り値なし
* 説明		:自弾のステータスを取得します。。
*******************************************************************************/
void SetLockonCursorState( float posX , float posY , float posZ , float moveZ );

#endif