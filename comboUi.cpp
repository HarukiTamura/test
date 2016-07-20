/*******************************************************************************
* タイトル	:コンボ表示.cpp
* ファイル名:comboUi.cpp
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
#include "comboUi.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"
#include "player01.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define COMBOUI_POS_X		( COMBONUMBER_POS_X )		//コンボ表示表示位置
#define COMBOUI_POS_Y		( COMBONUMBER_POS_Y )
#define COMBOUI_POS_PLUS_X	( -9 )
#define COMBOUI_POS_PLUS_Y	( 12 )

#define COMBOUI_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define COMBOUI_ANIM_MAX	( 1 )	/*アニメーションコマ数*/
#define COMBOUI_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define COMBOUI_ANIM_DIV_X	( 3 )	/*幅分割数*/
#define COMBOUI_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define COMBOUI_OUTPUT_SIZE	( 1.3 ) /*初期表示倍率*/ 

#define MOVE_NUM ( 2.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.20f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.05f )	/*減衰率*/

#define USE_QUOTE	( 3 )

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffcomboUi = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturecomboUi;	/*コンボ表示のテクスチャのポインタ*/
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

static int g_ComboUiPosX, g_ComboUiPosY;

static int g_fadeNum;		//フェードアウト状態
/*******************************************************************************
* 関数名	:HRESULT InitComboUi( void )
*		コンボ表示の初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitComboUi( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * COMBOUI_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffcomboUi , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/comboUi.png" , &g_pTexturecomboUi );
	D3DXGetImageInfoFromFile( "data/TEXTURE/comboUi.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffcomboUi->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / COMBOUI_ANIM_DIV_X ) * COMBOUI_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / COMBOUI_ANIM_DIV_Y ) * COMBOUI_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*ポインタのアンロック*/
	g_pVtxBuffcomboUi->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//コンボ表示基準位置設定
	g_ComboUiPosX = COMBOUI_POS_X;
	g_ComboUiPosY = COMBOUI_POS_Y;
	
	//コンボ表示初期化
	g_fadeNum = 0;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitComboUi( void )
