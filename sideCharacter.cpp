/*******************************************************************************
* タイトル	:キャラ＆背景.cpp
* ファイル名:sideCharacter.cpp
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
#include "sideCharacter.h"
#include "input.h"
#include "player01.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

#define SIDECHARACTER_VERTEX_MAX	( 4 )	/*最高頂点数*/

#define SIDECHARACTER_ANIM_MAX		( 1 )	/*アニメーションコマ数*/
#define SIDECHARACTER_ANIM_SPAN	( 30 )	/*アニメーション間隔*/
#define SIDECHARACTER_ANIM_DIV_X	( 6 )	/*幅分割数*/
#define SIDECHARACTER_ANIM_DIV_Y	( 1 )	/*高さ分割数*/

#define SIDECHARACTER_OUTPUT_SIZE	( 1 ) /*初期表示倍率*/ 

typedef enum
{
	CHARACTER_NULL = -1,
	CHARACTER_NORMAL,
	CHARACTER_DAMAGE,
	CHARACTER_HYPERMODE,
	CHARACTER_MODEMAX
}CHARACTER_MODE;
/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffsideCharacter = NULL;	/*頂点バッファ*/
static LPDIRECT3DTEXTURE9 g_pTexturesideCharacter;	/*キャラ＆背景のテクスチャのポインタ*/
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

static CHARACTER_MODE mode = CHARACTER_NORMAL;
static CHARACTER_MODE pastMode = CHARACTER_NULL;

/*******************************************************************************
* 関数名	:HRESULT InitSideCharacter( void )
*		キャラ＆背景の初期化関数です。
* 引数		:void 引数なし
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitSideCharacter( void )
{
	int outputSizeX,outputSizeY;
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*デバイスの取得*/
	
	/*頂点バッファの生成*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * SIDECHARACTER_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffsideCharacter , NULL ) ) )
	{
		Error("頂点バッファの生成に失敗しました。");
		return E_FAIL;
	}
	
	/*テクスチャの読み込み*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/sideCharacter.png" , &g_pTexturesideCharacter );
	D3DXGetImageInfoFromFile( "data/TEXTURE/sideCharacter.png" , &info );

	/*ポインタのロック*/
	g_pVtxBuffsideCharacter->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / SIDECHARACTER_ANIM_DIV_X ) * SIDECHARACTER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / SIDECHARACTER_ANIM_DIV_Y ) * SIDECHARACTER_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , 1 );

	/*ポインタのアンロック*/
	g_pVtxBuffsideCharacter->Unlock();

	/*回転用ステータス設定*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*拡大用ステータス*/
	polygonMag = 1;

	/*加速度制御用ステータス設定*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	mode = CHARACTER_NORMAL;
	pastMode = CHARACTER_NULL;
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitSideCharacter( void )
*		キャラ＆背景の終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitSideCharacter( void )
{
	if( g_pVtxBuffsideCharacter != NULL )
	{
		g_pVtxBuffsideCharacter->Release();
		g_pVtxBuffsideCharacter = NULL;
	}
	if( g_pTexturesideCharacter != NULL )
	{
		g_pTexturesideCharacter->Release();
		g_pTexturesideCharacter = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateSideCharacter( void )
*		キャラ＆背景の更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateSideCharacter( void )
{
	VERTEX_2D *pVtx;	/*頂点バッファへのポインタ*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*アニメーション用バッファ*/

	/*ポインタのロック*/ 
	g_pVtxBuffsideCharacter->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

#ifdef _DEBUG
	if( GetKeyboardPress( DIK_1 ) )
	{
		mode = CHARACTER_NORMAL;
	}
	if( GetKeyboardPress( DIK_2 ) )
	{
		mode = CHARACTER_DAMAGE;
	}
	if( GetKeyboardPress( DIK_3 ) )
	{
		mode = CHARACTER_HYPERMODE;
	}
