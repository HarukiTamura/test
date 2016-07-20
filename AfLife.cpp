/*******************************************************************************
* タイトル	:ライフ.cpp
* ファイル名:afLife.cpp
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
#include "afLife.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"
#include "AfPlayer.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define AF_LIFE_MAX				( 100 )	//ライフ最大値
#define AF_LIFE_NUMBER_OF_DIGITS	( 10 )	//ライフ桁数
#define AF_LIFE_WIDTH				( 54 )	//ライフ数字幅
#define POS_X_PLUS		( -2 )

#define AF_LIFE_POS_X		( SCREEN_WIDTH - ( AF_LIFE_WIDTH * AF_LIFE_NUMBER_OF_DIGITS ) - ( AF_LIFE_WIDTH * 0.6 ) - 46 )	//ライフ表示位置
#define AF_LIFE_POS_Y		( 560 )

#define AF_LIFE_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define AF_LIFE_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define AF_LIFE_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define AF_LIFE_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define AF_LIFE_ANIM_DIV_Y	( 12 )	/*高さ分割数*/

#define AF_LIFE_OUTPUT_SIZE	( 0.56 ) /*初期表示倍率*/ 

#define MOVE_NUM ( 2.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.20f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.05f )	/*減衰率*/

#define DEFAULT_LIFE	( 0 )

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffafLife = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureafLife;	/*ライフのテクスチャのポインタ*/
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

static int g_AfLifePosX, g_AfLifePosY;
static unsigned long nowAfLife = 0;
static unsigned long outputAfLife = 0;
static float sucoreChange = 0;
static bool g_aAfLifeUseFlag[AF_LIFE_NUMBER_OF_DIGITS];

static int fade;
/*******************************************************************************
* 関数名	:HRESULT InitAfLife( void )
*		ライフの初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfLife( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_LIFE_VERTEX_MAX * AF_LIFE_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffafLife , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfGameScore.png" , &g_pTextureafLife );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfGameScore.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffafLife->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / AF_LIFE_ANIM_DIV_X ) * AF_LIFE_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_LIFE_ANIM_DIV_Y ) * AF_LIFE_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , AF_LIFE_NUMBER_OF_DIGITS );

	/*ポインタのアンロック*/
	g_pVtxBuffafLife->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//ライフ初期化
	nowAfLife = DEFAULT_LIFE;
	outputAfLife = DEFAULT_LIFE;

	//各桁表示フラグ初期化
	for( int cntAfLife = 0; cntAfLife < AF_LIFE_NUMBER_OF_DIGITS; cntAfLife++ )
	{	
		g_aAfLifeUseFlag[cntAfLife] = false;
	}

	fade = 255;

	//ライフ基準位置設定
	g_AfLifePosX = AF_LIFE_POS_X;
	g_AfLifePosY = AF_LIFE_POS_Y;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitAfLife( void )
