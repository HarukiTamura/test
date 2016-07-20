/*******************************************************************************
* タイトル	:カーソル.cpp
* ファイル名:AfPauseButton.cpp
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
#include "input.h"
#include "AfPauseButton.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define AF_PAUSEBUTTON_DEFAULT_POSX	( 100 )
#define AF_PAUSEBUTTON_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_PAUSEBUTTON_DEFAULT_VECTOR	( 0.0f )

#define ICON_SPACE	( 100 )
#define AF_PAUSEBUTTON_SPIN	( 0 )
#define USE_ICON	( 3 )	//表示アイコン数

#define AF_PAUSEBUTTON_POLYGON_NUM	( USE_ICON )	/*使用ポリゴン数*/
#define AF_PAUSEBUTTON_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define AF_PAUSEBUTTON_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define AF_PAUSEBUTTON_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define AF_PAUSEBUTTON_ANIM_DIV_X	( 3 )	/*幅分割数*/
#define AF_PAUSEBUTTON_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define AF_PAUSEBUTTON_OUTPUT_SIZE	( 1.0f ) /*初期表示倍率*/ 

#define MOVE_NUM ( 1.3f )	/*移動量*/
#define SPIN_SPEED	( 0.06f )	/*回転スピード*/
#define MAG_SPEED	( 0.10f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.12f )	/*減衰率*/
#define ROT_ATTEN		( 0.85f )	/*減衰率*/
#define SIPIN_ROT_ATTEN ( 0.12f )

/*******************************************************************************
* 構造体定義
*******************************************************************************/


/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfPauseButton = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureAfPauseButton;	/*カーソルテクスチャのポインタ*/
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

static PAUSEBUTTON cursorState;
static int selectCursorNum;

/*******************************************************************************
* 関数名	:HRESULT InitAfPauseButton( void )
*		カーソル初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfPauseButton( void )
{
	HRESULT result;
	result = InitDirectAfPauseButton();
	if( result != S_OK )
	{
		Error("描画基準位置データの初期化に失敗しました。");
	}
	InitValueAfPauseButton();
	return result;
}

/*******************************************************************************
* 関数名	:HRESULT InitDirectAfPauseButton( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitDirectAfPauseButton( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_PAUSEBUTTON_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfPauseButton , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfPauseButton.png" , &g_pTextureAfPauseButton );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfPauseButton.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffAfPauseButton->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_PAUSEBUTTON_POLYGON_NUM );

	/*ポインタのアンロック*/
	g_pVtxBuffAfPauseButton->Unlock();
}

