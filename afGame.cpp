/*******************************************************************************
* タイトル	:ゲーム.cpp
* ファイル名:AfGame.cpp
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
#include "afGame.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

#include "afBackGround.h"
#include "afBackGround2.h"
#include "afPointConvert.h"
#include "AfPlayer.h"
#include "AfBulletPlayer.h"
#include "AfCursor.h"
#include "AfMissilePlayer.h"
#include "AfExplosion.h"
#include "AfLockonCursor.h"
#include "AfEnemy.h"
#include "AfScore.h"
#include "AfComboNumber.h"
#include "afBackSky.h"
#include "backPlusAlpha.h"
#include "AfpauseGraphic.h"
#include "AfGameUi.h"
#include "afBackGround3.h"
#include "afBackGround4.h"
#include "afBackgroundClear.h"
#include "AfLife.h"
#include "AfGameOver.h"
#include "AfPauseButton.h"

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
* グローバル変数
*******************************************************************************/
static AF_GAME_SEQUENCE g_afGameSequence;
static int soundLabelBufferPause;
static int timerFpsSwitch;
static int fps = 0;
static int g_busterEnemyNum = 0;
static bool g_boss1pop = false;
static bool g_boss1popEnd = false;
static bool g_busterEnemyBoss1 = false;
static int enemyRepopCount = 0;
static int clearFrame = 0;
/*******************************************************************************
* 関数名	:HRESULT InitAfGame( void )
*		ゲーム初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfGame( void )
{
	
	/*描画基準位置の設定*/
	if( InitAfPointConvert() != S_OK )
	{
		Error("描画基準位置データの初期化に失敗しました。");
	}
	/*背景の設定*/
	if( InitBackPlusAlpha() != S_OK )
	{
		Error("背景データの初期化に失敗しました。");
	}
	if( InitAfBackGround() != S_OK )
	{
		Error("背景データの初期化に失敗しました。");
	}
	if( InitAfBackGround2() != S_OK )
	{
		Error("背景データの初期化に失敗しました。");
	}
	if( InitAfBackGround3() != S_OK )
	{
		Error("背景データの初期化に失敗しました。");
	}
	if( InitAfBackGround4() != S_OK )
	{
		Error("背景データの初期化に失敗しました。");
	}
	if( InitAfBackGroundClear() != S_OK )
	{
		Error("背景データの初期化に失敗しました。");
	}
	if( InitAfPauseGraphic() != S_OK )
	{
		Error("ポーズ背景データの初期化に失敗しました。");
	}
	if( InitAfGameUi() != S_OK )
	{
		Error("ゲームUIデータの初期化に失敗しました。");
	}
	if( InitAfScore() != S_OK )
	{
		Error("スコアの初期化に失敗しました。");
	}
	if( InitAfLife() != S_OK )
	{
		Error("HPデータの初期化に失敗しました。");
	}
	if( InitAfGameOver() != S_OK )
	{
		Error("ゲームオーバーの初期化に失敗しました。");
	}
	InitAfCursor();
	InitAfPlayer();
	InitAfEnemy();
	InitAfBulletPlayer();
	InitAfLockonCursor();
	InitAfMissilePlayer();
	InitAfExplosion();
	InitAfScore();
	InitAfComboNumber();
	InitAfBackSky();
	InitBackPlusAlpha();
	InitAfPauseButton();

	g_afGameSequence = SEQUENCE_AF_COUNTDOWN;
	fps = 0;
	PlaySound(SOUND_LABEL_BGM_STAGE1);
	PlaySound(SOUND_LABEL_SE_FLYAWAY);
	
	g_boss1pop = false;
	g_boss1popEnd = false;
	g_busterEnemyBoss1 = false;
	clearFrame = 0;

	g_busterEnemyNum = 0;
	enemyRepopCount = 0;
	soundLabelBufferPause = SOUND_LABEL_BGM_STAGE1;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitAfGame( void )
*		ゲーム終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfGame( void )
{
	UninitAfBackGround();
	UninitAfBackGround2();
	UninitAfBackGround3();
	UninitAfBackGround4();
	UninitAfPointConvert();
	UninitAfPlayer();
	UninitAfCursor();
	UninitAfEnemy();
	UninitAfBulletPlayer();
	UninitAfLockonCursor();
	UninitAfMissilePlayer();
	UninitAfExplosion();
	UninitAfScore();
	UninitAfBackSky();
	UninitAfComboNumber();
	UninitBackPlusAlpha();
	UninitAfPauseGraphic();
	UninitAfGameUi();
	UninitAfLife();
	UninitAfGameOver();
	UninitAfPauseButton();
}

