/*******************************************************************************
* タイトル	:コンボ数.cpp
* ファイル名:comboNumber.cpp
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
#include "comboNumber.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define COMBONUMBER_NUMBER_OF_DIGITS	( 9 )	//コンボ数桁数
#define COMBONUMBER_WIDTH				( 50 )	//コンボ数数字幅


#define COMBONUMBER_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define COMBONUMBER_ANIM_MAX	( 1 )	/*アニメーションコマ数*/
#define COMBONUMBER_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define COMBONUMBER_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define COMBONUMBER_ANIM_DIV_Y	( 10 )	/*高さ分割数*/

#define COMBONUMBER_OUTPUT_SIZE	( 0.38 ) /*初期表示倍率*/ 

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffcomboNumber = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturecomboNumber;	/*コンボ数のテクスチャのポインタ*/
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

static int g_ComboNumberPosX, g_ComboNumberPosY;
static bool g_aComboNumberUseFlag[COMBONUMBER_NUMBER_OF_DIGITS];

static int nowComboNumber = 0;	//現在のコンボ数
static int g_fadeNum;		//フェードアウト状態
/*******************************************************************************
* 関数名	:HRESULT InitComboNumber( void )
*		コンボ数の初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitComboNumber( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * COMBONUMBER_VERTEX_MAX * COMBONUMBER_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffcomboNumber , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/comboNumber.png" , &g_pTexturecomboNumber );
	D3DXGetImageInfoFromFile( "data/TEXTURE/comboNumber.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffcomboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / COMBONUMBER_ANIM_DIV_X ) * COMBONUMBER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / COMBONUMBER_ANIM_DIV_Y ) * COMBONUMBER_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , COMBONUMBER_NUMBER_OF_DIGITS );

	/*ポインタのアンロック*/
	g_pVtxBuffcomboNumber->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	
	//各桁表示フラグ初期化
	for( int cntComboNumber = 0; cntComboNumber < COMBONUMBER_NUMBER_OF_DIGITS; cntComboNumber++ )
	{	
		g_aComboNumberUseFlag[cntComboNumber] = false;
	}

	//コンボ数基準位置設定
	g_ComboNumberPosX = COMBONUMBER_POS_X + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	g_ComboNumberPosY = COMBONUMBER_POS_Y;
	
	//コンボ数初期化
	nowComboNumber = 0;
	g_fadeNum = 0;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitComboNumber( void )
*		コンボ数の終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitComboNumber( void )
{
	if( g_pVtxBuffcomboNumber != NULL )
	{
		g_pVtxBuffcomboNumber->Release();
		g_pVtxBuffcomboNumber = NULL;
	}
	if( g_pTexturecomboNumber != NULL )
	{
		g_pTexturecomboNumber->Release();
		g_pTexturecomboNumber = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateComboNumber( void )
*		コンボ数の更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateComboNumber( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	int comboNumberCpy = nowComboNumber, outputComboNumberNum = 0, nCntComboNumber = COMBONUMBER_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	int posXPlus = 0, posYPlus = 0;
	float fadeDev = 0;
	int colorFade = 255;
	int numDigits = 0;

	//桁数取得と表示位置設定
	while( comboNumberCpy != 0 )
	{
		comboNumberCpy /= 10;
		numDigits++;
	}
	if( numDigits % 2 == 0 )
	{
		g_ComboNumberPosX = COMBONUMBER_POS_X - numDigits / 2 * COMBONUMBER_WIDTH - COMBONUMBER_WIDTH * 0.5 + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	else
	{
		g_ComboNumberPosX = COMBONUMBER_POS_X - ( numDigits - 1 ) / 2 * COMBONUMBER_WIDTH - COMBONUMBER_WIDTH + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	g_ComboNumberPosX -= ( COMBONUMBER_NUMBER_OF_DIGITS - numDigits ) * COMBONUMBER_WIDTH;
	comboNumberCpy = nowComboNumber;

	//段々透明化
	g_fadeNum = ( g_fadeNum > 0 ? g_fadeNum - 1 : 0 );
	if( g_fadeNum == 0 )
	{
		g_fadeNum = 0;
		nowComboNumber = 0;
	}
	fadeDev = ( float )( g_fadeNum ) / ( float )( COMBO_FADE_OUT );	
	fadeDev = sin( D3DX_PI / 2 * fadeDev );

	colorFade *= fadeDev;

	/*ポインタのロック*/ 
	g_pVtxBuffcomboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	//各桁表示フラグ初期化
	nCntComboNumber = COMBONUMBER_NUMBER_OF_DIGITS;
	while( nCntComboNumber-- )
	{	
		g_aComboNumberUseFlag[nCntComboNumber] = false;

		/*パターンアニメーション事前計算部分*/
		if( ++g_nCounterAnim %= COMBONUMBER_ANIM_SPAN )
		{
			++g_nPatternAnim %= COMBONUMBER_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == COMBONUMBER_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / COMBONUMBER_ANIM_DIV_X;
		texHeight = 1.0f / COMBONUMBER_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % COMBONUMBER_ANIM_DIV_X );

		comboNumberCpy = nowComboNumber;
		for( int cnt = 0; cnt < nCntComboNumber; cnt++ )
		{
			comboNumberCpy /= 10;
		}

		outputComboNumberNum = comboNumberCpy % 10;
		if( outputComboNumberNum != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / COMBONUMBER_ANIM_DIV_X ) ) + outputComboNumberNum;
		
		if( numPrintFlag == true )
		{
			g_aComboNumberUseFlag[nCntComboNumber] = true;
		}
		fadeDev = ( float )( g_fadeNum ) / ( float )( COMBO_FADE_OUT );
	//	colorFade *= COLOR_FADE_RATIO;

		polygonMag = 1.0f * fadeDev; 

		g_ComboNumberPosX += COMBONUMBER_WIDTH * polygonMag;
		posXPlus = ( COMBONUMBER_WIDTH * nCntComboNumber - COMBO_OUTPUT_POS_X_ADJUST_PLUS ) * ( 1.0f - polygonMag );
		posYPlus = -SCREEN_HEIGHT * 0.25f * ( 1.0f - polygonMag );

		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );

		/*テクスチャ座標生成*/
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;
		
		/*頂点座標設定*/
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].pos.x = g_ComboNumberPosX + posXPlus + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].pos.y = g_ComboNumberPosY + posYPlus + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].pos.x = g_ComboNumberPosX + posXPlus + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].pos.y = g_ComboNumberPosY + posYPlus + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].pos.x = g_ComboNumberPosX + posXPlus + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].pos.y = g_ComboNumberPosY + posYPlus + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].pos.x = g_ComboNumberPosX + posXPlus + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].pos.y = g_ComboNumberPosY + posYPlus + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_ComboNumberPosX = COMBONUMBER_POS_X + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	g_ComboNumberPosY = COMBONUMBER_POS_Y;
	g_aComboNumberUseFlag[0] = true;
	
	/*ポインタのアンロック*/
	g_pVtxBuffcomboNumber->Unlock();

}