*		コンボ表示の終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitComboUi( void )
{
	if( g_pVtxBuffcomboUi != NULL )
	{
		g_pVtxBuffcomboUi->Release();
		g_pVtxBuffcomboUi = NULL;
	}
	if( g_pTexturecomboUi != NULL )
	{
		g_pTexturecomboUi->Release();
		g_pTexturecomboUi = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateComboUi( void )
*		コンボ表示の更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateComboUi( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/
	int comboUiCpy = GetComboNumber(), outputComboUiNum = 0;
	bool numPrintFlag = false;
	int posXPlus = 0, posYPlus = 0;
	float fadeDev = 0;
	int colorFade = 255;
	int numDigits = 0;

	//桁数取得と表示位置設定
	while( comboUiCpy != 0 )
	{
		comboUiCpy /= 10;
		numDigits++;
	}

	if( numDigits % 2 == 0 )
	{
		g_ComboUiPosX = COMBOUI_POS_X + numDigits / 2 * COMBONUMBER_WIDTH + COMBONUMBER_WIDTH * 0.5 + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	else
	{
		g_ComboUiPosX = COMBOUI_POS_X + ( numDigits - 1 ) / 2 * COMBONUMBER_WIDTH + COMBONUMBER_WIDTH + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	//g_ComboUiPosX -= ( COMBONUMBER_NUMBER_OF_DIGITS - numDigits ) * COMBONUMBER_WIDTH;

	//段々透明化
	g_fadeNum = GetFadeComboNum();
	fadeDev = ( float )( g_fadeNum ) / ( float )( COMBO_FADE_OUT );	
	fadeDev = sin( D3DX_PI / 2 * fadeDev );

	colorFade *= fadeDev;

	/*ポインタのロック*/ 
	g_pVtxBuffcomboUi->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	for( int nCntComboUi = 0; nCntComboUi < USE_QUOTE; nCntComboUi++ )
	{
		/*パターンアニメーション事前計算部分*/
		if( ++g_nCounterAnim %= COMBOUI_ANIM_SPAN )
		{
			++g_nPatternAnim %= COMBOUI_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == COMBOUI_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / COMBOUI_ANIM_DIV_X;
		texHeight = 1.0f / COMBOUI_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % COMBOUI_ANIM_DIV_X ) + nCntComboUi;
		nowPatternY = ( float )( ( int )( g_nPatternAnim / COMBOUI_ANIM_DIV_X ) ) + outputComboUiNum;
		
		if( nCntComboUi == 0 )
		{
			fadeDev = ( float )( g_fadeNum ) / ( float )( COMBO_FADE_OUT );
			colorFade *= COLOR_FADE_RATIO;

			polygonMag = 1.0f * fadeDev;
			//polygonMag = COMBOUI_OUTPUT_SIZE;
			g_ComboUiPosX -= COMBOUI_POS_X * 0.05 * fadeDev + COMBOUI_POS_PLUS_X;
			g_ComboUiPosY += COMBOUI_POS_Y * 0.25 * fadeDev + COMBOUI_POS_PLUS_Y;
 			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			g_ComboUiPosX *= fadeDev;
			posYPlus = -SCREEN_HEIGHT * 0.25f * ( 1.0f - polygonMag );
		}
		else
		{
			fadeDev = ( float )( g_fadeNum ) / ( float )( COMBO_FADE_OUT );
			polygonMag = ( 0.5f + sqrt( ( ( float )( GetComboNumber() ) / ( float )( COMBONUMBER_MAX ) ) ) ) * fadeDev;
			g_ComboUiPosX = GetPlayerState01()->posX;
			g_ComboUiPosY = GetPlayerState01()->posY;
 			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			posYPlus = 0;
		}

		/*テクスチャ座標生成*/
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		/*頂点座標設定*/
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].pos.x = g_ComboUiPosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].pos.y = g_ComboUiPosY + posYPlus + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].pos.x = g_ComboUiPosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].pos.y = g_ComboUiPosY + posYPlus + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].pos.x = g_ComboUiPosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].pos.y = g_ComboUiPosY + posYPlus + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].pos.x = g_ComboUiPosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].pos.y = g_ComboUiPosY + posYPlus + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_ComboUiPosX = COMBOUI_POS_X;
	g_ComboUiPosY = COMBOUI_POS_Y;
	/*ポインタのアンロック*/
	g_pVtxBuffcomboUi->Unlock();

}

/*******************************************************************************
* 関数名	:void DrawComboUi( void )
*		コンボ表示の描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawComboUi( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffcomboUi , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturecomboUi );
	
	for( int nCntComboUi = 0; nCntComboUi < USE_QUOTE; nCntComboUi++ )
	{
		if( GetFadeComboNum() >= 0 && GetComboNumber() != 0 )
		{
			/*コンボ表示のの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntComboUi * COMBOUI_VERTEX_MAX , 2 );
		}
	}
}


/*******************************************************************************
* 関数名	:void DrawComboUi2( void )
*		コンボ表示の加算描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawComboUi2( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffcomboUi , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturecomboUi );
	
	/*ポインタのロック*/ 
	g_pVtxBuffcomboUi->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	for( int nCntComboUi = 0; nCntComboUi < USE_QUOTE; nCntComboUi++ )
	{
		if( GetFadeComboNum() >= 0 && GetComboNumber() != 0 )
		{
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );

			if( nCntComboUi != 0 )
			{
				//減算合成への切り替え
				GetDevice()->SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_REVSUBTRACT );
				GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_ZERO | D3DBLEND_SRCALPHA );
				GetDevice()->SetRenderState( D3DRS_DESTBLEND ,  D3DBLEND_ONE );

				pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 100 );
				pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 100 );
				pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 100 );
				pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 100 );

			}
			/*コンボ表示のの描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntComboUi * COMBOUI_VERTEX_MAX , 2 );
		}
	}	
	
	/*ポインタのアンロック*/
	g_pVtxBuffcomboUi->Unlock();

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRCを半透明合成するよ
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*アルファブレンド有効化*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*スクリーン側ブレンド設定*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*ポリゴン側ブレンド設定*/
}
