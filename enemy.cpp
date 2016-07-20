/*******************************************************************************
* タイトル	:エネミー.cpp
* ファイル名:enemy.cpp
* 作成者	:田村春樹
* 作成日	:2015/06/24
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "game.h"
#include "enemy.h"
#include "input.h"
#include "unoHitChecker.h"
#include "unoDirectX.h"
#include "barrier.h"
#include "bullet.h"
#include "player01.h"
#include "explosion.h"
#include "stageManager.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

#define ENEMY_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define ENEMY_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define ENEMY_ANIM_SPAN		( 10 )	/*アニメーション間隔*/
#define ENEMY_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define ENEMY_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define ENEMY_OUTPUT_SIZE	( 0.25 ) /*初期表示倍率*/ 

#define MOVE_NUM ( 2.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.20f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.05f )	/*減衰率*/

#define ENEMY_DELETE_RADIUS	( 800000 )

/*******************************************************************************
* 構造体定義
*******************************************************************************/


/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffenemy = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureenemy;	/*エネミーのテクスチャのポインタ*/
static D3DXIMAGE_INFO info;		/*画像情報*/

static ENEMY g_aEnemy[ENEMY_MAX];	/*エネミーの構造体*/
static bool frameSwitch = false;	/*交互に切り替わり、フレームが変わったかを識別する。*/
static int browseEnemyCount = ENEMY_MAX;	/*エネミー生成制カウントの開始地点*/
static int createEnemyCount = 0;	/*エネミー数カウント*/

static D3DXVECTOR3 g_movePolygon;	/*ポリゴンの位置*/
static D3DXVECTOR3 g_posPolygon;	/*ポリゴンの移動量*/
static D3DXVECTOR3 g_rotPolygon;	/*ポリゴンの回転角*/
static float g_fLengthPolygon;		/*ポリゴンの対角線の長さ*/
static float g_fAnglePolygon;		/*ポリゴンの対角線の角度*/
static float polygonWidth , polygonHeight;	/*ポリゴンの幅／高さ（全体の半分）*/

static float polygonMag;	/*表示倍率*/

static int g_nCounterAnim = 0;	/*アニメーション間隔カウント*/
static int g_nPatternAnim = 0;	/*パターン数カウント*/

static int lastEnemy = 0;	/*エネミーが存在した最後の部分*/

