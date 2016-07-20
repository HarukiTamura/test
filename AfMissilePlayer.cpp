/*******************************************************************************
* タイトル	:自弾.cpp
* ファイル名:AfMissilePlayer.cpp
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
#include "AfMissilePlayer.h"
#include "AfPlayer.h"
#include "AfScore.h"
#include "AfEnemy.h"
#include "AfCursor.h"
#include "unoDirectX.h"
#include "AfExplosion.h"
#include "unoHitChecker.h"
#include "AfComboNumber.h"
#include "afGame.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define AF_MISSILE_PLAYER_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_MISSILE_PLAYER_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_MISSILE_PLAYER_DEFAULT_VECTOR	( 0.0f )
#define AF_MISSILE_PLAYER_DEFAULT_POSZ	( 0.0f )
#define AF_MISSILE_PLAYER_DEFAULT_LIFE		( 100 )
#define AF_MISSILE_PLAYER_DEFAULT_SCORE		( 10 )
#define AF_MISSILE_PLAYER_ATTACK	( 10 )

#define AF_MISSILE_PLAYER_MOVE	( 3 )
#define AF_MISSILE_PLAYER_SPEED	( 0.003f )
#define AF_MISSILE_PLAYER_OUTSCREEN	( 1.20f )
#define AF_MISSILE_PLAYER_DELETE		( 0.00f )

#define AF_MISSILE_PLAYER_SPIN	( 1.23 )

#define AF_MISSILE_PLAYER_VERTEX_MAX		( 4 )	/*最高頂点数*/
#define AF_MISSILE_PLAYER_POLYGON_NUM	( MISSILE_PLAYER_MAX_NUM )	/*自機＋判定*/

#define AF_MISSILE_PLAYER_ANIM_MAX		( 3 )	/*アニメーションコマ数*/
#define AF_MISSILE_PLAYER_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define AF_MISSILE_PLAYER_ANIM_DIV_X	( 3 )	/*幅分割数*/
#define AF_MISSILE_PLAYER_ANIM_DIV_Y	( 5 )	/*高さ分割数*/

#define AF_MISSILE_PLAYER_OUTPUT_SIZE	( 1.0f ) /*初期表示倍率*/ 

#define MOVE_NUM ( 0.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.10f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.23f )	/*減衰率*/
#define ROT_ATTEN		( 0.85f )	/*減衰率*/
#define SIPIN_ROT_ATTEN ( 0.12f )

#define CHASE_SPEED		( 0.18f )

#define CHASE_START		( 0.90 )
#define STAY_END		( 30 )

/*******************************************************************************
* 構造体定義
*******************************************************************************/


/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfMissilePlayer = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureAfMissilePlayer;	/*自弾テクスチャのポインタ*/
static D3DXIMAGE_INFO info;		/*画像情報*/

static D3DXVECTOR3 g_movePolygon;	/*ポリゴンの位置*/
static D3DXVECTOR3 g_posPolygon;	/*ポリゴンの移動量*/
static D3DXVECTOR3 g_rotPolygon;	/*ポリゴンの回転角*/
static float g_fLengthPolygon;		/*ポリゴンの対角線の長さ*/
static float g_fAnglePolygon;		/*ポリゴンの対角線の角度*/
static float polygonWidth , polygonHeight;	/*ポリゴンの幅／高さ（全体の半分）*/

static float polygonMag;	/*表示倍率*/

static int g_nCounterAnim = 0;
static int g_nPatternAnim = 0;

static MISSILE_PLAYER_STATUS missilePlayerState[MISSILE_PLAYER_MAX_NUM]; 

/*******************************************************************************
* 関数名	:HRESULT InitAfMissilePlayer( void )
*		自弾初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfMissilePlayer( void )
{
	HRESULT result;
	result = InitDirectAfMissilePlayer();
	if( result != S_OK )
	{
		Error("描画基準位置データの初期化に失敗しました。");
	}
	InitValueAfMissilePlayer();
	return result;
}

/*******************************************************************************
* 関数名	:HRESULT InitDirectAfMissilePlayer( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitDirectAfMissilePlayer( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_MISSILE_PLAYER_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfMissilePlayer , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfMissilePlayer.png" , &g_pTextureAfMissilePlayer );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfMissilePlayer.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffAfMissilePlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_MISSILE_PLAYER_POLYGON_NUM );

	/*ポインタのアンロック*/
	g_pVtxBuffAfMissilePlayer->Unlock();
}

