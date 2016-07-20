/*******************************************************************************
* タイトル	:弾.cpp
* ファイル名:bullet.cpp
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
#include "bullet.h"
#include "explosion.h"
#include "input.h"
#include "unoHitChecker.h"
#include "unoDirectX.h"
#include "barrier.h"
#include "player01.h"
#include "enemy.h"
#include "comboNumber.h"
#include "score.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define BULLET_DELETE_RADIUS	( 600000 )

#define BULLET_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define BULLET_ANIM_MAX		( 3 )	/*アニメーションコマ数*/
#define BULLET_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define BULLET_ANIM_DIV_X	( 3 )	/*幅分割数*/
#define BULLET_ANIM_DIV_Y	( 5 )	/*高さ分割数*/

#define BULLET_OUTPUT_SIZE	( 0.65 ) /*初期表示倍率*/ 

#define MOVE_NUM ( 2.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.20f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.05f )	/*減衰率*/


/*******************************************************************************
* 構造体定義
*******************************************************************************/
//弾のステータス
typedef struct
{
	//処理軽減の為にint型として扱う。下三桁を小数点以下として扱う。
	int posX;	//位置
	int posY;	//位置
	int posZ;	//位置
	int moveX;	//移動量
	int moveY;	//移動量
	int moveZ;	//移動量
	int vector;	//角度
	int nowMode;	//現在モード
	//32byte
	int attack;	//攻撃力
	int radius;	//辺り半径
	int nCurveCnt;		//カーブ・バウンド数
	int nLife;			//寿命
	bool bCurve;		//カーブしたかどうか
	bool bUse;			//使用しているかどうか
	BULLET_TYPE type;	//弾の種類
	CATEGORY category;	//所属勢力
}BULLET;

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffbullet = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturebullet;	/*弾のテクスチャのポインタ*/
static D3DXIMAGE_INFO info;		/*画像情報*/

static BULLET g_aBullet[BULLET_MAX];	/*弾の構造体*/
static bool frameSwitch = false;	/*交互に切り替わり、フレームが変わったかを識別する。*/
static int createBulletCount = 0;	/*弾数カウント*/

static D3DXVECTOR3 g_movePolygon;	/*ポリゴンの位置*/
static D3DXVECTOR3 g_posPolygon;	/*ポリゴンの移動量*/
static D3DXVECTOR3 g_rotPolygon;	/*ポリゴンの回転角*/
static float g_fLengthPolygon;		/*ポリゴンの対角線の長さ*/
static float g_fAnglePolygon;		/*ポリゴンの対角線の角度*/
static float polygonWidth , polygonHeight;	/*ポリゴンの幅／高さ（全体の半分）*/

static float polygonMag;	/*表示倍率*/

static int g_nCounterAnim = 0;	/*アニメーション間隔カウント*/
static int g_nPatternAnim = 0;	/*パターン数カウント*/

static int lastBullet = 0;	/*弾が存在した最後の部分*/