/*******************************************************************************
* 関数名	:void UpdateAfGame( void )
*		ゲーム更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfGame( void )
{




	int clearFlag = false;

#ifdef _DEBUG
	//if( GetFade() == FADE_NONE && GetKeyboardTrigger( DIK_RETURN ) )
	{
		//clearFlag = true;
	}
	if( GetKeyboardTrigger( DIK_1 ) )
	{
		SetEnemyState( 100 , 100 , -2.0f , 0.0f , ENF_STRAIGHT , ENAI_NO_SHOT );
	}
	
	if( GetKeyboardTrigger( DIK_2 ) )
	{
		SetEnemyState( 700 , 500 , -2.0f , 0.0f , ENF_STRAIGHT , ENAI_NO_SHOT );
	}
	
	if( GetKeyboardTrigger( DIK_3 ) )
	{
		SetEnemyState( rand() % SCREEN_WIDTH , rand() % SCREEN_HEIGHT , -2.0f , 0.0f , ENF_STRAIGHT , ENAI_NO_SHOT );
	}
	if( GetKeyboardTrigger( DIK_4 ) )
	{
		PlusAfComboNumber( ( GetAfComboNumber() == 0 ? 1 : GetAfComboNumber() ) );
	}

	
	if( GetKeyboardTrigger( DIK_5 ) )
	{
		setRange( 1.0 );
	}


#endif

	fps = ( fps + 1 >= 60 ? 0 : fps + 1 );
	
	/*if( fps % 12 == 0 )
	{
		SetBackObjectState( SCREEN_WIDTH / ( rand() % 20000 + 1 ) , SCREEN_HEIGHT , 0.0f , 0.0f , OBJ_STRAIGHT , OBJAI_NO_SHOT );
		SetBackObjectState( SCREEN_WIDTH * 0.5 / ( rand() % 20000 + 1 ) , SCREEN_HEIGHT , 0.0f , 0.0f , OBJ_STRAIGHT , OBJAI_NO_SHOT );
		SetBackObjectState( SCREEN_WIDTH * 0.5 + SCREEN_WIDTH * 0.5 / ( rand() % 20000 + 1 ) , SCREEN_HEIGHT , 0.0f , 0.0f , OBJ_STRAIGHT , OBJAI_NO_SHOT );
	
	}*/
	UpdateBackPlusAlpha();
	switch( g_afGameSequence )
	{
		case SEQUENCE_AF_COUNTDOWN :
			{
				
				g_afGameSequence = SEQUENCE_AF_PLAY;
				//break;
			}
		case SEQUENCE_AF_PLAY :
			{
				
				if( g_boss1popEnd == false && g_busterEnemyNum >= 10 )
				{
					g_boss1pop = true;
				}
				if( g_boss1popEnd == false && g_boss1pop == true )
				{
					SetEnemyState( 300 , 200 , 0.8f , 0.0f , ENF_NO_MOVE , ENEMY_AI_BOSS1 );
					SetAfExplosion( 300000 , 200000 , 0.8f , 0.0f , 0.0f , 0.0f , 60 , AF_EXPLOSION_003 );
					StopSound(SOUND_LABEL_BGM_STAGE1);
					PlaySound(SOUND_LABEL_BGM_ED);
					g_boss1popEnd = true;
				}
				if( g_boss1popEnd == true && g_boss1pop == true )
				{
					enemyRepopCount += ( enemyRepopCount >= 3600 ? 3600 : enemyRepopCount + 1 );
				}
				
				if( g_busterEnemyBoss1 )//g_busterEnemyNum >= 70 )
				{
					clearFrame++;
				}
				if( clearFrame >= 180 )
				{
					g_afGameSequence = SEQUENCE_AF_GAMECLEAR; 
					clearFrame = 0;
				}
				if( g_boss1pop == false || enemyRepopCount >= 3600 )
				{
					
					//仕様に昇格
					if( rand() % 20 >= 19 )
					{
						SetEnemyState( rand() % SCREEN_WIDTH , rand() % SCREEN_HEIGHT , -2.0f , 0.0f , ENF_STRAIGHT , ENAI_NO_SHOT );
					}

				}

				if( GetKeyboardTrigger( DIK_P ) || GetGamePadTrigger( 9 ) )
				{
					UpdateAfPauseGraphic();
					UpdateAfPauseButton();
					g_afGameSequence = SEQUENCE_AF_PAUSE;
					break;
				}
#ifdef _DEBUG
				if(  GetKeyboardTrigger( DIK_Q )) 
				{
					g_afGameSequence = SEQUENCE_AF_GAMECLEAR; 
				}
#endif
				UpdateAfCursor();
				UpdateAfPlayer();
				UpdateAfPointConvert();
				UpdateAfBackGround();
				UpdateAfBackGround2();
				UpdateAfBackGround3();
				UpdateAfBackGround4();
				UpdateAfBulletPlayer();
				UpdateAfLockonCursor();
				UpdateAfMissilePlayer();
				UpdateAfExplosion();
				UpdateAfEnemy();
				UpdateAfComboNumber();
				UpdateAfGameUi();
				UpdateAfScore();
				UpdateAfBackSky();
				UpdateAfLife();
				if( getAfLife() >= GAMEOVER_DAMAGE )
				{
					g_afGameSequence = SEQUENCE_AF_GAMEOVER;
			//		PlaySound(SOUND_LABEL_SE_GAMEOVER);
				}
				break;
			}
		case SEQUENCE_AF_PAUSE :
			{
				UpdateAfPauseGraphic();
				UpdateAfPauseButton();
				if( GetKeyboardTrigger( DIK_P ) || GetGamePadTrigger( 9 ) )
				{
					g_afGameSequence = SEQUENCE_AF_PLAY;
					break;
				}
				break;
			}
		case SEQUENCE_AF_CONTINUE :
			{
				break;
			}
		case SEQUENCE_AF_GAMEOVER :
			{
				UpdateAfLife();	//プレイヤーに死を納得させる為。
				UpdateAfGameOver();
				if( GetKeyboardTrigger( DIK_RETURN ) || GetGamePadTrigger( 0 ) )
				{
					PlaySound(SOUND_LABEL_SE_ENTER);
					clearFlag = true;
				}
				break;
			}
		case SEQUENCE_AF_GAMECLEAR:
			{
				clearFrame++;
				if( clearFrame >= 180 )
				{
					g_afGameSequence = SEQUENCE_AF_GAMECLEAR; 
					clearFlag = true;
				}
				UpdateAfPlayer2();
				UpdateAfBackGround();
				UpdateAfBackGround2();
				UpdateAfBackGround3();
				UpdateAfBackGround4();
				UpdateAfBulletPlayer();
				UpdateAfLockonCursor();
				UpdateAfMissilePlayer();
				UpdateAfExplosion();
				UpdateAfGameUi();
				UpdateAfScore();
				UpdateAfBackSky();
				UpdateAfLife();
				UpdateAfBackGroundClear();
				break;
			}
		default :
			{
				Error("ゲーム中のシークエンスがありえない遷移をしました。");
			}
	}

	//モード移行に伴う最新スコア保存
	if( GetFade() == FADE_NONE && clearFlag == true )
	{
		int endScore = getAfScore();
		FILE *fp;
		
		if( ( fp = fopen( "data/scsaf.dat" , "wb" ) ) == NULL )
		{
			 Error( "スコアの記録に失敗しました" );
		}
		else
		{
			fwrite( &endScore ,sizeof( endScore ) , 1 , fp );
			fclose(fp);
		}
		SetFade( FADE_OUT_WHITE , MODE_AF_RESULT ); 
	}
}

