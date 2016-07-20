/*******************************************************************************
* タイトル	:プレイヤー.cpp
* ファイル名:AfBackGround2.cpp
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
#include "AfBackGround2.h"
#include "AfCursor.h"
#include "unoDirectX.h"
#include "AfPlayer.h"
/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define AF_BACKGROUND2_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_BACKGROUND2_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_BACKGROUND2_DEFAULT_VECTOR	( 0.0f )
#define AF_BACKGROUND2_DEFAULT_POSZ	( 1.0f )
#define AF_BACKGROUND2_DEFAULT_LIFE		( 100 )

#define AF_BACKGROUND2_SPIN	( 1.23 )

#define AF_BACKGROUND2_VERTEX_MAX		( 4 )	/*最高頂点数*/
#define AF_BACKGROUND2_POLYGON_NUM	( 2 )	/*自機＋判定*/

#define AF_BACKGROUND2_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define AF_BACKGROUND2_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define AF_BACKGROUND2_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define AF_BACKGROUND2_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define AF_BACKGROUND2_OUTPUT_SIZE	( 1.8f ) /*初期表示倍率*/ 

#define MOVE_NUM ( 0.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.10f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define SPEED_MAX	( 0.100f )	/*拡大限界*/
#define SPEED_MIN 	( 0.001f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.23f )	/*減衰率*/
#define ROT_ATTEN		( 0.85f )	/*減衰率*/
#define SIPIN_ROT_ATTEN ( 0.12f )

#define DAMAGE_OUTPUT_FPS ( 60 )	/*ダメージ時点滅FPS*/

#define PGPCHANGE_HEIGHT_0	( SCREEN_HEIGHT * 0.5 + 22 )
#define PGPCHANGE_HEIGHT_1	( SCREEN_HEIGHT * 0.5 + 3 )
#define PGPCHANGE_HEIGHT_3	( SCREEN_HEIGHT * 0.5 - 3 )
#define PGPCHANGE_HEIGHT_4	( SCREEN_HEIGHT * 0.5 - 22 )

/*******************************************************************************
* 構造体定義
*******************************************************************************/


/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfBackGround2 = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureAfBackGround2;	/*プレイヤーテクスチャのポインタ*/
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

/*******************************************************************************
* 関数名	:HRESULT InitAfBackGround2( void )
*		プレイヤー初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfBackGround2( void )
{
	HRESULT result;
	result = InitDirectAfBackGround2();
	if( result != S_OK )
	{
		Error("描画基準位置データの初期化に失敗しました。");
	}
	InitValueAfBackGround2();
	return result;
}

/*******************************************************************************
* 関数名	:HRESULT InitDirectAfBackGround2( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitDirectAfBackGround2( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_BACKGROUND2_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfBackGround2 , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/backSky.png" , &g_pTextureAfBackGround2 );
	D3DXGetImageInfoFromFile( "data/TEXTURE/backSky.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffAfBackGround2->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_BACKGROUND2_POLYGON_NUM );

	/*ポインタのアンロック*/
	g_pVtxBuffAfBackGround2->Unlock();
}

/*******************************************************************************
* 関数名	:HRESULT InitValueAfBackGround2( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void InitValueAfBackGround2( void )
{
	int outputSizeX , outputSizeY;
	
	//事前サイズ計算
	outputSizeX = ( int )( ( info.Width / AF_BACKGROUND2_ANIM_DIV_X ) * AF_BACKGROUND2_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_BACKGROUND2_ANIM_DIV_Y ) * AF_BACKGROUND2_OUTPUT_SIZE );

	//事前サイズ変換
	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	g_posPolygon.x = AF_BACKGROUND2_DEFAULT_POSX;
	g_posPolygon.y = AF_BACKGROUND2_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*アニメーション用ステータス*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	
}

/*******************************************************************************
* 関数名	:void UninitAfBackGround2( void )
*		プレイヤー終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfBackGround2( void )
{
	if( g_pVtxBuffAfBackGround2 != NULL )
	{
		g_pVtxBuffAfBackGround2->Release();
		g_pVtxBuffAfBackGround2 = NULL;
	}
	if( g_pTextureAfBackGround2 != NULL )
	{
		g_pTextureAfBackGround2->Release();
		g_pTextureAfBackGround2 = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfBackGround2( void )
*		プレイヤー更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfBackGround2( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;

	/*ポインタのロック*/ 
	g_pVtxBuffAfBackGround2->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	/*移動量反映*/
	g_movePolygon.x = 0.0f;
	g_movePolygon.y = 0.0f;

	g_posPolygon.x += g_movePolygon.x * MOVE_ATTEN;
	g_posPolygon.y += g_movePolygon.y * MOVE_ATTEN;
	g_rotPolygon.z += g_movePolygon.z;

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
	//正規値設定
	g_rotPolygon.x = getViewStatus()->centerPosX;
	g_posPolygon.y = ( SCREEN_HEIGHT - getViewStatus()->centerPosY );
	g_rotPolygon.z = -getViewStatus()->centerVector;

	/*移動量減衰*/ 
	g_movePolygon.z *= SIPIN_ROT_ATTEN;	

	/*ポリゴン拡大*/
	/*
	if( GetKeyboardPress( DIK_UP ) )
	{
		polygonMag = ( polygonMag + MAG_SPEED > MAG_MAX ? MAG_MAX : polygonMag + MAG_SPEED );
	}
	if( GetKeyboardPress( DIK_DOWN ) )
	{
		polygonMag = ( polygonMag - MAG_SPEED < MAG_MIN ? MAG_MIN : polygonMag - MAG_SPEED );
	}
	*/
	
	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	//アニメーション設定
	if( ++g_nCounterAnim == AF_BACKGROUND2_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == AF_BACKGROUND2_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / AF_BACKGROUND2_ANIM_DIV_X;
	texHeight = 1.0f / AF_BACKGROUND2_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % AF_BACKGROUND2_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_BACKGROUND2_ANIM_DIV_X ) );
	
	//テクスチャ座標設定
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;

	/*ポインタのアンロック*/
	g_pVtxBuffAfBackGround2->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawAfBackGround2( void )
*		プレイヤー描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfBackGround2( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfBackGround2 );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfBackGround2 , 0 , sizeof( VERTEX_2D ) );

	/*プレイヤーの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}

/*******************************************************************************
* 関数名	:void DrawAfBackGround2core( void )
*		プレイヤー描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfBackGround2core( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfBackGround2 );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfBackGround2 , 0 , sizeof( VERTEX_2D ) );
	
	
	/*プレイヤーの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_BACKGROUND2_VERTEX_MAX , 2 );

}
