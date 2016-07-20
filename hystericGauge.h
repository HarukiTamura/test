/*******************************************************************************
* タイトル	:ヒステリックゲージ.h
* ファイル名:hystericGauge.h
* 作成者	:田村春樹
* 作成日	:2015/06/24
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/
#ifndef _HYSTERICGAUGE_H_
#define _HYSTERICGAUGE_H_
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
* 関数名	:HRESULT InitHystericGauge( void )
*		ヒステリックゲージ初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitHystericGauge( void );

/*******************************************************************************
* 関数名	:void UninitHystericGauge( void )
*		ヒステリックゲージ終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitHystericGauge( void );

/*******************************************************************************
* 関数名	:void UpdateHystericGauge( void )
*		ヒステリックゲージ更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateHystericGauge( void );

/*******************************************************************************
* 関数名	:void DrawHystericGauge( void )
*		ヒステリックゲージ描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawHystericGauge( void );

#endif