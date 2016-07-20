/*******************************************************************************
* タイトル	:スコア.cpp
* ファイル名:afScoreMy.cpp
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
#include "afScoreMy.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define AF_SCORE_MY_MAX				( ULONG_MAX - 1 )	//スコア最大値
#define AF_SCORE_MY_NUMBER_OF_DIGITS	( 10 )	//スコア桁数
#define AF_SCORE_MY_WIDTH				( 50 )	//スコア数字幅

#define AF_SCORE_MY_POS_X		( 80 )	//スコア表示位置
#define AF_SCORE_MY_POS_Y		( 218 )

#define AF_SCORE_MY_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define AF_SCORE_MY_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define AF_SCORE_MY_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define AF_SCORE_MY_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define AF_SCORE_MY_ANIM_DIV_Y	( 10 )	/*高さ分割数*/

#define AF_SCORE_MY_OUTPUT_SIZE	( 0.65 ) /*初期表示倍率*/ 

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffafScoreMy = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureafScoreMy;	/*スコアのテクスチャのポインタ*/
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

static int g_AfScoreMyPosX, g_AfScoreMyPosY;
static unsigned long nowAfScoreMy = 0;
static unsigned long outputAfScoreMy = 0;
static float sucoreChange = 0;
static bool g_aAfScoreMyUseFlag[AF_SCORE_MY_NUMBER_OF_DIGITS];
/*******************************************************************************
* 関数名	:HRESULT InitAfScoreMy( void )
*		スコアの初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfScoreMy( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_SCORE_MY_VERTEX_MAX * AF_SCORE_MY_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffafScoreMy , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfScore.png" , &g_pTextureafScoreMy );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfScore.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffafScoreMy->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / AF_SCORE_MY_ANIM_DIV_X ) * AF_SCORE_MY_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_SCORE_MY_ANIM_DIV_Y ) * AF_SCORE_MY_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , AF_SCORE_MY_NUMBER_OF_DIGITS );

	/*ポインタのアンロック*/
	g_pVtxBuffafScoreMy->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//スコア初期化
	nowAfScoreMy = 0;
	outputAfScoreMy = 0;

	//各桁表示フラグ初期化
	for( int cntAfScoreMy = 0; cntAfScoreMy < AF_SCORE_MY_NUMBER_OF_DIGITS; cntAfScoreMy++ )
	{	
		g_aAfScoreMyUseFlag[cntAfScoreMy] = false;
	}

	//スコア基準位置設定
	g_AfScoreMyPosX = AF_SCORE_MY_POS_X;
	g_AfScoreMyPosY = AF_SCORE_MY_POS_Y;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitAfScoreMy( void )
