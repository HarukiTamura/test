/*******************************************************************************
* タイトル	:コンボ数.cpp
* ファイル名:afComboNumber.cpp
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
#include "afComboNumber.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define AF_COMBONUMBER_NUMBER_OF_DIGITS	( 9 )	//コンボ数桁数
#define AF_COMBONUMBER_WIDTH			( 83 )	//コンボ数数字幅

#define AF_COMBONUMBER_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define AF_COMBONUMBER_SWING	( 3 )

#define AF_COMBONUMBER_ANIM_MAX	( 1 )	/*アニメーションコマ数*/
#define AF_COMBONUMBER_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define AF_COMBONUMBER_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define AF_COMBONUMBER_ANIM_DIV_Y	( 10 )	/*高さ分割数*/

#define AF_COMBONUMBER_OUTPUT_SIZE	( 0.60 ) /*初期表示倍率*/ 

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffafComboNumber = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureafComboNumber;	/*コンボ数のテクスチャのポインタ*/
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

static int g_AfComboNumberPosX, g_AfComboNumberPosY;
static bool g_aAfComboNumberUseFlag[AF_COMBONUMBER_NUMBER_OF_DIGITS];

static int nowAfComboNumber = 0;	//現在のコンボ数
static int g_fadeNum;		//フェードアウト状態
static int g_fadeColor;		//フェードアウト状態
/*******************************************************************************
* 関数名	:HRESULT InitAfComboNumber( void )
*		コンボ数の初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfComboNumber( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_COMBONUMBER_VERTEX_MAX * AF_COMBONUMBER_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffafComboNumber , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfCcomboNumber.png" , &g_pTextureafComboNumber );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfCcomboNumber.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffafComboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / AF_COMBONUMBER_ANIM_DIV_X ) * AF_COMBONUMBER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_COMBONUMBER_ANIM_DIV_Y ) * AF_COMBONUMBER_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , AF_COMBONUMBER_NUMBER_OF_DIGITS );

	/*ポインタのアンロック*/
	g_pVtxBuffafComboNumber->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	
	//各桁表示フラグ初期化
	for( int cntAfComboNumber = 0; cntAfComboNumber < AF_COMBONUMBER_NUMBER_OF_DIGITS; cntAfComboNumber++ )
	{	
		g_aAfComboNumberUseFlag[cntAfComboNumber] = false;
	}

	//コンボ数基準位置設定
	g_AfComboNumberPosX = AF_COMBONUMBER_POS_X + AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	g_AfComboNumberPosY = AF_COMBONUMBER_POS_Y;
	
	//コンボ数初期化
	nowAfComboNumber = 0;
	g_fadeNum = 0;
	g_fadeColor = 0;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitAfComboNumber( void )