/*******************************************************************************
* 関数名	:HRESULT InitEnemy( void )
*		エネミーの初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitEnemy( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * ENEMY_VERTEX_MAX * ENEMY_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffenemy , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/enemy.png" , &g_pTextureenemy );
	D3DXGetImageInfoFromFile( "data/TEXTURE/enemy.png" , &info );

	ZeroMemory( &g_aEnemy[0] , sizeof( ENEMY ) * ENEMY_MAX );

	/*ポインタのロック*/
	g_pVtxBuffenemy->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / ENEMY_ANIM_DIV_X ) * ENEMY_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / ENEMY_ANIM_DIV_Y ) * ENEMY_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , ENEMY_MAX );

	/*ポインタのアンロック*/
	g_pVtxBuffenemy->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	/*エネミー更新用ステータスの初期化*/
	frameSwitch = false;
	browseEnemyCount = ENEMY_MAX;
	createEnemyCount = 0;
	
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitEnemy( void )
*		エネミーの終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitEnemy( void )
{
	if( g_pVtxBuffenemy != NULL )
	{
		g_pVtxBuffenemy->Release();
		g_pVtxBuffenemy = NULL;
	}
	if( g_pTextureenemy != NULL )
	{
		g_pTextureenemy->Release();
		g_pTextureenemy = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateEnemy( void )
*		エネミーの更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateEnemy( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	
	/*ポインタのロック*/ 
	g_pVtxBuffenemy->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*エネミーの更新*/
	for( int nCntEnemy = 0; nCntEnemy < ENEMY_MAX; nCntEnemy++ )
	{
		if( g_aEnemy[nCntEnemy].bUse == true )
		{
			if( HitTwinCircle( INT_FIELD_CENTER_X , INT_FIELD_CENTER_Y , ENEMY_DELETE_RADIUS , 
				g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , g_aEnemy[nCntEnemy].radius ) != true  )
			{
				g_aEnemy[nCntEnemy].bUse = false;
			}
			if( g_aEnemy[nCntEnemy].state == ENEMY_ST_EXPLOSION )
			{
				g_aEnemy[nCntEnemy].nLife--;
				SetExplosion( g_aEnemy[nCntEnemy].posX - rand() % g_aEnemy[nCntEnemy].radius * 2 + rand() % g_aEnemy[nCntEnemy].radius * 2 ,
					g_aEnemy[nCntEnemy].posY - rand() % g_aEnemy[nCntEnemy].radius * 2 + rand() % g_aEnemy[nCntEnemy].radius * 2, 0 , 0 , 0 , 0 , 12 , EXPLOSION_001 );
			}
			//寿命チェック
			if( g_aEnemy[nCntEnemy].nLife <= 0 )
			{
				if( g_aEnemy[nCntEnemy].state == ENEMY_ST_EXPLOSION )
				{
					g_aEnemy[nCntEnemy].bUse = false;
				}
				else
				{
					g_aEnemy[nCntEnemy].nLife = 100;
					g_aEnemy[nCntEnemy].state = ENEMY_ST_EXPLOSION;
				}
			}

			switch( g_aEnemy[nCntEnemy].aiType )
			{
				case ENEMY_AI_NORMAL:	/*入力値で移動*/
					g_aEnemy[nCntEnemy].posX += g_aEnemy[nCntEnemy].moveX;
					g_aEnemy[nCntEnemy].posY += g_aEnemy[nCntEnemy].moveY;
					break;
				case ENEMY_AI_0001://入力方向に徐々に加速しながら脱出
					if( getStageFps() % 31 == 30 )
					{
						int cnt = 0;
						for( cnt = 0; cnt < 100; cnt++ )
						{
							SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 5900 * sin( D3DX_PI * 0.05 * cnt + 0.01 ) , 5900  * cos( D3DX_PI * 0.05 * cnt + 0.01 ) , 0 , 12 , BULLET_NORMAL);
						}
					}
					if( getStageFps() % 200 == 199 )
					{
						int cnt = 0;
						for( cnt = 0; cnt < 100; cnt++ )
						{
							SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 2000 * sin( D3DX_PI * 0.05 * cnt + 0.01 ) , 2000  * cos( D3DX_PI * 0.05 * cnt + 0.01 ) , 0 , 12 , BULLET_NORMAL);
						}
					}
					g_aEnemy[nCntEnemy].moveY += 1.25;
					g_aEnemy[nCntEnemy].posX += g_aEnemy[nCntEnemy].moveX;
					g_aEnemy[nCntEnemy].posY += g_aEnemy[nCntEnemy].moveY;
					break;
				case ENEMY_AI_0002:
					if( getStageFps() % 36 <= 0 )
					{

						float angle = atan2f( ( float )( GetPlayerState01()->intPosX - g_aEnemy[nCntEnemy].posX ), ( float )( GetPlayerState01()->intPosY - g_aEnemy[nCntEnemy].posY ) );
						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 5000 * sin( angle ) , 5000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
					}
					g_aEnemy[nCntEnemy].moveY += 1.25;
					g_aEnemy[nCntEnemy].posX += g_aEnemy[nCntEnemy].moveX;
					g_aEnemy[nCntEnemy].posY += g_aEnemy[nCntEnemy].moveY;
					break;
				case ENEMY_AI_0003:
					if( getStageFps() % 36 <= 0 )
					{
						float angle = atan2f( ( float )( GetPlayerState01()->intPosX - g_aEnemy[nCntEnemy].posX ), ( float )( GetPlayerState01()->intPosY - g_aEnemy[nCntEnemy].posY ) );
						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 7000 * sin( angle ) , 7000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 7000 * sin( angle - 0.2 ) , 7000  * cos( angle - 0.2 ) , 0 , 12 , BULLET_NORMAL);
						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 7000 * sin( angle + 0.2 ) , 7000  * cos( angle + 0.2 ) , 0 , 12 , BULLET_NORMAL);
					}
					g_aEnemy[nCntEnemy].moveY += 1.02;
					g_aEnemy[nCntEnemy].posX += g_aEnemy[nCntEnemy].moveX;
					g_aEnemy[nCntEnemy].posY += g_aEnemy[nCntEnemy].moveY;
				case ENEMY_AI_0004:
					if( getStageFps() % 36 <= 0 )
					{
						float angle = atan2f( ( float )( GetPlayerState01()->intPosX - g_aEnemy[nCntEnemy].posX ), ( float )( GetPlayerState01()->intPosY - g_aEnemy[nCntEnemy].posY ) );
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 3000 * sin( angle ) , 3000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 4000 * sin( angle ) , 2000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 1000 * sin( angle ) , 1000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);

						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 3000 * sin( angle ) , 3000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 4000 * sin( angle ) , 2000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 1000 * sin( angle ) , 1000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);

						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 3000 * sin( angle ) , 3000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 4000 * sin( angle ) , 2000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 1000 * sin( angle ) , 1000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);

						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 3000 * sin( angle - 0.2 ) , 3000  * cos( angle - 0.2 ) , 0 , 12 , BULLET_NORMAL);
						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 3000 * sin( angle + 0.2 ) , 3000  * cos( angle + 0.2 ) , 0 , 12 , BULLET_NORMAL);

					}
					g_aEnemy[nCntEnemy].moveY += 1.02;
					g_aEnemy[nCntEnemy].posX += g_aEnemy[nCntEnemy].moveX;
					g_aEnemy[nCntEnemy].posY += g_aEnemy[nCntEnemy].moveY;
				case ENEMY_AI_0005:
				case ENEMY_AI_0006:
				case ENEMY_AI_0007:
				case ENEMY_AI_0008:
				case ENEMY_AI_0009:
				case ENEMY_AI_0010:
				case ENEMY_AI_0011:
				case ENEMY_AI_0012:
				case ENEMY_AI_MAX:
				default:
					{
						break;
					}
			}
			

			/*パターンアニメーション事前計算部分*/
			if( ++g_nCounterAnim %= ENEMY_ANIM_SPAN )
			{
				++g_nPatternAnim %= ENEMY_ANIM_MAX;
				//g_nPatternAnim = ( ++g_nPatternAnim == ENEMY_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
			texWidth = 1.0f / ENEMY_ANIM_DIV_X;
			texHeight = 1.0f / ENEMY_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % ENEMY_ANIM_DIV_X );
			
		
			nowPatternY = ( float )( ( int )( g_nPatternAnim / ENEMY_ANIM_DIV_X ) ) + g_aEnemy[nCntEnemy].type;
			
			
			switch( g_aEnemy[nCntEnemy].state )
			{
				case ENEMY_ST_NORMAL:
					{
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
						break;
					}
				case ENEMY_ST_DAMAGE:
					{
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 122 , 122 , 220 , 235 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 122 , 122 , 220 , 235 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 122 , 122 , 220 , 235 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 122 , 122 , 220 , 235 );					
						g_aEnemy[nCntEnemy].state = ENEMY_ST_NORMAL;
						break;
					}
				case ENEMY_ST_EXPLOSION:
					{
						int color = ( int )( 255 * ( float( g_aEnemy[nCntEnemy].nLife ) / 255.0 ) );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 0 , 0 , color );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 0 , 0 , color );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 0 , 0 , color );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 0 , 0 , color );
						break;
					}
				default:
					{
						break;
					}
			}

			/*テクスチャ座標生成*/
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

			/*頂点座標設定*/
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].pos.x = g_aEnemy[nCntEnemy].posX * 0.001 + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].pos.y = g_aEnemy[nCntEnemy].posY * 0.001 + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].pos.z = 0.0f;
											      
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].pos.x = g_aEnemy[nCntEnemy].posX * 0.001 + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].pos.y = g_aEnemy[nCntEnemy].posY * 0.001 + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].pos.z = 0.0f;
											      
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].pos.x = g_aEnemy[nCntEnemy].posX * 0.001 + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].pos.y = g_aEnemy[nCntEnemy].posY * 0.001 + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].pos.z = 0.0f;
											      
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].pos.x = g_aEnemy[nCntEnemy].posX * 0.001 + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].pos.y = g_aEnemy[nCntEnemy].posY * 0.001 + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].pos.z = 0.0f;
		}
		if( g_aEnemy[nCntEnemy].bUse == false )
		{
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );					
		}
	}
	
	/*ポインタのアンロック*/
	g_pVtxBuffenemy->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawEnemy( void )
