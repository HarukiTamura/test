/*******************************************************************************
* リザルト	:ゲーム.cpp
* ファイル名:Result.cpp
* 作成者	:田村春樹
* 作成日	:2015/05/29
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "result.h"
#include "input.h"
#include "fade.h"
#include "ResultGraphic.h"


#include "score1.h"
#include "score2.h"
#include "score3.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define RANKER_LIST_MAX			( 3 )
#define RANKER_LIST_BUFF_MAX	( 1 + RANKER_LIST_MAX )

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static int lastScore = 0;	//Playerのスコア
unsigned long rankingScore[RANKER_LIST_MAX] = {0};
unsigned long playerScore = 0;
static bool whoPlayerFlag[RANKER_LIST_BUFF_MAX] = {0};

/*******************************************************************************
* 関数名	:HRESULT InitResult( void )
*		ゲーム初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitResult( void )
{
	FILE *fp;
	PlaySound(SOUND_LABEL_BGM_ED);
	/*リザルトグラフィックの設定*/
	if( InitResultGraphic() != S_OK )
	{
		Error("ポリゴンの初期化に失敗しました。");
	}
	/*スコアランキングの設定*/
	if( InitScore1() != S_OK )
	{
		Error("スコアランキングの初期化に失敗しました。");
	}
	/*スコアランキングの設定*/
	if( InitScore2() != S_OK )
	{
		Error("スコアランキングの初期化に失敗しました。");
	}
	/*スコアランキングの設定*/
	if( InitScore3() != S_OK )
	{
		Error("スコアランキングの初期化に失敗しました。");
	}
	
	for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
	{
		rankingScore[cnt] = 0; 
	}
	for( int cnt = 0; cnt < RANKER_LIST_BUFF_MAX; cnt++ )
	{
		whoPlayerFlag[cnt] = false;
	}
	
	//過去スコアデータ読み込み
	if( ( fp = fopen( "data/sch.dat" , "rb" ) ) == NULL )
	{	
		//ERROR：元データ存在なし
		//対処：デフォルトデータ生成

		//デフォルトのスコアデータを用意
		if( ( fp = fopen( "data/sch.dat" , "wb" ) ) == NULL )
		{
			Error("ERROR:スコアデータの作成に失敗しました。");
		}
		else
		{
			rankingScore[0] = 8000000;
			rankingScore[1] = 4000000;
			rankingScore[2] = 2000000;
			for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
			{
				fwrite( &rankingScore[cnt]  ,sizeof( unsigned long ) , 1 , fp );
			}
		}
		fclose(fp);
	}
	else
	{
		//過去スコアデータの読み込み
		for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
		{
			fread( &rankingScore[cnt]  , sizeof( unsigned long ) , 1 , fp );
		}
		fclose(fp);
	}
	
	//最新プレイヤースコア読み込み
	if( ( fp = fopen( "data/scs.dat" , "rb" ) ) == NULL )
	{	
		Error("ERROR:プレイヤー最新スコアの読み込みに失敗しました。");
		playerScore = 0;
	}
	else
	{
		fread( &playerScore , sizeof( unsigned long ) , 1 , fp );
		fclose(fp);
	}

	//スコアのソート用意
	unsigned long scoreBuff[RANKER_LIST_BUFF_MAX];
	scoreBuff[0] = playerScore;
	whoPlayerFlag[0] = true;
	for( int cnt = 1; cnt < RANKER_LIST_BUFF_MAX; cnt++ )
	{
		scoreBuff[cnt] = rankingScore[cnt - 1];	
	}
	
	//スコアソート部分
	unsigned long sortBuff;
	bool flagBuff;
	for( int cnt1 = 0; cnt1 < RANKER_LIST_BUFF_MAX; cnt1++ )
	{
		for( int cnt2 = 0; cnt2 < RANKER_LIST_BUFF_MAX; cnt2++ )
		{
			if( scoreBuff[cnt1] > scoreBuff[cnt2] )
			{
				sortBuff = scoreBuff[cnt1];
				scoreBuff[cnt1] = scoreBuff[cnt2];
				scoreBuff[cnt2] = sortBuff;

				flagBuff = whoPlayerFlag[cnt1]; 
				whoPlayerFlag[cnt1] = whoPlayerFlag[cnt2];
				whoPlayerFlag[cnt2] = flagBuff;
			}
		}
	}
	
	//スコア書き換え
	for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
	{
		rankingScore[cnt] = scoreBuff[cnt];
	}

	//スコア設定
	resetScore1( rankingScore[0] );
	resetScore2( rankingScore[1] );
	resetScore3( rankingScore[2] );

	
	//最新スコアデータ書き込み
	if( ( fp = fopen( "data/sch.dat" , "wb" ) ) == NULL )
	{
		Error("ERROR:スコアデータの書き出しに失敗しました。");
	}
	else
	{
		for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
		{
			fwrite( &rankingScore[cnt]  ,sizeof( unsigned long ) , 1 , fp );
		}
		fclose(fp);
	}

	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitResult( void )
*		ゲーム終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitResult( void )
{
	
	/*リザルトグラフィックの解放*/
	UninitResultGraphic();

	/*スコアランキングの解放*/
	UninitScore1();
	/*スコアランキングの解放*/
	UninitScore2();
	/*スコアランキングの解放*/
	UninitScore3();
	


}

/*******************************************************************************
* 関数名	:void UpdateResult( void )
*		ゲーム更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateResult( void )
{
	
	/*リザルトグラフィックの更新*/
	UpdateResultGraphic();



	/*スコアランキングの更新*/
	UpdateScore1();
	/*スコアランキングの更新*/
	UpdateScore2();
	/*スコアランキングの更新*/
	UpdateScore3();

	if( GetFade() == FADE_NONE && ( GetKeyboardTrigger( DIK_RETURN ) || GetGamePadTrigger( 0 ) ) )
	{
		PlaySound(SOUND_LABEL_SE_ENTER);
		SetFade( FADE_OUT , MODE_TITLE ); 
	}
}

/*******************************************************************************
* 関数名	:void DrawResult( void )
*		ゲーム描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawResult( void )
{
	
	/*リザルトグラフィックの描画*/
	DrawResultGraphic();
	
	/*スコアランキングの描画*/
	DrawScore1();
	/*スコアランキングの描画*/
	DrawScore2();
	/*スコアランキングの描画*/
	DrawScore3();
}