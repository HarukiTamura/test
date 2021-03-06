/*******************************************************************************
* タイトル	:カーソル.h
* ファイル名:AfCursor.h
* 作成者	:田村春樹
* 作成日	:2015/05/29
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/
#ifndef _AF_CURSOR_H_
#define _AF_CURSOR_H_
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "afPointConvert.h"
/*******************************************************************************
* マクロ定義
*******************************************************************************/

#define CURSOR_RADIUS ( 5000 )	//当たり判定

typedef enum
{
	CURSOR_MODE_NORMAL = 0,
	CURSOR_MODE_LOCKON,
	CURSOR_MODE_MAX
}CURSOR_MODE;

/*******************************************************************************
* 構造体定義
*******************************************************************************/
typedef struct
{
	AF_OBJECT_STATUS posStatus;
	int intPosX;
	int intPosY;
	CURSOR_MODE mode;
}CURSOR;

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* 関数名	:HRESULT InitAfCursor( void )
*		カーソル初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfCursor( void );

/*******************************************************************************
* 関数名	:HRESULT InitDirectAfCursor( void )
*		カーソル初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitDirectAfCursor( void );

/*******************************************************************************
* 関数名	:HRESULT InitValueAfCursor( void )
*		カーソル初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void InitValueAfCursor( void );

/*******************************************************************************
* 関数名	:void UninitAfCursor( void )
*		カーソル終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfCursor( void );

/*******************************************************************************
* 関数名	:void UpdateAfCursor( void )
*		カーソル更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfCursor( void );

/*******************************************************************************
* 関数名	:void DrawAfCursor( void )
*		カーソル描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfCursor( void );

/*******************************************************************************
* 関数名	:void DrawAfCursorcore( void )
*		カーソル描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfCursorcore( void );

/*******************************************************************************
* 関数名	:CURSOR* GetCursorState( void )
*		カーソルのステータスを取得します。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:カーソルのステータスを取得します。。
*******************************************************************************/
CURSOR* GetCursorState( void );

#endif