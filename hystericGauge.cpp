/*******************************************************************************
* タイトル	:ヒステリックゲージ.cpp
* ファイル名:hystericGauge.cpp
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
#include "hystericGauge.h"
#include "input.h"
#include "player01.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

#define HYSTERICGAUGE_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define OUTPUT_GAUGE_POSX	( SCREEN_RIGHT - 206 )
#define OUTPUT_GAUGE_POSY	( 340 )

#define HYSTERICGAUGE_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define HYSTERICGAUGE_ANIM_SPAN		( 0 )	/*アニメーション間隔*/
#define HYSTERICGAUGE_ANIM_DIV_X	( 5 )	/*幅分割数*/
#define HYSTERICGAUGE_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define HYSTERICGAUGE_OUTPUT_SIZE	( 1 ) /*初期表示倍率*/ 

#define USE_HYSTERIC_POINT		( 60 )		//使用するゲージのポイント数
#define HYSTERIC_POINT_SPACE	( 3 )		//使用するゲージのポイント間隔
#define USE_QUOTE				( 4 + USE_HYSTERIC_POINT - 1 )	//使用四角形数

typedef enum
{
	CHARACTER_NULL = -1,
	CHARACTER_NORMAL,
	CHARACTER_DAMAGE,
	CHARACTER_HYPERMODE,
	CHARACTER_MODEMAX
}CHARACTER_MODE;
/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffhystericGauge = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturehystericGauge;	/*ヒステリックゲージのテクスチャのポインタ*/
static D3DXIMAGE_INFO info;		/*画像情報*/

static D3DXVECTOR3 g_movePolygon;	/*ポリゴンの位置*/
static D3DXVECTOR3 g_posPolygon;	/*ポリゴンの移動量*/
static D3DXVECTOR3 g_rotPolygon;	/*ポリゴンの回転角*/
static float g_fLengthPolygon;		/*ポリゴンの対角線の長さ*/
static float g_fAnglePolygon;		/*ポリゴンの対角線の角度*/
static float polygonWidth , polygonHeight;	/*ポリゴンの幅／高さ（全体の半分）*/

static float polygonMag;	/*表示倍率*/

static int g_nCounterAnim = 0;	/*アニメーション間隔カウント*/
static int g_nPatternAnim = 0;	/*パターン数カウント*/

