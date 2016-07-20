/*******************************************************************************
* タイトル	:タイマー.cpp
* ファイル名:timer.cpp
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
#include "timer.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define TIMER_MAX				( 80000000 )	//タイマー最大値
#define TIMER_WIDTH				( 25 )	//タイマー数字幅
#define TIMER_SPACE_WIDTH		( 25 )	//タイマー数字幅

#define TIMER_POS_X		( FIELD_RIGHT - ( TIMER_WIDTH * 0.60 ) )		//タイマー表示位置
#define TIMER_POS_Y		( 200 )

#define TIMER_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define TIMER_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define TIMER_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define TIMER_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define TIMER_ANIM_DIV_Y	( 12 )	/*高さ分割数*/

#define TIMER_OUTPUT_SIZE	( 0.35 ) /*初期表示倍率*/ 

#define MOVE_NUM ( 2.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.20f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.05f )	/*減衰率*/

#define USE_QUOTE	( 8 )

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBufftimer = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturetimer;	/*タイマーのテクスチャのポインタ*/
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

static int g_TimerPosX, g_TimerPosY;
static int nowTimer = 0;
/*******************************************************************************
* 関数名	:HRESULT InitTimer( void )
*		タイマーの初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitTimer( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * TIMER_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBufftimer , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/timer.png" , &g_pTexturetimer );
	D3DXGetImageInfoFromFile( "data/TEXTURE/timer.png" , &info );

	/*ポインタのロック*/
	g_pVtxBufftimer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / TIMER_ANIM_DIV_X ) * TIMER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / TIMER_ANIM_DIV_Y ) * TIMER_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*ポインタのアンロック*/
	g_pVtxBufftimer->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//タイマー初期化
	nowTimer = 0;

	//タイマー基準位置設定
	g_TimerPosX = TIMER_POS_X;
	g_TimerPosY = TIMER_POS_Y;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitTimer( void )
*		タイマーの終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitTimer( void )
{
	if( g_pVtxBufftimer != NULL )
	{
		g_pVtxBufftimer->Release();
		g_pVtxBufftimer = NULL;
	}
	if( g_pTexturetimer != NULL )
	{
		g_pTexturetimer->Release();
		g_pTexturetimer = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateTimer( void )
*		タイマーの更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateTimer( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	int timerCpy = nowTimer, outputTimerNum = 0,nCntTimer = USE_QUOTE, divideNum = 0;
	int outputNum[USE_QUOTE];
	bool numPrintFlag = false;
	
	//時間計算
	int timers; 
	timers = timerCpy;
	
	
	for( int nCntTimer = 0; nCntTimer < USE_QUOTE; nCntTimer++ )
	{
		outputNum[nCntTimer] = 0;
	}

	
	timers %= 3600000;
	outputNum[7] = timers / 600000;		timers = timers - ( outputNum[7] * 600000 );
	outputNum[6] = timers / 60000;		timers = timers - ( outputNum[6] * 60000 );
	outputNum[5] = 10;
	outputNum[4] = timers / 10000;		timers = timers - ( outputNum[4] * 10000 );
	outputNum[3] = timers / 1000;		timers = timers - ( outputNum[3] * 1000 );
	outputNum[2] = 10;
	outputNum[1] = timers / 100;		timers = timers - ( outputNum[1] * 100 ); 
	outputNum[0] = timers / 10;

	/*ポインタのロック*/ 
	g_pVtxBufftimer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	g_TimerPosX = TIMER_POS_X + TIMER_SPACE_WIDTH * USE_QUOTE;
	g_TimerPosY = TIMER_POS_Y;

	for( int nCntTimer = 0; nCntTimer < USE_QUOTE; nCntTimer++ )
	{	
		/*パターンアニメーション事前計算部分*/
		if( ++g_nCounterAnim %= TIMER_ANIM_SPAN )
		{
			++g_nPatternAnim %= TIMER_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == TIMER_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}

		outputTimerNum = outputNum[nCntTimer];

		texWidth = 1.0f / TIMER_ANIM_DIV_X;
		texHeight = 1.0f / TIMER_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % TIMER_ANIM_DIV_X );

		nowPatternY = ( float )( ( int )( g_nPatternAnim / TIMER_ANIM_DIV_X ) ) + outputTimerNum;

		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		
		if( nCntTimer == 5 || nCntTimer == 2 )
		{
			g_TimerPosX += ( TIMER_SPACE_WIDTH * 0.30 );
		}
		if( nCntTimer == 6 || nCntTimer == 3 )
		{
			g_TimerPosX += ( TIMER_SPACE_WIDTH * 0.22 );
		}

		/*テクスチャ座標生成*/
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		/*頂点座標設定*/
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].pos.x = g_TimerPosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].pos.y = g_TimerPosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].pos.x = g_TimerPosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].pos.y = g_TimerPosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].pos.x = g_TimerPosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].pos.y = g_TimerPosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].pos.x = g_TimerPosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].pos.y = g_TimerPosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].pos.z = 0.0f;

		g_TimerPosX -= TIMER_SPACE_WIDTH;
	}
	
	/*ポインタのアンロック*/
	g_pVtxBufftimer->Unlock();

}

/*******************************************************************************
* 関数名	:void DrawTimer( void )
*		タイマーの描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawTimer( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBufftimer , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturetimer );
	
	for( int nCntTimer = 0; nCntTimer < USE_QUOTE; nCntTimer++ )
	{
		/*タイマーのの描画*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntTimer * TIMER_VERTEX_MAX , 2 );
	}
}

/*******************************************************************************
* 関数名	:void PlusTimer( int plusNum )
*		タイマー加算関数です。
* 引数		:int plusNum	加算値
* 戻り値	:void 戻り値なし
* 説明		:タイマーを加算します。
*******************************************************************************/
void PlusTimer( int plusNum )
{
	
	if( nowTimer + plusNum > TIMER_MAX )
	{
		nowTimer = TIMER_MAX;
	}
	else if( nowTimer + plusNum < 0 )
	{
		nowTimer = 0;
	}
	else
	{
		nowTimer += plusNum;
	}
}

/*******************************************************************************
* 関数名	:resetTimer( int resetNum )
*		タイマーリセット関数です。
* 引数		:int resetNum	リセット後の値
* 戻り値	:void 戻り値なし
* 説明		:タイマーをリセットします。
*******************************************************************************/
void resetTimer( int resetNum )
{
	if( resetNum >= 0 && nowTimer <= TIMER_MAX )
	{
		nowTimer = resetNum;
	}
}

/*******************************************************************************
* 関数名	:int getTimer( void )
*		タイマーゲット関数です。
* 引数		:void 引数なし
* 戻り値	:int nowTimer	現在のミリ秒
* 説明		:タイマーをゲットします。
*******************************************************************************/
int getTimer( void )
{
	return nowTimer;
}
