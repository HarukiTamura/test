/*******************************************************************************
* タイトル	:自弾.cpp
* ファイル名:AfLockonCursor.cpp
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
#include "sound.h"
#include "input.h"
#include "AfLockonCursor.h"
#include "AfMissilePlayer.h"
#include "AfEnemy.h"
#include "AfPlayer.h"
#include "AfCursor.h"
#include "unoDirectX.h"
#include "AfExplosion.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define AF_LOCKON_CURSOR_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_LOCKON_CURSOR_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_LOCKON_CURSOR_DEFAULT_VECTOR	( 0.0f )
#define AF_LOCKON_CURSOR_DEFAULT_POSZ	( 0.0f )
#define AF_LOCKON_CURSOR_DEFAULT_LIFE		( 100 )
#define AF_LOCKON_CURSOR_DEFAULT_SCORE		( 10 )

#define AF_LOCKON_CURSOR_OUTRANGE	( 1.20f )

#define AF_LOCKON_CURSOR_SPIN	( 1.23 )

#define AF_LOCKON_CURSOR_VERTEX_MAX		( 4 )	/*最高頂点数*/
#define AF_LOCKON_CURSOR_POLYGON_NUM	( LOCKON_CURSOR_MAX_NUM )	/*自機＋判定*/

#define AF_LOCKON_CURSOR_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define AF_LOCKON_CURSOR_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define AF_LOCKON_CURSOR_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define AF_LOCKON_CURSOR_ANIM_DIV_Y	( 4 )	/*高さ分割数*/

#define AF_LOCKON_CURSOR_OUTPUT_SIZE	( 1.0f ) /*初期表示倍率*/ 

#define MOVE_NUM ( 0.5f )	/*移動量*/
#define SPIN_SPEED	( D3DX_PI * 0.25 )	/*回転スピード*/
#define MAG_SPEED	( 0.10f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.30f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.23f )	/*減衰率*/
#define ROT_ATTEN		( 0.85f )	/*減衰率*/
#define SIPIN_ROT_ATTEN ( 0.12f )

#define MISSILE_LAUNCHER_OUTLONG ( 72 )
#define MISSILE_MOVE	( 1.0f )

/*******************************************************************************
* 構造体定義
*******************************************************************************/


/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfLockonCursor = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureAfLockonCursor;	/*自弾テクスチャのポインタ*/
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

static LOCKON_CURSOR_STATUS lockonCursorState[LOCKON_CURSOR_MAX_NUM]; 

static int lockonFps = 0;

/*******************************************************************************
* 関数名	:HRESULT InitAfLockonCursor( void )
*		自弾初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfLockonCursor( void )
{
	HRESULT result;
	result = InitDirectAfLockonCursor();
	if( result != S_OK )
	{
		Error("描画基準位置データの初期化に失敗しました。");
	}
	InitValueAfLockonCursor();
	return result;
}

/*******************************************************************************
* 関数名	:HRESULT InitDirectAfLockonCursor( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitDirectAfLockonCursor( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_LOCKON_CURSOR_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfLockonCursor , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfLockonCursor.png" , &g_pTextureAfLockonCursor );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfLockonCursor.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffAfLockonCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_LOCKON_CURSOR_POLYGON_NUM );

	/*ポインタのアンロック*/
	g_pVtxBuffAfLockonCursor->Unlock();
}

