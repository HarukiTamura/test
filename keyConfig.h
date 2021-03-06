/*******************************************************************************
* タイトル	:キーコンフィグ.h
* ファイル名:keyConfig.h
* 作成者	:田村春樹
* 作成日	:2015/07/29
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/
#ifndef _KEYCONFIG_H_
#define _KEYCONFIG_H_
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
typedef enum
{
	CFG_ANGLE_UP = 0,
	CFG_ANGLE_DOWN,
	CFG_ANGLE_LEFT,
	CFG_ANGLE_RIGHT,
	CFG_A,
	CFG_B,
	CFG_X,
	CFG_Y,
	CFG_START,
	CFG_SELECT,
	CFG_LEFT,
	CFG_RIGHT,
	USE_KEY_MAX
}USE_KEY_DATA;

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* 関数名	:HRESULT InitKeyConfig( void )
*		コンフィグ情報初期化関数です。
* 引数		:void		引数無し
* 戻り値	:HRESULT	関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitKeyConfig( void );

/*******************************************************************************
* 関数名	:void UninitKeyConfig( void )
*		入力終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitKeyConfig( void );

/*******************************************************************************
* 関数名	:bool GetKeyboardPressConfig( int nKeyData )
*		キーボードからコンフィグされやキーのPress入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectKeyConfigに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardPressConfig( int nKeyData );

/*******************************************************************************
* 関数名	:bool GetKeyboardTriggerConfig( int nKeyData )
*		キーボードからコンフィグされやキーのTrigger入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectKeyConfigに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardTriggerConfig( int nKeyData );

/*******************************************************************************
* 関数名	:bool GetKeyboardReleaseConfig( int nKeyData )
*		キーボードからコンフィグされやキーのRelease入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectKeyConfigに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardReleaseConfig( int nKeyData );

/*******************************************************************************
* 関数名	:bool GetKeyboardRepeatConfig( int nKeyData )
*		キーボードからコンフィグされやキーのRepeat入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectKeyConfigに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardRepeatConfig( int nKeyData );

#endif