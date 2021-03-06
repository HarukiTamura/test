/*******************************************************************************
* タイトル	:背景.cpp
* ファイル名:BackGround5.cpp
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
#include "BackGround5.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define BACK5_VERTEX_MAX ( 4 )
/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffPolygon2 = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureBack2;	/*背景テクスチャのポインタ*/
static float texPos = 0;	/*座標加算値*/

/*******************************************************************************
* 関数名	:HRESULT InitBackGround5( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitBackGround5( void )
{
	VERTEX_2D *pVtxBack5;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * BACK5_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffPolygon2 , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}

	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/back004.png" , &g_pTextureBack2 );
	
	/*ポインタのロック*/
	g_pVtxBuffPolygon2->Lock( 0 , 0 , ( void** ) &pVtxBack5 , 0 );

	ZeroMemory( pVtxBack5 , sizeof( VERTEX_2D ) * BACK5_VERTEX_MAX );

	/*頂点座標設定*/
	pVtxBack5[0].pos = D3DXVECTOR3( 0 - 0.5f , 0 - 0.5f , 0.0f );
	pVtxBack5[1].pos = D3DXVECTOR3( WINDOW_WIDTH - 0.5f , 0 - 0.5f , 0.0f );
	pVtxBack5[2].pos = D3DXVECTOR3( 0 - 0.5f , WINDOW_HEIGHT - 0.5f , 0.0f );
	pVtxBack5[3].pos = D3DXVECTOR3( WINDOW_WIDTH - 0.5f , WINDOW_HEIGHT - 0.5f , 0.0f );

	/*座標変換用係数設定*/
	pVtxBack5[0].rhw = 1.0f;
	pVtxBack5[1].rhw = 1.0f;
	pVtxBack5[2].rhw = 1.0f;
	pVtxBack5[3].rhw = 1.0f;
	
	/*頂点カラー設定*/
	pVtxBack5[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack5[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack5[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack5[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	/*テクスチャ座標設定*/
	pVtxBack5[0].tex = D3DXVECTOR2( 0.0f , 0.0f );
	pVtxBack5[1].tex = D3DXVECTOR2( 1.0f , 0.0f );
	pVtxBack5[2].tex = D3DXVECTOR2( 0.0f , 1.0f );
	pVtxBack5[3].tex = D3DXVECTOR2( 1.0f , 1.0f );



	/*ポインタのアンロック*/
	g_pVtxBuffPolygon2->Unlock();

	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitBackGround5( void )
*		背景終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitBackGround5( void )
{
	if( g_pVtxBuffPolygon2 != NULL )
	{
		g_pVtxBuffPolygon2->Release();
		g_pVtxBuffPolygon2 = NULL;
	}
	if( g_pTextureBack2 != NULL )
	{
		g_pTextureBack2->Release();
		g_pTextureBack2 = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateBackGround5( void )
*		背景更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateBackGround5( void )
{
	VERTEX_2D *pVtxBack5;	/*頂点バッファへのポインタ*/

}

/*******************************************************************************
* 関数名	:void DrawBackGround5( void )
*		背景描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawBackGround5( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureBack2 );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffPolygon2 , 0 , sizeof( VERTEX_2D ) );

	/*背景の描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}
