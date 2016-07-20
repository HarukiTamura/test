/*******************************************************************************
* タイトル	:背景.cpp
* ファイル名:afBackGround4.cpp
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
#include "input.h"
#include "unoDirectX.h"
#include "unoHitChecker.h"
#include "afBackGround4.h"
#include "afPointConvert.h"
#include "AfPlayer.h"
/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define BACKGROUND4_USE_TRIANGLE_NUM		( 64 )	//使用三角形数
#define USE_VERTEX_NUM	( BACKGROUND4_USE_TRIANGLE_NUM + 2 )		//使用頂点数

#define DAY_SPEED	( 0.0025 )

#define DRAW_RANGE_X	( 800 )	//描画半径X
#define DRAW_RANGE_Y	( 800 )	//描画半径Y
#define DRAW_Y_PLUS		( 30 )	//描画半径Y追加分

#define ANIMATION_PATTERN	( 2 )	//PATTERN数
#define ANIMATION_MAX	( 60 )		//アニメーションループ秒数
#define ANIMATION_SPAN	( 2 )		//１コマのFPS数
/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffPolygon = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureBack;	/*背景テクスチャのポインタ*/

static float writeVector = 0;
static float writePosX = 12; 
static float writePosY = 300;

static int nowAnimationFps = 0;
static int nowAnimationPattern = 0;

static float posX1,posY1,posX2,posY2;
static float g_rangePlus;
static float g_morning;

/*******************************************************************************
* 関数名	:HRESULT InitAfBackGround4( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfBackGround4( void )
{
	HRESULT result = InitDirectAfBackGround4();
	InitValueAfBackGround4();
	return result;
}

/*******************************************************************************
* 関数名	:HRESULT InitDirectAfBackGround4( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitDirectAfBackGround4( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * USE_VERTEX_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffPolygon , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}

	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfBackground4.png" , &g_pTextureBack );
	
	/*ポインタのロック*/
	g_pVtxBuffPolygon->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInit( pVtx , USE_VERTEX_NUM );
	
	g_rangePlus = 2.0f;
	g_morning = 1.0f;
	/*ポインタのアンロック*/
	g_pVtxBuffPolygon->Unlock();
}

/*******************************************************************************
* 関数名	:HRESULT InitValueAfBackGround4( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void InitValueAfBackGround4( void )
{
	writeVector = 0;
	writePosX = SCREEN_WIDTH / 2; 
	writePosY = SCREEN_HEIGHT / 2;

	nowAnimationFps = 0;
	nowAnimationPattern = 0;
}

/*******************************************************************************
* 関数名	:void UninitAfBackGround4( void )
*		背景終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfBackGround4( void )
{
	if( g_pVtxBuffPolygon != NULL )
	{
		g_pVtxBuffPolygon->Release();
		g_pVtxBuffPolygon = NULL;
	}
	if( g_pTextureBack != NULL )
	{
		g_pTextureBack->Release();
		g_pTextureBack = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfBackGround4( void )
*		背景更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfBackGround4( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float frameVertex = 0.0f , drawRangeX = 0.0f , drawRangeY = 0.0f;
	float texturePosPlus = 0.0f;
	int fade = 0;
	writeVector = 0;//getViewStatus()->centerVector + D3DX_PI;
	writePosX = SCREEN_WIDTH / 2;
	writePosY = SCREEN_HEIGHT / 3 * 1;
	
	fade = 255 * ( getViewStatus()->centerPosY / SCREEN_HEIGHT ) * g_morning;// * cos( getViewStatus()->centerVector );
	g_morning = ( g_morning - DAY_SPEED <= 0.0f ? 0.0f : g_morning - DAY_SPEED );

	g_rangePlus = 1;
	
	//g_rangePlus = (	g_rangePlus + 0.3 >= 3.0f ? 3.0f : g_rangePlus + 0.3 );

	//アニメーションの生成
	nowAnimationFps = ( nowAnimationFps + 1 >= ANIMATION_MAX ? 0 : nowAnimationFps + 1 );
	if( nowAnimationFps % ANIMATION_SPAN )
	{
		nowAnimationPattern = ( nowAnimationPattern + 1 > ANIMATION_MAX ? 0 : nowAnimationPattern + 1 );
	}
	texturePosPlus = 1.0f / ANIMATION_PATTERN * nowAnimationPattern;

	/*ポインタのロック*/
	g_pVtxBuffPolygon->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	//描画中心点の生成
	pVtx[0].pos = D3DXVECTOR3( writePosX , writePosY , 0.0f );	//頂点座標設定
	pVtx[0].col = D3DCOLOR_RGBA( 122 , 122 , 255 , fade );	//頂点カラー設定
	pVtx[0].tex = D3DXVECTOR2( 0.5f , 0.5f );		//テクスチャ座標設定

	//残りの頂点の設定
	for( int cnt = 1; cnt < USE_VERTEX_NUM; cnt++ )
	{
		float posX , posY;
		float texX , texY;

		posX = writePosX + DRAW_RANGE_X * g_rangePlus * cosf( D3DX_PI * frameVertex + writeVector );
		posY = writePosY + DRAW_RANGE_Y * g_rangePlus * sinf( D3DX_PI * frameVertex + writeVector );
		
		//頂点座標設定
		pVtx[cnt].pos.x = posX;
		pVtx[cnt].pos.y = posY;
		pVtx[cnt].pos.z = 0.0f;
		
		if( cnt == 1 )
		{
			posX1 = posX;
			posY1 = posY;
		}
		if( cnt == USE_VERTEX_NUM - 1 )
		{
			posX2 = posX;
			posY2 = posY;
		}

		//頂点カラー設定
		pVtx[cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		
		//テクスチャ座標設定1
		pVtx[cnt].tex = D3DXVECTOR2( 0.5f + 0.5f * -cos( D3DX_PI * frameVertex ) ,  0.5f + 0.5f * -sin( D3DX_PI * frameVertex ) );
		//メモ：pVtx[0].texでやると気持ち悪くも面白い動きをする。

		frameVertex += 2.0f / BACKGROUND4_USE_TRIANGLE_NUM;
		
	}
	

	/*ポインタのアンロック*/
	g_pVtxBuffPolygon->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawAfBackGround4( void )
*		背景描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfBackGround4( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureBack );
	
	/*頂点バッファをストリームにバインド*/
	for( int cnt = 0; cnt < USE_VERTEX_NUM; cnt++ )
	{
		pD3DDevice->SetStreamSource( 0 , g_pVtxBuffPolygon , cnt , sizeof( VERTEX_2D ) );
		/*背景の描画*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLEFAN , 0 , BACKGROUND4_USE_TRIANGLE_NUM );
	}
	
	

}
