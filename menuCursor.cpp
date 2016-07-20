/*******************************************************************************
* タイトル	:メニューカーソル.cpp
* ファイル名:menuCursor.cpp
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
#include "menuCursor.h"
#include "input.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

#define MENUCURSOR_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define CURSOR_SWING	( 20 )	/*カーソル揺れ幅*/

#define OUTPUT_CURSOR_POSX	( SCREEN_WIDTH / 2 - SCREEN_WIDTH / 4 )
#define OUTPUT_CURSOR_POSY	( SCREEN_HEIGHT / 2 )

#define MENUCURSOR_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define MENUCURSOR_ANIM_SPAN		( 10 )	/*アニメーション間隔*/
#define MENUCURSOR_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define MENUCURSOR_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define MENUCURSOR_OUTPUT_SIZE	( 1 ) /*初期表示倍率*/ 

#define USE_QUOTE				( 1 )	//使用四角形数

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffmenuCursor = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturemenuCursor;			/*表示画像のテクスチャのポインタ*/
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
static int g_pastCommand;
static float g_nowCursorPosX;
static float g_CursorPosXPlus;	//メニュー突入時のスライドイン用
/*******************************************************************************
* 関数名	:HRESULT InitMenuCursor( void )
*		メニューカーソルの初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitMenuCursor( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * MENUCURSOR_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffmenuCursor , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/menuCursor.png" , &g_pTexturemenuCursor );
	D3DXGetImageInfoFromFile( "data/TEXTURE/menuCursor.png" , &info );
	
	/*ポインタのロック*/
	g_pVtxBuffmenuCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / MENUCURSOR_ANIM_DIV_X ) * MENUCURSOR_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUCURSOR_ANIM_DIV_Y ) * MENUCURSOR_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*ポインタのアンロック*/
	g_pVtxBuffmenuCursor->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	outputTexWidth = 1.0f / ( float )( USE_QUOTE );

	g_pastCommand = 0;
	g_nowCursorPosX = 0;
	g_CursorPosXPlus = 600;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitMenuCursor( void )
*		メニューカーソルの終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitMenuCursor( void )
{
	if( g_pVtxBuffmenuCursor != NULL )
	{
		g_pVtxBuffmenuCursor->Release();
		g_pVtxBuffmenuCursor = NULL;
	}
	if( g_pTexturemenuCursor != NULL )
	{
		g_pTexturemenuCursor->Release();
		g_pTexturemenuCursor = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateMenuCursor( int g_menuCommand )
*		メニューカーソルの更新処理関数です。
* 引数		:int g_menuCommand		現在選択しているコマンドの番号です。
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateMenuCursor( int g_menuCommand )
{
	int quoteCnt, hystericPointBuffer;
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	
	float hystericPer;
	int outputSizeHalf = outputSizeY * 0.5;
	int outputPointPos = 0, outputPointColor = 0;

	g_CursorPosXPlus = ( g_CursorPosXPlus - 25.0f >= 0 ? g_CursorPosXPlus - 25.0f : 0 );
	if( g_pastCommand != g_menuCommand )
	{
		g_nowCursorPosX = 0;
	}
	else
	{
		g_nowCursorPosX = ( g_nowCursorPosX + 0.025f > 1.0f ? 0.0f : g_nowCursorPosX + 0.025f );
	}

	
	/*ポインタのロック*/ 
	g_pVtxBuffmenuCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / MENUCURSOR_ANIM_DIV_X ) * MENUCURSOR_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUCURSOR_ANIM_DIV_Y ) * MENUCURSOR_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );

	texWidth = 1.0f / MENUCURSOR_ANIM_DIV_X;
	texHeight = 1.0f / MENUCURSOR_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % MENUCURSOR_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / MENUCURSOR_ANIM_DIV_X ) ) + 0;

	/*テクスチャ座標生成*/
	/*PleasePushAnykey*/
	pVtx[0].tex.x = 0.0f;
	pVtx[0].tex.y = 0.0f;
	pVtx[1].tex.x = 1.0f;
	pVtx[1].tex.y = 0.0f;
	pVtx[2].tex.x = 0.0f;
	pVtx[2].tex.y = 1.0f;
	pVtx[3].tex.x = 1.0f;
	pVtx[3].tex.y = 1.0f;
		
	pVtx[0].col = D3DCOLOR_RGBA( 255, 255 , 255 , 158 );
	pVtx[1].col = D3DCOLOR_RGBA( 255, 255 , 255 , 158 );
	pVtx[2].col = D3DCOLOR_RGBA( 255, 255 , 255 , 158 );
	pVtx[3].col = D3DCOLOR_RGBA( 255, 255 , 255 , 158 );

	g_posPolygon.x = OUTPUT_CURSOR_POSX + CURSOR_SWING * sin( 2 * D3DX_PI * g_nowCursorPosX ) - ( 2 * CURSOR_SWING * sin( 2 * D3DX_PI * g_nowCursorPosX ) ) + g_CursorPosXPlus;
	g_posPolygon.y = SCREEN_HEIGHT / MENU_MAX * g_menuCommand + ( SCREEN_HEIGHT / MENU_MAX / 2 );
	
	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
							g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*ポインタのアンロック*/
	g_pVtxBuffmenuCursor->Unlock();

	
	g_pastCommand = g_menuCommand;

}

/*******************************************************************************
* 関数名	:void DrawMenuCursor( void )
*		メニューカーソルの描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawMenuCursor( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffmenuCursor , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturemenuCursor );
	

	/*メニューカーソルの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}