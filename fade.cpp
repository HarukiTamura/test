/*******************************************************************************
* タイトル	:フェード.cpp
* ファイル名:Fade.cpp
* 作成者	:田村春樹
* 作成日	:2015/07/08
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "fade.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define FADE_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define FADE_IN_SPEED	( 0.050f )	//フェードイン・アウトの速度
#define FADE_OUT_SPEED	( 0.050f )

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffFade = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureFade;	/*フェードテクスチャのポインタ*/
static D3DXIMAGE_INFO info;		/*画像情報*/

//フェードに関する値
static D3DXCOLOR g_colorFade;	//フェード用のポリゴン色
static FADE g_fade;				//フェードの状態
static MODE g_modeNext;			//フェードアウト後の遷移先
static float g_alphaFade;		//ポリゴンのアルファ値
static float fadeOutR , fadeOutG , fadeOutB;
/*******************************************************************************
* 関数名	:HRESULT InitFade( void )
*		フェード初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitFade( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * FADE_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffFade , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/fade.png" , &g_pTextureFade );
	D3DXGetImageInfoFromFile( "data/TEXTURE/fade.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffFade->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( SCREEN_LEFT , SCREEN_TOP , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( SCREEN_RIGHT , SCREEN_TOP , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( SCREEN_LEFT , SCREEN_BOTTOM , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( SCREEN_RIGHT , SCREEN_BOTTOM , 0.0f );

	/*座標変換用係数設定*/
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;
	
	/*頂点カラー設定*/
	pVtx[0].col = D3DXCOLOR( 0.0f , 0.0f , 0.0f , 1.0f );
	pVtx[1].col = D3DXCOLOR( 0.0f , 0.0f , 0.0f , 1.0f );
	pVtx[2].col = D3DXCOLOR( 0.0f , 0.0f , 0.0f , 1.0f );
	pVtx[3].col = D3DXCOLOR( 0.0f , 0.0f , 0.0f , 1.0f );
	
	/*テクスチャ座標設定*/
	pVtx[0].tex = D3DXVECTOR2( 0 , 0 );
	pVtx[1].tex = D3DXVECTOR2( 1 , 0 );
	pVtx[2].tex = D3DXVECTOR2( 0 , 1 );
	pVtx[3].tex = D3DXVECTOR2( 1 , 1 );

	/*ポインタのアンロック*/
	g_pVtxBuffFade->Unlock();

	//フェード用変数初期化
	g_colorFade = D3DXCOLOR( 1.0f , 1.0f , 1.0f , 0.0f );
	g_fade = FADE_IN;
	g_modeNext = MODE_TITLE;
	g_alphaFade = 1.0f;

	fadeOutR = 0.0f;
	fadeOutG = 0.0f;
	fadeOutB = 0.0f;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitFade( void )
*		フェード終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitFade( void )
{
	if( g_pVtxBuffFade != NULL )
	{
		g_pVtxBuffFade->Release();
		g_pVtxBuffFade = NULL;
	}
	if( g_pTextureFade != NULL )
	{
		g_pTextureFade->Release();
		g_pTextureFade = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateFade( void )
*		フェード更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateFade( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	

	if( g_fade == FADE_IN )
	{
		g_alphaFade -= FADE_IN_SPEED;
		if( g_alphaFade <= 0.0f )
		{
			g_alphaFade = 0.0f;
			g_fade = FADE_NONE;
		}
	}
	else if( g_fade == FADE_OUT )
	{
		g_alphaFade += FADE_OUT_SPEED;
		if( g_alphaFade >= 1.0f )
		{
			g_alphaFade = 1.0f;
			g_fade = FADE_IN;
			SetMode( g_modeNext );
		}
		fadeOutR = 0.0f;
		fadeOutG = 0.0f;
		fadeOutB = 0.0f;
	}
	else if( g_fade == FADE_OUT_WHITE )
	{
		g_alphaFade += FADE_OUT_SPEED;
		if( g_alphaFade >= 1.0f )
		{
			g_alphaFade = 1.0f;
			g_fade = FADE_IN;
			SetMode( g_modeNext );
		}
		fadeOutR = 1.0f;
		fadeOutG = 1.0f;
		fadeOutB = 1.0f;
	}

	/*ポインタのロック*/
	if( g_pVtxBuffFade != NULL )
	{
		g_pVtxBuffFade->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
		pVtx[0].col = D3DXCOLOR( fadeOutR , fadeOutG , fadeOutB , g_alphaFade );
		pVtx[1].col = D3DXCOLOR( fadeOutR , fadeOutG , fadeOutB , g_alphaFade );
		pVtx[2].col = D3DXCOLOR( fadeOutR , fadeOutG , fadeOutB , g_alphaFade );
		pVtx[3].col = D3DXCOLOR( fadeOutR , fadeOutG , fadeOutB , g_alphaFade );

		/*ポインタのアンロック*/
		g_pVtxBuffFade->Unlock();
	}
}

/*******************************************************************************
* 関数名	:void DrawFade( void )
*		フェード描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawFade( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureFade );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffFade , 0 , sizeof( VERTEX_2D ) );

	/*フェードの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}

/*******************************************************************************
* 関数名	:void SetFade( FADE fade , MODE modeNext )
*		フェードの状態を設定します。
* 引数		:FADE fade		フェードの設定です。
*			 MODE modeNext	フェードアウト時の切り替え先です。
* 戻り値	:void 戻り値なし
* 説明		:フェードの状態を設定します。
*******************************************************************************/
void SetFade( FADE fade , MODE modeNext )
{
	g_fade = fade;
	g_modeNext = modeNext;
}


/*******************************************************************************
* 関数名	:FADE GetFade( void )
*		フェードの状態を取得します。
* 引数		:void 引数なし
* 戻り値	:FADE	現在のフェード状態
* 説明		:フェードの状態を取得します。
*******************************************************************************/
FADE GetFade( void )
{
	return g_fade;
}