/*******************************************************************************
* タイトル	:プレイヤー.cpp
* ファイル名:AfPlayer.cpp
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
#include "AfPlayer.h"
#include "AfCursor.h"
#include "AfBulletPlayer.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define AF_PLAYER_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_PLAYER_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_PLAYER_DEFAULT_VECTOR	( 0.0f )
#define AF_PLAYER_DEFAULT_POSZ	( 1.0f )
#define AF_PLAYER_DEFAULT_LIFE		( 100 )


#define AF_PLAYER_SPIN	( 1.23 )

#define AF_PLAYER_VERTEX_MAX		( 4 )	/*最高頂点数*/
#define AF_PLAYER_POLYGON_NUM	( 2 )	/*自機＋判定*/

#define AF_PLAYER_ANIM_MAX		( 2 )	/*アニメーションコマ数*/
#define AF_PLAYER_ANIM_SPAN	( 1 )	/*アニメーション間隔*/
#define AF_PLAYER_ANIM_DIV_X	( 2 )	/*幅分割数*/
#define AF_PLAYER_ANIM_DIV_Y	( 5 )	/*高さ分割数*/

#define AF_PLAYER_OUTPUT_SIZE	( 1.0f ) /*初期表示倍率*/ 

#define MOVE_NUM ( 0.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.10f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define SPEED_MAX	( 0.100f )	/*拡大限界*/
#define SPEED_MIN 	( 0.001f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.23f )	/*減衰率*/
#define ROT_ATTEN		( 0.85f )	/*減衰率*/
#define SIPIN_ROT_ATTEN ( 0.12f )

#define DAMAGE_OUTPUT_FPS ( 60 )	/*ダメージ時点滅FPS*/

#define PGPCHANGE_HEIGHT_0	( SCREEN_HEIGHT * 0.5 + 22 )
#define PGPCHANGE_HEIGHT_1	( SCREEN_HEIGHT * 0.5 + 3 )
#define PGPCHANGE_HEIGHT_3	( SCREEN_HEIGHT * 0.5 - 3 )
#define PGPCHANGE_HEIGHT_4	( SCREEN_HEIGHT * 0.5 - 22 )

#define MOVE_PLUS_SPEED		( 5.4f )
#define MOVE_MINUS_SPEED	( -5.4f )
#define MOVE_PLUS_MAX	( 122.0f )
#define MOVE_PLUS_MIN	( -122.0f )

typedef enum
{
	PGP_H_PULL_UP = 0,
	PGP_L_PULL_UP,
	PGP_NEUTRAL,
	PGP_L_PULL_DOWN,
	PGP_H_PULL_DOWN
}PLAYER_GRAPHIC_PATTERN;

/*******************************************************************************
* 構造体定義
*******************************************************************************/


/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfPlayer = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureAfPlayer;	/*プレイヤーテクスチャのポインタ*/
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

static PLAYER playerState; 
static float g_BarrelRoll;
static float movePlus = 0;
/*******************************************************************************
* 関数名	:HRESULT InitAfPlayer( void )
*		プレイヤー初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfPlayer( void )
{
	HRESULT result;
	result = InitDirectAfPlayer();
	if( result != S_OK )
	{
		Error("描画基準位置データの初期化に失敗しました。");
	}
	InitValueAfPlayer();
	return result;
}

/*******************************************************************************
* 関数名	:HRESULT InitDirectAfPlayer( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitDirectAfPlayer( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_PLAYER_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfPlayer , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfPlayer.png" , &g_pTextureAfPlayer );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfPlayer.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffAfPlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_PLAYER_POLYGON_NUM );

	/*ポインタのアンロック*/
	g_pVtxBuffAfPlayer->Unlock();
}

