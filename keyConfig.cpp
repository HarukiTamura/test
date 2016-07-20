/*******************************************************************************
* タイトル	:キーコンフィグ.cpp
* ファイル名:keyConfig.cpp
* 作成者	:田村春樹
* 作成日	:2015/07/29
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/

//改善必要箇所
//87行目	内容が違う場合のErrorも検討すること

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "keyConfig.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define CONFIG_FILE_ADDRESS		( "data/keyConfig.dat" )

#define USE_DEFAULT_KEY_DATA_FILE_ADDRESS	( NULL )	
	//コードにデフォルトの情報を直書きならNULL
	//ファイルから読み込むならdatファイルパスを指定

#define KEY_ON	( 0x80 )
#define KEY_OFF	( 0x00 )
/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static FILE *fp_config;
static FILE *fp_configDefault;
static int *gp_useKeyData;

/*なんらかの操作確認フラグ*/
static bool g_pushAnykey;

/*******************************************************************************
* 関数名	:HRESULT InitKeyConfig( void )
*		コンフィグ情報初期化関数です。
* 引数		:void		引数無し
* 戻り値	:HRESULT	関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitKeyConfig( void )
{
	fp_config = NULL;
	int keyDataCnt = 0;
	bool defaultConfigFileExist = false;
	/*
	//キー情報を確保と初期化（とりあえずエンターキーにしておきます）
	gp_useKeyData = new int[USE_KEY_MAX];
	for( keyDataCnt = 0; keyDataCnt < USE_KEY_MAX; keyDataCnt++ )
	{
		gp_useKeyData[keyDataCnt] = DIK_RETURN;
	}

	//ファイル有無の確認
	fp_config = fopen( CONFIG_FILE_ADDRESS , "r" );

	//ファイルを読み込めなかった（存在しない）場合実行
	if( fp_config == NULL )
	{
		//デフォルトのコンフィグ情報ファイルの指定がある場合のみ実行
		if( USE_DEFAULT_KEY_DATA_FILE_ADDRESS  != NULL )
		{	
			//デフォルトのコンフィグ情報ファイルを開く
			fp_configDefault = fopen( USE_DEFAULT_KEY_DATA_FILE_ADDRESS , "r" );
			
			//ファイルを読み込めなかった（存在しない）場合に実行
			if( fp_config == NULL )
			{			
				//Error("ファイルからの標準キー設定情報の読み込みに失敗しました。");
				defaultConfigFileExist = false;
			}
			else
			{	
				//ファイルが読み込めた場合、データを読み込む
				//内容が違う場合のErrorも検討すること

				fp_config = fopen( CONFIG_FILE_ADDRESS , "w" );
				for( keyDataCnt = 0; keyDataCnt < USE_KEY_MAX; keyDataCnt++ )
				{
					int keyDataCntDummy = 0;
					fscanf( fp_configDefault , "USE KEY[%d] = %d\n" , &keyDataCntDummy , &gp_useKeyData[keyDataCnt] );
					fprintf( fp_config , "USE KEY[%d] = %d\n" , keyDataCnt , gp_useKeyData[keyDataCnt] );
				}
				fclose( fp_config );
				fclose( fp_configDefault );
				defaultConfigFileExist = true;
			}
		}
		//デフォルトファイルが無い、或いは読み込みに失敗した場合。
		if( defaultConfigFileExist == false )
		{
			//ここに直書きする場合は書いてください
			//例:gp_useKeyData[keyDataCnt] = DIK_RETURN;
			gp_useKeyData[SHOT_KEY] = DIK_SPACE;
			gp_useKeyData[BARRIER_KEY] = DIK_C;
		}
		fp_config = NULL;
		fp_configDefault = NULL;
	}
	else//記録していたファイルを開く事に成功した場合
	{

		for( keyDataCnt = 0; keyDataCnt < USE_KEY_MAX; keyDataCnt++ )
		{
			int keyDataCntDummy = 0;
			fscanf( fp_config , "USE KEY[%d] = %d\n" , &keyDataCntDummy , &gp_useKeyData[keyDataCnt] );
		}
		fclose( fp_config );
		fp_config = NULL;
	}
	*/
	return S_OK;
}

/*******************************************************************************
* 関数名	:HRESULT InitKeyConfig( void )
*		コンフィグ情報初期化関数です。
* 引数		:void		引数無し
* 戻り値	:HRESULT	関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
void UpdateKeyConfig( void )
{

}

/*******************************************************************************
* 関数名	:void UninitKeyConfig( void )
*		入力終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitKeyConfig( void )
{
	//キー情報を解放
	delete [] gp_useKeyData;
}

/*******************************************************************************
* 関数名	:bool GetKeyboardPressConfig( int nKeyData )
*		キーボードからコンフィグされやキーのPress入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectKeyConfigに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardPressConfig( int nKeyData )
{
	return GetKeyboardPress(gp_useKeyData[nKeyData]);
}

/*******************************************************************************
* 関数名	:bool GetKeyboardTriggerConfig( int nKeyData )
*		キーボードからコンフィグされやキーのTrigger入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectKeyConfigに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardTriggerConfig( int nKeyData )
{
	return GetKeyboardTrigger(gp_useKeyData[nKeyData]);
}

/*******************************************************************************
* 関数名	:bool GetKeyboardReleaseConfig( int nKeyData )
*		キーボードからコンフィグされやキーのRelease入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectKeyConfigに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardReleaseConfig( int nKeyData )
{
	return GetKeyboardRelease(gp_useKeyData[nKeyData]);
}

/*******************************************************************************
* 関数名	:bool GetKeyboardRepeatConfig( int nKeyData )
*		キーボードからコンフィグされたキーのRepeat入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectKeyConfigに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardRepeatConfig( int nKeyData )
{
	return GetKeyboardRepeat(gp_useKeyData[nKeyData]);
}