*		コンボ数の終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfComboNumber( void )
{
	if( g_pVtxBuffafComboNumber != NULL )
	{
		g_pVtxBuffafComboNumber->Release();
		g_pVtxBuffafComboNumber = NULL;
	}
	if( g_pTextureafComboNumber != NULL )
	{
		g_pTextureafComboNumber->Release();
		g_pTextureafComboNumber = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfComboNumber( void )
*		コンボ数の更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfComboNumber( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	int afComboNumberCpy = nowAfComboNumber, outputAfComboNumberNum = 0, nCntAfComboNumber = AF_COMBONUMBER_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	int posXPlus = 0, posYPlus = 0;
	float fadeDev = 0;
	int colorFade = 255;
	int numDigits = 0;
	int centerNum = 0;
	bool EvenOd = false;	//偶数奇数 true＝偶数　false＝奇数

	//桁数取得と表示位置設定
	/*while( afComboNumberCpy != 0 )
	{
		afComboNumberCpy /= 10;
		numDigits++;
	}
	if( numDigits % 2 == 0 )
	{
		centerNum = numDigits / 2;
		EvenOd = true;
		g_AfComboNumberPosX = AF_COMBONUMBER_POS_X - numDigits / 2 * AF_COMBONUMBER_WIDTH - AF_COMBONUMBER_WIDTH * 0.5 + AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	else
	{
		if( numDigits - 1 != 0 )
		{
			centerNum = ( numDigits - 1 ) / 2;
		}
		else
		{
			centerNum = 0;
		}
		EvenOd = false;
		g_AfComboNumberPosX = AF_COMBONUMBER_POS_X - ( numDigits - 1 ) / 2 * AF_COMBONUMBER_WIDTH - AF_COMBONUMBER_WIDTH + AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	*/
	g_AfComboNumberPosX -= ( AF_COMBONUMBER_NUMBER_OF_DIGITS - numDigits ) * AF_COMBONUMBER_WIDTH;

	afComboNumberCpy = nowAfComboNumber;

	//段々透明化
	g_fadeNum = ( g_fadeNum > 0 ? g_fadeNum - 1 : 0 );
	if( g_fadeNum == 0 )
	{
		g_fadeNum = 0;
		nowAfComboNumber = 0;
	}
	fadeDev = ( float )( g_fadeNum ) / ( float )( AF_COMBO_FADE_OUT );	
	fadeDev = sin( D3DX_PI / 2 * fadeDev );

	colorFade *= fadeDev;

	/*ポインタのロック*/ 
	g_pVtxBuffafComboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	//各桁表示フラグ初期化
	nCntAfComboNumber = AF_COMBONUMBER_NUMBER_OF_DIGITS;
	while( nCntAfComboNumber-- )
	{	
		g_aAfComboNumberUseFlag[nCntAfComboNumber] = false;

		/*パターンアニメーション事前計算部分*/
		if( ++g_nCounterAnim %= AF_COMBONUMBER_ANIM_SPAN )
		{
			++g_nPatternAnim %= AF_COMBONUMBER_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == AF_COMBONUMBER_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / AF_COMBONUMBER_ANIM_DIV_X;
		texHeight = 1.0f / AF_COMBONUMBER_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % AF_COMBONUMBER_ANIM_DIV_X );

		afComboNumberCpy = nowAfComboNumber;
		for( int cnt = 0; cnt < nCntAfComboNumber; cnt++ )
		{
			afComboNumberCpy /= 10;
		}

		outputAfComboNumberNum = afComboNumberCpy % 10;
		if( outputAfComboNumberNum != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_COMBONUMBER_ANIM_DIV_X ) ) + outputAfComboNumberNum;
		
		if( numPrintFlag == true )
		{
			g_aAfComboNumberUseFlag[nCntAfComboNumber] = true;
		}
		fadeDev = ( float )( g_fadeNum ) / ( float )( AF_COMBO_FADE_OUT );
	//	colorFade *= COLOR_FADE_RATIO;

		polygonMag = 1.0f;// * ( fadeDev >= 0.9f ? fadeDev : 0.9f ); 
		colorFade = 255 * fadeDev;
		g_fadeColor = colorFade;
		if( g_aAfComboNumberUseFlag[nCntAfComboNumber] == true )
		{
			if( nCntAfComboNumber < centerNum )
			{
		//		g_AfComboNumberPosX += ( AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS + AF_COMBONUMBER_WIDTH ) * nCntAfComboNumber * ( 1.0f - fadeDev );
			}
			if( nCntAfComboNumber > centerNum )
			{
			//	g_AfComboNumberPosX -= ( AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS + AF_COMBONUMBER_WIDTH ) * nCntAfComboNumber * ( 1.0f - fadeDev );
			}
		}
		g_AfComboNumberPosX += AF_COMBONUMBER_WIDTH;

		
		g_AfComboNumberPosX = g_AfComboNumberPosX + ( rand() % AF_COMBONUMBER_SWING - rand() % AF_COMBONUMBER_SWING );
		g_AfComboNumberPosY = g_AfComboNumberPosY + ( rand() % AF_COMBONUMBER_SWING - rand() % AF_COMBONUMBER_SWING );
	//	posXPlus = ( AF_COMBONUMBER_WIDTH * nCntAfComboNumber - AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS ) * ( 1.0f - polygonMag );
	//	posYPlus = -SCREEN_HEIGHT * 0.25f * ( 1.0f - polygonMag );

		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , colorFade );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , colorFade );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , colorFade );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , colorFade );

		/*テクスチャ座標生成*/
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;
		
		/*頂点座標設定*/
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].pos.x = g_AfComboNumberPosX + posXPlus + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].pos.y = g_AfComboNumberPosY + posYPlus + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].pos.x = g_AfComboNumberPosX + posXPlus + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].pos.y = g_AfComboNumberPosY + posYPlus + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].pos.x = g_AfComboNumberPosX + posXPlus + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].pos.y = g_AfComboNumberPosY + posYPlus + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].pos.x = g_AfComboNumberPosX + posXPlus + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].pos.y = g_AfComboNumberPosY + posYPlus + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_AfComboNumberPosX = AF_COMBONUMBER_POS_X + AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	g_AfComboNumberPosY = AF_COMBONUMBER_POS_Y;
	g_aAfComboNumberUseFlag[0] = true;
	
	/*ポインタのアンロック*/
	g_pVtxBuffafComboNumber->Unlock();

}

