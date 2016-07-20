/*******************************************************************************
* タイトル	:メニューテキスト.cpp
* ファイル名:menuTxt.cpp
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
#include "menu.h"
#include "menuTxt.h"
#include "input.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

#define MENUTXT_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define OUTPUT_LOGO_POSX	( SCREEN_WIDTH / 2 )
#define OUTPUT_LOGO_POSY	( 0 )

#define LOGO_WAVE	( 20 )	//ロゴの揺れる範囲
#define COLOR_WAVE	( 10 )	//色の揺れる範囲

#define WAVELOGO_NUM			( 600 )

#define MENUTXT_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define MENUTXT_ANIM_SPAN		( 10 )	/*アニメーション間隔*/
#define MENUTXT_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define MENUTXT_ANIM_DIV_Y	( WAVELOGO_NUM )	/*高さ分割数*/

#define MENUTXT_OUTPUT_SIZE	( 1 ) /*初期表示倍率*/ 

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffmenuTxt = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturemenuTxt;			/*表示画像のテクスチャのポインタ*/
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
static float g_TxtPosXPlus;		//テキストスライドイン用
static float g_TxtPosXMinus;	//テキストスライド用
/*******************************************************************************
* 関数名	:HRESULT InitMenuTxt( void )
*		メニューテキストの初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitMenuTxt( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * MENUTXT_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffmenuTxt , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/menuTxt.png" , &g_pTexturemenuTxt );
	D3DXGetImageInfoFromFile( "data/TEXTURE/menuTxt.png" , &info );
	
	/*ポインタのロック*/
	g_pVtxBuffmenuTxt->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / MENUTXT_ANIM_DIV_X ) * MENUTXT_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUTXT_ANIM_DIV_Y ) * MENUTXT_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*ポインタのアンロック*/
	g_pVtxBuffmenuTxt->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	outputTexWidth = 1.0f / ( float )( USE_QUOTE );
	g_TxtPosXPlus = 1.0f;
	g_TxtPosXMinus = 0.0f;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitMenuTxt( void )
