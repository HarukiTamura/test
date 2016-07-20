/*******************************************************************************
* リザルト	:ゲーム.cpp
* ファイル名:AfResult.cpp
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
#include "afResult.h"
#include "input.h"
#include "fade.h"
#include "afResultGraphic.h"


#include "afScore1.h"
#include "afScore2.h"
#include "afScore3.h"
#include "afScoreMy.h"
#include "BackPlusAlpha2.h"

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
static int lastAfScore = 0;	//Playerのスコア
unsigned long rankingAfScore[RANKER_LIST_MAX] = {0};
unsigned long playerAfScore = 0;
static bool whoPlayerFlag[RANKER_LIST_BUFF_MAX] = {0};

/*******************************************************************************
* 関数名	:HRESULT InitAfResult( void )
*		ゲーム初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfResult( void )
{
	FILE *fp;
	PlaySound(SOUND_LABEL_BGM_ED);
	/*リザルトグラフィックの設定*/
	if( InitAfResultGraphic() != S_OK )
	{
		Error("ポリゴンの初期化に失敗しました。");
	}
	/*リザルトグラフィックの設定*/
	if( InitBackPlusAlpha2() != S_OK )
	{
		Error("ポリゴンの初期化に失敗しました。");
	}
	/*スコアランキングの設定*/
	if( InitAfScore1() != S_OK )
	{
		Error("スコアランキングの初期化に失敗しました。");
	}
	/*スコアランキングの設定*/
	if( InitAfScore2() != S_OK )
	{
		Error("スコアランキングの初期化に失敗しました。");
	}
	/*スコアランキングの設定*/
	if( InitAfScore3() != S_OK )
	{
		Error("スコアランキングの初期化に失敗しました。");
	}
	/*スコアランキングの設定*/
	if( InitAfScoreMy() != S_OK )
	{
		Error("スコアランキングの初期化に失敗しました。");
	}
	
	for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
	{
		rankingAfScore[cnt] = 0; 
	}
	for( int cnt = 0; cnt < RANKER_LIST_BUFF_MAX; cnt++ )
	{
		whoPlayerFlag[cnt] = false;
	}
	
	//過去スコアデータ読み込み
	if( ( fp = fopen( "data/schaf.dat" , "rb" ) ) == NULL )
	{	
		//ERROR：元データ存在なし
		//対処：デフォルトデータ生成

		//デフォルトのスコアデータを用意
		if( ( fp = fopen( "data/schaf.dat" , "wb" ) ) == NULL )
		{
			Error("ERROR:スコアデータの作成に失敗しました。");
		}
		else
		{
			rankingAfScore[0] = 80;
			rankingAfScore[1] = 40;
			rankingAfScore[2] = 20;
			for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
			{
				fwrite( &rankingAfScore[cnt]  ,sizeof( unsigned long ) , 1 , fp );
			}
		}
		fclose(fp);
	}
	else
	{
		//過去スコアデータの読み込み
		for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
		{
			fread( &rankingAfScore[cnt]  , sizeof( unsigned long ) , 1 , fp );
		}
		fclose(fp);
	}
	
	//最新プレイヤースコア読み込み
	if( ( fp = fopen( "data/scsaf.dat" , "rb" ) ) == NULL )
	{	
		Error("ERROR:プレイヤー最新スコアの読み込みに失敗しました。");
		playerAfScore = 0;
	}
	else
	{
		fread( &playerAfScore , sizeof( unsigned long ) , 1 , fp );
		fclose(fp);
	}

	//スコアのソート用意
	unsigned long afScoreBuff[RANKER_LIST_BUFF_MAX];
	afScoreBuff[0] = playerAfScore;
	whoPlayerFlag[0] = true;
	for( int cnt = 1; cnt < RANKER_LIST_BUFF_MAX; cnt++ )
	{
		afScoreBuff[cnt] = rankingAfScore[cnt - 1];	
	}
	
	//スコアソート部分
	unsigned long sortBuff;
	bool flagBuff;
	for( int cnt1 = 0; cnt1 < RANKER_LIST_BUFF_MAX; cnt1++ )
	{
		for( int cnt2 = 0; cnt2 < RANKER_LIST_BUFF_MAX; cnt2++ )
		{
			if( afScoreBuff[cnt1] > afScoreBuff[cnt2] )
			{
				sortBuff = afScoreBuff[cnt1];
				afScoreBuff[cnt1] = afScoreBuff[cnt2];
				afScoreBuff[cnt2] = sortBuff;

				flagBuff = whoPlayerFlag[cnt1]; 
				whoPlayerFlag[cnt1] = whoPlayerFlag[cnt2];
				whoPlayerFlag[cnt2] = flagBuff;
			}
		}
	}
	
	//スコア書き換え
	for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
	{
		rankingAfScore[cnt] = afScoreBuff[cnt];
	}

	//スコア設定
	resetAfScore1( rankingAfScore[0] );
	resetAfScore2( rankingAfScore[1] );
	resetAfScore3( rankingAfScore[2] );
	resetAfScoreMy( playerAfScore );

	
	//最新スコアデータ書き込み
	if( ( fp = fopen( "data/schaf.dat" , "wb" ) ) == NULL )
	{
		Error("ERROR:スコアデータの書き出しに失敗しました。");
	}
	else
	{
		for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
		{
			fwrite( &rankingAfScore[cnt]  ,sizeof( unsigned long ) , 1 , fp );
		}
		fclose(fp);
	}

	//PlaySound(SOUND_LABEL_SE_RESULT);

	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitAfResult( void )
*		ゲーム終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfResult( void )
{
	
	/*リザルトグラフィックの解放*/
	UninitAfResultGraphic();

	/*スコアランキングの解放*/
	UninitAfScore1();
	/*スコアランキングの解放*/
	UninitAfScore2();
	/*スコアランキングの解放*/
	UninitAfScore3();
	/*スコアランキングの解放*/
	UninitAfScoreMy();

	UninitBackPlusAlpha2();


}

/*******************************************************************************
* 関数名	:void UpdateAfResult( void )
*		ゲーム更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfResult( void )
{
	
	/*リザルトグラフィックの更新*/
	UpdateAfResultGraphic();

	/*スコアランキングの更新*/
	UpdateAfScore1();
	/*スコアランキングの更新*/
	UpdateAfScore2();
	/*スコアランキングの更新*/
	UpdateAfScore3();
	/*スコアランキングの更新*/
	UpdateAfScoreMy();

	UpdateBackPlusAlpha2();

	if( GetFade() == FADE_NONE && ( GetKeyboardTrigger( DIK_RETURN ) || GetGamePadTrigger( 0 ) ) )
	{
		PlaySound(SOUND_LABEL_SE_ENTER);
		SetFade( FADE_OUT , MODE_TITLE ); 
	}
}

/*******************************************************************************
* 関数名	:void DrawAfResult( void )
*		ゲーム描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfResult( void )
{
	
	/*リザルトグラフィックの描画*/
	DrawAfResultGraphic();
	DrawBackPlusAlpha2();
	/*スコアランキングの描画*/
	DrawAfScoreMy();
	/*スコアランキングの描画*/
	DrawAfScore1();
	/*スコアランキングの描画*/
	DrawAfScore2();
	/*スコアランキングの描画*/
	DrawAfScore3();
}

//プレイヤースコアゲッター
long getPlayerAfScoreResult( void )
{
	return playerAfScore;
}