*		エネミーの描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawEnemy( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffenemy , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureenemy );
	
	for( int nCntEnemy = 0; nCntEnemy < ENEMY_MAX; nCntEnemy++ )
	{
		if( g_aEnemy[nCntEnemy].bUse == true )
		{
			/*エネミーのの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntEnemy * ENEMY_VERTEX_MAX , 2 );
		}
	}
}

/*******************************************************************************
* 関数名	:void SetEnemy( D3DXVECTOR3 pos , D3DXVECTOR3 vecMove , int nLife , ENEMY_TYPE type )
*		エネミーを生成する関数です。
* 引数		:D3DXVECTOR3 pos		エネミーの出現位置です。
*			 D3DXVECTOR3 vecMove	エネミーの移動量です。
*			 int nLife				エネミーの寿命です。
*			(ENEMY_TYPE type		エネミーの種類です。)	
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void SetEnemy( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , CATEGORY category , ENEMY_TYPE type , ENEMY_AI_TYPE aiType )
{
	int nCntEnemy = ENEMY_MAX;//browseEnemyCount;
	while( nCntEnemy-- )
	{
		if( g_aEnemy[nCntEnemy].bUse == false )
		{
			memset( &g_aEnemy[nCntEnemy] , 0 , sizeof( ENEMY ) );
			g_aEnemy[nCntEnemy].posX = posX;
			g_aEnemy[nCntEnemy].posY = posY;
			g_aEnemy[nCntEnemy].posZ = posZrot;
			g_aEnemy[nCntEnemy].moveX = moveX;
			g_aEnemy[nCntEnemy].moveY = moveY;
			g_aEnemy[nCntEnemy].moveZ = moveZrot;
			g_aEnemy[nCntEnemy].nLife = nLife;
			g_aEnemy[nCntEnemy].radius = 70000;
			g_aEnemy[nCntEnemy].bUse = true;
			g_aEnemy[nCntEnemy].category = category;
			g_aEnemy[nCntEnemy].type = type;
			g_aEnemy[nCntEnemy].aiType = aiType;
			browseEnemyCount = nCntEnemy;	
			createEnemyCount++;
			break;
		}
	}
}

/*******************************************************************************
* 関数名	:ENEMY* GetEnemy( void )
*		エネミーを渡す処理関数です。
* 引数		:int enemyNum	エネミーの要素
* 戻り値	:ENEMY*			エネミー構造体のアドレス
* 説明		:エネミーの半径を渡す関数です。
*******************************************************************************/
ENEMY* GetEnemy( int enemyNum )
{
	return &g_aEnemy[enemyNum];
}
