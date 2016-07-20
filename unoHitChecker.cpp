/*******************************************************************************
* タイトル	:自作当たり判定cpp
* ファイル名:unoHitChecker.cpp
* 作成者	:田村春樹
* 作成日	:2015/07/05
********************************************************************************
* 更新履歴	ver 1.00 おおよその完成
*
*******************************************************************************/

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "unoHitChecker.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

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
* 関数名	:HitTwinCircle( int posX1 , int posY1 , int radius1 , int posX2 , int posY2 , int radius2 )
*		円と円の当たり判定を行います。
* 引数		:int posX1		円１のX座標
*			 int posY1		円１のY座標
*			 int radius1	円１の半径
*			 int posX2		円２のX座標
*			 int posY2		円２のY座標
*			 int radius2	円２の半径
* 戻り値	:bool true:接触 false:非接触
* 説明		:円と円の当たり判定を行います。
*******************************************************************************/
bool HitTwinCircle( int posX1 , int posY1 , int radius1 , int posX2 , int posY2 , int radius2 )
{
	double divisionX = 0, divisionY = 0, radiusPlus = 0;
	
	divisionX = posX2 - posX1;
	divisionY = posY2 - posY1;
	radiusPlus = radius1 + radius2;

	if( divisionX * divisionX + divisionY * divisionY <= radiusPlus * radiusPlus )
	{
		return true;
	}
	return false;
}


/*******************************************************************************
* 関数名	:bool HitTwinLine( int pastPosX1 , int pastPosY1 , int posX1 , int posY1 , int pastPosX2 , int pastPosY2 , int posX2 ,int posY2 )
*		線分と線分の当たり判定を行います。
* 引数		:int pastPosX1	線分１の始点X座標
*			 int pastPosY1	線分１の始点Y座標
*			 int posX1		線分１の終点X座標
*			 int posY1		線分１の終点Y座標
*			 int pastPosX2	線分２の始点X座標		
*			 int pastPosY2	線分２の始点Y座標		
*			 int posX2		線分２の終点X座標		
*			 int posY2		線分２の終点Y座標		
* 戻り値	:bool true:接触 false:非接触
* 説明		:線分と線分の当たり判定を行います。
*******************************************************************************/
bool HitTwinLine( int pastPosX1 , int pastPosY1 , int posX1 , int posY1 , int pastPosX2 , int pastPosY2 , int posX2 ,int posY2 )
{
	if( ( ( posX1 - pastPosX1 ) * ( pastPosY2 - pastPosY1 ) - ( posY1 - pastPosY1 ) * ( pastPosX2 - pastPosX1 ) )
	* ( ( posX1 - pastPosX1 ) * ( posY2 - pastPosY1 ) - ( posY1 - pastPosY1 ) * ( posX2 - pastPosX1 ) ) < 0 &&
	( ( posX2 - pastPosX2 ) * ( pastPosY1 - pastPosY2 ) - ( posY2 - pastPosY2 ) * ( pastPosX1 - pastPosX2 ) )
	* ( ( posX2 - pastPosX2 ) * ( posY1 - pastPosY2 ) - ( posY2 - pastPosY2 ) * ( posX1 - pastPosX2 ) ) < 0 )
	{
		return true;
	}
	return false;
}

/*******************************************************************************
* 関数名	:bool HitTwinLine( int pastPosX1 , int pastPosY1 , int posX1 , int posY1 , int pastPosX2 , int pastPosY2 , int posX2 ,int posY2 )
*		線分と線分の当たり判定を行います。
* 引数		:int pastPosX1	線分１の始点X座標
*			 int pastPosY1	線分１の始点Y座標
*			 int posX1		線分１の終点X座標
*			 int posY1		線分１の終点Y座標
*			 int pastPosX2	線分２の始点X座標		
*			 int pastPosY2	線分２の始点Y座標		
*			 int posX2		線分２の終点X座標		
*			 int posY2		線分２の終点Y座標		
* 戻り値	:bool true:接触 false:非接触
* 説明		:線分と線分の当たり判定を行います。
*******************************************************************************/
bool HitPosTwinLine( float posX1 , float posY1 , float posX2 , float posY2 , float posX3 , float posY3 , float posX4 ,float posY4 , float *closeX , float *closeY )
{
	float ksi = 0, eta = 0, delta = 0;
	float ramda = 0, mu = 0;

	ksi = ( posY4 - posY3 ) * ( posX3 - posX1 ) - ( posX4 - posX3 ) * ( posX4 - posX1 );

	eta = ( posX2 - posX1 ) * ( posY4 - posY1 ) - ( posY2 - posY1 ) * ( posX4 - posX1 );

	delta = ( posX2 - posX1 ) * ( posY4 - posY3 ) - ( posY2 - posY1 ) * ( posX4 - posX3 );

	ramda = ksi / delta;

	mu = eta / delta;

	if( ( ramda >= 0 && ramda <= 1 ) && ( mu >= 0 && mu <= 1 ) )
	{
		*closeX = posX1 + ramda * ( posX2 - posX1 );
		*closeY = posY1 + ramda * ( posY2 - posY1 );
		return true;
	}

	return false;
}