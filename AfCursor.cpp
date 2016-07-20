/*******************************************************************************
* タイトル	:カーソル.cpp
* ファイル名:AfCursor.cpp
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
#include "AfCursor.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define AF_CURSOR_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_CURSOR_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_CURSOR_DEFAULT_VECTOR	( 0.0f )

#define AF_CURSOR_SPIN	( 1.23 )

#define AF_CURSOR_VERTEX_MAX	( 4 )	/*最高頂点数*/
#define AF_CURSOR_POLYGON_NUM	( 4 )	/*使用ポリゴン数*/

#define AF_CURSOR_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define AF_CURSOR_ANIM_SPAN		( 10 )	/*アニメーション間隔*/
#define AF_CURSOR_ANIM_DIV_X	( 2 )	/*幅分割数*/
#define AF_CURSOR_ANIM_DIV_Y	( 2 )	/*高さ分割数*/

#define AF_CURSOR_OUTPUT_SIZE	( 1.0f ) /*初期表示倍率*/ 

#define MOVE_NUM ( 1.3f )	/*移動量*/
#define SPIN_SPEED	( 0.06f )	/*回転スピード*/
#define MAG_SPEED	( 0.10f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.12f )	/*減衰率*/
#define ROT_ATTEN		( 0.85f )	/*減衰率*/
#define SIPIN_ROT_ATTEN ( 0.12f )

#define DAMAGE_OUTPUT_FPS ( 60 )	/*ダメージ時点滅FPS*/

#define PGPCHANGE_HEIGHT_0	( SCREEN_HEIGHT * 0.5 + 22 )
#define PGPCHANGE_HEIGHT_1	( SCREEN_HEIGHT * 0.5 + 3 )
#define PGPCHANGE_HEIGHT_3	( SCREEN_HEIGHT * 0.5 - 3 )
#define PGPCHANGE_HEIGHT_4	( SCREEN_HEIGHT * 0.5 - 22 )


/*******************************************************************************
* 構造体定義
*******************************************************************************/


/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfCursor = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureAfCursor;	/*カーソルテクスチャのポインタ*/
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

static CURSOR cursorState; 

/*******************************************************************************
* 関数名	:HRESULT InitAfCursor( void )
*		カーソル初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfCursor( void )
{
	HRESULT result;
	result = InitDirectAfCursor();
	if( result != S_OK )
	{
		Error("描画基準位置データの初期化に失敗しました。");
	}
	InitValueAfCursor();
	return result;
}

/*******************************************************************************
* 関数名	:HRESULT InitDirectAfCursor( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitDirectAfCursor( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_CURSOR_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfCursor , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfCursor.png" , &g_pTextureAfCursor );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfCursor.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffAfCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_CURSOR_POLYGON_NUM );

	/*ポインタのアンロック*/
	g_pVtxBuffAfCursor->Unlock();
}