*		スコアの終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfScoreMy( void )
{
	if( g_pVtxBuffafScoreMy != NULL )
	{
		g_pVtxBuffafScoreMy->Release();
		g_pVtxBuffafScoreMy = NULL;
	}
	if( g_pTextureafScoreMy != NULL )
	{
		g_pTextureafScoreMy->Release();
		g_pTextureafScoreMy = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfScoreMy( void )
*		スコアの更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfScoreMy( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	int outputAfScoreMyNum = 0, nCntAfScoreMy = AF_SCORE_MY_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	unsigned long afScoreMyCpy = nowAfScoreMy;

	if( sucoreChange != 1.0f )
	{
		sucoreChange = ( sucoreChange + 0.0025f >= 1.0f ? 1.0f : sucoreChange + 0.0025f );
	}

	/*ポインタのロック*/ 
	g_pVtxBuffafScoreMy->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	if(	outputAfScoreMy > afScoreMyCpy )
	{
		outputAfScoreMy -= ( outputAfScoreMy - afScoreMyCpy ) * sin( sucoreChange ) - 1;
	}
	else if( outputAfScoreMy < afScoreMyCpy )
	{
		outputAfScoreMy += ( afScoreMyCpy - outputAfScoreMy ) * sin( sucoreChange ) + 1;
	}
	else
	{
		outputAfScoreMy = afScoreMyCpy;
	}

	//各桁表示フラグ初期化
	nCntAfScoreMy = AF_SCORE_MY_NUMBER_OF_DIGITS;
	while( nCntAfScoreMy-- )
	{	
		g_aAfScoreMyUseFlag[nCntAfScoreMy] = false;

		/*パターンアニメーション事前計算部分*/
		if( ++g_nCounterAnim %= AF_SCORE_MY_ANIM_SPAN )
		{
			++g_nPatternAnim %= AF_SCORE_MY_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == AF_SCORE_MY_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / AF_SCORE_MY_ANIM_DIV_X;
		texHeight = 1.0f / AF_SCORE_MY_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % AF_SCORE_MY_ANIM_DIV_X );

		afScoreMyCpy = outputAfScoreMy;
		for( int cnt = 0; cnt < nCntAfScoreMy; cnt++ )
		{
			afScoreMyCpy /= 10;
		}

		outputAfScoreMyNum = afScoreMyCpy % 10;
		if( outputAfScoreMyNum != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_SCORE_MY_ANIM_DIV_X ) ) + outputAfScoreMyNum;
		
		if( numPrintFlag == true )
		{
			g_aAfScoreMyUseFlag[nCntAfScoreMy] = true;
		}

		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		/*テクスチャ座標生成*/
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		g_AfScoreMyPosX += AF_SCORE_MY_WIDTH;
		/*頂点座標設定*/
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].pos.x = g_AfScoreMyPosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].pos.y = g_AfScoreMyPosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].pos.x = g_AfScoreMyPosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].pos.y = g_AfScoreMyPosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].pos.x = g_AfScoreMyPosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].pos.y = g_AfScoreMyPosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].pos.x = g_AfScoreMyPosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].pos.y = g_AfScoreMyPosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_AfScoreMyPosX = AF_SCORE_MY_POS_X;
	g_AfScoreMyPosY = AF_SCORE_MY_POS_Y;
	g_aAfScoreMyUseFlag[0] = true;
	
	/*ポインタのアンロック*/
	g_pVtxBuffafScoreMy->Unlock();

}

/*******************************************************************************
* 関数名	:void DrawAfScoreMy( void )
*		スコアの描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfScoreMy( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffafScoreMy , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureafScoreMy );
	
	for( int nCntAfScoreMy = 0; nCntAfScoreMy < AF_SCORE_MY_NUMBER_OF_DIGITS; nCntAfScoreMy++ )
	{
		if( g_aAfScoreMyUseFlag[nCntAfScoreMy] == true )
		{
			/*スコアのの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX , 2 );
		}
	}
	
}

/*******************************************************************************
* 関数名	:void PlusAfScoreMy( unsigned long plusNum )
*		スコア加算関数です。
* 引数		:unsigned long plusNum	加算値
* 戻り値	:void 戻り値なし
* 説明		:スコアを加算します。
*******************************************************************************/
void PlusAfScoreMy( long plusNum )
{
	long long buff,buff2;
	
	buff = ( long long )( nowAfScoreMy );
	buff2 = ( long long )( plusNum );
	sucoreChange = 0.0f;

	if( buff + buff2 > AF_SCORE_MY_MAX )
	{
		nowAfScoreMy = AF_SCORE_MY_MAX;
	}	
	else
	{
		if( -buff > buff2 )
		{
			nowAfScoreMy = 0;
		}
		else
		{
			nowAfScoreMy += plusNum;
		}
	}
}

/*******************************************************************************
* 関数名	:resetAfScoreMy( unsigned long resetNum )
*		スコアリセット関数です。
* 引数		:unsigned long resetNum	リセット後の値
* 戻り値	:void 戻り値なし
* 説明		:スコアをリセットします。
*******************************************************************************/
void resetAfScoreMy( unsigned long resetNum )
{
	if( resetNum >= 0 && nowAfScoreMy <= AF_SCORE_MY_MAX )
	{
		nowAfScoreMy = resetNum;
	}
}

/*******************************************************************************
* 関数名	:unsigned long getAfScoreMy( void )
*		スコアゲット関数です。
* 引数		:void	引数なし
* 戻り値	:int nowAfScoreMy	現在のスコア
* 説明		:スコアを取得します。
*******************************************************************************/
unsigned long getAfScoreMy( void )
{
	return nowAfScoreMy;
}