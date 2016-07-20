/*******************************************************************************
* タイトル	:ゲーム.cpp
* ファイル名:Game.cpp
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
#include "game.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

#include "cursor.h"
#include "backGround.h"
#include "backGround2.h"
#include "backGround3.h"
#include "backGround4.h"
#include "backGround5.h"
#include "player01.h"
#include "barrier.h"

#include "explosion.h"
#include "enemy.h"
#include "bullet.h"
#include "sideCharacter.h"
#include "startCounter.h"
#include "frameLeft.h"
#include "frameRight.h"

#include "hystericGauge.h"
#include "timer.h"
#include "score.h"
#include "comboNumber.h"
#include "comboUi.h"
#include "pauseGraphic.h"
#include "stageManager.h"

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
static GAME_SEQUENCE g_gameSequence;
static int soundLabelBufferPause;
static int timerFpsSwitch;
/*******************************************************************************
* 関数名	:HRESULT InitGame( void )
*		ゲーム初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitGame( void )
{
	
	/*背景の設定*/
	if( InitBackGround() != S_OK )
	{
		Error("背景データの初期化に失敗しました。");
	}
	if( InitBackGround2() != S_OK )
	{
		Error("背景データ2の初期化に失敗しました。");
	}
	if( InitBackGround3() != S_OK )
	{
		Error("背景データ3の初期化に失敗しました。");
	}
	if( InitBackGround4() != S_OK )
	{
		Error("背景データ4の初期化に失敗しました。");
	}
	if( InitBackGround5() != S_OK )
	{
		Error("背景データ5の初期化に失敗しました。");
	}
	/*ポリゴンの設定*/
	if( InitCursor() != S_OK )
	{
		Error("ポリゴンの初期化に失敗しました。");
	}
	/*プレイヤーの設定*/
	if( InitPlayer01() != S_OK )
	{
		Error("プレイヤーの初期化に失敗しました。");
	}
	/*バリアーの設定*/
	if( InitBarrier() != S_OK )
	{
		Error("バリアーの初期化に失敗しました。");
	}
	/*弾の設定*/
	if( InitBullet() != S_OK )
	{
		Error("弾の初期化に失敗しました。");
	}
	/*爆発の設定*/
	if( InitExplosion() != S_OK )
	{
		Error("爆発の初期化に失敗しました。");
	}
	/*弾の設定*/
	if( InitEnemy() != S_OK )
	{
		Error("弾の初期化に失敗しました。");
	}
	/*フィールド枠の設定*/
	if( InitFrameLeft() != S_OK )
	{
		Error("左フレーム枠の初期化に失敗しました。");
	}
	if( InitFrameRight() != S_OK )
	{
		Error("右フレーム枠の初期化に失敗しました。");
	}
	/*キャラクター背景の設定*/
	if( InitSideCharacter() != S_OK )
	{
		Error("キャラクター背景の初期化に失敗しました。");
	}
	/*ヒステリックゲージの設定*/
	if( InitHystericGauge() != S_OK )
	{
		Error("ゲージUIの初期化に失敗しました。");
	}
	/*タイマーの設定*/
	if( InitTimer() != S_OK )
	{
		Error("タイマーの初期化に失敗しました。");
	}
	/*スコアの設定*/
	if( InitScore() != S_OK )
	{
		Error("スコアの初期化に失敗しました。");
	}
	/*コンボ数の設定*/
	if( InitComboNumber() != S_OK )
	{
		Error("コンボ数の初期化に失敗しました。");
	}
	/*コンボ表示の設定*/
	if( InitComboUi() != S_OK )
	{
		Error("コンボ表示の初期化に失敗しました。");
	}
	/*ポーズ表示の設定*/
	if( InitPauseGraphic() != S_OK )
	{
		Error("ポーズ表示の初期化に失敗しました。");
	}
	/*スタートカウントの設定*/
	if( InitStartCounter() != S_OK )
	{
		Error("スタートカウントの初期化に失敗しました。");
	}
	//ステージマネージャーの設定
	if( InitStageManager() != S_OK )
	{
		Error("ステージマネージャーの初期化に失敗しました。");
	}

	g_gameSequence = SEQUENCE_PLAY;		//初期化がUpdate通る前提だった為、緊急回避策
	UpdateGame();
	
	/*タイマーゼロ固定*/
	resetTimer( 0 );
	UpdateTimer();
	g_gameSequence = SEQUENCE_COUNTDOWN;
#ifndef _DEBUG
	g_gameSequence = SEQUENCE_COUNTDOWN;
#endif
	timerFpsSwitch = 0;

	PlaySound(SOUND_LABEL_BGM_STAGE1);
	soundLabelBufferPause = SOUND_LABEL_BGM_STAGE1;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitGame( void )