/*******************************************************************************
* 関数名	:void DrawComboNumber( void )
*		コンボ数の描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawComboNumber( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffcomboNumber , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturecomboNumber );
	
	for( int nCntComboNumber = 0; nCntComboNumber < COMBONUMBER_NUMBER_OF_DIGITS; nCntComboNumber++ )
	{
		if( g_aComboNumberUseFlag[nCntComboNumber] == true )
		{
			/*コンボ数のの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntComboNumber * COMBONUMBER_VERTEX_MAX , 2 );
		}
	}
}


/*******************************************************************************
* 関数名	:void DrawComboNumber2( void )
*		コンボ数の加算描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawComboNumber2( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffcomboNumber , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturecomboNumber );

	/*ポインタのロック*/ 
	g_pVtxBuffcomboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	for( int nCntComboNumber = 0; nCntComboNumber < COMBONUMBER_NUMBER_OF_DIGITS; nCntComboNumber++ )
	{
		if( g_aComboNumberUseFlag[nCntComboNumber] == true && nowComboNumber != 0 )
		{
			pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );

			/*コンボ数のの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntComboNumber * COMBONUMBER_VERTEX_MAX , 2 );
		}
	}	
	
	/*ポインタのアンロック*/
	g_pVtxBuffcomboNumber->Unlock();
}

/*******************************************************************************
* 関数名	:void PlusComboNumber( int plusNum )
*		コンボ数加算関数です。
* 引数		:int plusNum	加算値
* 戻り値	:void 戻り値なし
* 説明		:コンボ数を加算します。
*******************************************************************************/
void PlusComboNumber( int plusNum )
{
	g_fadeNum = COMBO_FADE_OUT;
	if( nowComboNumber + plusNum > COMBONUMBER_MAX )
	{
		nowComboNumber = COMBONUMBER_MAX;
	}
	else if( nowComboNumber + plusNum < 0 )
	{
		nowComboNumber = 0;
	}
	else
	{
		nowComboNumber += plusNum;
	}
}

/*******************************************************************************
* 関数名	:int GetComboNumber( void )
*		コンボ数取得関数です。
* 引数		:void 引数無し
* 戻り値	:int g_nowComboNum	現在のコンボ数
* 説明		:コンボ数を加算します。
*******************************************************************************/
int GetComboNumber( void )
{
	return nowComboNumber;
}

/*******************************************************************************
* 関数名	:int GetComboNum( void )
*		コンボ数フェード値取得関数です。
* 引数		:void 引数無し
* 戻り値	:int g_fadeNum	現在のコンボ数フェード値
* 説明		:コンボ数を加算します。
*******************************************************************************/
int GetFadeComboNum( void )
{
	return g_fadeNum;
}

/*******************************************************************************
* 関数名	:resetComboNumber( int resetNum )
*		コンボ数リセット関数です。
* 引数		:int resetNum	リセット後の値
* 戻り値	:void 戻り値なし
* 説明		:コンボ数をリセットします。
*******************************************************************************/
void resetComboNumber( int resetNum )
{
	if( resetNum >= 0 && nowComboNumber <= COMBONUMBER_MAX )
	{
		nowComboNumber = resetNum;
	}
}