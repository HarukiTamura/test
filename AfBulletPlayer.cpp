/*******************************************************************************
* タイトル	:自弾.cpp
* ファイル名:AfBulletPlayer.cpp
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
#include "AfBulletPlayer.h"
#include "AfPlayer.h"
#include "AfCursor.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define AF_BULLET_PLAYER_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_BULLET_PLAYER_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_BULLET_PLAYER_DEFAULT_VECTOR	( 0.0f )
#define AF_BULLET_PLAYER_DEFAULT_POSZ	( 0.0f )
#define AF_BULLET_PLAYER_DEFAULT_LIFE		( 100 )
#define AF_BULLET_PLAYER_DEFAULT_SCORE		( 10 )

#define AF_BULLET_PLAYER_SPEED	( 0.016f )
#define AF_BULLET_PLAYER_OUTSCREEN	( 1.20f )
#define AF_BULLET_PLAYER_DELETE		( 0.00f )

#define AF_BULLET_PLAYER_SPIN	( 1.23 )

#define AF_BULLET_PLAYER_VERTEX_MAX		( 4 )	/*最高頂点数*/
#define AF_BULLET_PLAYER_POLYGON_NUM	( BULLET_PLAYER_MAX_NUM )	/*自機＋判定*/

#define AF_BULLET_PLAYER_ANIM_MAX		( 3 )	/*アニメーションコマ数*/
#define AF_BULLET_PLAYER_ANIM_SPAN	( 10 )	/*アニメーション間隔*/
#define AF_BULLET_PLAYER_ANIM_DIV_X	( 3 )	/*幅分割数*/
#define AF_BULLET_PLAYER_ANIM_DIV_Y	( 5 )	/*高さ分割数*/

#define AF_BULLET_PLAYER_OUTPUT_SIZE	( 1.0f ) /*初期表示倍率*/ 

#define MOVE_NUM ( 0.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.10f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN		( 0.23f )	/*減衰率*/
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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfBulletPlayer = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureAfBulletPlayer;	/*自弾テクスチャのポインタ*/
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

static BULLET_PLAYER_STATUS bulletPlayerState[BULLET_PLAYER_MAX_NUM]; 

/*******************************************************************************
* 関数名	:HRESULT InitAfBulletPlayer( void )
*		自弾初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitAfBulletPlayer( void )
{
	HRESULT result;
	result = InitDirectAfBulletPlayer();
	if( result != S_OK )
	{
		Error("描画基準位置データの初期化に失敗しました。");
	}
	InitValueAfBulletPlayer();
	return result;
}

/*******************************************************************************
* 関数名	:HRESULT InitDirectAfBulletPlayer( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitDirectAfBulletPlayer( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_BULLET_PLAYER_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfBulletPlayer , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfBulletPlayer.png" , &g_pTextureAfBulletPlayer );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfBulletPlayer.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffAfBulletPlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_BULLET_PLAYER_POLYGON_NUM );

	/*ポインタのアンロック*/
	g_pVtxBuffAfBulletPlayer->Unlock();
}

