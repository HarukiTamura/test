/*******************************************************************************
* タイトル	:バリアー.h
* ファイル名:barrier.h
* 作成者	:田村春樹
* 作成日	:2015/05/29
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/
#ifndef _BARRIER_H_
#define _BARRIER_H_
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define BARRIER_RADIUS	( 150 )		/*限界拡大時半径*/

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* 関数名	:HRESULT InitBarrier( void )
*		バリアー初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitBarrier( void );

/*******************************************************************************
* 関数名	:void UninitBarrier( void )
*		バリアー終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitBarrier( void );

/*******************************************************************************
* 関数名	:void UpdateBarrier( void )
*		バリアー更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateBarrier( void );

/*******************************************************************************
* 関数名	:void DrawBarrier( void )
*		バリアー描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawBarrier( void );

/*******************************************************************************
* 関数名	:int GetBarrierRadiusInt( void )
*		バリアー半径を渡す処理関数です。
* 引数		:void 引数なし
* 戻り値	:int　バリア半径int変換値
* 説明		:バリアーの半径を渡す関数です。
*******************************************************************************/
int GetBarrierRadiusInt( void );

#endif