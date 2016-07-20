/*******************************************************************************
* タイトル	:背景.cpp
* ファイル名:backGround.cpp
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
#include "backGround.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define BACK_VERTEX_MAX ( 4 )
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
static float texPos = 0;	/*座標加算値*/

/*******************************************************************************
* 関数名	:HRESULT InitBackGround( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitBackGround( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * BACK_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffPolygon , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}

	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/back000.png" , &g_pTextureBack );
	
	/*ポインタのロック*/
	g_pVtxBuffPolygon->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	ZeroMemory( pVtx , sizeof( VERTEX_2D ) * BACK_VERTEX_MAX );

	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( 0 - 0.5f , 0 - 0.5f , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( WINDOW_WIDTH - 0.5f, 0 - 0.5f , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( 0 - 0.5f , WINDOW_HEIGHT - 0.5f , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( WINDOW_WIDTH - 0.5f , WINDOW_HEIGHT - 0.5f , 0.0f );

	/*座標変換用係数設定*/
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;
	
	/*頂点カラー設定*/
	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	pVtx[0].tex = D3DXVECTOR2( 0.0 , 0.0 );
	pVtx[1].tex = D3DXVECTOR2( 1.0 , 0.0 );
	pVtx[2].tex = D3DXVECTOR2( 0.0 , 1.0 );
	pVtx[3].tex = D3DXVECTOR2( 1.0 , 1.0 );

	/*ポインタのアンロック*/
	g_pVtxBuffPolygon->Unlock();

	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitBackGround( void )
*		背景終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitBackGround( void )
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
* 関数名	:void UpdateBackGround( void )
*		背景更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateBackGround( void )
{
}

/*******************************************************************************
* 関数名	:void DrawBackGround( void )
*		背景描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawBackGround( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureBack );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffPolygon , 0 , sizeof( VERTEX_2D ) );

	/*背景の描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}