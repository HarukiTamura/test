/*******************************************************************************
* タイトル	:爆発.cpp
* ファイル名:AfAfExplosion.cpp
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
#include "AfExplosion.h"
#include "input.h"
#include "unoHitChecker.h"
#include "unoDirectX.h"
#include "barrier.h"
#include "player01.h"
#include "enemy.h"
#include "AfEnemy.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

#define AF_EXPLOSION_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define AF_EXPLOSION_ANIM_MAX		( 4 )	/*アニメーションコマ数*/
#define AF_EXPLOSION_ANIM_SPAN	( 20 )	/*アニメーション間隔*/
#define AF_EXPLOSION_ANIM_DIV_X	( 4 )	/*幅分割数*/
#define AF_EXPLOSION_ANIM_DIV_Y	( 3 )	/*高さ分割数*/

#define AF_EXPLOSION_OUTPUT_SIZE	( 0.2 ) /*初期表示倍率*/ 

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
//爆発のステータス
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
	int type;
	int attack;	//攻撃力
	int radius;	//辺り半径
	int nLife;			//寿命
	bool bCurve;		//カーブしたかどうか
	bool bUse;			//使用しているかどうか
}AF_EXPLOSION;

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfAfExplosion = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureAfAfExplosion;	/*爆発のテクスチャのポインタ*/
static D3DXIMAGE_INFO info;		/*画像情報*/

static AF_EXPLOSION g_aAfExplosion[AF_EXPLOSION_MAX];	/*爆発の構造体*/
static bool frameSwitch = false;	/*交互に切り替わり、フレームが変わったかを識別する。*/

static int createAfExplosionCount = 0;	/*爆発数カウント*/

static D3DXVECTOR3 g_movePolygon;	/*ポリゴンの位置*/
static D3DXVECTOR3 g_posPolygon;	/*ポリゴンの移動量*/
static D3DXVECTOR3 g_rotPolygon;	/*ポリゴンの回転角*/
static float g_fLengthPolygon;		/*ポリゴンの対角線の長さ*/
static float g_fAnglePolygon;		/*ポリゴンの対角線の角度*/
static float polygonWidth , polygonHeight;	/*ポリゴンの幅／高さ（全体の半分）*/

static float polygonMag;	/*表示倍率*/

static int g_nCounterAnim = 0;	/*アニメーション間隔カウント*/
static int g_nPatternAnim = 0;	/*パターン数カウント*/

static int lastAfExplosion = 0;	/*爆発が存在した最後の部分*/

static bool g_soundOn[AF_EXPLOSION_TYPE_MAX];
/*******************************************************************************
* 関数名	:HRESULT InitAfExplosion( void )
*		爆発の初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfExplosion( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_EXPLOSION_VERTEX_MAX * AF_EXPLOSION_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfAfExplosion , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfExplosion.png" , &g_pTextureAfAfExplosion );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfExplosion.png" , &info );

	ZeroMemory( &g_aAfExplosion[0] , sizeof( AF_EXPLOSION ) * AF_EXPLOSION_MAX );

	/*ポインタのロック*/
	g_pVtxBuffAfAfExplosion->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / AF_EXPLOSION_ANIM_DIV_X ) * AF_EXPLOSION_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_EXPLOSION_ANIM_DIV_Y ) * AF_EXPLOSION_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , AF_EXPLOSION_MAX );

	/*ポインタのアンロック*/
	g_pVtxBuffAfAfExplosion->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	/*爆発更新用ステータスの初期化*/
	frameSwitch = false;
	
	createAfExplosionCount = 0;
	
	for( int cnt = 0; cnt < AF_EXPLOSION_TYPE_MAX; cnt++ )
	{
		g_soundOn[cnt] = false;
	}

	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitAfExplosion( void )