*		ゲーム終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitGame( void )
{
	/*ステージマネージャーの解放*/
	UninitStageManager();

	/*背景の開放*/
	UninitBackGround();
	UninitBackGround2();
	UninitBackGround3();
	UninitBackGround4();
	UninitBackGround5();

	/*ポリゴンの開放*/
	UninitCursor();
	
	/*バリアーの解放*/
	UninitBarrier();

	/*プレイヤーの解放*/
	UninitPlayer01();
	
	/*フィールド枠の解放*/
	UninitFrameLeft();
	UninitFrameRight();

	/*キャラクター背景の解放*/
	UninitSideCharacter();

	/*ヒステリックゲージの解放*/
	UninitHystericGauge();

	
	/*タイマーの解放*/
	UninitTimer();

	/*スコアの解放*/
	UninitScore();
	
	/*コンボ数の解放*/
	UninitComboNumber();

	/*コンボ表示の解放*/
	UninitComboUi();

	/*爆発の解放*/
	UninitExplosion();

	/*弾の解放*/
	UninitBullet();

	/*エネミーの解放*/
	UninitEnemy();

	/*ポーズ表示の解放*/
	UninitPauseGraphic();

	/*スタートカウントの開放*/
	UninitStartCounter();
}

/*******************************************************************************
* 関数名	:void UpdateGame( void )
*		ゲーム更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateGame( void )
{
	int clearFlag = false;

	
#ifdef _DEBUG

	if( GetKeyboardTrigger( DIK_E) )
	{
		SetBulletPlayer( 500000 , 12 , 0 , 0 , -10000 , 0 , 12 );
	}
	if( GetKeyboardTrigger( DIK_R ) )
	{
		SetBulletLiar( 500000 ,12, 0 , 0 , -10000 , 0 , 12 , BULLET_HOMING );
	}
	if( GetKeyboardTrigger( DIK_1 ) )
	{
		SetEnemy( 400000 , 100000 , 0 ,0 , 0 , 0 , 50 , CATEGORY_ENEMY , ENEMY_002 );
		//	SetEnemyEnemy( 400000 , 100000 , 0 ,0 , 0 , 0 , 12 );
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
	}
	
	if( GetKeyboardTrigger( DIK_2 ) )
	{
		SetEnemy( 400000 , 100000 , 0 ,0 , 0 , 0 , 50 , CATEGORY_ENEMY , ENEMY_002 );
		//	SetEnemyEnemy( 400000 , 100000 , 0 ,0 , 100 , 0 , 12 );
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
	}
	
	if( GetKeyboardPress( DIK_T ) )
	{
		for( int i = 0; i<= 40; i++ )
		{
			SetBulletEnemy( 400000 , 100 , 0 , 5900 * sin( D3DX_PI * 0.05 * i + 0.01 ) , 5900  * cos( D3DX_PI * 0.05 * i + 0.01 ) , 0 , 12 );
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
		}
	}
	if( GetKeyboardPress( DIK_Y ) )
	{
		for( int i = 0; i<= 40; i++ )
		{
			SetBulletLiar( 400000 , 30000 , 0 , 5900 * sin( D3DX_PI * 0.05 * i + 0.01 ) , 5900  * cos( D3DX_PI * 0.05 * i + 0.01 ) , 0 , 12 , BULLET_NORMAL);
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
		}
	}

	if( GetKeyboardPress( DIK_UP ) )
	{	
		PlusScore( 12345678 );
	}
	if( GetKeyboardPress( DIK_DOWN ) )
	{
		PlusScore( -12345 );
	}


#endif

	switch( g_gameSequence )
	{
		case SEQUENCE_COUNTDOWN :
			{
				/*スタートカウントの更新*/

				/*タイマーゼロ固定*/
				resetTimer( 0 );

				/*背景の更新*/
				UpdateBackGround();
				UpdateBackGround2();
				UpdateBackGround3();
				UpdateBackGround4();
				UpdateBackGround5();
				
				/*フィールド枠の更新*/
				UpdateFrameLeft();
				UpdateFrameRight();

				/*キャラクター背景の更新*/
				UpdateSideCharacter();

				if( UpdateStartCounter() == true )//カウントしている最中は動けない（「GO！」からは動ける）
				{
					break;
				}
				g_gameSequence = SEQUENCE_PLAY;
			}
		case SEQUENCE_PLAY :
			{
				if( g_gameSequence == SEQUENCE_PLAY )
				{
					timerFpsSwitch = ( timerFpsSwitch + 1 == 3 ? 0 : timerFpsSwitch + 1 );
					if( timerFpsSwitch % 3 == 0 )
					{
						PlusTimer( 16 );
					}
					else
					{
						PlusTimer( 17 );
					}
					clearFlag = UpdateStageManager();
				}
				if( GetKeyboardTrigger( DIK_P ) || GetGamePadTrigger( 9 ) )
				{
					StopSoundOneTime( ( SOUND_LABEL )( soundLabelBufferPause ) );
					g_gameSequence = SEQUENCE_PAUSE;
					PlaySound( SOUND_LABEL_BGM_PAUSE );
					break;
				}
				UpdateStartCounter();

				/*背景の更新*/
				UpdateBackGround();
				UpdateBackGround2();
				UpdateBackGround3();
				UpdateBackGround4();
				UpdateBackGround5();

				/*ポリゴンの更新*/
				UpdateCursor();

				/*プレイヤーの更新*/
				UpdatePlayer01();

				/*バリアーの更新*/
				UpdateBarrier();
	
				/*弾の更新*/
				UpdateBullet();

				/*エネミーの更新*/
				UpdateEnemy();

				/*爆発の更新*/
				UpdateExplosion();
				
				/*フィールド枠の更新*/
				UpdateFrameLeft();
				UpdateFrameRight();

				/*キャラクター背景の更新*/
				//UpdateSideCharacter();

				/*ヒステリックゲージの更新*/
				UpdateHystericGauge();
	
				/*コンボ数の更新*/
				UpdateComboNumber();

				/*コンボ表示の更新*/
				UpdateComboUi();

				/*タイマーの更新*/
				UpdateTimer();

				/*スコアの更新*/
				UpdateScore();

				break;
			}
		case SEQUENCE_PAUSE :
			{
				/*ポーズ表示の更新*/
				UpdatePauseGraphic();
				if( GetKeyboardTrigger( DIK_P ) || GetGamePadTrigger( 9 ) )
				{
					StopSound( SOUND_LABEL_BGM_PAUSE );
					PlaySoundOneMoreTime( ( SOUND_LABEL )( soundLabelBufferPause ) );
					g_gameSequence = SEQUENCE_PLAY;
				}
				break;
			}
		case SEQUENCE_CONTINUE :
			{
				break;
			}
		default :
			{
				Error("ゲーム中のシークエンスがありえない遷移をしました。");
			}
	}

	if( GetFade() == FADE_NONE && ( GetKeyboardTrigger( DIK_RETURN ) ) )//|| GetGamePadTrigger( 0 ) ))
	{
		clearFlag = true;
	}

	//モード移行に伴う最新スコア保存
	if( GetFade() == FADE_NONE && clearFlag == true )
	{
		int endScore = getScore();
		FILE *fp;
		
		if( ( fp = fopen( "data/scs.dat" , "wb" ) ) == NULL )
		{
			 Error( "スコアの記録に失敗しました" );
		}
		else
		{
			fwrite( &endScore ,sizeof( endScore ) , 1 , fp );
			fclose(fp);
		}
		SetFade( FADE_OUT , MODE_RESULT ); 
	}
}

