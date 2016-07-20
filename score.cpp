/*******************************************************************************
* タイトル	:スコア.cpp
* ファイル名:score.cpp
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
#include "score.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define SCORE_MAX				( ULONG_MAX - 1 )	//スコア最大値
#define SCORE_NUMBER_OF_DIGITS	( 10 )	//スコア桁数
#define SCORE_WIDTH				( ( SCREEN_WIDTH - FIELD_RIGHT ) / ( SCORE_NUMBER_OF_DIGITS + 1 ) )	//スコア数字幅

#define SCORE_POS_X		( SCREEN_WIDTH - ( SCORE_WIDTH * SCORE_NUMBER_OF_DIGITS ) - ( SCORE_WIDTH * 0.6 ) )	//スコア表示位置
#define SCORE_POS_Y		( 100 )

#define SCORE_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define SCORE_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define SCORE_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define SCORE_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define SCORE_ANIM_DIV_Y	( 12 )	/*高さ分割数*/

#define SCORE_OUTPUT_SIZE	( 0.26 ) /*初期表示倍率*/ 

#define MOVE_NUM ( 2.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.20f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.05f )	/*減衰率*/


/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffscore = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturescore;	/*スコアのテクスチャのポインタ*/
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

static int g_ScorePosX, g_ScorePosY;
static unsigned long nowScore = 0;
static unsigned long outputScore = 0;
static bool g_aScoreUseFlag[SCORE_NUMBER_OF_DIGITS];
/*******************************************************************************
* 関数名	:HRESULT InitScore( void )
*		スコアの初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitScore( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * SCORE_VERTEX_MAX * SCORE_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffscore , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/timer.png" , &g_pTexturescore );
	D3DXGetImageInfoFromFile( "data/TEXTURE/timer.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffscore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / SCORE_ANIM_DIV_X ) * SCORE_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / SCORE_ANIM_DIV_Y ) * SCORE_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , SCORE_NUMBER_OF_DIGITS );

	/*ポインタのアンロック*/
	g_pVtxBuffscore->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//スコア初期化
	nowScore = 0;
	outputScore = 0;

	//各桁表示フラグ初期化
	for( int cntScore = 0; cntScore < SCORE_NUMBER_OF_DIGITS; cntScore++ )
	{	
		g_aScoreUseFlag[cntScore] = false;
	}

	//スコア基準位置設定
	g_ScorePosX = SCORE_POS_X;
	g_ScorePosY = SCORE_POS_Y;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitScore( void )
*		スコアの終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitScore( void )
{
	if( g_pVtxBuffscore != NULL )
	{
		g_pVtxBuffscore->Release();
		g_pVtxBuffscore = NULL;
	}
	if( g_pTexturescore != NULL )
	{
		g_pTexturescore->Release();
		g_pTexturescore = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateScore( void )
*		スコアの更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateScore( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	int outputScoreNum = 0, nCntScore = SCORE_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	unsigned long scoreCpy = nowScore;

	/*ポインタのロック*/ 
	g_pVtxBuffscore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	if(	outputScore > scoreCpy )
	{
		outputScore -= ( outputScore - scoreCpy ) / 5;
	}
	else if( outputScore < scoreCpy )
	{
		outputScore += ( scoreCpy - outputScore ) / 5;
	}
	else
	{
		outputScore = scoreCpy;
	}

	//各桁表示フラグ初期化
	nCntScore = SCORE_NUMBER_OF_DIGITS;
	while( nCntScore-- )
	{	
		g_aScoreUseFlag[nCntScore] = false;

		/*パターンアニメーション事前計算部分*/
		if( ++g_nCounterAnim %= SCORE_ANIM_SPAN )
		{
			++g_nPatternAnim %= SCORE_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == SCORE_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / SCORE_ANIM_DIV_X;
		texHeight = 1.0f / SCORE_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % SCORE_ANIM_DIV_X );

		scoreCpy = outputScore;
		for( int cnt = 0; cnt < nCntScore; cnt++ )
		{
			scoreCpy /= 10;
		}

		outputScoreNum = scoreCpy % 10;
		if( outputScoreNum != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / SCORE_ANIM_DIV_X ) ) + outputScoreNum;
		
		if( numPrintFlag == true )
		{
			g_aScoreUseFlag[nCntScore] = true;
		}

		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		/*テクスチャ座標生成*/
		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		g_ScorePosX += SCORE_WIDTH;
		/*頂点座標設定*/
		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].pos.x = g_ScorePosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].pos.y = g_ScorePosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].pos.x = g_ScorePosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].pos.y = g_ScorePosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].pos.x = g_ScorePosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].pos.y = g_ScorePosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].pos.x = g_ScorePosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].pos.y = g_ScorePosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_ScorePosX = SCORE_POS_X;
	g_ScorePosY = SCORE_POS_Y;
	g_aScoreUseFlag[0] = true;
	
	/*ポインタのアンロック*/
	g_pVtxBuffscore->Unlock();

}

/*******************************************************************************
* 関数名	:void DrawScore( void )
*		スコアの描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawScore( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffscore , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturescore );
	
	for( int nCntScore = 0; nCntScore < SCORE_NUMBER_OF_DIGITS; nCntScore++ )
	{
		if( g_aScoreUseFlag[nCntScore] == true )
		{
			/*スコアのの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntScore * SCORE_VERTEX_MAX , 2 );
		}
	}
	
}

/*******************************************************************************
* 関数名	:void PlusScore( unsigned long plusNum )
*		スコア加算関数です。
* 引数		:unsigned long plusNum	加算値
* 戻り値	:void 戻り値なし
* 説明		:スコアを加算します。
*******************************************************************************/
void PlusScore( long plusNum )
{
	long long buff,buff2;
	
	buff = ( long long )( nowScore );
	buff2 = ( long long )( plusNum );

	if( buff + buff2 > SCORE_MAX )
	{
		nowScore = SCORE_MAX;
	}	
	else
	{
		if( -buff > buff2 )
		{
			nowScore = 0;
		}
		else
		{
			nowScore += plusNum;
		}
	}
}

/*******************************************************************************
* 関数名	:resetScore( unsigned long resetNum )
*		スコアリセット関数です。
* 引数		:unsigned long resetNum	リセット後の値
* 戻り値	:void 戻り値なし
* 説明		:スコアをリセットします。
*******************************************************************************/
void resetScore( unsigned long resetNum )
{
	if( resetNum >= 0 && nowScore <= SCORE_MAX )
	{
		nowScore = resetNum;
	}
}

/*******************************************************************************
* 関数名	:unsigned long getScore( void )
*		スコアゲット関数です。
* 引数		:void	引数なし
* 戻り値	:int nowScore	現在のスコア
* 説明		:スコアを取得します。
*******************************************************************************/
unsigned long getScore( void )
{
	return nowScore;
}