/*******************************************************************************
* リザルト	:リザルトグラフィック.h
* ファイル名:AfResultGraphic.h
* 作成者	:田村春樹
* 作成日	:2015/05/29
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/
#ifndef _AF_RESULT_GRAPHIC_H_
#define _AF_RESULT_GRAPHIC_H_
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"

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
* 関数名	:HRESULT InitAfResultGraphic( void )
*		リザルトグラフィック初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HAF_RESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfResultGraphic( void );

/*******************************************************************************
* 関数名	:void UninitAfResultGraphic( void )
*		リザルトグラフィック終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfResultGraphic( void );

/*******************************************************************************
* 関数名	:void UpdateAfResultGraphic( void )
*		リザルトグラフィック更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfResultGraphic( void );

/*******************************************************************************
* 関数名	:void DrawAfResultGraphic( void )
*		リザルトグラフィック描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfResultGraphic( void );

#endif