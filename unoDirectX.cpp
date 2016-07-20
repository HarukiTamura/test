/*******************************************************************************
* タイトル	:自作DirectX便利系header
* ファイル名:unoMath.h
* 作成者	:田村春樹
* 作成日	:2015/07/03
********************************************************************************
* 更新履歴	ver 1.00 おおよその完成
*
*******************************************************************************/

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "unoDirectX.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define QUADRANGLE_VERTEX_NUM ( 4 )

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名	:void vertexInit( VERTEX_2D *p_vertex , int vertexNum )
*		VERTEX_2D、頂点バッファの値を初期化します。
* 引数		:VERTEX_2D *p_vertex	初期化するバッファの先頭アドレス	
*			 int vertexNum			初期化する頂点数
* 戻り値	:void 戻り値なし
* 説明		:VERTEX_2Dの値を初期化します。
*******************************************************************************/
void vertexInit( VERTEX_2D *p_vertex , int vertexNum )
{
	for( int nCntVertex = 0; nCntVertex < vertexNum; nCntVertex++ )
	{
		/*頂点座標設定*/
		p_vertex[nCntVertex].pos.x = 0.0f;
		p_vertex[nCntVertex].pos.y = 0.0f;
		p_vertex[nCntVertex].pos.z = 0.0f;

		/*座標変換用係数設定*/
		p_vertex[nCntVertex].rhw = 1.0f;
	
		/*頂点カラー設定*/
		p_vertex[nCntVertex].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		/*テクスチャ座標設定*/
		p_vertex[nCntVertex].tex.x = 0.0f;
		p_vertex[nCntVertex].tex.y = 0.0f;
		p_vertex[nCntVertex].tex.x = 0.0f;
	}
}


/*******************************************************************************
* 関数名	:void vertexInitQuad( VERTEX_2D *p_vertex , int polygonNum )
*		VERTEX_2D、頂点バッファの値をポリゴン単位で初期化します。
* 引数		:VERTEX_2D *p_vertex	初期化するバッファの先頭アドレス	
*			 int vertexNum			初期化する頂点数
* 戻り値	:void 戻り値なし
* 説明		:VERTEX_2Dの値をポリゴン単位で初期化します。
*******************************************************************************/
void vertexInitQuad( VERTEX_2D *p_vertex , int polygonNum = 1 )
{
	for( int nCntPolygon = 0; nCntPolygon < polygonNum; nCntPolygon++ )
	{
		/*頂点座標設定*/
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 0].pos = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 1].pos = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 2].pos = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 3].pos = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

		/*座標変換用係数設定*/
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 0].rhw = 1.0f;
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 1].rhw = 1.0f;
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 2].rhw = 1.0f;
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 3].rhw = 1.0f;
	
		/*頂点カラー設定*/
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
		/*テクスチャ座標設定*/
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 0].tex = D3DXVECTOR2( 0 , 0 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 1].tex = D3DXVECTOR2( 1 , 0 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 2].tex = D3DXVECTOR2( 0 , 1 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 3].tex = D3DXVECTOR2( 1 , 1 );
	}
}