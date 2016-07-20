/*******************************************************************************
* タイトル	:タイトルグラフィック.cpp
* ファイル名:TitleGraphic.cpp
* 作成者	:田村春樹
* 作成日	:2015/05/29
********************************************************************************
* 更新履歴	ver 1.00	昨日としての完成
*			ver 1.01	ズーム時の速度倍率define定義時に動作不良、原因不明。
*******************************************************************************/

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "TitleGraphic.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define TITLE_GRAPHIC_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define TITLE_GRAPHIC_OUTPUT_POS_X	( SCREEN_WIDTH * 0.5 )		/*表示X座標*/
#define TITLE_GRAPHIC_OUTPUT_POS_Y	( SCREEN_HEIGHT * 0.5 )		/*表示Y座標*/

#define ZOOM_CORRECTION		( )　//ズーム時の速度倍率(define定義時に動作不良を確認：原因不明な為後回し)

#define TITLE_GRAPHIC_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define TITLE_GRAPHIC_ANIM_SPAN		( 0 )	/*アニメーション間隔*/
#define TITLE_GRAPHIC_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define TITLE_GRAPHIC_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define TITLE_GRAPHIC_OUTPUT_SIZE	( 1 ) /*初期表示倍率*/ 

#define MOVE_NUM ( 1.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( D3DX_PI / 2 * 0.025f )	/*拡大スピード*/

#define MAG_MAX		( D3DX_PI / 2 * 1.0f )	/*拡大限界*/
#define MAG_MIN 	( -D3DX_PI / 2 * 0.02f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN	( 0.05f )	/*減衰率*/
/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTitleGraphic = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureTitleGraphic;	/*タイトルグラフィックテクスチャのポインタ*/
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

static int outputSizeX , outputSizeY;
static int zoomSwitch, zoomCnt;
static float magNum;

/*******************************************************************************
* 関数名	:HRESULT InitTitleGraphic( void )
*		タイトルグラフィック初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitTitleGraphic( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * TITLE_GRAPHIC_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffTitleGraphic , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/titleBackGround.png" , &g_pTextureTitleGraphic );
	D3DXGetImageInfoFromFile( "data/TEXTURE/titleBackGround.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffTitleGraphic->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / TITLE_GRAPHIC_ANIM_DIV_X ) * TITLE_GRAPHIC_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / TITLE_GRAPHIC_ANIM_DIV_Y ) * TITLE_GRAPHIC_OUTPUT_SIZE );

	g_posPolygon.x = TITLE_GRAPHIC_OUTPUT_POS_X;
	g_posPolygon.y = TITLE_GRAPHIC_OUTPUT_POS_Y;
	g_posPolygon.z = 0.0f;

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( SCREEN_LEFT	, SCREEN_TOP , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( SCREEN_RIGHT	, SCREEN_TOP , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( SCREEN_LEFT	, SCREEN_BOTTOM , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( SCREEN_RIGHT , SCREEN_BOTTOM , 0.0f );

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
	g_pVtxBuffTitleGraphic->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	zoomSwitch = 0;
	zoomCnt = 0;
	magNum = MAG_MAX;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitTitleGraphic( void )
*		タイトルグラフィック終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitTitleGraphic( void )
{
	if( g_pVtxBuffTitleGraphic != NULL )
	{
		g_pVtxBuffTitleGraphic->Release();
		g_pVtxBuffTitleGraphic = NULL;
	}
	if( g_pTextureTitleGraphic != NULL )
	{
		g_pTextureTitleGraphic->Release();
		g_pTextureTitleGraphic = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateTitleGraphic( void )
*		タイトルグラフィック更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateTitleGraphic( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	

	if( ++g_nCounterAnim == TITLE_GRAPHIC_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == TITLE_GRAPHIC_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / TITLE_GRAPHIC_ANIM_DIV_X;
	texHeight = 1.0f / TITLE_GRAPHIC_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % TITLE_GRAPHIC_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / TITLE_GRAPHIC_ANIM_DIV_X ) );
	/*ポインタのロック*/ 
	g_pVtxBuffTitleGraphic->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	/*移動量反映*/
	g_posPolygon.x += g_movePolygon.x;
	g_posPolygon.y += g_movePolygon.y;

	if( g_posPolygon.x < SCREEN_LEFT )
	{
		g_posPolygon.x = SCREEN_LEFT;
	}
	if( g_posPolygon.x > SCREEN_RIGHT )
	{
		g_posPolygon.x = SCREEN_RIGHT;
	}
	if( g_posPolygon.y < SCREEN_TOP )
	{
		g_posPolygon.y = SCREEN_TOP;
	}
	if( g_posPolygon.y > SCREEN_HEIGHT )
	{
		g_posPolygon.y = SCREEN_HEIGHT;
	}
	/*移動量減衰*/
	g_movePolygon.x += ( DEFAULT_MOVE - g_movePolygon.x ) * MOVE_ATTEN;
	g_movePolygon.y += ( DEFAULT_MOVE - g_movePolygon.y ) * MOVE_ATTEN;	 	
		
	/*ポリゴン回転*/
	
	g_rotPolygon.z += SPIN_SPEED;
	
	if( g_rotPolygon.z > D3DX_PI )
	{
		g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
	}
	if( g_rotPolygon.z < -D3DX_PI )
	{
		g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
	}

	if( zoomSwitch == 0 )
	{
		magNum = ( magNum + MAG_SPEED * 0.02 > MAG_MAX ? MAG_MAX : magNum + MAG_SPEED * 0.02);
		if( magNum == MAG_MAX )
		{
			zoomSwitch = 1;
		}
	}
	else
	{
		magNum = ( magNum - MAG_SPEED < MAG_MIN ? MAG_MIN : magNum - MAG_SPEED );
		if( magNum == MAG_MIN )
		{
			zoomSwitch = 0;
		}
	}
	polygonMag = 1.1f + sin( magNum );

	/*ポリゴン拡大*/
	/*if( GetKeyboardPress( DIK_UP ) )
	{
		polygonMag = ( polygonMag + MAG_SPEED > MAG_MAX ? MAG_MAX : polygonMag + MAG_SPEED );
	}
	if( GetKeyboardPress( DIK_DOWN ) )
	{
		polygonMag = ( polygonMag - MAG_SPEED < MAG_MIN ? MAG_MIN : polygonMag - MAG_SPEED );
	}*/
	
	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	*/
	/*ポインタのアンロック*/
	g_pVtxBuffTitleGraphic->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawTitleGraphic( void )
*		タイトルグラフィック描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawTitleGraphic( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureTitleGraphic );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffTitleGraphic , 0 , sizeof( VERTEX_2D ) );

	/*タイトルグラフィックの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}