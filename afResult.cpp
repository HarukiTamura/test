/*******************************************************************************
* ���U���g	:�Q�[��.cpp
* �t�@�C����:AfResult.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "afResult.h"
#include "input.h"
#include "fade.h"
#include "afResultGraphic.h"


#include "afScore1.h"
#include "afScore2.h"
#include "afScore3.h"
#include "afScoreMy.h"
#include "BackPlusAlpha2.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define RANKER_LIST_MAX			( 3 )
#define RANKER_LIST_BUFF_MAX	( 1 + RANKER_LIST_MAX )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static int lastAfScore = 0;	//Player�̃X�R�A
unsigned long rankingAfScore[RANKER_LIST_MAX] = {0};
unsigned long playerAfScore = 0;
static bool whoPlayerFlag[RANKER_LIST_BUFF_MAX] = {0};

/*******************************************************************************
* �֐���	:HRESULT InitAfResult( void )
*		�Q�[���������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfResult( void )
{
	FILE *fp;
	PlaySound(SOUND_LABEL_BGM_ED);
	/*���U���g�O���t�B�b�N�̐ݒ�*/
	if( InitAfResultGraphic() != S_OK )
	{
		Error("�|���S���̏������Ɏ��s���܂����B");
	}
	/*���U���g�O���t�B�b�N�̐ݒ�*/
	if( InitBackPlusAlpha2() != S_OK )
	{
		Error("�|���S���̏������Ɏ��s���܂����B");
	}
	/*�X�R�A�����L���O�̐ݒ�*/
	if( InitAfScore1() != S_OK )
	{
		Error("�X�R�A�����L���O�̏������Ɏ��s���܂����B");
	}
	/*�X�R�A�����L���O�̐ݒ�*/
	if( InitAfScore2() != S_OK )
	{
		Error("�X�R�A�����L���O�̏������Ɏ��s���܂����B");
	}
	/*�X�R�A�����L���O�̐ݒ�*/
	if( InitAfScore3() != S_OK )
	{
		Error("�X�R�A�����L���O�̏������Ɏ��s���܂����B");
	}
	/*�X�R�A�����L���O�̐ݒ�*/
	if( InitAfScoreMy() != S_OK )
	{
		Error("�X�R�A�����L���O�̏������Ɏ��s���܂����B");
	}
	
	for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
	{
		rankingAfScore[cnt] = 0; 
	}
	for( int cnt = 0; cnt < RANKER_LIST_BUFF_MAX; cnt++ )
	{
		whoPlayerFlag[cnt] = false;
	}
	
	//�ߋ��X�R�A�f�[�^�ǂݍ���
	if( ( fp = fopen( "data/schaf.dat" , "rb" ) ) == NULL )
	{	
		//ERROR�F���f�[�^���݂Ȃ�
		//�Ώ��F�f�t�H���g�f�[�^����

		//�f�t�H���g�̃X�R�A�f�[�^��p��
		if( ( fp = fopen( "data/schaf.dat" , "wb" ) ) == NULL )
		{
			Error("ERROR:�X�R�A�f�[�^�̍쐬�Ɏ��s���܂����B");
		}
		else
		{
			rankingAfScore[0] = 80;
			rankingAfScore[1] = 40;
			rankingAfScore[2] = 20;
			for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
			{
				fwrite( &rankingAfScore[cnt]  ,sizeof( unsigned long ) , 1 , fp );
			}
		}
		fclose(fp);
	}
	else
	{
		//�ߋ��X�R�A�f�[�^�̓ǂݍ���
		for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
		{
			fread( &rankingAfScore[cnt]  , sizeof( unsigned long ) , 1 , fp );
		}
		fclose(fp);
	}
	
	//�ŐV�v���C���[�X�R�A�ǂݍ���
	if( ( fp = fopen( "data/scsaf.dat" , "rb" ) ) == NULL )
	{	
		Error("ERROR:�v���C���[�ŐV�X�R�A�̓ǂݍ��݂Ɏ��s���܂����B");
		playerAfScore = 0;
	}
	else
	{
		fread( &playerAfScore , sizeof( unsigned long ) , 1 , fp );
		fclose(fp);
	}

	//�X�R�A�̃\�[�g�p��
	unsigned long afScoreBuff[RANKER_LIST_BUFF_MAX];
	afScoreBuff[0] = playerAfScore;
	whoPlayerFlag[0] = true;
	for( int cnt = 1; cnt < RANKER_LIST_BUFF_MAX; cnt++ )
	{
		afScoreBuff[cnt] = rankingAfScore[cnt - 1];	
	}
	
	//�X�R�A�\�[�g����
	unsigned long sortBuff;
	bool flagBuff;
	for( int cnt1 = 0; cnt1 < RANKER_LIST_BUFF_MAX; cnt1++ )
	{
		for( int cnt2 = 0; cnt2 < RANKER_LIST_BUFF_MAX; cnt2++ )
		{
			if( afScoreBuff[cnt1] > afScoreBuff[cnt2] )
			{
				sortBuff = afScoreBuff[cnt1];
				afScoreBuff[cnt1] = afScoreBuff[cnt2];
				afScoreBuff[cnt2] = sortBuff;

				flagBuff = whoPlayerFlag[cnt1]; 
				whoPlayerFlag[cnt1] = whoPlayerFlag[cnt2];
				whoPlayerFlag[cnt2] = flagBuff;
			}
		}
	}
	
	//�X�R�A��������
	for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
	{
		rankingAfScore[cnt] = afScoreBuff[cnt];
	}

	//�X�R�A�ݒ�
	resetAfScore1( rankingAfScore[0] );
	resetAfScore2( rankingAfScore[1] );
	resetAfScore3( rankingAfScore[2] );
	resetAfScoreMy( playerAfScore );

	
	//�ŐV�X�R�A�f�[�^��������
	if( ( fp = fopen( "data/schaf.dat" , "wb" ) ) == NULL )
	{
		Error("ERROR:�X�R�A�f�[�^�̏����o���Ɏ��s���܂����B");
	}
	else
	{
		for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
		{
			fwrite( &rankingAfScore[cnt]  ,sizeof( unsigned long ) , 1 , fp );
		}
		fclose(fp);
	}

	//PlaySound(SOUND_LABEL_SE_RESULT);

	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitAfResult( void )