/*******************************************************************************
* 関数名	:HRESULT InitValueAfPlayer( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void InitValueAfPlayer( void )
{
	int outputSizeX , outputSizeY;
	
	//事前サイズ計算
	outputSizeX = ( int )( ( info.Width / AF_PLAYER_ANIM_DIV_X ) * AF_PLAYER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_PLAYER_ANIM_DIV_Y ) * AF_PLAYER_OUTPUT_SIZE );

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
	memset( &playerState , 0 , sizeof( PLAYER ) );

	playerState.posStatus.posX = AF_PLAYER_DEFAULT_POSX;
	playerState.posStatus.posY = AF_PLAYER_DEFAULT_POSY;
	playerState.posStatus.vector = AF_PLAYER_DEFAULT_VECTOR;
	
	g_BarrelRoll = 0.0f;
	

	playerState.intPosX = ( int )( playerState.posStatus.posX ) * 1000;
	playerState.intPosY = ( int )( playerState.posStatus.posY ) * 1000;

	playerState.nLife = AF_PLAYER_DEFAULT_LIFE;
	playerState.nCombo = 0;
	playerState.mode = PLAYER_MODE_NORMAL;   
	playerState.speed = SPEED_MIN;  

	g_posPolygon.x = AF_PLAYER_DEFAULT_POSX;
	g_posPolygon.y = AF_PLAYER_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*アニメーション用ステータス*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	
}

/*******************************************************************************
* 関数名	:void UninitAfPlayer( void )
*		プレイヤー終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfPlayer( void )
{
	if( g_pVtxBuffAfPlayer != NULL )
	{
		g_pVtxBuffAfPlayer->Release();
		g_pVtxBuffAfPlayer = NULL;
	}
	if( g_pTextureAfPlayer != NULL )
	{
		g_pTextureAfPlayer->Release();
		g_pTextureAfPlayer = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfPlayer( void )
*		プレイヤー更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfPlayer( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	PLAYER_GRAPHIC_PATTERN nowPatternNum = PGP_NEUTRAL;

	/*ポインタのロック*/ 
	g_pVtxBuffAfPlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	pVtx[AF_PLAYER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	if( GetKeyboardPress( DIK_W ) || GetGamePadAnglePress( GP_ANGLE_UP ) )
	{
		if( GetKeyboardPress( DIK_A ) || GetGamePadAnglePress( GP_ANGLE_LEFT ) )
		{	
			g_movePolygon.z += SPIN_SPEED;
		}
		else if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
		{
		}
		else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{
			g_movePolygon.z -= SPIN_SPEED;
		}
		else
		{
		}
	}
	else if( GetKeyboardPress( DIK_A ) || GetGamePadAnglePress( GP_ANGLE_LEFT ) )
	{
		if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
		{
			g_movePolygon.z += SPIN_SPEED;
		}
		else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{

		}
		else
		{	
			g_movePolygon.z += SPIN_SPEED;
		}
	}
	else if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
	{
		if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{	
			g_movePolygon.z -= SPIN_SPEED;
		}
		else
		{
		}
	}
	else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
	{
		g_movePolygon.z -= SPIN_SPEED;
	}
	else
	{

	}
	
	/*
	if( GetGamePadPress( 4 ) && !GetGamePadPress( 5 ) )
	{
		movePlus = ( movePlus + MOVE_MINUS_SPEED <= MOVE_PLUS_MIN ? MOVE_PLUS_MIN : movePlus + MOVE_MINUS_SPEED );
	}
	if( GetGamePadPress( 5 ) && !GetGamePadPress( 4 ) )
	{
		movePlus = ( movePlus + MOVE_PLUS_SPEED	>= MOVE_PLUS_MAX ? MOVE_PLUS_MAX : movePlus + MOVE_PLUS_SPEED );
	}
	else
	{
		movePlus = 0.0f;
	}
	g_movePolygon.x += movePlus;
	*/

	/*移動量反映*/
	g_movePolygon.x = GetCursorState()->posStatus.DrawPosX - playerState.posStatus.DrawPosX;
	g_movePolygon.y = GetCursorState()->posStatus.DrawPosY - playerState.posStatus.DrawPosY;

	g_BarrelRoll = 0;

	g_posPolygon.x += g_movePolygon.x * MOVE_ATTEN;
	g_posPolygon.y += g_movePolygon.y * MOVE_ATTEN;
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
	
	if(	playerState.speed < SPEED_MIN )
	{
		playerState.speed = SPEED_MIN;  
	}
	if(	playerState.speed > SPEED_MAX )
	{
		playerState.speed = SPEED_MAX;  
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
	playerState.posStatus.DrawPosX = g_posPolygon.x;
	playerState.posStatus.DrawPosY = g_posPolygon.y;
	g_rotPolygon.z *= ROT_ATTEN;
	playerState.posStatus.DrawVector = g_rotPolygon.z * 0.5;
	playerState.intPosX = ( int )( g_posPolygon.x * 1000 );
	playerState.intPosY = ( int )( g_posPolygon.y * 1000 );

#ifdef _DEBUG
	if( GetKeyboardTrigger( DIK_SPACE ) == true )
	{
		SetBulletPlayerState( g_posPolygon.x , g_posPolygon.y , AF_PLAYER_DEFAULT_POSZ , 0.0f );
	}
	if( GetKeyboardPress( DIK_C ) == true )
	{
		SetBulletPlayerState( g_posPolygon.x , g_posPolygon.y , AF_PLAYER_DEFAULT_POSZ , 0.0f );
	}
#endif

	/*移動量減衰*/
	g_movePolygon.x += ( DEFAULT_MOVE - g_movePolygon.x ) * MOVE_ATTEN;
	g_movePolygon.y += ( DEFAULT_MOVE - g_movePolygon.y ) * MOVE_ATTEN;	 
	g_movePolygon.z *= SIPIN_ROT_ATTEN;	

	/*ポリゴン拡大*/
	/*
	if( GetKeyboardPress( DIK_UP ) )
	{
		polygonMag = ( polygonMag + MAG_SPEED > MAG_MAX ? MAG_MAX : polygonMag + MAG_SPEED );
	}
	if( GetKeyboardPress( DIK_DOWN ) )
	{
		polygonMag = ( polygonMag - MAG_SPEED < MAG_MIN ? MAG_MIN : polygonMag - MAG_SPEED );
	}
	*/
	
	float rot = 0.0f;
	rot = g_rotPolygon.z + g_BarrelRoll;

	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[AF_PLAYER_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	if( playerState.posStatus.DrawPosY < PGPCHANGE_HEIGHT_4 )
	{
		nowPatternNum = PGP_H_PULL_DOWN;
	}
	else if( playerState.posStatus.DrawPosY < PGPCHANGE_HEIGHT_3 )
	{
		nowPatternNum = PGP_L_PULL_DOWN;
	}
	else if( playerState.posStatus.DrawPosY > PGPCHANGE_HEIGHT_0 )
	{
		nowPatternNum = PGP_H_PULL_UP;
	}
	else if( playerState.posStatus.DrawPosY > PGPCHANGE_HEIGHT_1 )
	{
		nowPatternNum = PGP_L_PULL_UP;
	}

	//アニメーション設定
	if( ++g_nCounterAnim == AF_PLAYER_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == AF_PLAYER_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / AF_PLAYER_ANIM_DIV_X;
	texHeight = 1.0f / AF_PLAYER_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % AF_PLAYER_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_PLAYER_ANIM_DIV_X ) ) + nowPatternNum;
	
	//テクスチャ座標設定
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	
	//nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_PLAYER_ANIM_DIV_X ) ) + 1;

	pVtx[AF_PLAYER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
	pVtx[AF_PLAYER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY + 1;
	pVtx[AF_PLAYER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[AF_PLAYER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY + 1;
	pVtx[AF_PLAYER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
	pVtx[AF_PLAYER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + 1 + texHeight;
	pVtx[AF_PLAYER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[AF_PLAYER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + 1 + texHeight;

	/*ポインタのアンロック*/
	g_pVtxBuffAfPlayer->Unlock();
}

/*******************************************************************************
* 関数名	:void UpdateAfPlayer( void )
*		プレイヤー更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfPlayer2( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	PLAYER_GRAPHIC_PATTERN nowPatternNum = PGP_NEUTRAL;

	/*ポインタのロック*/ 
	g_pVtxBuffAfPlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	pVtx[AF_PLAYER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	
	
	/*
	if( GetGamePadPress( 4 ) && !GetGamePadPress( 5 ) )
	{
		movePlus = ( movePlus + MOVE_MINUS_SPEED <= MOVE_PLUS_MIN ? MOVE_PLUS_MIN : movePlus + MOVE_MINUS_SPEED );
	}
	if( GetGamePadPress( 5 ) && !GetGamePadPress( 4 ) )
	{
		movePlus = ( movePlus + MOVE_PLUS_SPEED	>= MOVE_PLUS_MAX ? MOVE_PLUS_MAX : movePlus + MOVE_PLUS_SPEED );
	}
	else
	{
		movePlus = 0.0f;
	}
	g_movePolygon.x += movePlus;
	*/

	/*移動量反映*/
	g_movePolygon.x = GetCursorState()->posStatus.DrawPosX - playerState.posStatus.DrawPosX;
	g_movePolygon.y = GetCursorState()->posStatus.DrawPosY - playerState.posStatus.DrawPosY;

	g_BarrelRoll = 0;
	playerState.posStatus.DrawPosX = SCREEN_WIDTH / 2 + playerState.posStatus.DrawPosX * 0.75;
	playerState.posStatus.DrawPosY = SCREEN_WIDTH / 2 + playerState.posStatus.DrawPosY * 0.75;
	

	g_posPolygon.x += g_movePolygon.x * MOVE_ATTEN;
	g_posPolygon.y += g_movePolygon.y * MOVE_ATTEN;
	if( polygonMag <= 0.01 )
	{
		getViewStatus()->centerPosY *= 0.95;
	}

	polygonMag *= 0.75;
	//g_rotPolygon.z += g_movePolygon.z;

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
	
	if(	playerState.speed < SPEED_MIN )
	{
		playerState.speed = SPEED_MIN;  
	}
	if(	playerState.speed > SPEED_MAX )
	{
		playerState.speed = SPEED_MAX;  
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
	playerState.posStatus.DrawPosX = g_posPolygon.x;
	playerState.posStatus.DrawPosY = g_posPolygon.y;
	g_rotPolygon.z *= ROT_ATTEN;
	playerState.posStatus.DrawVector = g_rotPolygon.z * 0.5;
	playerState.intPosX = ( int )( g_posPolygon.x * 1000 );
	playerState.intPosY = ( int )( g_posPolygon.y * 1000 );

#ifdef _DEBUG
	if( GetKeyboardTrigger( DIK_SPACE ) == true )
	{
		SetBulletPlayerState( g_posPolygon.x , g_posPolygon.y , AF_PLAYER_DEFAULT_POSZ , 0.0f );
	}
	if( GetKeyboardPress( DIK_C ) == true )
	{
		SetBulletPlayerState( g_posPolygon.x , g_posPolygon.y , AF_PLAYER_DEFAULT_POSZ , 0.0f );
	}
#endif

	/*移動量減衰*/
	g_movePolygon.x += ( DEFAULT_MOVE - g_movePolygon.x ) * MOVE_ATTEN;
	g_movePolygon.y += ( DEFAULT_MOVE - g_movePolygon.y ) * MOVE_ATTEN;	 
	g_movePolygon.z *= SIPIN_ROT_ATTEN;	

	/*ポリゴン拡大*/
	/*
	if( GetKeyboardPress( DIK_UP ) )
	{
		polygonMag = ( polygonMag + MAG_SPEED > MAG_MAX ? MAG_MAX : polygonMag + MAG_SPEED );
	}
	if( GetKeyboardPress( DIK_DOWN ) )
	{
		polygonMag = ( polygonMag - MAG_SPEED < MAG_MIN ? MAG_MIN : polygonMag - MAG_SPEED );
	}
	*/
	
	float rot = 0.0f;
	rot = g_rotPolygon.z + g_BarrelRoll;

	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[AF_PLAYER_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	if( playerState.posStatus.DrawPosY < PGPCHANGE_HEIGHT_4 )
	{
		nowPatternNum = PGP_H_PULL_DOWN;
	}
	else if( playerState.posStatus.DrawPosY < PGPCHANGE_HEIGHT_3 )
	{
		nowPatternNum = PGP_L_PULL_DOWN;
	}
	else if( playerState.posStatus.DrawPosY > PGPCHANGE_HEIGHT_0 )
	{
		nowPatternNum = PGP_H_PULL_UP;
	}
	else if( playerState.posStatus.DrawPosY > PGPCHANGE_HEIGHT_1 )
	{
		nowPatternNum = PGP_L_PULL_UP;
	}

	//アニメーション設定
	if( ++g_nCounterAnim == AF_PLAYER_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == AF_PLAYER_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / AF_PLAYER_ANIM_DIV_X;
	texHeight = 1.0f / AF_PLAYER_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % AF_PLAYER_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_PLAYER_ANIM_DIV_X ) ) + nowPatternNum;
	
	//テクスチャ座標設定
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	
	//nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_PLAYER_ANIM_DIV_X ) ) + 1;

	pVtx[AF_PLAYER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
	pVtx[AF_PLAYER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY + 1;
	pVtx[AF_PLAYER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[AF_PLAYER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY + 1;
	pVtx[AF_PLAYER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
	pVtx[AF_PLAYER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + 1 + texHeight;
	pVtx[AF_PLAYER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[AF_PLAYER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + 1 + texHeight;
	
	playerState.intPosX = ( int )( playerState.posStatus.posX ) * 1000;
	playerState.intPosY = ( int )( playerState.posStatus.posY ) * 1000;
	/*ポインタのアンロック*/
	g_pVtxBuffAfPlayer->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawAfPlayer( void )
*		プレイヤー描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfPlayer( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfPlayer );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfPlayer , 0 , sizeof( VERTEX_2D ) );

	/*プレイヤーの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}

/*******************************************************************************
* 関数名	:void DrawAfPlayercore( void )
*		プレイヤー描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfPlayercore( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfPlayer );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfPlayer , 0 , sizeof( VERTEX_2D ) );
	
	
	/*プレイヤーの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_PLAYER_VERTEX_MAX , 2 );

#ifdef _DEBUG
	char txt[60];
	RECT rectW = { ( LONG )( 0 ) , ( LONG )( 100 ) , WINDOW_WIDTH , WINDOW_HEIGHT };
	sprintf( &txt[0] , "PposX:%3.6f \nPposY:%3.6f \nProtV:%3.6f", 
		playerState.posStatus.posX , playerState.posStatus.posY , playerState.posStatus.vector );
	getFont()->DrawText( NULL , &txt[0] , -1 , &rectW , DT_LEFT , D3DCOLOR_RGBA( 255 , 222 , 222 , 255 ) );
#endif
}

/*******************************************************************************
* 関数名	:PLAYER* GetPlayerState( void )
*		プレイヤーのステータスを取得します。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:プレイヤーのステータスを取得します。。
*******************************************************************************/
PLAYER* GetPlayerState( void )
{
	return &playerState;
}