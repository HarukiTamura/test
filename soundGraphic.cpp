/*******************************************************************************
* サウンド	:サウンドグラフィック.cpp
* ファイル名:soundGraphic.cpp
* 作成者	:田村春樹
* 作成日	:2015/05/29
********************************************************************************
* 更新履歴	ver 1.00	昨日としての完成
*			ver 1.01	ズーム時の速度倍率define定義時に動作不良、原因不明。
*******************************************************************************/

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "sound.h"
#include "soundGraphic.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define SOUND_GRAPHIC_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define SOUND_GRAPHIC_PICKUP_FPS	( 45 )

#define SOUND_GRAPHIC_OUTPUT_POS_X	( SCREEN_WIDTH * 0.5 )		/*表示X座標*/
#define SOUND_GRAPHIC_OUTPUT_POS_Y	( SCREEN_HEIGHT * 0.5 )		/*表示Y座標*/

#define SOUND_GRAPHIC_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define SOUND_GRAPHIC_ANIM_SPAN		( 0 )	/*アニメーション間隔*/
#define SOUND_GRAPHIC_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define SOUND_GRAPHIC_ANIM_DIV_Y	( 3 )	/*高さ分割数*/

#define SOUND_GRAPHIC_OUTPUT_SIZE	( 0.65 ) /*初期表示倍率*/ 

#define MOVE_NUM ( 1.5f )	/*移動量*/
#define SPIN_SPEED	( 0.0f )	/*回転スピード*/
#define MAG_SPEED	( 0.025f )	/*拡大スピード*/

#define MAG_MAX		( 1.0f )	/*拡大限界*/
#define MAG_MIN 	( 0.8f )	/*縮小限界*/

#define DEFAULT_MOVE	( 0.0f )	/*移動量最終値*/
#define MOVE_ATTEN	( 0.05f )	/*減衰率*/
/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffSoundGraphic = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTextureSoundGraphic;	/*サウンドグラフィックテクスチャのポインタ*/
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

static int outputSizeX , outputSizeY;
static int zoomSwitch, zoomCnt;
static float magNum;

static int outputStatus = SOUND_NORMAL;
static int g_soundStatus;
static int pastState;
static int writeCnt;
/*******************************************************************************
* 関数名	:HRESULT InitSoundGraphic( void )
*		サウンドグラフィック初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitSoundGraphic( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * SOUND_GRAPHIC_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffSoundGraphic , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/soundGraphic.png" , &g_pTextureSoundGraphic );
	D3DXGetImageInfoFromFile( "data/TEXTURE/soundGraphic.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffSoundGraphic->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / SOUND_GRAPHIC_ANIM_DIV_X ) * SOUND_GRAPHIC_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / SOUND_GRAPHIC_ANIM_DIV_Y ) * SOUND_GRAPHIC_OUTPUT_SIZE );

	g_posPolygon.x = SOUND_GRAPHIC_OUTPUT_POS_X;
	g_posPolygon.y = SOUND_GRAPHIC_OUTPUT_POS_Y;
	g_posPolygon.z = 0.0f;

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( SCREEN_LEFT	, SCREEN_TOP , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( SCREEN_RIGHT	, SCREEN_TOP , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( SCREEN_LEFT	, SCREEN_BOTTOM , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( SCREEN_RIGHT , SCREEN_BOTTOM , 0.0f );

	/*座標変換用係数設定*/
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;
	
	/*頂点カラー設定*/
	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	/*テクスチャ座標設定*/
	pVtx[0].tex = D3DXVECTOR2( 0 , 0 );
	pVtx[1].tex = D3DXVECTOR2( 1 , 0 );
	pVtx[2].tex = D3DXVECTOR2( 0 , 1 );
	pVtx[3].tex = D3DXVECTOR2( 1 , 1 );

	/*ポインタのアンロック*/
	g_pVtxBuffSoundGraphic->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	zoomSwitch = 0;
	zoomCnt = 0;
	magNum = MAG_MAX;

	outputStatus = SOUND_NORMAL;
	g_soundStatus = SOUND_NORMAL;
	pastState = SOUND_NORMAL;
	writeCnt = 0;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitSoundGraphic( void )
