/*******************************************************************************
* タイトル	:バリアー.cpp
* ファイル名:barrier.cpp
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
#include "barrier.h"
#include "player01.h"
#include "bullet.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define BARRIER_VERTEX_MAX	( 4 )	/*最高頂点数*/
#define BARRIER_POLYGON_NUM	( 2 )	/*自機＋判定*/

#define BARRIER_ANIM_MAX	( 5 )	/*アニメーションコマ数*/
#define BARRIER_ANIM_SPAN	( 5 )	/*アニメーション間隔*/
#define BARRIER_ANIM_DIV_X	( 5 )	/*幅分割数*/
#define BARRIER_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define BARRIER_OUTPUT_SIZE	( 0.375 ) /*限界表示倍率*/ 
#define BARRIER_RADIUS	( 112 )		/*限界拡大時半径*/

#define MOVE_NUM ( 0.4f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.10f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN	( 0.23f )	/*減衰率*/
/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffbarrier = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturebarrier;	/*バリアーテクスチャのポインタ*/
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

float nowRadius = 0, radiusAccel = 0;
float nowRadiusInt = 0;
bool pastBarrier = false;	//バリア終了トリガー用

/*******************************************************************************
* 関数名	:HRESULT InitBarrier( void )
*		バリアー初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitBarrier( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * BARRIER_VERTEX_MAX * BARRIER_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffbarrier , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/barrier.png" , &g_pTexturebarrier );
	D3DXGetImageInfoFromFile( "data/TEXTURE/barrier.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffbarrier->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , BARRIER_POLYGON_NUM );

	/*ポインタのアンロック*/
	g_pVtxBuffbarrier->Unlock();
	
	outputSizeX = ( int )( ( info.Width / BARRIER_ANIM_DIV_X ) * BARRIER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / BARRIER_ANIM_DIV_Y ) * BARRIER_OUTPUT_SIZE );

	g_posPolygon.x = 0;
	g_posPolygon.y = 0;
	g_posPolygon.z = 0.0f;

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );


	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	nowRadius = 0;
	radiusAccel = 0;
	pastBarrier = false;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitBarrier( void )
*		バリアー終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitBarrier( void )
{
	if( g_pVtxBuffbarrier != NULL )
	{
		g_pVtxBuffbarrier->Release();
		g_pVtxBuffbarrier = NULL;
	}
	if( g_pTexturebarrier != NULL )
	{
		g_pTexturebarrier->Release();
		g_pTexturebarrier = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateBarrier( void )
*		バリアー更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateBarrier( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	float rebirthRotate = 0;

	//バリアが有効な場合のみバリア拡大：そうでなければ縮小
	if( GetPlayerState01()->liarBarrier == true )
	{
		radiusAccel += 0.21f; // 10.5f;
	}
	else
	{
		radiusAccel -= 0.21f;
	}
	if( GetPlayerState01()->liarBarrier == false && pastBarrier == true )
	{
		radiusAccel = 0;
	}
	
	pastBarrier = GetPlayerState01()->liarBarrier;
	nowRadius += radiusAccel;
	if( nowRadius <= 0 )
	{
		nowRadius = 0;
		radiusAccel = 0;
	}
	if( nowRadius >= BARRIER_RADIUS )
	{
		nowRadius = BARRIER_RADIUS;
	}
	nowRadiusInt = ( int )( nowRadius * 1000 );
	polygonMag = nowRadius / BARRIER_RADIUS;

	if( ++g_nCounterAnim == BARRIER_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == BARRIER_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	

	texWidth = 1.0f / BARRIER_ANIM_DIV_X;
	texHeight = 1.0f / BARRIER_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % BARRIER_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / BARRIER_ANIM_DIV_X ) );

	/*ポインタのロック*/ 
	g_pVtxBuffbarrier->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );


	pVtx[BARRIER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[BARRIER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[BARRIER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[BARRIER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
						
	//テクスチャ座標設定
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;

	pVtx[BARRIER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
	pVtx[BARRIER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
	pVtx[BARRIER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[BARRIER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
	pVtx[BARRIER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
	pVtx[BARRIER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[BARRIER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[BARRIER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

	//バリアは自機に追従する。
	g_movePolygon.x = GetPlayerState01()->posX;
	g_movePolygon.x = GetPlayerState01()->posY;

	/*移動量反映*/
	g_posPolygon.x = GetPlayerState01()->posX;
	g_posPolygon.y = GetPlayerState01()->posY;

	if( g_posPolygon.x < FIELD_LEFT )
	{
		g_posPolygon.x = FIELD_LEFT;
	}
	if( g_posPolygon.x > FIELD_RIGHT )
	{
		g_posPolygon.x = FIELD_RIGHT;
	}
	if( g_posPolygon.y < FIELD_TOP )
	{
		g_posPolygon.y = FIELD_TOP;
	}
	if( g_posPolygon.y > FIELD_HEIGHT )
	{
		g_posPolygon.y = FIELD_HEIGHT;
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
	
	rebirthRotate = D3DX_PI - g_rotPolygon.z;

	pVtx[BARRIER_VERTEX_MAX + 0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[BARRIER_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[BARRIER_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[BARRIER_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	*/
	/*ポインタのアンロック*/
	g_pVtxBuffbarrier->Unlock();

}

/*******************************************************************************
* 関数名	:void DrawBarrier( void )
*		バリアー描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawBarrier( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturebarrier );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffbarrier , 0 , sizeof( VERTEX_2D ) );

	/*バリアーの描画*/
	
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + BARRIER_VERTEX_MAX , 2 );


}

/*******************************************************************************
* 関数名	:int GetBarrierRadiusInt( void )
*		バリアー半径を渡す処理関数です。
* 引数		:void 引数なし
* 戻り値	:int　バリア半径int変換値
* 説明		:バリアーの半径を渡す関数です。
*******************************************************************************/
int GetBarrierRadiusInt( void )
{
	return nowRadiusInt;
}