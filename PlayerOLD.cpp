/*******************************************************************************
* タイトル	:弾.cpp
* ファイル名:bullet.cpp
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
#include "bullet.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define BULLET_OUTPUT_POS_X	( 320 )		/*表示X座標*/
#define BULLET_OUTPUT_POS_Y	( 230 )		/*表示Y座標*/

#define BULLET_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define BULLET_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define BULLET_ANIM_SPAN	( 0 )	/*アニメーション間隔*/
#define BULLET_ANIM_DIV_X	( 1 )	/*幅分割数*/
#define BULLET_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define BULLET_OUTPUT_SIZE	( 0.5 ) /*初期表示倍率*/ 

#define MOVE_NUM ( 2.5f )	/*移動量*/
#define SPIN_SPEED	( 0.12f )	/*回転スピード*/
#define MAG_SPEED	( 0.20f )	/*拡大スピード*/

#define MAG_MAX		( 10.00f )	/*拡大限界*/
#define MAG_MIN 	( 0.00f )	/*縮小限界*/

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffbullet = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturebullet;	/*弾のテクスチャのポインタ*/
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

/*******************************************************************************
* 関数名	:HRESULT InitBullet( void )
*		弾の初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitBullet( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * BULLET_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffbullet , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/bullet000.png" , &g_pTexturebullet );
	D3DXGetImageInfoFromFile( "data/TEXTURE/bullet000.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffbullet->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / BULLET_ANIM_DIV_X ) * BULLET_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / BULLET_ANIM_DIV_Y ) * BULLET_OUTPUT_SIZE );

	g_posPolygon.x = BULLET_OUTPUT_POS_X;
	g_posPolygon.y = BULLET_OUTPUT_POS_Y;
	g_posPolygon.z = 0.0f;

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );

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
	g_pVtxBuffbullet->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitBullet( void )
*		弾の終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitBullet( void )
{
	if( g_pVtxBuffbullet != NULL )
	{
		g_pVtxBuffbullet->Release();
		g_pVtxBuffbullet = NULL;
	}
	if( g_pTexturebullet != NULL )
	{
		g_pTexturebullet->Release();
		g_pTexturebullet = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateBullet( void )
*		弾の更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateBullet( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	
	if( ++g_nCounterAnim == BULLET_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == BULLET_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / BULLET_ANIM_DIV_X;
	texHeight = 1.0f / BULLET_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % BULLET_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) );
	/*ポインタのロック*/ 
	g_pVtxBuffbullet->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*読み込んだものとは左右反転*/

	pVtx[1].tex.x = texWidth * nowPatternX;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[0].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[3].tex.x = texWidth * nowPatternX;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[2].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;


	if( GetKeyboardPress( DIK_W ) )
	{
		if( GetKeyboardPress( DIK_A ) )
		{
			g_posPolygon.x += ( float )( -sinf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
			g_posPolygon.y += ( float )( cosf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
		}
		else if( GetKeyboardPress( DIK_S ) )
		{

		}
		else if( GetKeyboardPress( DIK_D ) )
		{
			g_posPolygon.x += ( float )( sinf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
			g_posPolygon.y += ( float )( cosf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
		}
		else
		{
			g_posPolygon.y -= MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_A ) )
	{
		if( GetKeyboardPress( DIK_S ) )
		{
			g_posPolygon.x += -sinf( D3DX_PI * 0.25 ) * MOVE_NUM;
			g_posPolygon.y += cosf( D3DX_PI * 0.25 ) * MOVE_NUM;
		}
		else if( GetKeyboardPress( DIK_D ) )
		{

		}
		else
		{
			g_posPolygon.x -= MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_S ) )
	{
		if( GetKeyboardPress( DIK_D ) )
		{
			g_posPolygon.x += sinf( D3DX_PI * 0.25 ) * MOVE_NUM;
			g_posPolygon.y += cosf( D3DX_PI * 0.25 ) * MOVE_NUM;
		}
		else
		{
			g_posPolygon.y += MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_D ) )
	{
		g_posPolygon.x += MOVE_NUM;
	}
	else
	{

	}

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
	/*ポリゴン回転*/
	if( GetKeyboardPress( DIK_LEFT ) )/*時計回り*/
	{
		g_rotPolygon.z += SPIN_SPEED;
	}
	if( GetKeyboardPress( DIK_RIGHT ) )/*逆時計回り*/
	{
		g_rotPolygon.z -= SPIN_SPEED;
	}
	
	if( g_rotPolygon.z > D3DX_PI )
	{
		g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
	}
	if( g_rotPolygon.z < -D3DX_PI )
	{
		g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
	}

	/*ポリゴン拡大*/
	if( GetKeyboardTrigger( DIK_UP ) )
	{
		polygonMag = ( polygonMag + MAG_SPEED > MAG_MAX ? MAG_MAX : polygonMag + MAG_SPEED );
	}
	if( GetKeyboardTrigger( DIK_DOWN ) )
	{
		polygonMag = ( polygonMag - MAG_SPEED < MAG_MIN ? MAG_MIN : polygonMag - MAG_SPEED );
	}
	
	/*頂点座標設定*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );


	DebugMessage("こゃーん", 50 , 50 );
	/*ポインタのアンロック*/
	g_pVtxBuffbullet->Unlock();
}

/*******************************************************************************
* 関数名	:void DrawBullet( void )
*		弾の描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawBullet( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturebullet );
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffbullet , 0 , sizeof( VERTEX_2D ) );

	/*弾のの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}