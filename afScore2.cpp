/*******************************************************************************
* タイトル	:スコア.cpp
* ファイル名:afAfScore.cpp
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
#include "AfScore.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"
#include "afResult.h"
/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define AF_SCORE2_MAX				( ULONG_MAX )	//スコア最大値
#define AF_SCORE2_NUMBER_OF_DIGITS	( 10 )	//スコア桁数
#define AF_SCORE2_WIDTH				( 37 )	//スコア数字幅

#define AF_SCORE2_POS_X		( 192 )		//スコア表示位置
#define AF_SCORE2_POS_Y		( 488 )

#define AF_SCORE2_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define AF_SCORE2_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define AF_SCORE2_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define AF_SCORE2_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define AF_SCORE2_ANIM_DIV_Y	( 10 )	/*高さ分割数*/

#define AF_SCORE2_OUTPUT_SIZE	( 0.48 ) /*初期表示倍率*/ 

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffafAfScore = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureafAfScore;	/*スコアのテクスチャのポインタ*/
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

static int g_AfScore2PosX, g_AfScore2PosY;
static unsigned long nowAfScore2 = 0;
static bool g_aAfScore2UseFlag[AF_SCORE2_NUMBER_OF_DIGITS];
/*******************************************************************************
* 関数名	:HRESULT InitAfScore2( void )
*		スコアの初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfScore2( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_SCORE2_VERTEX_MAX * AF_SCORE2_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffafAfScore , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfScore.png" , &g_pTextureafAfScore );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfScore.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffafAfScore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / AF_SCORE2_ANIM_DIV_X ) * AF_SCORE2_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_SCORE2_ANIM_DIV_Y ) * AF_SCORE2_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , AF_SCORE2_NUMBER_OF_DIGITS );

	/*ポインタのアンロック*/
	g_pVtxBuffafAfScore->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//スコア初期化
	nowAfScore2 = 0;
	
	//各桁表示フラグ初期化
	for( int cntAfScore2 = 0; cntAfScore2 < AF_SCORE2_NUMBER_OF_DIGITS; cntAfScore2++ )
	{	
		g_aAfScore2UseFlag[cntAfScore2] = false;
	}

	//スコア基準位置設定
	g_AfScore2PosX = AF_SCORE2_POS_X;
	g_AfScore2PosY = AF_SCORE2_POS_Y;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitAfScore2( void )
*		スコアの終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfScore2( void )
{
	if( g_pVtxBuffafAfScore != NULL )
	{
		g_pVtxBuffafAfScore->Release();
		g_pVtxBuffafAfScore = NULL;
	}
	if( g_pTextureafAfScore != NULL )
	{
		g_pTextureafAfScore->Release();
		g_pTextureafAfScore = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfScore2( void )
*		スコアの更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfScore2( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	int outputAfScore2Num = 0, nCntAfScore2 = AF_SCORE2_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	unsigned long afAfScoreCpy = nowAfScore2;

	/*ポインタのロック*/ 
	g_pVtxBuffafAfScore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	//各桁表示フラグ初期化
	nCntAfScore2 = AF_SCORE2_NUMBER_OF_DIGITS;
	while( nCntAfScore2-- )
	{	
		g_aAfScore2UseFlag[nCntAfScore2] = false;

		/*パターンアニメーション事前計算部分*/
		if( ++g_nCounterAnim %= AF_SCORE2_ANIM_SPAN )
		{
			++g_nPatternAnim %= AF_SCORE2_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == AF_SCORE2_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / AF_SCORE2_ANIM_DIV_X;
		texHeight = 1.0f / AF_SCORE2_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % AF_SCORE2_ANIM_DIV_X );

		afAfScoreCpy = nowAfScore2;
		for( int cnt = 0; cnt < nCntAfScore2; cnt++ )
		{
			afAfScoreCpy /= 10;
		}

		outputAfScore2Num = afAfScoreCpy % 10;
		if( outputAfScore2Num != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_SCORE2_ANIM_DIV_X ) ) + outputAfScore2Num;
		
		if( numPrintFlag == true )
		{
			g_aAfScore2UseFlag[nCntAfScore2] = true;
		}
		
		if( getPlayerAfScoreResult() == nowAfScore2 )
		{
			pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		}
		else
		{
			pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 0 , 182 , 55 );
			pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 0 , 182 , 55 );
			pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 0 , 182 , 55 );
			pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 0 , 182 , 55 );
		}

		/*テクスチャ座標生成*/
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		g_AfScore2PosX += AF_SCORE2_WIDTH;
		/*頂点座標設定*/
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 0].pos.x = g_AfScore2PosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 0].pos.y = g_AfScore2PosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 1].pos.x = g_AfScore2PosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 1].pos.y = g_AfScore2PosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 2].pos.x = g_AfScore2PosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 2].pos.y = g_AfScore2PosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 3].pos.x = g_AfScore2PosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 3].pos.y = g_AfScore2PosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore2 * AF_SCORE2_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_AfScore2PosX = AF_SCORE2_POS_X;
	g_AfScore2PosY = AF_SCORE2_POS_Y;
	g_aAfScore2UseFlag[0] = true;
	
	/*ポインタのアンロック*/
	g_pVtxBuffafAfScore->Unlock();

}

/*******************************************************************************
* 関数名	:void DrawAfScore2( void )
*		スコアの描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfScore2( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffafAfScore , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureafAfScore );
	
	for( int nCntAfScore2 = 0; nCntAfScore2 <= AF_SCORE2_NUMBER_OF_DIGITS; nCntAfScore2++ )
	{
		if( g_aAfScore2UseFlag[nCntAfScore2] == true )
		{
			/*スコアのの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfScore2 * AF_SCORE2_VERTEX_MAX , 2 );
		}
	}
	
}

/*******************************************************************************
* 関数名	:void PlusAfScore2( unsigned long plusNum )
*		スコア加算関数です。
* 引数		:unsigned long plusNum	加算値
* 戻り値	:void 戻り値なし
* 説明		:スコアを加算します。
*******************************************************************************/
void PlusAfScore2( unsigned long plusNum )
{
	
	if( nowAfScore2 + plusNum > AF_SCORE2_MAX )
	{
		nowAfScore2 = AF_SCORE2_MAX;
	}
	else if( nowAfScore2 + plusNum < 0 )
	{
		nowAfScore2 = 0;
	}
	else
	{
		nowAfScore2 += plusNum;
	}
}

/*******************************************************************************
* 関数名	:resetAfScore2( unsigned long resetNum )
*		スコアリセット関数です。
* 引数		:unsigned long resetNum	リセット後の値
* 戻り値	:void 戻り値なし
* 説明		:スコアをリセットします。
*******************************************************************************/
void resetAfScore2( unsigned long resetNum )
{
	if( resetNum >= 0 && nowAfScore2 <= AF_SCORE2_MAX )
	{
		nowAfScore2 = resetNum;
	}
}