/*******************************************************************************
* 関数名	:HRESULT InitValueAfBulletPlayer( void )
*		背景初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void InitValueAfBulletPlayer( void )
{
	int outputSizeX , outputSizeY;
	
	//事前サイズ計算
	outputSizeX = ( int )( ( info.Width / AF_BULLET_PLAYER_ANIM_DIV_X ) * AF_BULLET_PLAYER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_BULLET_PLAYER_ANIM_DIV_Y ) * AF_BULLET_PLAYER_OUTPUT_SIZE );

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
	memset( &bulletPlayerState , 0 , sizeof( BULLET_PLAYER_STATUS ) * BULLET_PLAYER_MAX_NUM );
	
	for( int cnt = 0; cnt < BULLET_PLAYER_MAX_NUM; cnt++ )
	{
		bulletPlayerState[cnt].bUse = false;
		bulletPlayerState[cnt].posStatus.posX = AF_BULLET_PLAYER_DEFAULT_POSX;
		bulletPlayerState[cnt].posStatus.posY = AF_BULLET_PLAYER_DEFAULT_POSY;
		bulletPlayerState[cnt].posStatus.vector = AF_BULLET_PLAYER_DEFAULT_VECTOR;
		bulletPlayerState[cnt].moveZ = 0.0f;
		bulletPlayerState[cnt].posZ = AF_BULLET_PLAYER_DEFAULT_POSZ;

		bulletPlayerState[cnt].intPosX = ( int )( bulletPlayerState[cnt].posStatus.posX ) * 1000;
		bulletPlayerState[cnt].intPosY = ( int )( bulletPlayerState[cnt].posStatus.posY ) * 1000;
				  
		bulletPlayerState[cnt].nLife = AF_BULLET_PLAYER_DEFAULT_LIFE;
	}

	g_posPolygon.x = AF_BULLET_PLAYER_DEFAULT_POSX;
	g_posPolygon.y = AF_BULLET_PLAYER_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*アニメーション用ステータス*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	
}

/*******************************************************************************
* 関数名	:void UninitAfBulletPlayer( void )
*		自弾終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitAfBulletPlayer( void )
{
	if( g_pVtxBuffAfBulletPlayer != NULL )
	{
		g_pVtxBuffAfBulletPlayer->Release();
		g_pVtxBuffAfBulletPlayer = NULL;
	}
	if( g_pTextureAfBulletPlayer != NULL )
	{
		g_pTextureAfBulletPlayer->Release();
		g_pTextureAfBulletPlayer = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateAfBulletPlayer( void )
*		自弾更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateAfBulletPlayer( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;

	/*ポインタのロック*/ 
	g_pVtxBuffAfBulletPlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	for( int cnt = 0; cnt < BULLET_PLAYER_MAX_NUM; cnt++ )
	{
		if( bulletPlayerState[cnt].bUse == true )
		{
			//行動
			bulletPlayerState[cnt].moveZ = -AF_BULLET_PLAYER_SPEED;
			
			//死亡確認
			if( bulletPlayerState[cnt].nLife <= 0 || bulletPlayerState[cnt].posZ >= AF_BULLET_PLAYER_OUTSCREEN || bulletPlayerState[cnt].posZ <= AF_BULLET_PLAYER_DELETE )
			{
				bulletPlayerState[cnt].bUse = false;
			}
			//実質位置確定

			//描画座標・サイズ確定
			bulletPlayerState[cnt].posZ += bulletPlayerState[cnt].moveZ;
			bulletPlayerState[cnt].posStatus.DrawPosX = bulletPlayerState[cnt].posStatus.posX;
			bulletPlayerState[cnt].posStatus.DrawPosY = bulletPlayerState[cnt].posStatus.posY;
			bulletPlayerState[cnt].posStatus.DrawVector = bulletPlayerState[cnt].posStatus.vector - getViewStatus()->centerVector;
			polygonMag = bulletPlayerState[cnt].posZ;

			bulletPlayerState[cnt].posStatus.DrawPosX = ( bulletPlayerState[cnt].posStatus.DrawPosX * ( polygonMag <= 0.0f ? 0.0f : polygonMag ) ) + SCREEN_WIDTH  * 0.5 * ( 1.0f - ( polygonMag >= 1.0f ? 1.0f : polygonMag) ); 
			bulletPlayerState[cnt].posStatus.DrawPosY = ( bulletPlayerState[cnt].posStatus.DrawPosY * ( polygonMag <= 0.0f ? 0.0f : polygonMag ) ) + ( SCREEN_HEIGHT - getViewStatus()->centerPosY ) * ( 1.0f - ( polygonMag >= 1.0f ? 1.0f : polygonMag) );
			g_posPolygon.x = bulletPlayerState[cnt].posStatus.DrawPosX;
			g_posPolygon.y = bulletPlayerState[cnt].posStatus.DrawPosY;
			g_rotPolygon.z = bulletPlayerState[cnt].posStatus.DrawVector;
						
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
			if( ++g_nCounterAnim == AF_BULLET_PLAYER_ANIM_SPAN )
			{
				g_nCounterAnim = 0;
				g_nPatternAnim = ( ++g_nPatternAnim == AF_BULLET_PLAYER_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
	
			texWidth = 1.0f / AF_BULLET_PLAYER_ANIM_DIV_X;
			texHeight = 1.0f / AF_BULLET_PLAYER_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % AF_BULLET_PLAYER_ANIM_DIV_X );
			nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_BULLET_PLAYER_ANIM_DIV_X ) );
	
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
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		}
		if( bulletPlayerState[cnt].bUse == false )
		{			
			pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
		}
	}

	/*ポインタのアンロック*/
	g_pVtxBuffAfBulletPlayer->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawAfBulletPlayer( void )
*		自弾描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfBulletPlayer( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfBulletPlayer );
	
	for( int cnt = 0; cnt < BULLET_PLAYER_MAX_NUM; cnt++ )
	{
		/*頂点バッファをストリームにバインド*/
		pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfBulletPlayer , 0 , sizeof( VERTEX_2D ) );

		/*自弾の描画*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , QUOTE_VERTEX * cnt , 2 );
	}
}

/*******************************************************************************
* 関数名	:void DrawAfBulletPlayercore( void )
*		自弾描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawAfBulletPlayercore( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfBulletPlayer );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfBulletPlayer , 0 , sizeof( VERTEX_2D ) );
	
	
	/*自弾の描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_BULLET_PLAYER_VERTEX_MAX , 2 );

}

/*******************************************************************************
* 関数名	:BULLET_PLAYER_STATUS* GetBulletPlayerState( int num )
*		自弾のステータスを取得します。
* 引数		:int num	入手する構造体の番号
* 戻り値	:void 戻り値なし
* 説明		:自弾のステータスを取得します。。
*******************************************************************************/
BULLET_PLAYER_STATUS* GetBulletPlayerState( int num )
{
	return &bulletPlayerState[num];
}

/*******************************************************************************
* 関数名	:BULLET_PLAYER_STATUS* GetBulletPlayerState( int num )
*		自弾のステータスを取得します。
* 引数		:int num	入手する構造体の番号
* 戻り値	:void 戻り値なし
* 説明		:自弾のステータスを取得します。。
*******************************************************************************/
void SetBulletPlayerState( float posX , float posY , float posZ , float moveZ )
{
	for( int cnt = 0; cnt < BULLET_PLAYER_MAX_NUM; cnt++ )
	{
		if( bulletPlayerState[cnt].bUse == false )
		{
			bulletPlayerState[cnt].bUse = true;
			bulletPlayerState[cnt].posStatus.posX = posX;
			bulletPlayerState[cnt].posStatus.posY = posY;
			bulletPlayerState[cnt].posStatus.vector = 0.0f;
			bulletPlayerState[cnt].moveZ = moveZ;
			bulletPlayerState[cnt].posZ = posZ;

			bulletPlayerState[cnt].intPosX = ( int )( bulletPlayerState[cnt].posStatus.posX ) * 1000;
			bulletPlayerState[cnt].intPosY = ( int )( bulletPlayerState[cnt].posStatus.posY ) * 1000;
				  
			bulletPlayerState[cnt].nLife = AF_BULLET_PLAYER_DEFAULT_LIFE;
			break;
		}
	}
}