/*******************************************************************************
* 関数名	:HRESULT InitBullet( void )
*		弾の初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitBullet( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * BULLET_VERTEX_MAX * BULLET_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffbullet , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/bullet.png" , &g_pTexturebullet );
	D3DXGetImageInfoFromFile( "data/TEXTURE/bullet.png" , &info );

	ZeroMemory( &g_aBullet[0] , sizeof( BULLET ) * BULLET_MAX );

	/*ポインタのロック*/
	g_pVtxBuffbullet->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / BULLET_ANIM_DIV_X ) * BULLET_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / BULLET_ANIM_DIV_Y ) * BULLET_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , BULLET_MAX );

	/*ポインタのアンロック*/
	g_pVtxBuffbullet->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	/*弾更新用ステータスの初期化*/
	frameSwitch = false;
	createBulletCount = 0;
	
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitBullet( void )
*		弾の終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitBullet( void )
{
	if( g_pVtxBuffbullet != NULL )
	{
		g_pVtxBuffbullet->Release();
		g_pVtxBuffbullet = NULL;
	}
	if( g_pTexturebullet != NULL )
	{
		g_pTexturebullet->Release();
		g_pTexturebullet = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateBullet( void )
*		弾の更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateBullet( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	
	/*ポインタのロック*/ 
	g_pVtxBuffbullet->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*弾の更新*/
	for( int nCntBullet = 0; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == true )
		{
#ifdef _DEBUG
			/*おあそび*/
			if( GetKeyboardPress( DIK_UP ) )
			{
				g_aBullet[nCntBullet].moveY-=100;
			}
			if( GetKeyboardPress( DIK_DOWN ) )
			{
				g_aBullet[nCntBullet].moveY+=100;
			}
			if( GetKeyboardPress( DIK_LEFT ) )
			{
				g_aBullet[nCntBullet].moveX-=100;
			}
			if( GetKeyboardPress( DIK_RIGHT ) )
			{
				g_aBullet[nCntBullet].moveX+=100;
			}
#endif		
		
				
			if( g_aBullet[nCntBullet].type == BULLET_NORMAL )
			{
				//寿命チェック&通常弾消滅判定(画面外)
				if( g_aBullet[nCntBullet].nLife <= 0 ||
					HitTwinCircle( INT_FIELD_CENTER_X , INT_FIELD_CENTER_Y , BULLET_DELETE_RADIUS , 
					g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , g_aBullet[nCntBullet].radius ) != true  )
				{
					
					/*//通常弾消滅判定(画面外)
					if( g_aBullet[nCntBullet].bUse == true &&	//処理軽減（前処理で弾が消えていた場合処理数が増える為）
						( g_aBullet[nCntBullet].posX  < INT_FIELD_LEFT || g_aBullet[nCntBullet].posX > INT_FIELD_RIGHT ||
						g_aBullet[nCntBullet].posY < INT_FIELD_TOP || g_aBullet[nCntBullet].posY > INT_FIELD_BOTTOM  ) )
					{
						g_aBullet[nCntBullet].bUse = false;
						continue;
					}
					*/
					g_aBullet[nCntBullet].bUse = false;
					createBulletCount--;
				}

				/*//お遊び反転弾
				if( ( g_aBullet[nCntBullet].posX  < INT_FIELD_LEFT || g_aBullet[nCntBullet].posX > INT_FIELD_RIGHT ) )
				{
					g_aBullet[nCntBullet].moveX = -g_aBullet[nCntBullet].moveX;
				}
				if( ( g_aBullet[nCntBullet].posY < INT_FIELD_TOP || g_aBullet[nCntBullet].posY > INT_FIELD_BOTTOM ) )
				{
					g_aBullet[nCntBullet].moveY = -g_aBullet[nCntBullet].moveY;
				}*/
			}
			if( g_aBullet[nCntBullet].type == BULLET_HOMING )
			{
				//寿命チェック
				if( g_aBullet[nCntBullet].nLife <= 0 )
				{
					g_aBullet[nCntBullet].bUse = false;
				}
				//通常弾消滅判定(画面外)
				if( g_aBullet[nCntBullet].bUse == true &&	//処理軽減（前処理で弾が消えていた場合処理数が増える為）
					( g_aBullet[nCntBullet].posX < INT_SCREEN_LEFT || g_aBullet[nCntBullet].posX > INT_SCREEN_RIGHT ||
					g_aBullet[nCntBullet].posY < INT_SCREEN_TOP || g_aBullet[nCntBullet].posY > INT_SCREEN_BOTTOM ) )
				{
					g_aBullet[nCntBullet].bUse = false;
				}
			}
			g_aBullet[nCntBullet].posX += g_aBullet[nCntBullet].moveX;
			g_aBullet[nCntBullet].posY += g_aBullet[nCntBullet].moveY;
			
			//当たり判定
			if( ( g_aBullet[nCntBullet].posX < INT_FIELD_LEFT || g_aBullet[nCntBullet].posX > INT_FIELD_RIGHT ||
				g_aBullet[nCntBullet].posY < INT_FIELD_TOP || g_aBullet[nCntBullet].posY > INT_FIELD_BOTTOM ) )//画面外にある場合判定をスルー
			{

			}
			else if( ( g_aBullet[nCntBullet].category == CATEGORY_ENEMY || g_aBullet[nCntBullet].category == CATEGORY_LIAR ) )
			{
				if(	GetPlayerState01()->mode !=	PLAYER_MODE_HYPERMODE &&
					HitTwinCircle( GetPlayerState01()->intPosX , GetPlayerState01()->intPosY , PLAYER_RADIUS , 
					g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , g_aBullet[nCntBullet].radius ) == true )
				{
					GetPlayerState01()->mode = PLAYER_MODE_DAMAGE;
				}

				if( HitTwinCircle( GetPlayerState01()->intPosX , GetPlayerState01()->intPosY , GetBarrierRadiusInt() , 
					g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , g_aBullet[nCntBullet].radius ) == true )
				{
					//SetExplosion( g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , 0 , 0 , 0 , 0 , 12 , EXPLOSION_001 );
					if(	g_aBullet[nCntBullet].category == CATEGORY_LIAR )
					{					
						GetPlayerState01()->liarNum = ( GetPlayerState01()->liarNum < LIAR_MAX ? GetPlayerState01()->liarNum + 3 : GetPlayerState01()->liarNum );
					}
					SetExplosion( g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , 0 , 0 , 0 , 0 , 12 , EXPLOSION_002 );
					
					g_aBullet[nCntBullet].bUse = false;
				}
			}
			else if( g_aBullet[nCntBullet].category == CATEGORY_PLAYER )
			{
				for( int enemyCnt = 0; enemyCnt < ENEMY_MAX; enemyCnt++ )
				{
					if( GetEnemy( enemyCnt )->bUse == true && GetEnemy( enemyCnt )->state != ENEMY_ST_EXPLOSION &&
						HitTwinCircle( GetEnemy( enemyCnt )->posX , GetEnemy( enemyCnt )->posY , GetEnemy( enemyCnt )->radius , 
						g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , g_aBullet[nCntBullet].radius ) == true )
					{	
						g_aBullet[nCntBullet].bUse = false;
						SetExplosion( g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , 0 , 0 , 0 , 0 , 12 , EXPLOSION_001 );							
						GetEnemy( enemyCnt )->nLife -= g_aBullet[nCntBullet].attack;
						PlusComboNumber( 1 );
						if( GetEnemy( enemyCnt )->nLife <= 0 )
						{
							PlusScore( GetEnemy( enemyCnt )->radius * ( 1 + ( 0.03 * GetComboNumber() ) ) );
						}
						GetEnemy( enemyCnt )->state = ENEMY_ST_DAMAGE;
						//GetPlayerState01()->nCombo = 1;
						
						break;
					}
				}
			}


			/*パターンアニメーション事前計算部分*/
			if( ++g_nCounterAnim %= BULLET_ANIM_SPAN )
			{
				++g_nPatternAnim %= BULLET_ANIM_MAX;
				//g_nPatternAnim = ( ++g_nPatternAnim == BULLET_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
			
			texWidth = 1.0f / BULLET_ANIM_DIV_X;
			texHeight = 1.0f / BULLET_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % BULLET_ANIM_DIV_X );
			
			if( g_aBullet[nCntBullet].type == BULLET_NORMAL )
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) ) + 0;
			}
			else if( g_aBullet[nCntBullet].type == BULLET_RAPID )
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) ) + 1;
			}
			else if( g_aBullet[nCntBullet].type == BULLET_HOMING )
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) ) + 2;
			}
			else if( g_aBullet[nCntBullet].type == BULLET_PENETRATE )
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) ) + 3;
			}
			else
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) ) + 2;
			}
			
			switch( g_aBullet[nCntBullet].category )
			{
				case CATEGORY_PLAYER:
					{
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
						/*
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 255 , 122 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 255 , 122 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 255 , 122 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 255 , 122 , 255 );
						*/
						break;
					}
				case CATEGORY_ENEMY:
					{
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 54 , 182 , 250 , 200 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 54 , 182 , 250 , 200 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 54 , 182 , 250 , 200 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 54 , 182 , 250 , 200 );
						break;
					}
				case CATEGORY_NEUTRAL:
					{
						
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 255 , 0 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 255 , 0 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 255 , 0 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 255 , 0 , 122 );
						break;
					}
				case CATEGORY_ANTIBREAK:
					{
						break;
					}
				case CATEGORY_LIAR:
					{
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 0 , 0 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 0 , 0 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 0 , 0 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 0 , 0 , 255 );
						break;
					}
				default:
					{
						break;
					}
			}

			/*テクスチャ座標生成*/
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

			/*頂点座標設定*/
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].pos.x = g_aBullet[nCntBullet].posX * 0.001 + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].pos.y = g_aBullet[nCntBullet].posY * 0.001 + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].pos.z = 0.0f;
											      
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].pos.x = g_aBullet[nCntBullet].posX * 0.001 + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].pos.y = g_aBullet[nCntBullet].posY * 0.001 + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].pos.z = 0.0f;
											      
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].pos.x = g_aBullet[nCntBullet].posX * 0.001 + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].pos.y = g_aBullet[nCntBullet].posY * 0.001 + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].pos.z = 0.0f;
											      
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].pos.x = g_aBullet[nCntBullet].posX * 0.001 + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].pos.y = g_aBullet[nCntBullet].posY * 0.001 + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].pos.z = 0.0f;

		}
		if( g_aBullet[nCntBullet].bUse == false )
		{
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
		}
	}
	
	/*ポインタのアンロック*/
	g_pVtxBuffbullet->Unlock();


	//if( frameSwitch  )
	//bool frameSwitch;	/*交互に切り替わり、フレームが変わったかを識別する。*/
	createBulletCount = 0;
}