/*******************************************************************************
* 関数名	:void DrawAfComboNumber( void )
*		コンボ数の描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfComboNumber( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffafComboNumber , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureafComboNumber );
	
	for( int nCntAfComboNumber = 0; nCntAfComboNumber < AF_COMBONUMBER_NUMBER_OF_DIGITS; nCntAfComboNumber++ )
	{
		if( g_aAfComboNumberUseFlag[nCntAfComboNumber] == true )
		{
			/*コンボ数のの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX , 2 );
		}
	}
}


/*******************************************************************************
* 関数名	:void DrawAfComboNumber2( void )
*		コンボ数の加算描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfComboNumber2( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffafComboNumber , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureafComboNumber );

	/*ポインタのロック*/ 
	g_pVtxBuffafComboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	for( int nCntAfComboNumber = 0; nCntAfComboNumber < AF_COMBONUMBER_NUMBER_OF_DIGITS; nCntAfComboNumber++ )
	{
		if( g_aAfComboNumberUseFlag[nCntAfComboNumber] == true && nowAfComboNumber != 0 )
		{
			pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );

			/*コンボ数のの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX , 2 );
		}
	}	
	
	/*ポインタのアンロック*/
	g_pVtxBuffafComboNumber->Unlock();
}

/*******************************************************************************
* 関数名	:void PlusAfComboNumber( int plusNum )
*		コンボ数加算関数です。
* 引数		:int plusNum	加算値
* 戻り値	:void 戻り値なし
* 説明		:コンボ数を加算します。
*******************************************************************************/
void PlusAfComboNumber( int plusNum )
{
	g_fadeNum = AF_COMBO_FADE_OUT;
	if( nowAfComboNumber + plusNum > AF_COMBONUMBER_MAX )
	{
		nowAfComboNumber = AF_COMBONUMBER_MAX;
	}
	else if( nowAfComboNumber + plusNum < 0 )
	{
		nowAfComboNumber = 0;
	}
	else
	{
		nowAfComboNumber += plusNum;
	}
}

/*******************************************************************************
* 関数名	:int GetAfComboNumber( void )
*		コンボ数取得関数です。
* 引数		:void 引数無し
* 戻り値	:int g_nowAfComboNum	現在のコンボ数
* 説明		:コンボ数を加算します。
*******************************************************************************/
int GetAfComboNumber( void )
{
	return nowAfComboNumber;
}

/*******************************************************************************
* 関数名	:int GetAfComboNum( void )
*		コンボ数フェード値取得関数です。
* 引数		:void 引数無し
* 戻り値	:int g_fadeNum	現在のコンボ数フェード値
* 説明		:コンボ数を加算します。
*******************************************************************************/
int GetFadeAfComboNum( void )
{
	return g_fadeColor;
}

/*******************************************************************************
* 関数名	:resetAfComboNumber( int resetNum )
*		コンボ数リセット関数です。
* 引数		:int resetNum	リセット後の値
* 戻り値	:void 戻り値なし
* 説明		:コンボ数をリセットします。
*******************************************************************************/
void resetAfComboNumber( int resetNum )
{
	if( resetNum >= 0 && nowAfComboNumber <= AF_COMBONUMBER_MAX )
	{
		nowAfComboNumber = resetNum;
	}
}