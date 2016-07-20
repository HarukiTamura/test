/*******************************************************************************
* タイトル	:ステージマネージャー.cpp
* ファイル名:StageManager.cpp
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
#include "stageManager.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

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
#include "timer.h"

#include "hystericGauge.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define FPS_MAX	( 99999999 )

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static long long nowFps = 0;
static bool clearFlag = false;

/*******************************************************************************
* 関数名	:HRESULT InitStageManager( void )
*		ステージマネージャー初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitStageManager( void )
{
	clearFlag = false;
	resetTimer( 0 );
	nowFps = 0;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitStageManager( void )
*		ステージマネージャー終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitStageManager( void )
{
	
}

/*******************************************************************************
* 関数名	:bool UpdateStageManager( void )
*		ステージマネージャー更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:bool clearFlag	ステージクリアしたかどうかのフラグ
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
bool UpdateStageManager( void )
{
	nowFps = ( nowFps + 1 >= FPS_MAX ? FPS_MAX : nowFps + 1 );
	if( nowFps % 175 == 12 )
	{
		SetEnemy( 400000 - ( rand() % 2000 ) , 100000 - ( rand() % 2000 ) , 0 , 0 , 0 , 0 , 20 , CATEGORY_ENEMY , ENEMY_002 , ENEMY_AI_0004 );
	}
	if( nowFps % 175 == 4 )
	{
		SetEnemy( 200000 - ( rand() % 2000 ) , 100000 - ( rand() % 2000 ) , 0 , 0 , 500 , 0 , 20 , CATEGORY_ENEMY , ENEMY_002 , ENEMY_AI_0001 );
	}
	if( nowFps >= 1800 )//仮に3600秒でclearにしておく。
	{
		clearFlag = true;
	}

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
#endif
	return clearFlag;
}

/*******************************************************************************
* 関数名	:void DrawStageManager( void )
*		ステージマネージャー描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawStageManager( void )
{
	
}

/*******************************************************************************
* 関数名	:int getStageFps( void )
*		経過FPSゲット関数です。
* 引数		:void 引数なし
* 戻り値	:int nowFps		現在の経過FPS
* 説明		:経過FPSをゲットします。
*******************************************************************************/
int getStageFps( void )
{
	return nowFps;
}