*		ライフの終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfLife( void )
{
	if( g_pVtxBuffafLife != NULL )
	{
		g_pVtxBuffafLife->Release();
		g_pVtxBuffafLife = NULL;
	}
	if( g_pTextureafLife != NULL )
	{
		g_pTextureafLife->Release();
		g_pTextureafLife = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfLife( void )
*		ライフの更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfLife( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	int outputAfLifeNum = 0, nCntAfLife = AF_LIFE_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	unsigned long afLifeCpy = nowAfLife;

	sucoreChange = ( sucoreChange + 0.0025f >= 1.0f ? 1.0f : sucoreChange + 0.0025f );

	/*ポインタのロック*/ 
	g_pVtxBuffafLife->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	if(	outputAfLife > afLifeCpy )
	{
		outputAfLife -= ( outputAfLife - afLifeCpy ) * sin( sucoreChange ) - 1;
	}
	else if( outputAfLife < afLifeCpy )
	{
		outputAfLife += ( afLifeCpy - outputAfLife ) * sin( sucoreChange ) + 1;
	}
	else
	{
		outputAfLife = afLifeCpy;
	}

	//各桁表示フラグ初期化
	nCntAfLife = AF_LIFE_NUMBER_OF_DIGITS;
	while( nCntAfLife-- )
	{	
		g_aAfLifeUseFlag[nCntAfLife] = false;

		/*パターンアニメーション事前計算部分*/
		if( ++g_nCounterAnim %= AF_LIFE_ANIM_SPAN )
		{
			++g_nPatternAnim %= AF_LIFE_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == AF_LIFE_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / AF_LIFE_ANIM_DIV_X;
		texHeight = 1.0f / AF_LIFE_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % AF_LIFE_ANIM_DIV_X );

		afLifeCpy = outputAfLife;
		for( int cnt = 0; cnt < nCntAfLife; cnt++ )
		{
			afLifeCpy /= 10;
		}

		outputAfLifeNum = afLifeCpy % 10;
		if( outputAfLifeNum != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_LIFE_ANIM_DIV_X ) ) + outputAfLifeNum;
		
		if( numPrintFlag == true )
		{
			g_aAfLifeUseFlag[nCntAfLife] = true;
		}

		
		if( GetPlayerState()->posStatus.DrawPosY >= AF_PLAYER_DOWN_LIMIT &&
			GetPlayerState()->posStatus.DrawPosX >= AF_PLAYER_RIGHT_LIMIT )
		{
			fade = ( fade - 1 > 0 ? fade - 1 : 0 );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		}
		else
		{
			fade = ( fade + 1 < 255 ? fade + 1 : 255 );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		}

		/*テクスチャ座標生成*/
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		g_AfLifePosX += AF_LIFE_WIDTH;
		if( nCntAfLife <= 1 )
		{
			g_AfLifePosX += POS_X_PLUS * AF_LIFE_NUMBER_OF_DIGITS * nCntAfLife;
		}
		/*頂点座標設定*/
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].pos.x = g_AfLifePosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].pos.y = g_AfLifePosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].pos.x = g_AfLifePosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].pos.y = g_AfLifePosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].pos.x = g_AfLifePosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].pos.y = g_AfLifePosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].pos.x = g_AfLifePosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].pos.y = g_AfLifePosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_AfLifePosX = AF_LIFE_POS_X;
	g_AfLifePosY = AF_LIFE_POS_Y;
	g_aAfLifeUseFlag[0] = true;
	
	/*ポインタのアンロック*/
	g_pVtxBuffafLife->Unlock();

}

/*******************************************************************************
* 関数名	:void DrawAfLife( void )
*		ライフの描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfLife( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffafLife , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureafLife );
	
	for( int nCntAfLife = 0; nCntAfLife < AF_LIFE_NUMBER_OF_DIGITS; nCntAfLife++ )
	{
		if( g_aAfLifeUseFlag[nCntAfLife] == true )
		{
			/*ライフのの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfLife * AF_LIFE_VERTEX_MAX , 2 );
		}
	}
	
}

/*******************************************************************************
* 関数名	:void PlusAfLife( unsigned long plusNum )
*		ライフ加算関数です。
* 引数		:unsigned long plusNum	加算値
* 戻り値	:void 戻り値なし
* 説明		:ライフを加算します。
*******************************************************************************/
void PlusAfLife( long plusNum )
{
	long long buff,buff2;
	
	buff = ( long long )( nowAfLife );
	buff2 = ( long long )( plusNum );
	sucoreChange = 0.0f;

	if( buff + buff2 > AF_LIFE_MAX )
	{
		nowAfLife = AF_LIFE_MAX;
	}	
	else
	{
		if( -buff > buff2 )
		{
			nowAfLife = 0;
		}
		else
		{
			nowAfLife += plusNum;
		}
	}
}

/*******************************************************************************
* 関数名	:resetAfLife( unsigned long resetNum )
*		ライフリセット関数です。
* 引数		:unsigned long resetNum	リセット後の値
* 戻り値	:void 戻り値なし
* 説明		:ライフをリセットします。
*******************************************************************************/
void resetAfLife( unsigned long resetNum )
{
	if( resetNum >= 0 && nowAfLife <= AF_LIFE_MAX )
	{
		nowAfLife = resetNum;
	}
}

/*******************************************************************************
* 関数名	:unsigned long getAfLife( void )
*		ライフゲット関数です。
* 引数		:void	引数なし
* 戻り値	:int nowAfLife	現在のライフ
* 説明		:ライフを取得します。
*******************************************************************************/
unsigned long getAfLife( void )
{
	return nowAfLife;
}