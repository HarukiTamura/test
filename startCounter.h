/*******************************************************************************
* ポーズ	:スタートカウント.h
* ファイル名:StartCounter.h
* 作成者	:田村春樹
* 作成日	:2015/05/29
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/
#ifndef _STARTCOUNTER_H_
#define _STARTCOUNTER_H_
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
* 関数名	:HRESULT InitStartCounter( void )
*		スタートカウント初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitStartCounter( void );

/*******************************************************************************
* 関数名	:void UninitStartCounter( void )
*		スタートカウント終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitStartCounter( void );

/*******************************************************************************
* 関数名	:bool UpdateStartCounter( void )
*		スタートカウント更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:bool true カウント中	false カウントゼロ
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
bool UpdateStartCounter( void );

/*******************************************************************************
* 関数名	:void DrawStartCounter( void )
*		スタートカウント描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawStartCounter( void );

/*******************************************************************************
* 関数名	:void ResetStartCounter( void )
*		スタートカウントをリセットする関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:スタートカウントをリセットする関数です。
*******************************************************************************/
void ResetStartCounter( void );

#endif