*		サウンドグラフィック終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitSoundGraphic( void )
{
	if( g_pVtxBuffSoundGraphic != NULL )
	{
		g_pVtxBuffSoundGraphic->Release();
		g_pVtxBuffSoundGraphic = NULL;
	}
	if( g_pTextureSoundGraphic != NULL )
	{
		g_pTextureSoundGraphic->Release();
		g_pTextureSoundGraphic = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateSoundGraphic( int soundStatus )
*		サウンドグラフィック更新処理関数です。
* 引数		:int soundStatus	サウンドの状態
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateSoundGraphic( int soundStatus )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX = 0, nowPatternY = 0;
	
	/*例外処理*/ 
	if( g_pVtxBuffSoundGraphic == NULL )
	{
		return;
	}

	writeCnt = ( writeCnt >= 0 ? writeCnt - 1 : writeCnt );
	g_soundStatus = soundStatus;
	//サウンドステータスが変更され、それが前のものと違う場合にのみ動作。
	if( soundStatus != SOUND_NORMAL && soundStatus != pastState )
	{
		writeCnt = SOUND_GRAPHIC_PICKUP_FPS;
		outputStatus = soundStatus;
		magNum = MAG_MAX;
	}
	pastState = soundStatus;

	if( ++g_nCounterAnim == SOUND_GRAPHIC_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == SOUND_GRAPHIC_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / SOUND_GRAPHIC_ANIM_DIV_X;
	texHeight = 1.0f / SOUND_GRAPHIC_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % SOUND_GRAPHIC_ANIM_DIV_X );

	switch( outputStatus )
	{
		case SOUND_MUTEON:
			{	
				nowPatternY = ( float )( ( int )( g_nPatternAnim / SOUND_GRAPHIC_ANIM_DIV_Y ) ) + 1;
				break;
			}
		case SOUND_MUTEOFF:
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / SOUND_GRAPHIC_ANIM_DIV_Y ) );
				break;
			}
		case SOUND_VOLCHANGE:
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / SOUND_GRAPHIC_ANIM_DIV_Y ) ) + 2;
				break;
			}
		default:
			{
				break;
			}
	}
	
	g_pVtxBuffSoundGraphic->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*テクスチャ座標反映*/
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	

	/*移動量反映*/
	g_posPolygon.x += g_movePolygon.x;
	g_posPolygon.y += g_movePolygon.y;

	if( g_posPolygon.x < SCREEN_LEFT )
	{
		g_posPolygon.x = SCREEN_LEFT;
	}
	if( g_posPolygon.x > SCREEN_RIGHT )
	{
		g_posPolygon.x = SCREEN_RIGHT;
	}
	if( g_posPolygon.y < SCREEN_TOP )
	{
		g_posPolygon.y = SCREEN_TOP;
	}
	if( g_posPolygon.y > SCREEN_HEIGHT )
	{
		g_posPolygon.y = SCREEN_HEIGHT;
	}
	/*移動量減衰*/
	g_movePolygon.x += ( DEFAULT_MOVE - g_movePolygon.x ) * MOVE_ATTEN;
	g_movePolygon.y += ( DEFAULT_MOVE - g_movePolygon.y ) * MOVE_ATTEN;	 	
		
	/*ポリゴン回転*/
	
	g_rotPolygon.z += SPIN_SPEED;
	
	if( g_rotPolygon.z > D3DX_PI )
	{
		g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
	}
	if( g_rotPolygon.z < -D3DX_PI )
	{
		g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
	}

	if( writeCnt >= SOUND_GRAPHIC_PICKUP_FPS * 0.25 )
	{
		magNum = ( magNum - MAG_SPEED < MAG_MIN ? MAG_MIN : magNum - MAG_SPEED );
	}
	polygonMag = 1.0f + sin( magNum );

	pVtx[0].col = D3DCOLOR_RGBA( 22 , 22 , 255 , 244 );
	pVtx[1].col = D3DCOLOR_RGBA( 22 , 22 , 255 , 244 );
	pVtx[2].col = D3DCOLOR_RGBA( 22 , 22 , 255 , 244 );
	pVtx[3].col = D3DCOLOR_RGBA( 22 , 22 , 255 , 244 );

	
	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	*/
	/*ポインタのアンロック*/
	g_pVtxBuffSoundGraphic->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawSoundGraphic( void )
*		サウンドグラフィック描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawSoundGraphic( void )
{
	if( g_soundStatus == SOUND_NORMAL && writeCnt < 0 )
	{
		return;
	}

	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTextureSoundGraphic );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffSoundGraphic , 0 , sizeof( VERTEX_2D ) );

	/*サウンドグラフィックの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}