*		爆発の終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfExplosion( void )
{
	if( g_pVtxBuffAfAfExplosion != NULL )
	{
		g_pVtxBuffAfAfExplosion->Release();
		g_pVtxBuffAfAfExplosion = NULL;
	}
	if( g_pTextureAfAfExplosion != NULL )
	{
		g_pTextureAfAfExplosion->Release();
		g_pTextureAfAfExplosion = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfExplosion( void )
*		爆発の更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfExplosion( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	
	for( int cnt = 0; cnt < AF_EXPLOSION_TYPE_MAX; cnt++ )
	{
		g_soundOn[cnt] = false;
	}

	/*ポインタのロック*/ 
	g_pVtxBuffAfAfExplosion->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*爆発の更新*/
	for( int nCntAfExplosion = 0; nCntAfExplosion < AF_EXPLOSION_MAX; nCntAfExplosion++ )
	{
		if( g_aAfExplosion[nCntAfExplosion].bUse == true )
		{
				
			g_aAfExplosion[nCntAfExplosion].nLife--;
			//寿命チェック
			if( g_aAfExplosion[nCntAfExplosion].nLife <= 0 )
			{
				g_aAfExplosion[nCntAfExplosion].bUse = false;
				continue;
			}
			
			g_aAfExplosion[nCntAfExplosion].posX += g_aAfExplosion[nCntAfExplosion].moveX;
			g_aAfExplosion[nCntAfExplosion].posY += g_aAfExplosion[nCntAfExplosion].moveY;


			/*パターンアニメーション事前計算部分*/
			if( ++g_nCounterAnim %= AF_EXPLOSION_ANIM_SPAN )
			{
				++g_nPatternAnim %= AF_EXPLOSION_ANIM_MAX;
				//g_nPatternAnim = ( ++g_nPatternAnim == AF_EXPLOSION_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
			texWidth = 1.0f / AF_EXPLOSION_ANIM_DIV_X;
			texHeight = 1.0f / AF_EXPLOSION_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % AF_EXPLOSION_ANIM_DIV_X );
			

			nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_EXPLOSION_ANIM_DIV_X ) ) + g_aAfExplosion[nCntAfExplosion].type;

			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			
			
			/*テクスチャ座標生成*/
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;
			
			g_aAfExplosion[nCntAfExplosion].posZ += g_aAfExplosion[nCntAfExplosion].moveZ;
			if( g_aAfExplosion[nCntAfExplosion].type == AF_EXPLOSION_002 )
			{
				float bonbSize = ( float )( g_aAfExplosion[nCntAfExplosion].posZ ) * 0.015;
				if( bonbSize <= 0 )
				{
					bonbSize = 1.0f;

					g_rotPolygon.z =  g_rotPolygon.z + ( rand() % 100 - rand() % 100 ) * 0.01;  
				}
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				polygonMag = ( float ) bonbSize;
				polygonMag *= 1.4;
			
			}
			else if( g_aAfExplosion[nCntAfExplosion].type == AF_EXPLOSION_003 )
			{
				
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
			}
			else
			{
				polygonMag *= 1.4;
			}
			polygonMag *= 0.76;


			/*頂点座標設定*/
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].pos.x = g_aAfExplosion[nCntAfExplosion].posX * 0.001 + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].pos.y = g_aAfExplosion[nCntAfExplosion].posY * 0.001 + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].pos.z = 0.0f;
											      
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].pos.x = g_aAfExplosion[nCntAfExplosion].posX * 0.001 + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].pos.y = g_aAfExplosion[nCntAfExplosion].posY * 0.001 + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].pos.z = 0.0f;
											      
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].pos.x = g_aAfExplosion[nCntAfExplosion].posX * 0.001 + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].pos.y = g_aAfExplosion[nCntAfExplosion].posY * 0.001 + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].pos.z = 0.0f;
											      
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].pos.x = g_aAfExplosion[nCntAfExplosion].posX * 0.001 + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].pos.y = g_aAfExplosion[nCntAfExplosion].posY * 0.001 + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].pos.z = 0.0f;

		}
	}
	
	/*ポインタのアンロック*/
	g_pVtxBuffAfAfExplosion->Unlock();
	
	createAfExplosionCount = 0;

	//if( frameSwitch  )
	//bool frameSwitch;	/*交互に切り替わり、フレームが変わったかを識別する。*/
}

/*******************************************************************************
* 関数名	:void DrawAfExplosion( void )
*		爆発の描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfExplosion( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	
	

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfAfExplosion , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfAfExplosion );
	
	for( int nCntAfExplosion = 0; nCntAfExplosion < AF_EXPLOSION_MAX; nCntAfExplosion++ )
	{
		if( g_aAfExplosion[nCntAfExplosion].bUse == true )
		{
			/*爆発のの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX , 2 );
		}
	}

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DESTをそのまま描画するよ
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	for( int nCntAfExplosion = 0; nCntAfExplosion < AF_EXPLOSION_MAX; nCntAfExplosion++ )
	{
		if( g_aAfExplosion[nCntAfExplosion].bUse == true )
		{
			/*爆発のの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX , 2 );
		}
	}
	
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*アルファブレンド有効化*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*スクリーン側ブレンド設定*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*ポリゴン側ブレンド設定*/
	
}

/*******************************************************************************
* 関数名	:void SetAfExplosion( D3DXVECTOR3 pos , D3DXVECTOR3 vecMove , int nLife , ENEMY_TYPE type )
*		爆発を生成する関数です。
* 引数		:D3DXVECTOR3 pos		爆発の出現位置です。
*			 D3DXVECTOR3 vecMove	爆発の移動量です。
*			 int nLife				爆発の寿命です。
*			(ENEMY_TYPE type		爆発の種類です。)	
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void SetAfExplosion( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , AF_EXPLOSION_TYPE type = AF_EXPLOSION_001 )
{
	for( int nCntAfExplosion = createAfExplosionCount; nCntAfExplosion < AF_EXPLOSION_MAX; nCntAfExplosion++ )
	{
		if( g_aAfExplosion[nCntAfExplosion].bUse == false )
		{
			memset( &g_aAfExplosion[nCntAfExplosion] , 0 , sizeof( AF_EXPLOSION ) );
			g_aAfExplosion[nCntAfExplosion].posX = posX;
			g_aAfExplosion[nCntAfExplosion].posY = posY;
			g_aAfExplosion[nCntAfExplosion].posZ = posZrot;
			g_aAfExplosion[nCntAfExplosion].moveX = moveX;
			g_aAfExplosion[nCntAfExplosion].moveY = moveY;
			g_aAfExplosion[nCntAfExplosion].moveZ = moveZrot;
			g_aAfExplosion[nCntAfExplosion].nLife = nLife;
			g_aAfExplosion[nCntAfExplosion].type = type;
			switch( type )
			{
				case AF_EXPLOSION_001:
					if( g_soundOn[AF_EXPLOSION_001] == false )
					{
						PlaySound(SOUND_LABEL_SE_MISSILESHOOT);
						g_soundOn[AF_EXPLOSION_001] = true;
					}
					break;
				case AF_EXPLOSION_002:
					if( g_soundOn[AF_EXPLOSION_002] == false )
					{
						//落ちるので使用しない
						//PlaySound(SOUND_LABEL_SE_MISSILEFLY);
						g_soundOn[AF_EXPLOSION_002] = true;
					}
					//
					break;
				default:
					break;
			}
			g_aAfExplosion[nCntAfExplosion].attack = 1;
			g_aAfExplosion[nCntAfExplosion].radius = 100;
			g_aAfExplosion[nCntAfExplosion].bUse = true;
			createAfExplosionCount = nCntAfExplosion;	
			break;
		}
	}
}