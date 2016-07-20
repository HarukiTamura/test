/*******************************************************************************
* リザルト	:スタートカウント.cpp
* ファイル名:StartCounter.cpp
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
#include "StartCounter.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define STARTCOUNTER_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define ONE_COUNT_FPS		( 60 )
#define COUNT_COUNTDOWN_FPS	( ONE_COUNT_FPS * 3 )	/*数字表示FPS*/
#define COUNT_FADEOUT_FPS	( 20 )	/*GO表示残留FPS*/
#define COUNT_START_FPS	( COUNT_COUNTDOWN_FPS + COUNT_FADEOUT_FPS )	/*カウントゼロまでのFPS*/

#define STARTCOUNTER_OUTPUT_POS_X	( SCREEN_WIDTH * 0.5 )		/*表示X座標*/
#define STARTCOUNTER_OUTPUT_POS_Y	( SCREEN_HEIGHT * 0.5 )		/*表示Y座標*/

#define STARTCOUNTER_ANIM_MAX		( 4 )	/*アニメーションコマ数*/
#define STARTCOUNTER_ANIM_SPAN		( 60 )	/*アニメーション間隔*/
#define STARTCOUNTER_ANIM_DIV_X	( 4 )	/*幅分割数*/
#define STARTCOUNTER_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define STARTCOUNTER_OUTPUT_SIZE	( 1 ) /*初期表示倍率*/ 

#define MOVE_NUM ( 1.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.10f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffStartCounter = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureStartCounter;	/*スタートカウントテクスチャのポインタ*/
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

/*******************************************************************************
* 関数名	:HRESULT InitStartCounter( void )
*		スタートカウント初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitStartCounter( void )
{
	g_nCounterAnim = COUNT_START_FPS;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * STARTCOUNTER_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffStartCounter , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/startCounter.png" , &g_pTextureStartCounter );
	D3DXGetImageInfoFromFile( "data/TEXTURE/startCounter.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffStartCounter->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / STARTCOUNTER_ANIM_DIV_X ) * STARTCOUNTER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / STARTCOUNTER_ANIM_DIV_Y ) * STARTCOUNTER_OUTPUT_SIZE );

	g_posPolygon.x = STARTCOUNTER_OUTPUT_POS_X;
	g_posPolygon.y = STARTCOUNTER_OUTPUT_POS_Y;
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
	g_pVtxBuffStartCounter->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = STARTCOUNTER_OUTPUT_SIZE;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitStartCounter( void )
*		スタートカウント終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitStartCounter( void )
{
	if( g_pVtxBuffStartCounter != NULL )
	{
		g_pVtxBuffStartCounter->Release();
		g_pVtxBuffStartCounter = NULL;
	}
	if( g_pTextureStartCounter != NULL )
	{
		g_pTextureStartCounter->Release();
		g_pTextureStartCounter = NULL;
	}
}

/*******************************************************************************
* 関数名	:bool UpdateStartCounter( void )
*		スタートカウント更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:bool true カウント中	false カウントゼロ
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
bool UpdateStartCounter( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	
	if( g_nCounterAnim > COUNT_FADEOUT_FPS )
	{
		int cnt = 1;
		while( g_nCounterAnim > ONE_COUNT_FPS * cnt + COUNT_FADEOUT_FPS )
		{
			cnt++;
		} 
		g_nPatternAnim = cnt;
	}
	else
	{
		g_nPatternAnim = 0;
	}
	
	if( g_nCounterAnim >= 0 )
	{
		g_nCounterAnim--;
	}
	//polygonサイズ変更
	if( ( g_nCounterAnim - COUNT_FADEOUT_FPS ) % ONE_COUNT_FPS == 0 )
	{
		polygonMag = STARTCOUNTER_OUTPUT_SIZE;
	}
	else
	{
		if( ( g_nCounterAnim - COUNT_FADEOUT_FPS ) % ONE_COUNT_FPS > ONE_COUNT_FPS * 0.5 || g_nCounterAnim < COUNT_FADEOUT_FPS )
		{
			polygonMag += 0.0025f;
		}
		else
		{
			polygonMag -= 0.0025f;
		}
	}
	//アニメーション
	texWidth = 1.0f / STARTCOUNTER_ANIM_DIV_X;
	texHeight = 1.0f / STARTCOUNTER_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % STARTCOUNTER_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / STARTCOUNTER_ANIM_DIV_X ) );
	/*ポインタのロック*/ 
	g_pVtxBuffStartCounter->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;

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
	
	if( g_rotPolygon.z > D3DX_PI )
	{
		g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
	}
	if( g_rotPolygon.z < -D3DX_PI )
	{
		g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
	}
	//頂点カラー設定
	if( g_nPatternAnim == 0 )
	{
		int fade = ( int )( 215.0f * ( ( float )( g_nCounterAnim ) / ( float )(  COUNT_FADEOUT_FPS ) ) );
		pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
	}
	else
	{
		pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
		pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
		pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
		pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
	}
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
	g_pVtxBuffStartCounter->Unlock();

	return ( g_nCounterAnim > COUNT_FADEOUT_FPS );
}

/*******************************************************************************
* 関数名	:void DrawStartCounter( void )
*		スタートカウント描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawStartCounter( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	if( g_nCounterAnim >= 0 )
	{
		/*頂点フォーマットの設定*/
		pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
		/*テクスチャの設定*/
		pD3DDevice->SetTexture( 0 , g_pTextureStartCounter );
	
		/*頂点バッファをストリームにバインド*/
		pD3DDevice->SetStreamSource( 0 , g_pVtxBuffStartCounter , 0 , sizeof( VERTEX_2D ) );

		/*スタートカウントの描画*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
	}
}


/*******************************************************************************
* 関数名	:void ResetStartCounter( void )
*		スタートカウントをリセットする関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:スタートカウントをリセットする関数です。
*******************************************************************************/
void ResetStartCounter( void )
{
	g_nCounterAnim = COUNT_START_FPS;
}