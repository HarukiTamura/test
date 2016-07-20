/*******************************************************************************
* タイトル	:カーソル.cpp
* ファイル名:Cursor.cpp
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
#include "cursor.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define POLYGON_VERTEX_MAX ( 4 )
#define CURSOR_SIZE	( 0.2f )	/*カーソルの表示倍率です*/
/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffCursor = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureCursorAkr;	/*カーソルテクスチャのポインタ*/
static D3DXIMAGE_INFO info;		/*画像情報*/

static float texPos = 0;	/*座標加算値*/

/*******************************************************************************
* 関数名	:HRESULT InitCursor( void )
*		カーソル初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitCursor( void )
{
	int verCnt;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * POLYGON_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffCursor , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}

	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/cursor.png" , &g_pTextureCursorAkr );
	D3DXGetImageInfoFromFile( "data/TEXTURE/cursor.png" , &info );
	
	/*ポインタのロック*/
	g_pVtxBuffCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( 200 , 50 , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( 200 + info.Width * CURSOR_SIZE , 50 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( 200 , 50 + info.Height * CURSOR_SIZE, 0.0f );
	pVtx[3].pos = D3DXVECTOR3( 200 + info.Width * CURSOR_SIZE , 50 + info.Height * CURSOR_SIZE , 0.0f );

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
	
	/*テクスチャ座標設定*/
	pVtx[0].tex = D3DXVECTOR2( 0 , 0 );
	pVtx[1].tex = D3DXVECTOR2( 1 , 0 );
	pVtx[2].tex = D3DXVECTOR2( 0 , 1 );
	pVtx[3].tex = D3DXVECTOR2( 1 , 1 );

	/*ポインタのアンロック*/
	g_pVtxBuffCursor->Unlock();

	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitCursor( void )
*		カーソル終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitCursor( void )
{
	if( g_pVtxBuffCursor != NULL )
	{
		g_pVtxBuffCursor->Release();
		g_pVtxBuffCursor = NULL;
	}
	if( g_pTextureCursorAkr != NULL )
	{
		g_pTextureCursorAkr->Release();
		g_pTextureCursorAkr = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateCursor( void )
*		カーソル更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateCursor( void )
{
	int verCnt;
	static float vector = 0;
	/*回転用変数*/
	int x , x1 , x2 , x3 , x4;	/*対角のXY座標値*/
	int y , y1 , y2 , y3 , y4;
	int width , height;
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	POINT s_mouse;

	/*ポインタのロック*/
	g_pVtxBuffCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	GetCursorPos( &s_mouse );
	ScreenToClient( GetWnd() , &s_mouse );
	
	width = info.Width * CURSOR_SIZE;
	height = info.Height * CURSOR_SIZE;

	if( GetMousePress( 0 ) == true )
	{
		pVtx[0].col = D3DCOLOR_RGBA( 255 , 12 , 12 , 255 );
		pVtx[1].col = D3DCOLOR_RGBA( 255 , 12 , 12 , 255 );
		pVtx[2].col = D3DCOLOR_RGBA( 255 , 12 , 12 , 255 );
		pVtx[3].col = D3DCOLOR_RGBA( 255 , 12 , 12 , 255 );
	}
	else
	{
		pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	}

	pVtx[0].pos = D3DXVECTOR3( s_mouse.x , s_mouse.y , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( s_mouse.x + width , s_mouse.y , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( s_mouse.x , s_mouse.y + height , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( s_mouse.x + width , s_mouse.y + height , 0.0f );
	
	

	/*ここで変形処理とか*/
	
	/*ポインタのアンロック*/
	g_pVtxBuffCursor->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawCursor( void )
*		カーソル描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawCursor( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureCursorAkr );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffCursor , 0 , sizeof( VERTEX_2D ) );

	/*カーソルの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}