/*******************************************************************************
* タイトル	:タイトルロゴ.cpp
* ファイル名:titleLogo.cpp
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
#include "titleLogo.h"
#include "input.h"
#include "player01.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

#define TITLELOGO_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define OUTPUT_LOGO_POSX	( 400 )
#define OUTPUT_LOGO_POSY	( 0 )

#define LOGO_WAVE	( 40 )	//ロゴの揺れる範囲
#define COLOR_WAVE	( 250 )	//色の揺れる範囲

#define WAVELOGO_NUM			( 600 )

#define TITLELOGO_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define TITLELOGO_ANIM_SPAN		( 10 )	/*アニメーション間隔*/
#define TITLELOGO_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define TITLELOGO_ANIM_DIV_Y	( WAVELOGO_NUM )	/*高さ分割数*/

#define TITLELOGO_OUTPUT_SIZE	( 1 ) /*初期表示倍率*/ 

#define USE_QUOTE				( WAVELOGO_NUM + 1 )	//使用四角形数

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBufftitleLogo = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturetitleLogo;			/*表示画像のテクスチャのポインタ*/
static D3DXIMAGE_INFO info;				/*表示画像情報*/

static D3DXVECTOR3 g_movePolygon;	/*ポリゴンの位置*/
static D3DXVECTOR3 g_posPolygon;	/*ポリゴンの移動量*/
static D3DXVECTOR3 g_rotPolygon;	/*ポリゴンの回転角*/
static float g_fLengthPolygon;		/*ポリゴンの対角線の長さ*/
static float g_fAnglePolygon;		/*ポリゴンの対角線の角度*/
static float polygonWidth , polygonHeight;	/*ポリゴンの幅／高さ（全体の半分）*/

static float polygonMag;	/*表示倍率*/

static int g_nCounterAnim = 0;	/*アニメーション間隔カウント*/
static int g_nPatternAnim = 0;	/*パターン数カウント*/

static int outputSizeX,outputSizeY;
static float outputTexWidth;
/*******************************************************************************
* 関数名	:HRESULT InitTitleLogo( void )
*		タイトルロゴの初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitTitleLogo( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * TITLELOGO_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBufftitleLogo , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/titleLogo.png" , &g_pTexturetitleLogo );
	D3DXGetImageInfoFromFile( "data/TEXTURE/titleLogo.png" , &info );
	
	/*ポインタのロック*/
	g_pVtxBufftitleLogo->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / TITLELOGO_ANIM_DIV_X ) * TITLELOGO_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / TITLELOGO_ANIM_DIV_Y ) * TITLELOGO_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*ポインタのアンロック*/
	g_pVtxBufftitleLogo->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	outputTexWidth = 1.0f / ( float )( USE_QUOTE );

	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitTitleLogo( void )
*		タイトルロゴの終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitTitleLogo( void )
{
	if( g_pVtxBufftitleLogo != NULL )
	{
		g_pVtxBufftitleLogo->Release();
		g_pVtxBufftitleLogo = NULL;
	}
	if( g_pTexturetitleLogo != NULL )
	{
		g_pTexturetitleLogo->Release();
		g_pTexturetitleLogo = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateTitleLogo( void )
*		タイトルロゴの更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateTitleLogo( void )
{
	int quoteCnt, hystericPointBuffer;
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	
	float hystericPer;
	int outputSizeHalf = outputSizeY * 0.5;
	int outputPointPos = 0, outputPointColor = 0;

	/*ポインタのロック*/ 
	g_pVtxBufftitleLogo->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / TITLELOGO_ANIM_DIV_X ) * TITLELOGO_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / TITLELOGO_ANIM_DIV_Y ) * TITLELOGO_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );

	texWidth = 1.0f / TITLELOGO_ANIM_DIV_X;
	texHeight = 1.0f / TITLELOGO_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % TITLELOGO_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / TITLELOGO_ANIM_DIV_X ) ) + 0;

	for( quoteCnt = 0; quoteCnt < WAVELOGO_NUM; quoteCnt++ )
	{
		/*テクスチャ座標生成*/
		/*PleasePushAnykey*/
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX].tex.x = 0.0f;
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX].tex.y = outputTexWidth * quoteCnt;
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 1].tex.x = 1.0f;
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 1].tex.y = outputTexWidth * quoteCnt + outputTexWidth;
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 2].tex.x = 0.0f;
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 2].tex.y = outputTexWidth * quoteCnt;
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 3].tex.x = 1.0f;
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 3].tex.y = outputTexWidth * quoteCnt + outputTexWidth;
		
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 255 , 155 );
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 255 , 155 );
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 255 , 155 );
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 255 , 155 );

		/*頂点座標設定*/
		if( quoteCnt == 0 )
		{
			g_posPolygon.x = OUTPUT_LOGO_POSX;
			g_posPolygon.y = OUTPUT_LOGO_POSY;
		}
		else
		{
			g_posPolygon.x = OUTPUT_LOGO_POSX + rand() % LOGO_WAVE - rand() % LOGO_WAVE;
			g_posPolygon.y += outputSizeY;
		}
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
		
		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	}

	
	outputSizeX = ( int )( ( info.Width / TITLELOGO_ANIM_DIV_X ) * TITLELOGO_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / 1 ) * TITLELOGO_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );

	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX].tex.x = 0.0f;
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX].tex.y = 0.0f;
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 1].tex.x = 1.0f;
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 1].tex.y = 0.0f;
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 2].tex.x = 0.0f;
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 2].tex.y = 1.0f;
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 3].tex.x = 1.0f;
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 3].tex.y = 1.0f;
	
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX].col	  = D3DCOLOR_RGBA( 122 , 122 , 255 , 225 );
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 122 , 122 , 255 , 225 );
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 122 , 122 , 255 , 225 );
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 122 , 122 , 255 , 225 );
	
	/*頂点座標設定*/
	g_posPolygon.x = OUTPUT_LOGO_POSX;
	g_posPolygon.y = 300;
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
							g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * TITLELOGO_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*ポインタのアンロック*/
	g_pVtxBufftitleLogo->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawTitleLogo( void )
*		タイトルロゴの描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawTitleLogo( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBufftitleLogo , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturetitleLogo );
	
	for( int quoteCnt = 0; quoteCnt < USE_QUOTE; quoteCnt++ )
	{
		if( quoteCnt >= WAVELOGO_NUM )
		{
			GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
			GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DESTをそのまま描画するよ
			GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		}
	
		/*タイトルロゴの描画*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , quoteCnt * TITLELOGO_VERTEX_MAX , 2 );
	}
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*アルファブレンド有効化*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*スクリーン側ブレンド設定*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*ポリゴン側ブレンド設定*/
	//pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}