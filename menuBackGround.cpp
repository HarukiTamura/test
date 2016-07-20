/*******************************************************************************
* タイトル	:メニュー背景.cpp
* ファイル名:menuBackGround.cpp
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
#include "menuBackGround.h"
#include "input.h"
#include "player01.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

#define MENUBACKGROUND_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define MENUBACKGROUND_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define MENUBACKGROUND_ANIM_SPAN		( 10 )	/*アニメーション間隔*/
#define MENUBACKGROUND_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define MENUBACKGROUND_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define MENUBACKGROUND_OUTPUT_SIZE	( 1 ) /*初期表示倍率*/ 

#define USE_QUOTE				( 4 )	//使用四角形数

#define VERTEX_HEIGHT_DEV_X	( 30 )		//幅の差
#define VERTEX_HEIGHT_DEV_Y	( 40 )		//高さの差

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffmenuBackGround = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturemenuBackGround;			/*表示画像のテクスチャのポインタ*/
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

static float scrollNum;
/*******************************************************************************
* 関数名	:HRESULT InitMenuBackGround( void )
*		メニュー背景の初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitMenuBackGround( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * MENUBACKGROUND_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffmenuBackGround , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/menuBackGround.png" , &g_pTexturemenuBackGround );
	D3DXGetImageInfoFromFile( "data/TEXTURE/menuBackGround.png" , &info );
	
	/*ポインタのロック*/
	g_pVtxBuffmenuBackGround->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / MENUBACKGROUND_ANIM_DIV_X ) * MENUBACKGROUND_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUBACKGROUND_ANIM_DIV_Y ) * MENUBACKGROUND_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*ポインタのアンロック*/
	g_pVtxBuffmenuBackGround->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	scrollNum = 0;

	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitMenuBackGround( void )