*		メニューテキストの終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitMenuTxt( void )
{
	if( g_pVtxBuffmenuTxt != NULL )
	{
		g_pVtxBuffmenuTxt->Release();
		g_pVtxBuffmenuTxt = NULL;
	}
	if( g_pTexturemenuTxt != NULL )
	{
		g_pTexturemenuTxt->Release();
		g_pTexturemenuTxt = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateMenuTxt( int g_menuCommand )
*		メニューテキストの更新処理関数です。
* 引数		:int g_menuCommand		現在選択しているコマンドの番号です。
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateMenuTxt( int g_menuCommand )
{
	int quoteCnt, hystericPointBuffer;
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	
	float TxtPosXPlus = 0.0f;
	float hystericPer;
	int outputSizeHalf = outputSizeY * 0.5;
	int outputPointPos = 0, outputPointColor = 0;
	int colRed = 0, colGreen = 0, colBlue = 0;
	int posPolygonY = 0;

	/*ポインタのロック*/ 
	g_pVtxBuffmenuTxt->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	g_TxtPosXPlus = ( g_TxtPosXPlus - 0.05f >= 0.0f ? g_TxtPosXPlus - 0.05f : 0 ); 
	if( g_TxtPosXPlus == 0 )
	{
		g_TxtPosXMinus = ( g_TxtPosXMinus + 0.012f >= 0.5f ? 0.5f : g_TxtPosXMinus + 0.012f ); 
	}
	

	if( GetAnykeyPress() )
	{
		g_TxtPosXPlus = 0.0f;
		g_TxtPosXMinus = 0.5f;
	}
	outputSizeX = ( int )( ( info.Width / MENUTXT_ANIM_DIV_X ) * MENUTXT_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUTXT_ANIM_DIV_Y ) * MENUTXT_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );

	texWidth = 1.0f / MENUTXT_ANIM_DIV_X;
	texHeight = 1.0f / MENUTXT_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % MENUTXT_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / MENUTXT_ANIM_DIV_X ) ) + 0;

	for( quoteCnt = 0; quoteCnt < WAVELOGO_NUM; quoteCnt++ )
	{
		/*基礎カラー設定*/
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 100 , 55 - rand() % COLOR_WAVE , 55 - rand() % COLOR_WAVE , 155 );
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 100 , 55 - rand() % COLOR_WAVE , 55 - rand() % COLOR_WAVE , 155 );
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 100 , 55 - rand() % COLOR_WAVE , 55 - rand() % COLOR_WAVE , 155 );
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 100 , 55 - rand() % COLOR_WAVE , 55 - rand() % COLOR_WAVE , 155 );

		/*頂点座標設定*/
		if( quoteCnt == 0 )
		{
			g_posPolygon.x = OUTPUT_LOGO_POSX;
			g_posPolygon.y = OUTPUT_LOGO_POSY;
		}
		else
		{	
			g_posPolygon.y += outputSizeY;
			if( g_posPolygon.y >= SCREEN_HEIGHT / MENU_MAX * g_menuCommand &&  g_posPolygon.y <= SCREEN_HEIGHT / MENU_MAX * g_menuCommand + ( SCREEN_HEIGHT / MENU_MAX ) )
			{
				//選択中コマンドは揺らさない。
				g_posPolygon.x = OUTPUT_LOGO_POSX;
				//選択中コマンド専用カラー
				pVtx[quoteCnt * MENUTXT_VERTEX_MAX].col		= D3DCOLOR_RGBA( 255 , 255 , 255 , 225 );
				pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 225 );
				pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 225 );
				pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 225 );
			}
			else
			{
				g_posPolygon.x = OUTPUT_LOGO_POSX + rand() % LOGO_WAVE - rand() % LOGO_WAVE;
			}
		}
		
		/*テクスチャ座標生成*/
		/*PleasePushAnykey*/
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX].tex.x = 0.0f;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX].tex.y = outputTexWidth * quoteCnt;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].tex.x = 1.0f;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].tex.y = outputTexWidth * quoteCnt + outputTexWidth;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].tex.x = 0.0f;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].tex.y = outputTexWidth * quoteCnt;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].tex.x = 1.0f;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].tex.y = outputTexWidth * quoteCnt + outputTexWidth;

		g_posPolygon.x -= ( SCREEN_WIDTH * 0.25f ) * sin( D3DX_PI * g_TxtPosXMinus );
		g_posPolygon.y = - 10 + SCREEN_HEIGHT * sin( D3DX_PI * g_TxtPosXPlus );
		

		pVtx[quoteCnt * MENUTXT_VERTEX_MAX].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
		
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	}

	
	outputSizeX = ( int )( ( info.Width / MENUTXT_ANIM_DIV_X ) * MENUTXT_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / 1 ) * MENUTXT_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );

	pVtx[quoteCnt * MENUTXT_VERTEX_MAX].tex.x = 0.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX].tex.y = 0.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].tex.x = 1.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].tex.y = 0.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].tex.x = 0.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].tex.y = 1.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].tex.x = 1.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].tex.y = 1.0f;
	

	/*
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255, 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 155 );
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255, 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 155 );
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255, 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 155 );
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255, 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 155 );
	*/

	/*頂点座標設定*/
	g_posPolygon.x = OUTPUT_LOGO_POSX - ( SCREEN_WIDTH * 0.25f ) * sin( D3DX_PI * g_TxtPosXMinus );
	g_posPolygon.y = 300;
	g_posPolygon.y *= cos( D3DX_PI * g_TxtPosXPlus );
	
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
							g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*ポインタのアンロック*/
	g_pVtxBuffmenuTxt->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawMenuTxt( void )
*		メニューテキストの描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawMenuTxt( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffmenuTxt , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturemenuTxt );
	
	for( int quoteCnt = 0; quoteCnt < USE_QUOTE; quoteCnt++ )
	{
		if( quoteCnt >= WAVELOGO_NUM )
		{
			GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
			GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DESTをそのまま描画するよ
			GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		}
	
		/*メニューテキストの描画*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , quoteCnt * MENUTXT_VERTEX_MAX , 2 );
	}
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*アルファブレンド有効化*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*スクリーン側ブレンド設定*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*ポリゴン側ブレンド設定*/
	//pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}