/*******************************************************************************
* 関数名	:void DrawAfGame( void )
*		ゲーム描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfGame( void )
{
	switch( g_afGameSequence )
	{	
		case SEQUENCE_AF_PAUSE :
			{
				//プレイ中の画面を透かすためにbreakはしない
			}
		case SEQUENCE_AF_COUNTDOWN :
			{
				//プレイ中の画面を透かすためにbreakはしない

			}
		case SEQUENCE_AF_GAMEOVER :
			{
				//プレイ中の画面を透かすためにbreakはしない
			}
		case SEQUENCE_AF_GAMECLEAR :
			{
				//プレイ中の画面を透かすためにbreakはしない
			}
		case SEQUENCE_AF_PLAY :
			{
				DrawAfBackGround2();
				DrawAfBackSky();
				DrawAfBackGround();
				//DrawAfBackObject();
				DrawAfPointConvert();
				DrawAfEnemy();
				DrawAfBackGround4();
				DrawAfBackGround3();
				DrawAfLockonCursor();
				DrawAfBulletPlayer();
				DrawAfMissilePlayer();
				DrawAfExplosion();
				DrawAfPlayer();
				//DrawAfCursor();
				DrawAfGameUi();
				DrawAfScore();
				DrawAfLife();
				DrawAfComboNumber();
				if( g_afGameSequence == SEQUENCE_AF_PAUSE )
				{				
					if( GetGamePadPress( 0 ) && GetGamePadPress( 1 ) && GetGamePadPress( 2 ) && GetGamePadPress( 4 ) )
					{
					}
					else
					{
						DrawAfPauseGraphic();
						DrawBackPlusAlpha();
						DrawAfPauseButton();
					}
				}
				if( g_afGameSequence == SEQUENCE_AF_GAMEOVER )
				{
					DrawAfGameOver();
				}
				if( g_afGameSequence == SEQUENCE_AF_GAMECLEAR )
				{
					DrawAfBackGroundClear();
				}
				break;
			}
		case SEQUENCE_AF_CONTINUE :
			{
				break;
			}
		default :
			{
				Error("ゲーム中のシークエンスがありえない遷移をしました。");
			}
	}
}

void busterEnemyNumPlusOne( void )
{
	g_busterEnemyNum++;
}

void busterEnemyBoss1( void )
{
	g_busterEnemyBoss1 = true;
}