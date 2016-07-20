/*******************************************************************************
* �^�C�g��	:�X�R�A.cpp
* �t�@�C����:score.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/06/24
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "game.h"
#include "score.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define SCORE_MAX				( ULONG_MAX - 1 )	//�X�R�A�ő�l
#define SCORE_NUMBER_OF_DIGITS	( 10 )	//�X�R�A����
#define SCORE_WIDTH				( ( SCREEN_WIDTH - FIELD_RIGHT ) / ( SCORE_NUMBER_OF_DIGITS + 1 ) )	//�X�R�A������

#define SCORE_POS_X		( SCREEN_WIDTH - ( SCORE_WIDTH * SCORE_NUMBER_OF_DIGITS ) - ( SCORE_WIDTH * 0.6 ) )	//�X�R�A�\���ʒu
#define SCORE_POS_Y		( 100 )

#define SCORE_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define SCORE_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define SCORE_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define SCORE_ANIM_DIV_X	( 1 )	/*��������*/
#define SCORE_ANIM_DIV_Y	( 12 )	/*����������*/

#define SCORE_OUTPUT_SIZE	( 0.26 ) /*�����\���{��*/ 

#define MOVE_NUM ( 2.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.20f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.05f )	/*������*/


/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffscore = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturescore;	/*�X�R�A�̃e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;		/*�摜���*/

static D3DXVECTOR3 g_movePolygon;	/*�|���S���̈ʒu*/
static D3DXVECTOR3 g_posPolygon;	/*�|���S���̈ړ���*/
static D3DXVECTOR3 g_rotPolygon;	/*�|���S���̉�]�p*/
static float g_fLengthPolygon;		/*�|���S���̑Ίp���̒���*/
static float g_fAnglePolygon;		/*�|���S���̑Ίp���̊p�x*/
static float polygonWidth , polygonHeight;	/*�|���S���̕��^�����i�S�̂̔����j*/

static float polygonMag;	/*�\���{��*/

static int g_nCounterAnim = 0;	/*�A�j���[�V�����Ԋu�J�E���g*/
static int g_nPatternAnim = 0;	/*�p�^�[�����J�E���g*/

static int g_ScorePosX, g_ScorePosY;
static unsigned long nowScore = 0;
static unsigned long outputScore = 0;
static bool g_aScoreUseFlag[SCORE_NUMBER_OF_DIGITS];
/*******************************************************************************
* �֐���	:HRESULT InitScore( void )
*		�X�R�A�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitScore( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * SCORE_VERTEX_MAX * SCORE_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffscore , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/timer.png" , &g_pTexturescore );
	D3DXGetImageInfoFromFile( "data/TEXTURE/timer.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffscore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / SCORE_ANIM_DIV_X ) * SCORE_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / SCORE_ANIM_DIV_Y ) * SCORE_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , SCORE_NUMBER_OF_DIGITS );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffscore->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//�X�R�A������
	nowScore = 0;
	outputScore = 0;

	//�e���\���t���O������
	for( int cntScore = 0; cntScore < SCORE_NUMBER_OF_DIGITS; cntScore++ )
	{	
		g_aScoreUseFlag[cntScore] = false;
	}

	//�X�R�A��ʒu�ݒ�
	g_ScorePosX = SCORE_POS_X;
	g_ScorePosY = SCORE_POS_Y;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitScore( void )
*		�X�R�A�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitScore( void )
{
	if( g_pVtxBuffscore != NULL )
	{
		g_pVtxBuffscore->Release();
		g_pVtxBuffscore = NULL;
	}
	if( g_pTexturescore != NULL )
	{
		g_pTexturescore->Release();
		g_pTexturescore = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateScore( void )
*		�X�R�A�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateScore( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	int outputScoreNum = 0, nCntScore = SCORE_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	unsigned long scoreCpy = nowScore;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffscore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	if(	outputScore > scoreCpy )
	{
		outputScore -= ( outputScore - scoreCpy ) / 5;
	}
	else if( outputScore < scoreCpy )
	{
		outputScore += ( scoreCpy - outputScore ) / 5;
	}
	else
	{
		outputScore = scoreCpy;
	}

	//�e���\���t���O������
	nCntScore = SCORE_NUMBER_OF_DIGITS;
	while( nCntScore-- )
	{	
		g_aScoreUseFlag[nCntScore] = false;

		/*�p�^�[���A�j���[�V�������O�v�Z����*/
		if( ++g_nCounterAnim %= SCORE_ANIM_SPAN )
		{
			++g_nPatternAnim %= SCORE_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == SCORE_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / SCORE_ANIM_DIV_X;
		texHeight = 1.0f / SCORE_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % SCORE_ANIM_DIV_X );

		scoreCpy = outputScore;
		for( int cnt = 0; cnt < nCntScore; cnt++ )
		{
			scoreCpy /= 10;
		}

		outputScoreNum = scoreCpy % 10;
		if( outputScoreNum != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / SCORE_ANIM_DIV_X ) ) + outputScoreNum;
		
		if( numPrintFlag == true )
		{
			g_aScoreUseFlag[nCntScore] = true;
		}

		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		/*�e�N�X�`�����W����*/
		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		g_ScorePosX += SCORE_WIDTH;
		/*���_���W�ݒ�*/
		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].pos.x = g_ScorePosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].pos.y = g_ScorePosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].pos.x = g_ScorePosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].pos.y = g_ScorePosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].pos.x = g_ScorePosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].pos.y = g_ScorePosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].pos.x = g_ScorePosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].pos.y = g_ScorePosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore * SCORE_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_ScorePosX = SCORE_POS_X;
	g_ScorePosY = SCORE_POS_Y;
	g_aScoreUseFlag[0] = true;
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffscore->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawScore( void )
*		�X�R�A�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawScore( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffscore , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturescore );
	
	for( int nCntScore = 0; nCntScore < SCORE_NUMBER_OF_DIGITS; nCntScore++ )
	{
		if( g_aScoreUseFlag[nCntScore] == true )
		{
			/*�X�R�A�̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntScore * SCORE_VERTEX_MAX , 2 );
		}
	}
	
}

/*******************************************************************************
* �֐���	:void PlusScore( unsigned long plusNum )
*		�X�R�A���Z�֐��ł��B
* ����		:unsigned long plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z���܂��B
*******************************************************************************/
void PlusScore( long plusNum )
{
	long long buff,buff2;
	
	buff = ( long long )( nowScore );
	buff2 = ( long long )( plusNum );

	if( buff + buff2 > SCORE_MAX )
	{
		nowScore = SCORE_MAX;
	}	
	else
	{
		if( -buff > buff2 )
		{
			nowScore = 0;
		}
		else
		{
			nowScore += plusNum;
		}
	}
}

/*******************************************************************************
* �֐���	:resetScore( unsigned long resetNum )
*		�X�R�A���Z�b�g�֐��ł��B
* ����		:unsigned long resetNum	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z�b�g���܂��B
*******************************************************************************/
void resetScore( unsigned long resetNum )
{
	if( resetNum >= 0 && nowScore <= SCORE_MAX )
	{
		nowScore = resetNum;
	}
}

/*******************************************************************************
* �֐���	:unsigned long getScore( void )
*		�X�R�A�Q�b�g�֐��ł��B
* ����		:void	�����Ȃ�
* �߂�l	:int nowScore	���݂̃X�R�A
* ����		:�X�R�A���擾���܂��B
*******************************************************************************/
unsigned long getScore( void )
{
	return nowScore;
}