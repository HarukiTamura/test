/*******************************************************************************
* タイトル	:フレーム画像.cpp
* ファイル名:frameRight.cpp
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
#include "frameRight.h"
#include "input.h"
#include "player01.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

#define FRAMERIGHT_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define FRAMERIGHT_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define FRAMERIGHT_ANIM_SPAN	( 30 )	/*アニメーション間隔*/
#define FRAMERIGHT_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define FRAMERIGHT_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define FRAMERIGHT_OUTPUT_SIZE	( 1 ) /*初期表示倍率*/ 

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffframeRight = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureframeRight;	/*フレーム画像のテクスチャのポインタ*/
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

static int mode;
static int pastMode;

/*******************************************************************************
* 関数名	:HRESULT InitFrameRight( void )
*		フレーム画像の初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitFrameRight( void )
{
	int outputSizeX,outputSizeY;
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * FRAMERIGHT_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffframeRight , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/frameRight.png" , &g_pTextureframeRight );
	D3DXGetImageInfoFromFile( "data/TEXTURE/frameRight.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffframeRight->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / FRAMERIGHT_ANIM_DIV_X ) * FRAMERIGHT_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / FRAMERIGHT_ANIM_DIV_Y ) * FRAMERIGHT_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , 1 );

	/*ポインタのアンロック*/
	g_pVtxBuffframeRight->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	mode = 0;
	pastMode = -1;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitFrameRight( void )
*		フレーム画像の終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitFrameRight( void )
{
	if( g_pVtxBuffframeRight != NULL )
	{
		g_pVtxBuffframeRight->Release();
		g_pVtxBuffframeRight = NULL;
	}
	if( g_pTextureframeRight != NULL )
	{
		g_pTextureframeRight->Release();
		g_pTextureframeRight = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateFrameRight( void )
*		フレーム画像の更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateFrameRight( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/

	/*ポインタのロック*/ 
	g_pVtxBuffframeRight->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	mode = GetPlayerState01()->mode;
				
	texWidth = 1.0f / FRAMERIGHT_ANIM_DIV_X;
	texHeight = 1.0f / FRAMERIGHT_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % FRAMERIGHT_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / FRAMERIGHT_ANIM_DIV_X ) ) + 0;
	
	/*テクスチャ座標生成*/
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	
	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );

	/*頂点座標設定*/
	pVtx[0].pos.x = FIELD_START_X + FIELD_WIDTH - ( SCREEN_WIDTH * 0.5 );
	pVtx[0].pos.y = SCREEN_TOP;
	pVtx[0].pos.z = 0.0f;
		
	pVtx[1].pos.x = FIELD_START_X + FIELD_WIDTH + SCREEN_WIDTH - ( SCREEN_WIDTH * 0.5 );
	pVtx[1].pos.y = SCREEN_TOP;
	pVtx[1].pos.z = 0.0f;
		
	pVtx[2].pos.x = FIELD_START_X + FIELD_WIDTH - ( SCREEN_WIDTH * 0.5 );
	pVtx[2].pos.y = SCREEN_BOTTOM;
	pVtx[2].pos.z = 0.0f;
		 
	pVtx[3].pos.x = FIELD_START_X + FIELD_WIDTH + SCREEN_WIDTH - ( SCREEN_WIDTH * 0.5 );
	pVtx[3].pos.y = SCREEN_BOTTOM;
	pVtx[3].pos.z = 0.0f;

	
	/*ポインタのアンロック*/
	g_pVtxBuffframeRight->Unlock();

	pastMode = mode;
}

/*******************************************************************************
* 関数名	:void DrawFrameRight( void )
*		フレーム画像の描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawFrameRight( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffframeRight , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureframeRight );

	/*フレーム画像のの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}