/*******************************************************************************
* 関数名	:void DrawBullet( void )
*		弾の描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawBullet( void )
{
	int lastBnemy = 0;
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffbullet , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturebullet );
	
	for( int nCntBullet = 0; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == true )
		{
			/*弾のの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntBullet * BULLET_VERTEX_MAX , 2 );
			lastBullet = nCntBullet;
		}
	}

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DESTをそのまま描画するよ
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	
	for( int nCntBullet = 0; nCntBullet <= lastBullet; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == true )
		{
			/*弾のの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntBullet * BULLET_VERTEX_MAX , 2 );
		}
	}
	
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*アルファブレンド有効化*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*スクリーン側ブレンド設定*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*ポリゴン側ブレンド設定*/
	
}

/*******************************************************************************
* 関数名	:void SetBullet( D3DXVECTOR3 pos , D3DXVECTOR3 vecMove , int nLife , ENEMY_TYPE type )
*		弾を生成する関数です。
* 引数		:D3DXVECTOR3 pos		弾の出現位置です。
*			 D3DXVECTOR3 vecMove	弾の移動量です。
*			 int nLife				弾の寿命です。
*			(ENEMY_TYPE type		弾の種類です。)	
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void SetBullet( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , CATEGORY category , BULLET_TYPE type )
{
	
	for( int nCntBullet = createBulletCount; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == false )
		{
			memset( &g_aBullet[nCntBullet] , 0 , sizeof( BULLET ) );
			g_aBullet[nCntBullet].posX = posX;
			g_aBullet[nCntBullet].posY = posY;
			g_aBullet[nCntBullet].posZ = posZrot;
			g_aBullet[nCntBullet].moveX = moveX;
			g_aBullet[nCntBullet].moveY = moveY;
			g_aBullet[nCntBullet].moveZ = moveZrot;
			g_aBullet[nCntBullet].nLife = nLife;
			g_aBullet[nCntBullet].type = type;
			g_aBullet[nCntBullet].attack = 1;
			g_aBullet[nCntBullet].radius = 100;
			g_aBullet[nCntBullet].bUse = true;
			g_aBullet[nCntBullet].category = category;
			createBulletCount = nCntBullet;	
			break;
		}
	}
}
/*プレイヤー指定版*/
void SetBulletPlayer( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , BULLET_TYPE type )
{
	for( int nCntBullet = createBulletCount; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == false )
		{
			memset( &g_aBullet[nCntBullet] , 0 , sizeof( BULLET ) );
			g_aBullet[nCntBullet].posX = posX;
			g_aBullet[nCntBullet].posY = posY;
			g_aBullet[nCntBullet].posZ = posZrot;
			g_aBullet[nCntBullet].moveX = moveX;
			g_aBullet[nCntBullet].moveY = moveY;
			g_aBullet[nCntBullet].moveZ = moveZrot;
			g_aBullet[nCntBullet].nLife = nLife;
			g_aBullet[nCntBullet].attack = 1;
			g_aBullet[nCntBullet].radius = 100;
			g_aBullet[nCntBullet].type = type;
			g_aBullet[nCntBullet].bUse = true;
			g_aBullet[nCntBullet].category = CATEGORY_PLAYER;
			createBulletCount = nCntBullet;	
			break;
		}
	}
}
/*エネミー指定版*/
void SetBulletEnemy( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , BULLET_TYPE type )
{
	for( int nCntBullet = createBulletCount; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == false )
		{
			memset( &g_aBullet[nCntBullet] , 0 , sizeof( BULLET ) );
			g_aBullet[nCntBullet].posX = posX;
			g_aBullet[nCntBullet].posY = posY;
			g_aBullet[nCntBullet].posZ = posZrot;
			g_aBullet[nCntBullet].moveX = moveX;
			g_aBullet[nCntBullet].moveY = moveY;
			g_aBullet[nCntBullet].moveZ = moveZrot;
			g_aBullet[nCntBullet].nLife = nLife;
			g_aBullet[nCntBullet].attack = 1;
			g_aBullet[nCntBullet].radius = 1;
			g_aBullet[nCntBullet].type = type;
			g_aBullet[nCntBullet].bUse = true;
			g_aBullet[nCntBullet].category = CATEGORY_ENEMY;
			createBulletCount = nCntBullet;	
			break;
		}
	}
}
/*嘘属性指定版*/
void SetBulletLiar( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , BULLET_TYPE type )
{
	for( int nCntBullet = createBulletCount; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == false )
		{
			memset( &g_aBullet[nCntBullet] , 0 , sizeof( BULLET ) );
			g_aBullet[nCntBullet].posX = posX;
			g_aBullet[nCntBullet].posY = posY;
			g_aBullet[nCntBullet].posZ = posZrot;
			g_aBullet[nCntBullet].moveX = moveX;
			g_aBullet[nCntBullet].moveY = moveY;
			g_aBullet[nCntBullet].moveZ = moveZrot;
			g_aBullet[nCntBullet].nLife = nLife;
			g_aBullet[nCntBullet].attack = 1;
			g_aBullet[nCntBullet].radius = 100;
			g_aBullet[nCntBullet].type = type;
			g_aBullet[nCntBullet].bUse = true;
			g_aBullet[nCntBullet].category = CATEGORY_LIAR;
			createBulletCount = nCntBullet;	
			break;
		}
	}
}


/*******************************************************************************
* 関数名	:void DeleteBullet( int posX , int posY , int radius )
*		弾消しを発生させる関数です。
* 引数		:int posX		発生地点X座標	
*			 int posY		発生地点Y座標
*			 int radius		半径
* 戻り値	:void 戻り値なし
* 説明		:指定地点を中心とした円の中にある敵弾を消滅させる関数です。
*******************************************************************************/
void DeleteBullet( int posX , int posY , int radius )
{
	for( int nCntBullet = createBulletCount; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == true )
		{
			
			if( HitTwinCircle( posX , posY , radius , g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , g_aBullet[nCntBullet].radius ) == true )
			{	
				g_aBullet[nCntBullet].bUse = false;
				SetExplosion( g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , 0 , 0 , 0 , 0 , 12 , EXPLOSION_001 );							
			}
		}
	}
}