/*******************************************************************************
* 関数名	:HRESULT InitValueAfCursor( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void InitValueAfCursor( void )
{
	int outputSizeX , outputSizeY;
	
	//事前サイズ計算
	outputSizeX = ( int )( ( info.Width / AF_CURSOR_ANIM_DIV_X ) * AF_CURSOR_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_CURSOR_ANIM_DIV_Y ) * AF_CURSOR_OUTPUT_SIZE );

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
	memset( &cursorState , 0 , sizeof( CURSOR ) );

	cursorState.posStatus.posX = AF_CURSOR_DEFAULT_POSX;
	cursorState.posStatus.posY = AF_CURSOR_DEFAULT_POSY;
	cursorState.posStatus.vector = AF_CURSOR_DEFAULT_VECTOR;
	
	cursorState.intPosX = ( int )( cursorState.posStatus.posX ) * 1000;
	cursorState.intPosY = ( int )( cursorState.posStatus.posY ) * 1000;

	cursorState.mode = CURSOR_MODE_NORMAL;   
	
	g_posPolygon.x = AF_CURSOR_DEFAULT_POSX;
	g_posPolygon.y = AF_CURSOR_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*アニメーション用ステータス*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	
}

/*******************************************************************************
* 関数名	:void UninitAfCursor( void )
*		カーソル終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfCursor( void )
{
	if( g_pVtxBuffAfCursor != NULL )
	{
		g_pVtxBuffAfCursor->Release();
		g_pVtxBuffAfCursor = NULL;
	}
	if( g_pTextureAfCursor != NULL )
	{
		g_pTextureAfCursor->Release();
		g_pTextureAfCursor = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfCursor( void )
*		カーソル更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfCursor( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	int nowPatternNum = 0;

	/*ポインタのロック*/ 
	g_pVtxBuffAfCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	pVtx[AF_CURSOR_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_CURSOR_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_CURSOR_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_CURSOR_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	
	if( cursorState.mode == CURSOR_MODE_NORMAL )
	{
		g_movePolygon.z += SPIN_SPEED;
		nowPatternNum = 0;
	}
	else
	{
		nowPatternNum = 1;
	}

	if( GetKeyboardPress( DIK_W ) || GetGamePadAnglePress( GP_ANGLE_UP ) )
	{
		if( GetKeyboardPress( DIK_A ) || GetGamePadAnglePress( GP_ANGLE_LEFT ) )
		{	
			g_movePolygon.x += ( float )( -sinf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
			g_movePolygon.y += ( float )( cosf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
		}
		else if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
		{
		}
		else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{
			g_movePolygon.x += ( float )( sinf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
			g_movePolygon.y += ( float )( cosf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
		}
		else
		{
			g_movePolygon.y -= MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_A ) || GetGamePadAnglePress( GP_ANGLE_LEFT ) )
	{
		if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
		{
			g_movePolygon.x += -sinf( D3DX_PI * 0.25 ) * MOVE_NUM;
			g_movePolygon.y += cosf( D3DX_PI * 0.25 ) * MOVE_NUM;
		}
		else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{

		}
		else
		{	
			g_movePolygon.x -= MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
	{
		if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{	
			g_movePolygon.x += sinf( D3DX_PI * 0.25 ) * MOVE_NUM;
			g_movePolygon.y += cosf( D3DX_PI * 0.25 ) * MOVE_NUM;
		}
		else
		{
			g_movePolygon.y += MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
	{
		g_movePolygon.x += MOVE_NUM;
	}
	else
	{

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
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_CURSOR_VERTEX_MAX + 0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[AF_CURSOR_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_CURSOR_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_CURSOR_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );


	//アニメーション設定
	if( ++g_nCounterAnim == AF_CURSOR_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == AF_CURSOR_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / AF_CURSOR_ANIM_DIV_X;
	texHeight = 1.0f / AF_CURSOR_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % AF_CURSOR_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_CURSOR_ANIM_DIV_X ) ) + nowPatternNum;
	
	//テクスチャ座標設定
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	
	nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_CURSOR_ANIM_DIV_X ) ) + 1;

	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;

	/*ポインタのアンロック*/
	g_pVtxBuffAfCursor->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawAfCursor( void )
*		カーソル描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfCursor( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfCursor );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfCursor , 0 , sizeof( VERTEX_2D ) );

	/*カーソルの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}

/*******************************************************************************
* 関数名	:void DrawAfCursorcore( void )
*		カーソル描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfCursorcore( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfCursor );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfCursor , 0 , sizeof( VERTEX_2D ) );
	
	
	/*カーソルの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_CURSOR_VERTEX_MAX , 2 );

#ifdef _DEBUG
	char txt[60];
	RECT rectW = { ( LONG )( 0 ) , ( LONG )( 100 ) , WINDOW_WIDTH , WINDOW_HEIGHT };
	sprintf( &txt[0] , "PposX:%3.6f \nPposY:%3.6f \nProtV:%3.6f", 
		cursorState.posStatus.posX , cursorState.posStatus.posY , cursorState.posStatus.vector );
	getFont()->DrawText( NULL , &txt[0] , -1 , &rectW , DT_LEFT , D3DCOLOR_RGBA( 255 , 222 , 222 , 255 ) );
#endif
}

/*******************************************************************************
* 関数名	:CURSOR* GetCursorState( void )
*		カーソルのステータスを取得します。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:カーソルのステータスを取得します。。
*******************************************************************************/
CURSOR* GetCursorState( void )
{
	return &cursorState;
}