/*******************************************************************************
* 関数名	:HRESULT InitValueAfMissilePlayer( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void InitValueAfMissilePlayer( void )
{
	int outputSizeX , outputSizeY;
	
	//事前サイズ計算
	outputSizeX = ( int )( ( info.Width / AF_MISSILE_PLAYER_ANIM_DIV_X ) * AF_MISSILE_PLAYER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_MISSILE_PLAYER_ANIM_DIV_Y ) * AF_MISSILE_PLAYER_OUTPUT_SIZE );

	//事前サイズ変換
	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	
	//初期化
	memset( &missilePlayerState , 0 , sizeof( MISSILE_PLAYER_STATUS ) * MISSILE_PLAYER_MAX_NUM );
	
	for( int cnt = 0; cnt < MISSILE_PLAYER_MAX_NUM; cnt++ )
	{
		missilePlayerState[cnt].bUse = false;
		missilePlayerState[cnt].posStatus.posX = AF_MISSILE_PLAYER_DEFAULT_POSX;
		missilePlayerState[cnt].posStatus.posY = AF_MISSILE_PLAYER_DEFAULT_POSY;
		missilePlayerState[cnt].posStatus.vector = AF_MISSILE_PLAYER_DEFAULT_VECTOR;
		missilePlayerState[cnt].moveZ = 0.0f;
		missilePlayerState[cnt].posZ = AF_MISSILE_PLAYER_DEFAULT_POSZ;

		missilePlayerState[cnt].intPosX = ( int )( missilePlayerState[cnt].posStatus.posX ) * 1000;
		missilePlayerState[cnt].intPosY = ( int )( missilePlayerState[cnt].posStatus.posY ) * 1000;
				  
		missilePlayerState[cnt].nLife = AF_MISSILE_PLAYER_DEFAULT_LIFE;
	}

	g_posPolygon.x = AF_MISSILE_PLAYER_DEFAULT_POSX;
	g_posPolygon.y = AF_MISSILE_PLAYER_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*アニメーション用ステータス*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	
}

/*******************************************************************************
* 関数名	:void UninitAfMissilePlayer( void )
*		自弾終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfMissilePlayer( void )
{
	if( g_pVtxBuffAfMissilePlayer != NULL )
	{
		g_pVtxBuffAfMissilePlayer->Release();
		g_pVtxBuffAfMissilePlayer = NULL;
	}
	if( g_pTextureAfMissilePlayer != NULL )
	{
		g_pTextureAfMissilePlayer->Release();
		g_pTextureAfMissilePlayer = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfMissilePlayer( void )
*		自弾更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfMissilePlayer( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	int targetNum = 0;
	float targetPosX , targetPosY;
	bool soundFlag = false;

	/*ポインタのロック*/ 
	g_pVtxBuffAfMissilePlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	targetPosY = SCREEN_HEIGHT * 0.5f; 
				///getViewStatus()->centerPosX;
	targetPosX = SCREEN_WIDTH  * 0.5f; 
				///getViewStatus()->centerPosX;
	for( int cnt = 0; cnt < MISSILE_PLAYER_MAX_NUM; cnt++ )
	{
	//	
	//	
		for( ; targetNum < ENEMY_MAX_NUM; targetNum++ )
		{
			if( GetEnemyState(cnt)->bUse == true && GetEnemyState(cnt)->bLockOn == true )
			{		
				if( missilePlayerState[cnt].posZ >= GetEnemyState(cnt)->posZ )
				{
					targetPosX = GetEnemyState(cnt)->posStatus.DrawPosX;
					targetPosY = GetEnemyState(cnt)->posStatus.DrawPosY;
					targetNum++;
					break;
				}
			}
		}

		if( missilePlayerState[cnt].bUse == true )
		{
			for( int cnt2 = 0; cnt2 < AF_MISSILE_PLAYER_MOVE; cnt2++ )
			{
				//行動
				missilePlayerState[cnt].moveZ = -AF_MISSILE_PLAYER_SPEED;
				
				pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

				if( missilePlayerState[cnt].posZ >= CHASE_START )
				{
					//点火前
					pVtx[QUOTE_VERTEX * cnt].col	 = D3DCOLOR_RGBA( 100 , 155 , 155 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 100 , 155 , 155 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 100 , 155 , 155 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 100 , 155 , 155 , 255 );

					missilePlayerState[cnt].posStatus.posX += missilePlayerState[cnt].moveX;
					missilePlayerState[cnt].posStatus.posY += missilePlayerState[cnt].moveY;
				}
				else if( missilePlayerState[cnt].stayCnt < STAY_END )
				{
					if( soundFlag == false )
					{
						PlaySound( SOUND_LABEL_SE_MISSILEFLY2 );
						soundFlag = true;
					}
					pVtx[QUOTE_VERTEX * cnt].col	 = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
					missilePlayerState[cnt].stayCnt++;
					missilePlayerState[cnt].posStatus.posX += missilePlayerState[cnt].moveX;
					missilePlayerState[cnt].posStatus.posY += missilePlayerState[cnt].moveY;
				}
				else if( missilePlayerState[cnt].posZ >= GetEnemyState(cnt)->posZ  )
				{
					if( targetPosX < missilePlayerState[cnt].posStatus.posX )
					{
						missilePlayerState[cnt].posStatus.posX -= ( missilePlayerState[cnt].posStatus.posX - targetPosX ) * CHASE_SPEED * sin( missilePlayerState[cnt].posZ );
					}
					if( targetPosX > missilePlayerState[cnt].posStatus.posX )
					{
						missilePlayerState[cnt].posStatus.posX += ( targetPosX - missilePlayerState[cnt].posStatus.posX ) * CHASE_SPEED * sin( missilePlayerState[cnt].posZ );
					}
					if( targetPosY < missilePlayerState[cnt].posStatus.posY )
					{
						missilePlayerState[cnt].posStatus.posY -= ( missilePlayerState[cnt].posStatus.posY - targetPosY ) * CHASE_SPEED * sin( missilePlayerState[cnt].posZ );
					}
					if( targetPosY > missilePlayerState[cnt].posStatus.posY )
					{
						missilePlayerState[cnt].posStatus.posY += ( targetPosY - missilePlayerState[cnt].posStatus.posY ) * CHASE_SPEED * sin( missilePlayerState[cnt].posZ );
					}
				}
				//死亡確認
				if( missilePlayerState[cnt].nLife <= 0 || missilePlayerState[cnt].posZ >= AF_MISSILE_PLAYER_OUTSCREEN || missilePlayerState[cnt].posZ <= AF_MISSILE_PLAYER_DELETE )
				{
					missilePlayerState[cnt].bUse = false;
				}
				//実質位置確定

				//描画座標・サイズ確定
				missilePlayerState[cnt].posZ += missilePlayerState[cnt].moveZ;
				missilePlayerState[cnt].posStatus.DrawPosX = missilePlayerState[cnt].posStatus.posX;
				missilePlayerState[cnt].posStatus.DrawPosY = missilePlayerState[cnt].posStatus.posY;
				missilePlayerState[cnt].posStatus.DrawVector = missilePlayerState[cnt].posStatus.vector - getViewStatus()->centerVector;
				polygonMag = 0.8 - sin( 1.0 - sin( ( missilePlayerState[cnt].posZ >= 0.0f ? missilePlayerState[cnt].posZ : 0.0f ) ) );
				//polygonMag = missilePlayerState[cnt].posZ;
				
				SetAfExplosion( missilePlayerState[cnt].intPosX , missilePlayerState[cnt].intPosY , missilePlayerState[cnt].posZ * 100 , 0 , 0 , missilePlayerState[cnt].moveZ * 1300  , 4 , AF_EXPLOSION_002 );
				
				//missilePlayerState[cnt].posStatus.DrawPosX = ( missilePlayerState[cnt].posStatus.DrawPosX * ( polygonMag <= 0.0f ? 0.0f : polygonMag ) ) + SCREEN_WIDTH  * 0.5 * ( 1.0f - ( polygonMag * 0.5 >= 1.0f ? 1.0f : polygonMag * 0.5 ) ); 
				//missilePlayerState[cnt].posStatus.DrawPosY = ( missilePlayerState[cnt].posStatus.DrawPosY * ( polygonMag <= 0.0f ? 0.0f : polygonMag ) ) + ( SCREEN_HEIGHT - getViewStatus()->centerPosY ) * ( 1.0f - ( polygonMag >= 1.0f ? 1.0f : polygonMag) );
			
				g_posPolygon.x = missilePlayerState[cnt].posStatus.DrawPosX;
				g_posPolygon.y = missilePlayerState[cnt].posStatus.DrawPosY;
				g_rotPolygon.z = missilePlayerState[cnt].posStatus.DrawVector;
			
				missilePlayerState[cnt].intPosX = ( int )( missilePlayerState[cnt].posStatus.DrawPosX ) * 1000;
				missilePlayerState[cnt].intPosY = ( int )( missilePlayerState[cnt].posStatus.DrawPosY ) * 1000;
				
				for( int cnt3 = 0; cnt3 < ENEMY_MAX_NUM; cnt3++ )
				{
					if( GetEnemyState(cnt3)->bUse == true && 
						HitTwinCircle( GetEnemyState(cnt3)->intPosX , GetEnemyState(cnt3)->intPosY , ENEMY_RADIUS ,
						missilePlayerState[cnt].intPosX , missilePlayerState[cnt].intPosY , MISSILE_PLAYER_RADIUS ) == true &&
						GetEnemyState(cnt3)->posZ > missilePlayerState[cnt].posZ - MISSILE_PLAYER_HIT_ZONE && 
						GetEnemyState(cnt3)->posZ < missilePlayerState[cnt].posZ + MISSILE_PLAYER_HIT_ZONE )
					{
						GetEnemyState(cnt3)->nLife = GetEnemyState(cnt3)->nLife - AF_MISSILE_PLAYER_ATTACK;
						if( GetEnemyState(cnt3)->nLife <= 0 )
						{
							busterEnemyNumPlusOne();
							GetEnemyState(cnt3)->bUse = false;
							GetEnemyState(cnt3)->bLockOn = false;
							missilePlayerState[cnt].bUse = false;
							SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 100 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 100 , 10 , AF_EXPLOSION_001 );
							SetAfExplosion( missilePlayerState[cnt].intPosX , missilePlayerState[cnt].intPosY , GetEnemyState(cnt3)->posZ * 800 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 800 , 18 , AF_EXPLOSION_001 );
							SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 100 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 100 , 10 , AF_EXPLOSION_001 );
							if( GetEnemyState(cnt3)->aiPattern == ENEMY_AI_BOSS1 )
							{
								busterEnemyBoss1();
								SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 100 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 100 , 10 , AF_EXPLOSION_003 );
								SetAfExplosion( missilePlayerState[cnt].intPosX , missilePlayerState[cnt].intPosY , GetEnemyState(cnt3)->posZ * 800 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 800 , 18 , AF_EXPLOSION_003 );
								SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 100 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 100 , 10 , AF_EXPLOSION_003 );
							
								SetAfExplosion( missilePlayerState[cnt].intPosX , missilePlayerState[cnt].intPosY , GetEnemyState(cnt3)->posZ * 800 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 800 , 18 , AF_EXPLOSION_003 );
								SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 100 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 100 , 10 , AF_EXPLOSION_003 );
								PlusAfScore( 1000000 );
								PlusAfScore( ( 100 - GetPlayerState()->nLife ) * 1000 );
							}
							PlusAfComboNumber( 1 );
							PlusAfScore( 100 * GetAfComboNumber() );
						}
						SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 1 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 1 , 10 , AF_EXPLOSION_001 );

						PlusAfScore( 3 * GetAfComboNumber() );
						break;
					}
				}
			}

			//オーバー修正
			if( g_posPolygon.x < SCREEN_LEFT )
			{
				g_posPolygon.x = SCREEN_LEFT;
			}					 
			if( g_posPolygon.x > SCREEN_RIGHT )
			{
				g_posPolygon.x = SCREEN_RIGHT;
			}
			if( g_posPolygon.y < FIELD_TOP )
			{
				g_posPolygon.y = FIELD_TOP;
			}
			if( g_posPolygon.y > FIELD_HEIGHT )
			{
				g_posPolygon.y = FIELD_HEIGHT;
			}
	
			if( g_rotPolygon.z > D3DX_PI )
			{
				g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
			}
			if( g_rotPolygon.z < -D3DX_PI )
			{
				g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
			}


			//アニメーション設定
			if( ++g_nCounterAnim == AF_MISSILE_PLAYER_ANIM_SPAN )
			{
				g_nCounterAnim = 0;
				g_nPatternAnim = ( ++g_nPatternAnim == AF_MISSILE_PLAYER_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
	
			texWidth = 1.0f / AF_MISSILE_PLAYER_ANIM_DIV_X;
			texHeight = 1.0f / AF_MISSILE_PLAYER_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % AF_MISSILE_PLAYER_ANIM_DIV_X );
			nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_MISSILE_PLAYER_ANIM_DIV_X ) ) + 0;
	
			//テクスチャ座標設定
			pVtx[QUOTE_VERTEX * cnt].tex.x = texWidth * nowPatternX;
			pVtx[QUOTE_VERTEX * cnt].tex.y = texHeight * nowPatternY;
			pVtx[QUOTE_VERTEX * cnt + 1].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[QUOTE_VERTEX * cnt + 1].tex.y = texHeight * nowPatternY;
			pVtx[QUOTE_VERTEX * cnt + 2].tex.x = texWidth * nowPatternX;
			pVtx[QUOTE_VERTEX * cnt + 2].tex.y = texHeight * nowPatternY + texHeight;
			pVtx[QUOTE_VERTEX * cnt + 3].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[QUOTE_VERTEX * cnt + 3].tex.y = texHeight * nowPatternY + texHeight;

			//頂点座標設定
			pVtx[QUOTE_VERTEX * cnt].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
			pVtx[QUOTE_VERTEX * cnt + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

			pVtx[QUOTE_VERTEX * cnt + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

			pVtx[QUOTE_VERTEX * cnt + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
			
		}
		if( missilePlayerState[cnt].bUse == false )
		{			
			pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
		}
	}

	/*ポインタのアンロック*/
	g_pVtxBuffAfMissilePlayer->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawAfMissilePlayer( void )
*		自弾描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfMissilePlayer( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfMissilePlayer );
	
	for( int cnt = 0; cnt < MISSILE_PLAYER_MAX_NUM; cnt++ )
	{
		if( missilePlayerState[cnt].bUse != false )
		{			
			/*頂点バッファをストリームにバインド*/
			pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfMissilePlayer , 0 , sizeof( VERTEX_2D ) );

			/*自弾の描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , QUOTE_VERTEX * cnt , 2 );
		}
	}
}

/*******************************************************************************
* 関数名	:void DrawAfMissilePlayercore( void )
*		自弾描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfMissilePlayercore( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfMissilePlayer );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfMissilePlayer , 0 , sizeof( VERTEX_2D ) );
	
	
	/*自弾の描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_MISSILE_PLAYER_VERTEX_MAX, 2 );
}

/*******************************************************************************
* 関数名	:MISSILE_PLAYER_STATUS* GetMissilePlayerState( int num )
*		自弾のステータスを取得します。
* 引数		:int num	入手する構造体の番号
* 戻り値	:void 戻り値なし
* 説明		:自弾のステータスを取得します。。
*******************************************************************************/
MISSILE_PLAYER_STATUS* GetMissilePlayerState( int num )
{
	return &missilePlayerState[num];
}

/*******************************************************************************
* 関数名	:MISSILE_PLAYER_STATUS* GetMissilePlayerState( int num )
*		自弾のステータスを取得します。
* 引数		:int num	入手する構造体の番号
* 戻り値	:void 戻り値なし
* 説明		:自弾のステータスを取得します。。
*******************************************************************************/
void SetMissilePlayerState( float posX , float posY , float posZ , float moveX , float moveY , float moveZ )
{
	for( int cnt = 0; cnt < MISSILE_PLAYER_MAX_NUM; cnt++ )
	{
		if( missilePlayerState[cnt].bUse == false )
		{
			missilePlayerState[cnt].bUse = true;
			missilePlayerState[cnt].posStatus.posX = posX;
			missilePlayerState[cnt].posStatus.posY = posY;
			missilePlayerState[cnt].posStatus.vector = 0.0f;
			missilePlayerState[cnt].moveX = moveX;
			missilePlayerState[cnt].moveY = moveY;
			missilePlayerState[cnt].moveZ = moveZ;
			missilePlayerState[cnt].posZ = posZ;
			missilePlayerState[cnt].stayCnt = 0;

			missilePlayerState[cnt].intPosX = ( int )( missilePlayerState[cnt].posStatus.posX ) * 1000;
			missilePlayerState[cnt].intPosY = ( int )( missilePlayerState[cnt].posStatus.posY ) * 1000;
				  
			missilePlayerState[cnt].nLife = AF_MISSILE_PLAYER_DEFAULT_LIFE;
			break;
		}
	}
}