#endif

	//MODE変更部
	if( GetPlayerState01()->mode == PLAYER_MODE_HYPERMODE )
	{
		mode = CHARACTER_HYPERMODE;
	}
	else
	{
		if( GetPlayerState01()->mode == PLAYER_MODE_DAMAGE )
		{
			mode = CHARACTER_DAMAGE;
		}
		else
		{
			mode = CHARACTER_NORMAL;
		}
	}

	if( mode != pastMode )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = 0;
	}
	/*パターンアニメーション事計算部分*/
	switch( mode )
	{
		case CHARACTER_NORMAL:
			{
				g_nCounterAnim++;
				if( g_nPatternAnim > 2 )
				{
					g_nPatternAnim = 0;
					g_nCounterAnim = 0; 
				}
				if( g_nPatternAnim == 0 && g_nCounterAnim >= 72 )
				{
					g_nPatternAnim++;
					g_nCounterAnim = 0; 
				}
				if( g_nPatternAnim == 1 && g_nCounterAnim >= 5 )
				{
					g_nPatternAnim++;
					g_nCounterAnim = 0; 
				}
				if( g_nPatternAnim == 2 && g_nCounterAnim >= 5 )
				{
					g_nPatternAnim = 0;
					g_nCounterAnim = 0; 
				}
				break;
			}
		case CHARACTER_DAMAGE:
			{
				if( g_nPatternAnim != 1 )
				{
					g_nPatternAnim = 1; 
				}
				g_nCounterAnim++;
				if( g_nCounterAnim > 45 )
				{
					g_nPatternAnim = 1; 
					mode = CHARACTER_NORMAL;
				}
				break;
			}
		case CHARACTER_HYPERMODE:
			{
				/*モード継続によるオーバーフロー防止*/
				g_nCounterAnim += ( g_nCounterAnim > 60 ? g_nCounterAnim : g_nCounterAnim + 1 );
				if( g_nPatternAnim < 3 )
				{
					g_nPatternAnim = 3;
					g_nCounterAnim = 0; 
				}
				if( g_nPatternAnim == 3 && g_nCounterAnim >= 5 )
				{
					g_nPatternAnim++;
					g_nCounterAnim = 0; 
				}
				if( g_nPatternAnim == 4 && g_nCounterAnim >= 5 )
				{
					g_nPatternAnim++;
					g_nCounterAnim = 0; 
				}
				break;
			}
		default :
			{
				break;
			}
	}
				
	texWidth = 1.0f / SIDECHARACTER_ANIM_DIV_X;
	texHeight = 1.0f / SIDECHARACTER_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % SIDECHARACTER_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / SIDECHARACTER_ANIM_DIV_X ) ) + 0;
	
	/*テクスチャ座標生成*/
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	
	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );

	/*頂点座標設定*/
	pVtx[0].pos.x = SCREEN_LEFT;
	pVtx[0].pos.y = SCREEN_TOP;
	pVtx[0].pos.z = 0.0f;
		
	pVtx[1].pos.x = SCREEN_RIGHT;
	pVtx[1].pos.y = SCREEN_TOP;
	pVtx[1].pos.z = 0.0f;
		
	pVtx[2].pos.x = SCREEN_LEFT;
	pVtx[2].pos.y = SCREEN_BOTTOM;
	pVtx[2].pos.z = 0.0f;
		 
	pVtx[3].pos.x = SCREEN_RIGHT;
	pVtx[3].pos.y = SCREEN_BOTTOM;
	pVtx[3].pos.z = 0.0f;

	
	/*ポインタのアンロック*/
	g_pVtxBuffsideCharacter->Unlock();

	pastMode = mode;
}

/*******************************************************************************
* 関数名	:void DrawSideCharacter( void )
*		キャラ＆背景の描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void DrawSideCharacter( void )
{
	/*デバイスの取得*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*頂点フォーマットの設定*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*頂点バッファをストリームにバインド*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffsideCharacter , 0 , sizeof( VERTEX_2D ) );

	/*テクスチャの設定*/
	pD3DDevice->SetTexture( 0 , g_pTexturesideCharacter );

	/*キャラ＆背景のの描画*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}