/*******************************************************************************
* 関数名	:HRESULT InitValueAfPauseButton( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void InitValueAfPauseButton( void )
{
	int outputSizeX , outputSizeY;
	
	//事前サイズ計算
	outputSizeX = ( int )( ( info.Width / AF_PAUSEBUTTON_ANIM_DIV_X ) * AF_PAUSEBUTTON_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_PAUSEBUTTON_ANIM_DIV_Y ) * AF_PAUSEBUTTON_OUTPUT_SIZE );

	//事前サイズ変換
	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	
	//初期化
	memset( &cursorState , 0 , sizeof( PAUSEBUTTON ) );

	cursorState.posStatus.posX = AF_PAUSEBUTTON_DEFAULT_POSX;
	cursorState.posStatus.posY = AF_PAUSEBUTTON_DEFAULT_POSY;
	cursorState.posStatus.vector = AF_PAUSEBUTTON_DEFAULT_VECTOR;
	
	cursorState.intPosX = ( int )( cursorState.posStatus.posX ) * 1000;
	cursorState.intPosY = ( int )( cursorState.posStatus.posY ) * 1000;

	cursorState.mode = PAUSEBUTTON_MODE_NORMAL;   
	
	g_posPolygon.x = AF_PAUSEBUTTON_DEFAULT_POSX;
	g_posPolygon.y = AF_PAUSEBUTTON_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*拡大用ステータス*/
	polygonMag = 1.0f;

	/*アニメーション用ステータス*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	
	selectCursorNum = 0;
}

/*******************************************************************************
* 関数名	:void UninitAfPauseButton( void )
*		カーソル終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfPauseButton( void )
{
	if( g_pVtxBuffAfPauseButton != NULL )
	{
		g_pVtxBuffAfPauseButton->Release();
		g_pVtxBuffAfPauseButton = NULL;
	}
	if( g_pTextureAfPauseButton != NULL )
	{
		g_pTextureAfPauseButton->Release();
		g_pTextureAfPauseButton = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfPauseButton( void )
*		カーソル更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfPauseButton( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	int nowPatternNum = 0;

	/*ポインタのロック*/ 
	g_pVtxBuffAfPauseButton->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	if( GetKeyboardPress( DIK_A ) || GetGamePadAnglePress( GP_ANGLE_LEFT ) )
	{	
		selectCursorNum = ( selectCursorNum - 1 < 0 ? 0 : selectCursorNum - 1 );
	}
	else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
	{
		selectCursorNum = ( selectCursorNum + 1 < USE_ICON ? USE_ICON : selectCursorNum + 1 );
	}
	
	g_posPolygon.x = AF_PAUSEBUTTON_DEFAULT_POSX;

	for( int cnt = 0; cnt < USE_ICON; cnt++ )
	{
		g_posPolygon.x += ICON_SPACE;
		if( cursorState.mode == PAUSEBUTTON_MODE_NORMAL )
		{
			polygonMag = 4.0f;

			pVtx[cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		}
		else
		{
			polygonMag = 1.0f;//+= MAG_SPEED;

			pVtx[cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		}

		/*移動量反映*/
		g_posPolygon.x += g_movePolygon.x;
		g_posPolygon.y += g_movePolygon.y;
		g_rotPolygon.z += g_movePolygon.z;

		//オーバー修正
		if( g_posPolygon.x < SCREEN_LEFT )
		{
			g_posPolygon.x = SCREEN_LEFT;
		}					 
		if( g_posPolygon.x > SCREEN_RIGHT )
		{
			g_posPolygon.x = SCREEN_RIGHT;
		}
		if( g_posPolygon.y < FIELD_TOP )
		{
			g_posPolygon.y = FIELD_TOP;
		}
		if( g_posPolygon.y > FIELD_HEIGHT )
		{
			g_posPolygon.y = FIELD_HEIGHT;
		}
	
		if( g_rotPolygon.z > D3DX_PI )
		{
			g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
		}
		if( g_rotPolygon.z < -D3DX_PI )
		{
			g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
		}
	
		//正規値設定
		cursorState.posStatus.DrawPosX = g_posPolygon.x;
		cursorState.posStatus.DrawPosY = g_posPolygon.y;
		//g_rotPolygon.z *= ROT_ATTEN;
		cursorState.posStatus.DrawVector = g_rotPolygon.z * 0.5;
		cursorState.intPosX = ( int )( g_posPolygon.x * 1000 );
		cursorState.intPosY = ( int )( g_posPolygon.y * 1000 );

		/*移動量減衰*/
		g_movePolygon.x += ( DEFAULT_MOVE - g_movePolygon.x ) * MOVE_ATTEN;
		g_movePolygon.y += ( DEFAULT_MOVE - g_movePolygon.y ) * MOVE_ATTEN;	 
		g_movePolygon.z *= SIPIN_ROT_ATTEN;	
	
		/*頂点座標設定*/
		pVtx[cnt].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
		pVtx[cnt + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		pVtx[cnt + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		pVtx[cnt + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		//アニメーション設定
		if( ++g_nCounterAnim == AF_PAUSEBUTTON_ANIM_SPAN )
		{
			g_nCounterAnim = 0;
			g_nPatternAnim = ( ++g_nPatternAnim == AF_PAUSEBUTTON_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
	
		texWidth = 1.0f / AF_PAUSEBUTTON_ANIM_DIV_X;
		texHeight = 1.0f / AF_PAUSEBUTTON_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % AF_PAUSEBUTTON_ANIM_DIV_X );
		nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_PAUSEBUTTON_ANIM_DIV_X ) ) + nowPatternNum;
	
		//テクスチャ座標設定
		pVtx[cnt].tex.x = 0.33f * cnt;
		pVtx[cnt].tex.y = 0.0f;
		pVtx[cnt + 1].tex.x = 0.33f * cnt;
		pVtx[cnt + 1].tex.y = 0.0f;
		pVtx[cnt + 2].tex.x = 0.33f * cnt + 0.33f;
		pVtx[cnt + 2].tex.y = 1.0f;
		pVtx[cnt + 3].tex.x = 0.33f * cnt + 0.33f;
		pVtx[cnt + 3].tex.y = 1.0f;
	}

	/*ポインタのアンロック*/
	g_pVtxBuffAfPauseButton->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawAfPauseButton( void )
*		カーソル描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfPauseButton( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfPauseButton );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfPauseButton , 0 , sizeof( VERTEX_2D ) );

	for( int cnt = 0; cnt < USE_ICON; cnt++ )
	{
		/*カーソルの描画*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , cnt * AF_PAUSEBUTTON_VERTEX_MAX , 2 );
	}
}