*		メニュー背景の終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitMenuBackGround( void )
{
	if( g_pVtxBuffmenuBackGround != NULL )
	{
		g_pVtxBuffmenuBackGround->Release();
		g_pVtxBuffmenuBackGround = NULL;
	}
	if( g_pTexturemenuBackGround != NULL )
	{
		g_pTexturemenuBackGround->Release();
		g_pTexturemenuBackGround = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateMenuBackGround( void )
*		メニュー背景の更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateMenuBackGround( void )
{
	int quoteCnt, hystericPointBuffer;
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	
	float hystericPer;
	int outputSizeHalf = outputSizeY * 0.5;
	int outputPointPos = 0, outputPointColor = 0;

	/*ポインタのロック*/ 
	g_pVtxBuffmenuBackGround->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / MENUBACKGROUND_ANIM_DIV_X ) * MENUBACKGROUND_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUBACKGROUND_ANIM_DIV_Y ) * MENUBACKGROUND_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	texWidth = 1.0f / MENUBACKGROUND_ANIM_DIV_X;
	texHeight = 1.0f / MENUBACKGROUND_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % MENUBACKGROUND_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / MENUBACKGROUND_ANIM_DIV_X ) ) + 0;

	/*背景スクロール用変数*/
	scrollNum = ( scrollNum + 0.005f == 0.0f ? 1.0f : scrollNum + 0.005f );

	for( quoteCnt = 0; quoteCnt < USE_QUOTE; quoteCnt++ )
	{
		/*テクスチャ座標生成*/
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].tex.x = 0.0f;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].tex.y = 0.0f + scrollNum;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].tex.x = 0.5f;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].tex.y = 0.0f + scrollNum;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].tex.x = 0.0;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].tex.y = 2.0f + scrollNum;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].tex.x = 0.5f;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].tex.y = 2.0f + scrollNum;
		if( quoteCnt == 0 || quoteCnt == 3 )
		{
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].tex.x = 0.0f;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].tex.y = 0.0f + scrollNum;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].tex.x = 0.5f;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].tex.y = 0.0f + scrollNum;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].tex.x = 0.0;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].tex.y = 2.0f + scrollNum;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].tex.x = 0.5f;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].tex.y = 2.0f + scrollNum;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].tex.y	 += 0.603;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].tex.y += 0.603;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].tex.y += 1.017;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].tex.y += 1.017;
		}

		
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 255 );
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 255 );
	}


	pVtx[0].pos = D3DXVECTOR3( 0 - VERTEX_HEIGHT_DEV_X , 0.0f - VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( SCREEN_WIDTH / 4	- VERTEX_HEIGHT_DEV_X , 0.0f - VERTEX_HEIGHT_DEV_Y * 3 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( 0 , SCREEN_HEIGHT + VERTEX_HEIGHT_DEV_Y * 3 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4	- VERTEX_HEIGHT_DEV_X , SCREEN_HEIGHT	+ VERTEX_HEIGHT_DEV_Y , 0.0f );

	pVtx[4].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4	- VERTEX_HEIGHT_DEV_X , 0.0f , 0.0f );
	pVtx[5].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4 * 2	, 0.0f - VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[6].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4	- VERTEX_HEIGHT_DEV_X , SCREEN_HEIGHT + VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[7].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4	* 2	, SCREEN_HEIGHT	, 0.0f );

	pVtx[8].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4	* 2	 , 0.0f - VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[9].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4 * 3	+ VERTEX_HEIGHT_DEV_X , 0.0f , 0.0f );
	pVtx[10].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 2 , SCREEN_HEIGHT , 0.0f );
	pVtx[11].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 3 + VERTEX_HEIGHT_DEV_X , SCREEN_HEIGHT + VERTEX_HEIGHT_DEV_Y , 0.0f );

	pVtx[12].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 3 + VERTEX_HEIGHT_DEV_X , 0.0f - VERTEX_HEIGHT_DEV_Y * 3 , 0.0f );
	pVtx[13].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 4 + VERTEX_HEIGHT_DEV_X , 0.0f - VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[14].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 3 + VERTEX_HEIGHT_DEV_X , SCREEN_HEIGHT + VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[15].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 4 , SCREEN_HEIGHT + VERTEX_HEIGHT_DEV_Y * 3 , 0.0f );

	pVtx[0].pos.x -= VERTEX_HEIGHT_DEV_X;
	pVtx[13].pos.x += VERTEX_HEIGHT_DEV_X;
	
	pVtx[1].pos.x -= VERTEX_HEIGHT_DEV_X;
	pVtx[4].pos.x -= VERTEX_HEIGHT_DEV_X;
	
	pVtx[9].pos.x += VERTEX_HEIGHT_DEV_X;
	pVtx[12].pos.x += VERTEX_HEIGHT_DEV_X;

	/*
	pVtx[3].pos.x += VERTEX_HEIGHT_DEV_X;
	pVtx[6].pos.x += VERTEX_HEIGHT_DEV_X;
	
	pVtx[11].pos.x -= VERTEX_HEIGHT_DEV_X;
	pVtx[14].pos.x -= VERTEX_HEIGHT_DEV_X;
	*/
	/*
	g_posPolygon.x = OUTPUT_LOGO_POSX;
	g_posPolygon.y = 300;
	pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
							g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	*/

	/*ポインタのアンロック*/
	g_pVtxBuffmenuBackGround->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawMenuBackGround( void )
*		メニュー背景の描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawMenuBackGround( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffmenuBackGround , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturemenuBackGround );
		
	/*メニュー背景の描画*/
	
	for( int quoteCnt = 0; quoteCnt < USE_QUOTE; quoteCnt++ )
	{
		/*テクスチャ座標生成*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , MENUBACKGROUND_VERTEX_MAX * quoteCnt , 2 );
	}

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DESTをそのまま描画するよ
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	
	

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*アルファブレンド有効化*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*スクリーン側ブレンド設定*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*ポリゴン側ブレンド設定*/
	//pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}