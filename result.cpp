/*******************************************************************************
* ���U���g	:�Q�[��.cpp
* �t�@�C����:Result.cpp
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
#include "result.h"
#include "input.h"
#include "fade.h"
#include "ResultGraphic.h"


#include "score1.h"
#include "score2.h"
#include "score3.h"

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
static int lastScore = 0;	//Player�̃X�R�A
unsigned long rankingScore[RANKER_LIST_MAX] = {0};
unsigned long playerScore = 0;
static bool whoPlayerFlag[RANKER_LIST_BUFF_MAX] = {0};

/*******************************************************************************
* �֐���	:HRESULT InitResult( void )
*		�Q�[���������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitResult( void )
{
	FILE *fp;
	PlaySound(SOUND_LABEL_BGM_ED);
	/*���U���g�O���t�B�b�N�̐ݒ�*/
	if( InitResultGraphic() != S_OK )
	{
		Error("�|���S���̏������Ɏ��s���܂����B");
	}
	/*�X�R�A�����L���O�̐ݒ�*/
	if( InitScore1() != S_OK )
	{
		Error("�X�R�A�����L���O�̏������Ɏ��s���܂����B");
	}
	/*�X�R�A�����L���O�̐ݒ�*/
	if( InitScore2() != S_OK )
	{
		Error("�X�R�A�����L���O�̏������Ɏ��s���܂����B");
	}
	/*�X�R�A�����L���O�̐ݒ�*/
	if( InitScore3() != S_OK )
	{
		Error("�X�R�A�����L���O�̏������Ɏ��s���܂����B");
	}
	
	for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
	{
		rankingScore[cnt] = 0; 
	}
	for( int cnt = 0; cnt < RANKER_LIST_BUFF_MAX; cnt++ )
	{
		whoPlayerFlag[cnt] = false;
	}
	
	//�ߋ��X�R�A�f�[�^�ǂݍ���
	if( ( fp = fopen( "data/sch.dat" , "rb" ) ) == NULL )
	{	
		//ERROR�F���f�[�^���݂Ȃ�
		//�Ώ��F�f�t�H���g�f�[�^����

		//�f�t�H���g�̃X�R�A�f�[�^��p��
		if( ( fp = fopen( "data/sch.dat" , "wb" ) ) == NULL )
		{
			Error("ERROR:�X�R�A�f�[�^�̍쐬�Ɏ��s���܂����B");
		}
		else
		{
			rankingScore[0] = 8000000;
			rankingScore[1] = 4000000;
			rankingScore[2] = 2000000;
			for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
			{
				fwrite( &rankingScore[cnt]  ,sizeof( unsigned long ) , 1 , fp );
			}
		}
		fclose(fp);
	}
	else
	{
		//�ߋ��X�R�A�f�[�^�̓ǂݍ���
		for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
		{
			fread( &rankingScore[cnt]  , sizeof( unsigned long ) , 1 , fp );
		}
		fclose(fp);
	}
	
	//�ŐV�v���C���[�X�R�A�ǂݍ���
	if( ( fp = fopen( "data/scs.dat" , "rb" ) ) == NULL )
	{	
		Error("ERROR:�v���C���[�ŐV�X�R�A�̓ǂݍ��݂Ɏ��s���܂����B");
		playerScore = 0;
	}
	else
	{
		fread( &playerScore , sizeof( unsigned long ) , 1 , fp );
		fclose(fp);
	}

	//�X�R�A�̃\�[�g�p��
	unsigned long scoreBuff[RANKER_LIST_BUFF_MAX];
	scoreBuff[0] = playerScore;
	whoPlayerFlag[0] = true;
	for( int cnt = 1; cnt < RANKER_LIST_BUFF_MAX; cnt++ )
	{
		scoreBuff[cnt] = rankingScore[cnt - 1];	
	}
	
	//�X�R�A�\�[�g����
	unsigned long sortBuff;
	bool flagBuff;
	for( int cnt1 = 0; cnt1 < RANKER_LIST_BUFF_MAX; cnt1++ )
	{
		for( int cnt2 = 0; cnt2 < RANKER_LIST_BUFF_MAX; cnt2++ )
		{
			if( scoreBuff[cnt1] > scoreBuff[cnt2] )
			{
				sortBuff = scoreBuff[cnt1];
				scoreBuff[cnt1] = scoreBuff[cnt2];
				scoreBuff[cnt2] = sortBuff;

				flagBuff = whoPlayerFlag[cnt1]; 
				whoPlayerFlag[cnt1] = whoPlayerFlag[cnt2];
				whoPlayerFlag[cnt2] = flagBuff;
			}
		}
	}
	
	//�X�R�A��������
	for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
	{
		rankingScore[cnt] = scoreBuff[cnt];
	}

	//�X�R�A�ݒ�
	resetScore1( rankingScore[0] );
	resetScore2( rankingScore[1] );
	resetScore3( rankingScore[2] );

	
	//�ŐV�X�R�A�f�[�^��������
	if( ( fp = fopen( "data/sch.dat" , "wb" ) ) == NULL )
	{
		Error("ERROR:�X�R�A�f�[�^�̏����o���Ɏ��s���܂����B");
	}
	else
	{
		for( int cnt = 0; cnt < RANKER_LIST_MAX; cnt++ )
		{
			fwrite( &rankingScore[cnt]  ,sizeof( unsigned long ) , 1 , fp );
		}
		fclose(fp);
	}

	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitResult( void )
*		�Q�[���I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitResult( void )
{
	
	/*���U���g�O���t�B�b�N�̉��*/
	UninitResultGraphic();

	/*�X�R�A�����L���O�̉��*/
	UninitScore1();
	/*�X�R�A�����L���O�̉��*/
	UninitScore2();
	/*�X�R�A�����L���O�̉��*/
	UninitScore3();
	


}

/*******************************************************************************
* �֐���	:void UpdateResult( void )
*		�Q�[���X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateResult( void )
{
	
	/*���U���g�O���t�B�b�N�̍X�V*/
	UpdateResultGraphic();



	/*�X�R�A�����L���O�̍X�V*/
	UpdateScore1();
	/*�X�R�A�����L���O�̍X�V*/
	UpdateScore2();
	/*�X�R�A�����L���O�̍X�V*/
	UpdateScore3();

	if( GetFade() == FADE_NONE && ( GetKeyboardTrigger( DIK_RETURN ) || GetGamePadTrigger( 0 ) ) )
	{
		PlaySound(SOUND_LABEL_SE_ENTER);
		SetFade( FADE_OUT , MODE_TITLE ); 
	}
}

/*******************************************************************************
* �֐���	:void DrawResult( void )
*		�Q�[���`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawResult( void )
{
	
	/*���U���g�O���t�B�b�N�̕`��*/
	DrawResultGraphic();
	
	/*�X�R�A�����L���O�̕`��*/
	DrawScore1();
	/*�X�R�A�����L���O�̕`��*/
	DrawScore2();
	/*�X�R�A�����L���O�̕`��*/
	DrawScore3();
}