static int hystericPoint[USE_HYSTERIC_POINT];
static int outputSizeX,outputSizeY;
/*******************************************************************************
* 関数名	:HRESULT InitHystericGauge( void )
*		ヒステリックゲージの初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitHystericGauge( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * HYSTERICGAUGE_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffhystericGauge , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/hystericGauge.png" , &g_pTexturehystericGauge );
	D3DXGetImageInfoFromFile( "data/TEXTURE/hystericGauge.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffhystericGauge->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / HYSTERICGAUGE_ANIM_DIV_X ) * HYSTERICGAUGE_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / HYSTERICGAUGE_ANIM_DIV_Y ) * HYSTERICGAUGE_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*ポインタのアンロック*/
	g_pVtxBuffhystericGauge->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//ハイパーモード維持用エネルギー
	memset( &hystericPoint[0] , 0 , sizeof( int ) * USE_HYSTERIC_POINT );
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitHystericGauge( void )
*		ヒステリックゲージの終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitHystericGauge( void )
{
	if( g_pVtxBuffhystericGauge != NULL )
	{
		g_pVtxBuffhystericGauge->Release();
		g_pVtxBuffhystericGauge = NULL;
	}
	if( g_pTexturehystericGauge != NULL )
	{
		g_pTexturehystericGauge->Release();
		g_pTexturehystericGauge = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateHystericGauge( void )
*		ヒステリックゲージの更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateHystericGauge( void )
{
	int quoteCnt, hystericPointBuffer;
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	
	float hystericPer;
	int outputSizeHalf = outputSizeY * 0.5;
	int outputPointPos = 0, outputPointColor = 0;

	/*ポインタのロック*/ 
	g_pVtxBuffhystericGauge->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	int pointCnt = USE_HYSTERIC_POINT - 1;
	while( pointCnt-- )//for( int pointCnt = 0; pointCnt < USE_HYSTERIC_POINT - 1; pointCnt++ )
	{
		hystericPoint[pointCnt+1] = hystericPoint[pointCnt];
	}
	hystericPoint[0] = GetPlayerState01()->liarNum;
				
	texWidth = 1.0f / HYSTERICGAUGE_ANIM_DIV_X;
	texHeight = 1.0f / HYSTERICGAUGE_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % HYSTERICGAUGE_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / HYSTERICGAUGE_ANIM_DIV_X ) ) + 0;

	/*テクスチャ座標生成*/
	/*ゲージ基礎（右）*/
	pVtx[0].tex.x = 0.0f;
	pVtx[0].tex.y = 0.0f;
	pVtx[1].tex.x = 0.2f;
	pVtx[1].tex.y = 0.0f;
	pVtx[2].tex.x = 0.0f;
	pVtx[2].tex.y = 1.0f;
	pVtx[3].tex.x = 0.2f;
	pVtx[3].tex.y = 1.0f;

	/*現在値ライン*/
	pVtx[4].tex.x = 0.2f;
	pVtx[4].tex.y = 0.0f;
	pVtx[5].tex.x = 0.4f;
	pVtx[5].tex.y = 0.0f;
	pVtx[6].tex.x = 0.2f;
	pVtx[6].tex.y = 1.0f;
	pVtx[7].tex.x = 0.4f;
	pVtx[7].tex.y = 1.0f;
	
	for( quoteCnt = 2; quoteCnt < USE_QUOTE - 2; quoteCnt++ )
	{
		/*ポイント*/
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].tex.x = 0.4f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].tex.y = 0.0f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].tex.x = 0.6f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].tex.y = 0.0f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].tex.x = 0.4f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].tex.y = 1.0f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].tex.x = 0.6f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].tex.y = 1.0f;
	}
	
	/*ゲージ視準*/
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].tex.x = 0.6f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].tex.y = 0.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].tex.x = 0.8f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].tex.y = 0.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].tex.x = 0.6f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].tex.y = 1.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].tex.x = 0.8f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].tex.y = 1.0f;
	
	/*ゲージ枠*/
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 4].tex.x = 0.8f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 4].tex.y = 0.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 5].tex.x = 1.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 5].tex.y = 0.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 6].tex.x = 0.8f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 6].tex.y = 1.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 7].tex.x = 1.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 7].tex.y = 1.0f;
	
	for( quoteCnt = 0; quoteCnt < USE_QUOTE; quoteCnt++ )
	{
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		/*頂点座標設定*/
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.x = SCREEN_WIDTH - outputSizeX;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.y = OUTPUT_GAUGE_POSY;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.z = 0.0f;
		
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.x = SCREEN_WIDTH;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.y = OUTPUT_GAUGE_POSY;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.z = 0.0f;
		
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.x = SCREEN_WIDTH - outputSizeX;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.y = OUTPUT_GAUGE_POSY + outputSizeY;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.z = 0.0f;

		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.x = SCREEN_WIDTH;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.y = OUTPUT_GAUGE_POSY + outputSizeY;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.z = 0.0f;
		
		
		
		if( quoteCnt >= 1 && quoteCnt <= USE_QUOTE - 2 )
		{
			if( quoteCnt < USE_QUOTE - 2 )
			{
				hystericPer = ( float )( hystericPoint[quoteCnt - 1] ) / ( float )( LIAR_MAX );
				
				outputPointPos = outputSizeHalf * hystericPer;

				if( hystericPoint[quoteCnt - 1] > 0 )
				{
					outputPointColor = 255 - ( int )( 255 * hystericPer );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].col = D3DCOLOR_RGBA( 255 , outputPointColor , outputPointColor , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , outputPointColor , outputPointColor , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , outputPointColor , outputPointColor , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , outputPointColor , outputPointColor , 255 );
				}
				else
				{				
					outputPointColor = 255 + ( int )( 255 * hystericPer );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].col = D3DCOLOR_RGBA( 255 , outputPointColor , outputPointColor , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( outputPointColor , outputPointColor , 255 , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( outputPointColor , outputPointColor , 255 , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( outputPointColor , outputPointColor , 255 , 255 );
				}

				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.y		-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.y	-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.y	-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.y	-= outputPointPos;

				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.x		-= ( quoteCnt - 1 ) * HYSTERIC_POINT_SPACE;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.x	-= ( quoteCnt - 1 ) * HYSTERIC_POINT_SPACE;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.x	-= ( quoteCnt - 1 ) * HYSTERIC_POINT_SPACE;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.x	-= ( quoteCnt - 1 ) * HYSTERIC_POINT_SPACE;
			}
			else
			{
				hystericPer = ( float )( hystericPoint[0] ) / ( float )( LIAR_MAX );
				outputPointPos = outputSizeHalf * hystericPer;

				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.y		-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.y	-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.y	-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.y	-= outputPointPos;
			}
		}	
	}		

	/*ポインタのアンロック*/
	g_pVtxBuffhystericGauge->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawHystericGauge( void )
*		ヒステリックゲージの描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawHystericGauge( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffhystericGauge , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturehystericGauge );
	
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DESTをそのまま描画するよ
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	
	for( int quoteCnt = 0; quoteCnt < USE_QUOTE; quoteCnt++ )
	{
		/*ヒステリックゲージのの描画*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , quoteCnt * HYSTERICGAUGE_VERTEX_MAX , 2 );
	}

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*アルファブレンド有効化*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*スクリーン側ブレンド設定*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*ポリゴン側ブレンド設定*/
}