*		�Q�[���I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfResult( void )
{
	
	/*���U���g�O���t�B�b�N�̉��*/
	UninitAfResultGraphic();

	/*�X�R�A�����L���O�̉��*/
	UninitAfScore1();
	/*�X�R�A�����L���O�̉��*/
	UninitAfScore2();
	/*�X�R�A�����L���O�̉��*/
	UninitAfScore3();
	/*�X�R�A�����L���O�̉��*/
	UninitAfScoreMy();

	UninitBackPlusAlpha2();


}

/*******************************************************************************
* �֐���	:void UpdateAfResult( void )
*		�Q�[���X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfResult( void )
{
	
	/*���U���g�O���t�B�b�N�̍X�V*/
	UpdateAfResultGraphic();

	/*�X�R�A�����L���O�̍X�V*/
	UpdateAfScore1();
	/*�X�R�A�����L���O�̍X�V*/
	UpdateAfScore2();
	/*�X�R�A�����L���O�̍X�V*/
	UpdateAfScore3();
	/*�X�R�A�����L���O�̍X�V*/
	UpdateAfScoreMy();

	UpdateBackPlusAlpha2();

	if( GetFade() == FADE_NONE && ( GetKeyboardTrigger( DIK_RETURN ) || GetGamePadTrigger( 0 ) ) )
	{
		PlaySound(SOUND_LABEL_SE_ENTER);
		SetFade( FADE_OUT , MODE_TITLE ); 
	}
}

/*******************************************************************************
* �֐���	:void DrawAfResult( void )
*		�Q�[���`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfResult( void )
{
	
	/*���U���g�O���t�B�b�N�̕`��*/
	DrawAfResultGraphic();
	DrawBackPlusAlpha2();
	/*�X�R�A�����L���O�̕`��*/
	DrawAfScoreMy();
	/*�X�R�A�����L���O�̕`��*/
	DrawAfScore1();
	/*�X�R�A�����L���O�̕`��*/
	DrawAfScore2();
	/*�X�R�A�����L���O�̕`��*/
	DrawAfScore3();
}

//�v���C���[�X�R�A�Q�b�^�[
long getPlayerAfScoreResult( void )
{
	return playerAfScore;
}