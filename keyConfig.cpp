/*******************************************************************************
* �^�C�g��	:�L�[�R���t�B�O.cpp
* �t�@�C����:keyConfig.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/07/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/

//���P�K�v�ӏ�
//87�s��	���e���Ⴄ�ꍇ��Error���������邱��

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "keyConfig.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define CONFIG_FILE_ADDRESS		( "data/keyConfig.dat" )

#define USE_DEFAULT_KEY_DATA_FILE_ADDRESS	( NULL )	
	//�R�[�h�Ƀf�t�H���g�̏��𒼏����Ȃ�NULL
	//�t�@�C������ǂݍ��ނȂ�dat�t�@�C���p�X���w��

#define KEY_ON	( 0x80 )
#define KEY_OFF	( 0x00 )
/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static FILE *fp_config;
static FILE *fp_configDefault;
static int *gp_useKeyData;

/*�Ȃ�炩�̑���m�F�t���O*/
static bool g_pushAnykey;

/*******************************************************************************
* �֐���	:HRESULT InitKeyConfig( void )
*		�R���t�B�O��񏉊����֐��ł��B
* ����		:void		��������
* �߂�l	:HRESULT	�֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitKeyConfig( void )
{
	fp_config = NULL;
	int keyDataCnt = 0;
	bool defaultConfigFileExist = false;
	/*
	//�L�[�����m�ۂƏ������i�Ƃ肠�����G���^�[�L�[�ɂ��Ă����܂��j
	gp_useKeyData = new int[USE_KEY_MAX];
	for( keyDataCnt = 0; keyDataCnt < USE_KEY_MAX; keyDataCnt++ )
	{
		gp_useKeyData[keyDataCnt] = DIK_RETURN;
	}

	//�t�@�C���L���̊m�F
	fp_config = fopen( CONFIG_FILE_ADDRESS , "r" );

	//�t�@�C����ǂݍ��߂Ȃ������i���݂��Ȃ��j�ꍇ���s
	if( fp_config == NULL )
	{
		//�f�t�H���g�̃R���t�B�O���t�@�C���̎w�肪����ꍇ�̂ݎ��s
		if( USE_DEFAULT_KEY_DATA_FILE_ADDRESS  != NULL )
		{	
			//�f�t�H���g�̃R���t�B�O���t�@�C�����J��
			fp_configDefault = fopen( USE_DEFAULT_KEY_DATA_FILE_ADDRESS , "r" );
			
			//�t�@�C����ǂݍ��߂Ȃ������i���݂��Ȃ��j�ꍇ�Ɏ��s
			if( fp_config == NULL )
			{			
				//Error("�t�@�C������̕W���L�[�ݒ���̓ǂݍ��݂Ɏ��s���܂����B");
				defaultConfigFileExist = false;
			}
			else
			{	
				//�t�@�C�����ǂݍ��߂��ꍇ�A�f�[�^��ǂݍ���
				//���e���Ⴄ�ꍇ��Error���������邱��

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
		//�f�t�H���g�t�@�C���������A�����͓ǂݍ��݂Ɏ��s�����ꍇ�B
		if( defaultConfigFileExist == false )
		{
			//�����ɒ���������ꍇ�͏����Ă�������
			//��:gp_useKeyData[keyDataCnt] = DIK_RETURN;
			gp_useKeyData[SHOT_KEY] = DIK_SPACE;
			gp_useKeyData[BARRIER_KEY] = DIK_C;
		}
		fp_config = NULL;
		fp_configDefault = NULL;
	}
	else//�L�^���Ă����t�@�C�����J�����ɐ��������ꍇ
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
* �֐���	:HRESULT InitKeyConfig( void )
*		�R���t�B�O��񏉊����֐��ł��B
* ����		:void		��������
* �߂�l	:HRESULT	�֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void UpdateKeyConfig( void )
{

}

/*******************************************************************************
* �֐���	:void UninitKeyConfig( void )
*		���͏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitKeyConfig( void )
{
	//�L�[�������
	delete [] gp_useKeyData;
}

/*******************************************************************************
* �֐���	:bool GetKeyboardPressConfig( int nKeyData )
*		�L�[�{�[�h����R���t�B�O�����L�[��Press���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectKeyConfig�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardPressConfig( int nKeyData )
{
	return GetKeyboardPress(gp_useKeyData[nKeyData]);
}

/*******************************************************************************
* �֐���	:bool GetKeyboardTriggerConfig( int nKeyData )
*		�L�[�{�[�h����R���t�B�O�����L�[��Trigger���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectKeyConfig�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardTriggerConfig( int nKeyData )
{
	return GetKeyboardTrigger(gp_useKeyData[nKeyData]);
}

/*******************************************************************************
* �֐���	:bool GetKeyboardReleaseConfig( int nKeyData )
*		�L�[�{�[�h����R���t�B�O�����L�[��Release���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectKeyConfig�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardReleaseConfig( int nKeyData )
{
	return GetKeyboardRelease(gp_useKeyData[nKeyData]);
}

/*******************************************************************************
* �֐���	:bool GetKeyboardRepeatConfig( int nKeyData )
*		�L�[�{�[�h����R���t�B�O���ꂽ�L�[��Repeat���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectKeyConfig�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardRepeatConfig( int nKeyData )
{
	return GetKeyboardRepeat(gp_useKeyData[nKeyData]);
}