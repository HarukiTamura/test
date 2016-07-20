/*******************************************************************************
* タイトル	:スコア.h
* ファイル名:afAfScore.h
* 作成者	:田村春樹
* 作成日	:2015/06/24
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/
#ifndef _AF_SCORE2_H_
#define _AF_SCORE2_H_
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "game.h"

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
* 関数名	:HRESULT InitAfScore2( void )
*		スコア初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfScore2( void );

/*******************************************************************************
* 関数名	:void UninitAfScore2( void )
*		スコア終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfScore2( void );

/*******************************************************************************
* 関数名	:void UpdateAfScore2( void )
*		スコア更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfScore2( void );

/*******************************************************************************
* 関数名	:void DrawAfScore2( void )
*		スコア描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfScore2( void );

/*******************************************************************************
* 関数名	:void PlusAfScore2( unsigned long plusNum )
*		スコア加算関数です。
* 引数		:unsigned long plusNum	加算値
* 戻り値	:void 戻り値なし
* 説明		:スコアを加算します。
*******************************************************************************/
void PlusAfScore2( unsigned long plusNum );

/*******************************************************************************
* 関数名	:resetAfScore2( unsigned long resetNum )
*		スコアリセット関数です。
* 引数		:unsigned long resetNum	リセット後の値
* 戻り値	:void 戻り値なし
* 説明		:スコアをリセットします。
*******************************************************************************/
void resetAfScore2( unsigned long resetNum );

#endif