/*******************************************************************************
* タイトル	:背景.cpp
* ファイル名:BackPlusAlpha.cpp
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
#include "BackPlusAlpha.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define BACK2_VERTEX_MAX ( 12 )

#define GAUGEA_POSY	( 460 )

#define BAR_POSX	( 0 )
#define BAR_POSY	( 40 )
#define BAR2_POSY	( 150 )
#define BAR_HEIGHT	( 120 )

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
* 関数名	:HRESULT InitBackPlusAlpha( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitBackPlusAlpha( void )
{
	VERTEX_2D *pVtxBack2;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * BACK2_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffPolygon2 , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}

	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/BackPlusAlpha.png" , &g_pTextureBack2 );
	
	/*ポインタのロック*/
	g_pVtxBuffPolygon2->Lock( 0 , 0 , ( void** ) &pVtxBack2 , 0 );
	
	ZeroMemory( pVtxBack2 , sizeof( VERTEX_2D ) * BACK2_VERTEX_MAX );

	/*頂点座標設定*/
	pVtxBack2[0].pos = D3DXVECTOR3( 0.0f , GAUGEA_POSY , 0.0f );
	pVtxBack2[1].pos = D3DXVECTOR3( WINDOW_WIDTH , GAUGEA_POSY , 0.0f );
	pVtxBack2[2].pos = D3DXVECTOR3( 0.0f , WINDOW_HEIGHT , 0.0f );
	pVtxBack2[3].pos = D3DXVECTOR3( WINDOW_WIDTH , WINDOW_HEIGHT , 0.0f );

	pVtxBack2[4].pos = D3DXVECTOR3( 0.0f ,  BAR_POSY , 0.0f );
	pVtxBack2[5].pos = D3DXVECTOR3( WINDOW_WIDTH ,  BAR_POSY , 0.0f );
	pVtxBack2[6].pos = D3DXVECTOR3( 0.0f ,  BAR_POSY + BAR_HEIGHT , 0.0f );
	pVtxBack2[7].pos = D3DXVECTOR3( WINDOW_WIDTH ,  BAR_POSY + BAR_HEIGHT , 0.0f );

	pVtxBack2[8].pos = D3DXVECTOR3( 0.0f ,  BAR2_POSY , 0.0f );
	pVtxBack2[9].pos = D3DXVECTOR3( WINDOW_WIDTH ,  BAR2_POSY , 0.0f );
	pVtxBack2[10].pos = D3DXVECTOR3( 0.0f ,  BAR2_POSY + BAR_HEIGHT , 0.0f );
	pVtxBack2[11].pos = D3DXVECTOR3( WINDOW_WIDTH ,  BAR2_POSY + BAR_HEIGHT , 0.0f );


	/*座標変換用係数設定*/
	pVtxBack2[0].rhw = 1.0f;
	pVtxBack2[1].rhw = 1.0f;
	pVtxBack2[2].rhw = 1.0f;
	pVtxBack2[3].rhw = 1.0f;
	
	pVtxBack2[4].rhw = 1.0f;
	pVtxBack2[5].rhw = 1.0f;
	pVtxBack2[6].rhw = 1.0f;
	pVtxBack2[7].rhw = 1.0f;
	
	pVtxBack2[8].rhw = 1.0f;
	pVtxBack2[9].rhw = 1.0f;
	pVtxBack2[10].rhw = 1.0f;
	pVtxBack2[11].rhw = 1.0f;
	
	/*頂点カラー設定*/
	pVtxBack2[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	pVtxBack2[4].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[5].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[6].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[7].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	pVtxBack2[8].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[9].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[10].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[11].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	/*テクスチャ座標設定*/
	pVtxBack2[0].tex = D3DXVECTOR2( 0.0 , 0.0 );
	pVtxBack2[1].tex = D3DXVECTOR2( 0.5 , 0.0 );
	pVtxBack2[2].tex = D3DXVECTOR2( 0.0 , 0.5 );
	pVtxBack2[3].tex = D3DXVECTOR2( 0.5 , 0.5 );
	
	pVtxBack2[4].tex = D3DXVECTOR2( 0.0 , 0.5 );
	pVtxBack2[5].tex = D3DXVECTOR2( 0.5 , 0.5 );
	pVtxBack2[6].tex = D3DXVECTOR2( 0.0 , 1.0 );
	pVtxBack2[7].tex = D3DXVECTOR2( 0.5 , 1.0 );
	
	pVtxBack2[8].tex = D3DXVECTOR2( 0.0 , 0.5 );
	pVtxBack2[9].tex = D3DXVECTOR2( 0.5 , 0.5 );
	pVtxBack2[10].tex = D3DXVECTOR2( 0.0 , 1.0 );
	pVtxBack2[11].tex = D3DXVECTOR2( 0.5 , 1.0 );

	/*ポインタのアンロック*/
	g_pVtxBuffPolygon2->Unlock();

	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitBackPlusAlpha( void )
*		背景終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitBackPlusAlpha( void )
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
* 関数名	:void UpdateBackPlusAlpha( void )
*		背景更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateBackPlusAlpha( void )
{
	VERTEX_2D *pVtxBack2;	/*頂点バッファへのポインタ*/

	texPos = ( texPos >= 1.0 ? 0 : texPos + 0.0025 );
	
	/*ポインタのロック*/
	g_pVtxBuffPolygon2->Lock( 0 , 0 , ( void** ) &pVtxBack2 , 0 );
	
	/*テクスチャ座標設定*/
	pVtxBack2[0].tex = D3DXVECTOR2( 0.0 + texPos , 0.0 );
	pVtxBack2[1].tex = D3DXVECTOR2( 2.0 + texPos , 0.0 );
	pVtxBack2[2].tex = D3DXVECTOR2( 0.0 + texPos , 0.5 );
	pVtxBack2[3].tex = D3DXVECTOR2( 2.0 + texPos , 0.5 );
	
	pVtxBack2[4].tex = D3DXVECTOR2( 0.0 + texPos , 0.5 );
	pVtxBack2[5].tex = D3DXVECTOR2( 2.0 + texPos , 0.5 );
	pVtxBack2[6].tex = D3DXVECTOR2( 0.0 + texPos , 1.0 );
	pVtxBack2[7].tex = D3DXVECTOR2( 2.0 + texPos , 1.0 );

	pVtxBack2[8].tex = D3DXVECTOR2( 0.0 - texPos , 0.5 );
	pVtxBack2[9].tex = D3DXVECTOR2( 2.0 - texPos , 0.5 );
	pVtxBack2[10].tex = D3DXVECTOR2( 0.0 - texPos , 1.0 );
	pVtxBack2[11].tex = D3DXVECTOR2( 2.0 - texPos , 1.0 );
	
	/*ポインタのアンロック*/
	g_pVtxBuffPolygon2->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawBackPlusAlpha( void )
*		背景描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawBackPlusAlpha( void )
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
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 4 , 2 );
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 8 , 2 );
}