/*******************************************************************************
* 関数名	:HRESULT InitValueAfLockonCursor( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void InitValueAfLockonCursor( void )
{
	int outputSizeX , outputSizeY;
	
	//事前サイズ計算
	outputSizeX = ( int )( ( info.Width / AF_LOCKON_CURSOR_ANIM_DIV_X ) * AF_LOCKON_CURSOR_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_LOCKON_CURSOR_ANIM_DIV_Y ) * AF_LOCKON_CURSOR_OUTPUT_SIZE );

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
	memset( &lockonCursorState , 0 , sizeof( LOCKON_CURSOR_STATUS ) * LOCKON_CURSOR_MAX_NUM );
	
	for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
	{
		lockonCursorState[cnt].bUse = false;
		lockonCursorState[cnt].posStatus.posX = AF_LOCKON_CURSOR_DEFAULT_POSX;
		lockonCursorState[cnt].posStatus.posY = AF_LOCKON_CURSOR_DEFAULT_POSY;
		lockonCursorState[cnt].posStatus.vector = AF_LOCKON_CURSOR_DEFAULT_VECTOR;

		lockonCursorState[cnt].intPosX = ( int )( lockonCursorState[cnt].posStatus.posX ) * 1000;
		lockonCursorState[cnt].intPosY = ( int )( lockonCursorState[cnt].posStatus.posY ) * 1000;
				  
		lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_UNLOCK;
	}

	g_posPolygon.x = AF_LOCKON_CURSOR_DEFAULT_POSX;
	g_posPolygon.y = AF_LOCKON_CURSOR_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*アニメーション用ステータス*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	lockonFps = 0;
}

/*******************************************************************************
* 関数名	:void UninitAfLockonCursor( void )
*		自弾終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfLockonCursor( void )
{
	if( g_pVtxBuffAfLockonCursor != NULL )
	{
		g_pVtxBuffAfLockonCursor->Release();
		g_pVtxBuffAfLockonCursor = NULL;
	}
	if( g_pTextureAfLockonCursor != NULL )
	{
		g_pTextureAfLockonCursor->Release();
		g_pTextureAfLockonCursor = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfLockonCursor( void )
*		自弾更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfLockonCursor( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;

	/*ポインタのロック*/ 
	g_pVtxBuffAfLockonCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	if( GetKeyboardPress( DIK_V ) || GetGamePadPress( 0 ) )
	{
		lockonFps = ( lockonFps + 1 >= 60 ? 0 : lockonFps + 1 );
		
		if( lockonFps % 3 == 0 )
		{
			for( int cnt = 0; cnt < ENEMY_MAX_NUM; cnt++ )
			{
				if( GetEnemyState(cnt)->bUse == true && lockonCursorState[cnt].mode != LOCKON_CURSOR_MODE_LOCK && GetEnemyState(cnt)->posZ < AF_LOCKON_CURSOR_LOST )	
				{
					GetEnemyState(cnt)->bLockOn = true;
					lockonCursorState[cnt].bUse = true;	
					lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_LOCK;
					PlaySound(SOUND_LABEL_SE_LOCKON);
					break;
				}	
				if( GetEnemyState(cnt)->posZ > AF_LOCKON_CURSOR_LOST )
				{
					
					lockonCursorState[cnt].bUse = false;	
					lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_UNLOCK;
					GetEnemyState(cnt)->bLockOn = false;
				}
			}
			/*if( lockonFps % 3 == 0 && lockonCursorState[cnt].mode != LOCKON_CURSOR_MODE_LOCK )
			{	
				if( GetEnemyState(cnt)->bUse == true &&  GetEnemyState(cnt)->posZ < AF_LOCKON_CURSOR_LOST )
				{
					GetEnemyState(cnt)->bLockOn = true;
					lockonCursorState[cnt].bUse = true;	
					lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_CHANGE;
					break;
				}	
			}

			if( lockonCursorState[cnt].mode == LOCKON_CURSOR_MODE_CHANGE )
			{
				lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_LOCK;
			}
			if( GetEnemyState(cnt)->bUse == false )
			{
				GetEnemyState(cnt)->bLockOn = false;
				lockonCursorState[cnt].bUse = false;
				break;
			}*/
		} 
	}
	else if( GetKeyboardRelease( DIK_V ) || GetGamePadRelease( 0 ) )
	{
		float posVec = -0.33f;
		int missileShotCnt = 0;
		for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
		{
			if( lockonCursorState[cnt].bUse == true )
			{
				missileShotCnt++;
				SetMissilePlayerState( GetPlayerState()->posStatus.DrawPosX + MISSILE_LAUNCHER_OUTLONG * sin( D3DX_PI * posVec ), GetPlayerState()->posStatus.DrawPosY + MISSILE_LAUNCHER_OUTLONG * cos( D3DX_PI * posVec ),
					1.0f , MISSILE_MOVE * sin( D3DX_PI * posVec ) , MISSILE_MOVE * cos( D3DX_PI * posVec ) , 0.0f );
				posVec += 0.2;
				SetAfExplosion( GetPlayerState()->posStatus.DrawPosX * 1000 + rand() % MISSILE_LAUNCHER_OUTLONG - rand() % MISSILE_LAUNCHER_OUTLONG
					, GetPlayerState()->posStatus.DrawPosY * 1000 + rand() % MISSILE_LAUNCHER_OUTLONG - rand() % MISSILE_LAUNCHER_OUTLONG , 1.2 , 0 , 0 , 300  , 4 , AF_EXPLOSION_002 );
				lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_UNLOCK;
			}
		}
		if( missileShotCnt > 0 )
		{
			PlaySound(SOUND_LABEL_SE_MISSILESHOOT);
		}
	}
	else
	{
		lockonFps = 0;
		for( int cnt = 0; cnt < ENEMY_MAX_NUM; cnt++ )
		{
			if( GetEnemyState(cnt)->bUse == true )
			{
				//GetEnemyState(cnt)->bLockOn = false;
			}
		}
		for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
		{
			lockonCursorState[cnt].bUse = false;
		}
	}

	for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
	{
		if( lockonCursorState[cnt].bUse != true && GetEnemyState(cnt)->bUse == true && GetEnemyState(cnt)->bLockOn == false && GetEnemyState(cnt)->posZ < 0.0f )
		{
			lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_NONLOCK;
			lockonCursorState[cnt].bUse = true;
		}
		if( lockonCursorState[cnt].mode != LOCKON_CURSOR_MODE_UNLOCK && GetEnemyState(cnt)->bUse == false )
		{
			lockonCursorState[cnt].bUse = false;
		}
		if( lockonCursorState[cnt].bUse == true )
		{

			//実質位置確定

			//描画座標・サイズ確定
			lockonCursorState[cnt].posStatus.DrawPosX = GetEnemyState(cnt)->posStatus.DrawPosX;
			lockonCursorState[cnt].posStatus.DrawPosY = GetEnemyState(cnt)->posStatus.DrawPosY;
			lockonCursorState[cnt].posStatus.DrawVector = 0.0f;
			polygonMag = ( GetEnemyState(cnt)->posZ <= MAG_MIN ? MAG_MIN : GetEnemyState(cnt)->posZ );

			g_posPolygon.x = lockonCursorState[cnt].posStatus.DrawPosX;
			g_posPolygon.y = lockonCursorState[cnt].posStatus.DrawPosY;
			g_rotPolygon.z = lockonCursorState[cnt].posStatus.DrawVector;
			
			if( lockonCursorState[cnt].mode == LOCKON_CURSOR_MODE_LOCK )
			{
				g_rotPolygon.z += SPIN_SPEED;
			}
			
			if( lockonCursorState[cnt].mode == LOCKON_CURSOR_MODE_UNLOCK )
			{
				g_rotPolygon.z += SPIN_SPEED * 0.5f;
			}

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


			//アニメーション設定
			if( ++g_nCounterAnim == AF_LOCKON_CURSOR_ANIM_SPAN )
			{
				g_nCounterAnim = 0;
				g_nPatternAnim = ( ++g_nPatternAnim == AF_LOCKON_CURSOR_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
	
			texWidth = 1.0f / AF_LOCKON_CURSOR_ANIM_DIV_X;
			texHeight = 1.0f / AF_LOCKON_CURSOR_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % AF_LOCKON_CURSOR_ANIM_DIV_X );

			nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_LOCKON_CURSOR_ANIM_DIV_X ) ) + lockonCursorState[cnt].mode;

			//テクスチャ座標設定
			pVtx[QUOTE_VERTEX * cnt].tex.x = texWidth * nowPatternX;
			pVtx[QUOTE_VERTEX * cnt].tex.y = texHeight * nowPatternY;
			pVtx[QUOTE_VERTEX * cnt + 1].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[QUOTE_VERTEX * cnt + 1].tex.y = texHeight * nowPatternY;
			pVtx[QUOTE_VERTEX * cnt + 2].tex.x = texWidth * nowPatternX;
			pVtx[QUOTE_VERTEX * cnt + 2].tex.y = texHeight * nowPatternY + texHeight;
			pVtx[QUOTE_VERTEX * cnt + 3].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[QUOTE_VERTEX * cnt + 3].tex.y = texHeight * nowPatternY + texHeight;

			//頂点座標設定
			pVtx[QUOTE_VERTEX * cnt].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
			pVtx[QUOTE_VERTEX * cnt + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

			pVtx[QUOTE_VERTEX * cnt + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

			pVtx[QUOTE_VERTEX * cnt + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
			
			pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 235 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 235 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 235 );
		}
		if( lockonCursorState[cnt].bUse == false )
		{			
			pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
		}
	}

	/*ポインタのアンロック*/
	g_pVtxBuffAfLockonCursor->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawAfLockonCursor( void )
*		自弾描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfLockonCursor( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfLockonCursor );
	
	for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
	{
		
		if( lockonCursorState[cnt].bUse == true )
		{

			/*頂点バッファをストリームにバインド*/
			pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfLockonCursor , 0 , sizeof( VERTEX_2D ) );

			/*自弾の描画*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , QUOTE_VERTEX * cnt , 2 );
		}
	}
}

/*******************************************************************************
* 関数名	:void DrawAfLockonCursorcore( void )
*		自弾描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfLockonCursorcore( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfLockonCursor );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfLockonCursor , 0 , sizeof( VERTEX_2D ) );
	
	
	/*自弾の描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_LOCKON_CURSOR_VERTEX_MAX , 2 );

}

/*******************************************************************************
* 関数名	:LOCKON_CURSOR_STATUS* GetLockonCursorState( int num )
*		自弾のステータスを取得します。
* 引数		:int num	入手する構造体の番号
* 戻り値	:void 戻り値なし
* 説明		:自弾のステータスを取得します。。
*******************************************************************************/
LOCKON_CURSOR_STATUS* GetLockonCursorState( int num )
{
	return &lockonCursorState[num];
}

/*******************************************************************************
* 関数名	:LOCKON_CURSOR_STATUS* GetLockonCursorState( int num )
*		自弾のステータスを取得します。
* 引数		:int num	入手する構造体の番号
* 戻り値	:void 戻り値なし
* 説明		:自弾のステータスを取得します。。
*******************************************************************************/
void SetLockonCursorState( float posX , float posY , float posZ , float moveZ )
{
	for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
	{
		if( lockonCursorState[cnt].bUse == false )
		{
			lockonCursorState[cnt].bUse = true;
			lockonCursorState[cnt].posStatus.posX = posX;
			lockonCursorState[cnt].posStatus.posY = posY;
			lockonCursorState[cnt].posStatus.vector = 0.0f;

			lockonCursorState[cnt].intPosX = ( int )( lockonCursorState[cnt].posStatus.posX ) * 1000;
			lockonCursorState[cnt].intPosY = ( int )( lockonCursorState[cnt].posStatus.posY ) * 1000;
				  
			break;
		}
	}
}