/*******************************************************************************
* 関数名	:void DrawGame( void )
*		ゲーム描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawGame( void )
{
	switch( g_gameSequence )
	{	
		case SEQUENCE_PAUSE :
			{
				//プレイ中の画面を透かすためにbreakはしない
			}
		case SEQUENCE_COUNTDOWN :
			{
				//プレイ中の画面を透かすためにbreakはしない

			}
		case SEQUENCE_PLAY :
			{
				/*背景の描画:遠景順*/
				DrawBackGround5();
				DrawBackGround4();
				DrawBackGround();
				DrawBackGround2();
				DrawBackGround3();
	
	
				/*コンボ数の描画*/
				DrawComboNumber();

				/*コンボ表示の描画*/
				DrawComboUi();
	
				/*バリアーの描画*/
				DrawBarrier();
	
				/*プレイヤーの描画*/
				DrawPlayer01();
	


				DrawBullet();
	
				DrawEnemy();
				DrawExplosion();
				
				/*コンボ数の描画*/
				DrawComboNumber2();
				
				/*コンボ表示の描画*/
				DrawComboUi2();

				DrawPlayer01core();

				/*フィールド枠の描画*/
				DrawFrameLeft();
				DrawFrameRight();

				/*キャラクター背景の描画*/
				//DrawSideCharacter();

				/*ヒステリックゲージの描画*/
				DrawHystericGauge();

				
	
				/*タイマーの描画*/
				DrawTimer();

				/*スコアの描画*/
				DrawScore();


				/*カーソルの描画*/
				DrawCursor();
				
				/*スタートカウントの描画*/
				DrawStartCounter();

				if( g_gameSequence == SEQUENCE_PAUSE )
				{
					/*ポーズ表示の描画*/
					DrawPauseGraphic();
				}
				break;
			}
		case SEQUENCE_CONTINUE :
			{
				break;
			}
		case SEQUENCE_GAMEOVER :
			{
				break;
			}
		default :
			{
				Error("ゲーム中のシークエンスがありえない遷移をしました。");
			}
	}

	DrawStageManager();
}