/*******************************************************************************
* �^�C�g��	:�X�R�A.cpp
* �t�@�C����:afScore.cpp
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
#include "afScore.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"
#include "AfPlayer.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_SCORE_MAX				( ULONG_MAX - 1 )	//�X�R�A�ő�l
#define AF_SCORE_NUMBER_OF_DIGITS	( 10 )	//�X�R�A����
#define AF_SCORE_WIDTH				( 27 )	//�X�R�A������

#define AF_SCORE_POS_X		( SCREEN_WIDTH - ( AF_SCORE_WIDTH * AF_SCORE_NUMBER_OF_DIGITS ) - ( AF_SCORE_WIDTH * 0.6 ) )	//�X�R�A�\���ʒu
#define AF_SCORE_POS_Y		( 467 )

#define AF_SCORE_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define AF_SCORE_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define AF_SCORE_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define AF_SCORE_ANIM_DIV_X	( 1 )	/*��������*/
#define AF_SCORE_ANIM_DIV_Y	( 12 )	/*����������*/

#define AF_SCORE_OUTPUT_SIZE	( 0.26 ) /*�����\���{��*/ 

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffafScore = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureafScore;	/*�X�R�A�̃e�N�X�`���̃|�C���^*/
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

static int g_AfScorePosX, g_AfScorePosY;
static unsigned long nowAfScore = 0;
static unsigned long outputAfScore = 0;
static float sucoreChange = 0;
static bool g_aAfScoreUseFlag[AF_SCORE_NUMBER_OF_DIGITS];

static int fade;
/*******************************************************************************
* �֐���	:HRESULT InitAfScore( void )
*		�X�R�A�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfScore( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_SCORE_VERTEX_MAX * AF_SCORE_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffafScore , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfGameScore.png" , &g_pTextureafScore );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfGameScore.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffafScore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / AF_SCORE_ANIM_DIV_X ) * AF_SCORE_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_SCORE_ANIM_DIV_Y ) * AF_SCORE_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , AF_SCORE_NUMBER_OF_DIGITS );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffafScore->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//�X�R�A������
	nowAfScore = 0;
	outputAfScore = 0;

	//�e���\���t���O������
	for( int cntAfScore = 0; cntAfScore < AF_SCORE_NUMBER_OF_DIGITS; cntAfScore++ )
	{	
		g_aAfScoreUseFlag[cntAfScore] = false;
	}

	fade = 255;

	//�X�R�A��ʒu�ݒ�
	g_AfScorePosX = AF_SCORE_POS_X;
	g_AfScorePosY = AF_SCORE_POS_Y;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitAfScore( void )
*		�X�R�A�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfScore( void )
{
	if( g_pVtxBuffafScore != NULL )
	{
		g_pVtxBuffafScore->Release();
		g_pVtxBuffafScore = NULL;
	}
	if( g_pTextureafScore != NULL )
	{
		g_pTextureafScore->Release();
		g_pTextureafScore = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfScore( void )
*		�X�R�A�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfScore( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	int outputAfScoreNum = 0, nCntAfScore = AF_SCORE_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	unsigned long afScoreCpy = nowAfScore;

	sucoreChange = ( sucoreChange + 0.0025f >= 1.0f ? 1.0f : sucoreChange + 0.0025f );

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffafScore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	if(	outputAfScore > afScoreCpy )
	{
		outputAfScore -= ( outputAfScore - afScoreCpy ) * sin( sucoreChange ) - 1;
	}
	else if( outputAfScore < afScoreCpy )
	{
		outputAfScore += ( afScoreCpy - outputAfScore ) * sin( sucoreChange ) + 1;
	}
	else
	{
		outputAfScore = afScoreCpy;
	}

	//�e���\���t���O������
	nCntAfScore = AF_SCORE_NUMBER_OF_DIGITS;
	while( nCntAfScore-- )
	{	
		g_aAfScoreUseFlag[nCntAfScore] = false;

		/*�p�^�[���A�j���[�V�������O�v�Z����*/
		if( ++g_nCounterAnim %= AF_SCORE_ANIM_SPAN )
		{
			++g_nPatternAnim %= AF_SCORE_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == AF_SCORE_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / AF_SCORE_ANIM_DIV_X;
		texHeight = 1.0f / AF_SCORE_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % AF_SCORE_ANIM_DIV_X );

		afScoreCpy = outputAfScore;
		for( int cnt = 0; cnt < nCntAfScore; cnt++ )
		{
			afScoreCpy /= 10;
		}

		outputAfScoreNum = afScoreCpy % 10;
		if( outputAfScoreNum != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_SCORE_ANIM_DIV_X ) ) + outputAfScoreNum;
		
		if( numPrintFlag == true )
		{
			g_aAfScoreUseFlag[nCntAfScore] = true;
		}

		
		if( GetPlayerState()->posStatus.DrawPosY >= AF_PLAYER_DOWN_LIMIT &&
			GetPlayerState()->posStatus.DrawPosX >= AF_PLAYER_RIGHT_LIMIT )
		{
			fade = ( fade - 1 > 0 ? fade - 1 : 0 );
			pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		}
		else
		{
			fade = ( fade + 1 < 255 ? fade + 1 : 255 );
			pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		}

		/*�e�N�X�`�����W����*/
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		g_AfScorePosX += AF_SCORE_WIDTH;
		/*���_���W�ݒ�*/
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 0].pos.x = g_AfScorePosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 0].pos.y = g_AfScorePosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 1].pos.x = g_AfScorePosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 1].pos.y = g_AfScorePosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 2].pos.x = g_AfScorePosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 2].pos.y = g_AfScorePosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 3].pos.x = g_AfScorePosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 3].pos.y = g_AfScorePosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore * AF_SCORE_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_AfScorePosX = AF_SCORE_POS_X;
	g_AfScorePosY = AF_SCORE_POS_Y;
	g_aAfScoreUseFlag[0] = true;
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffafScore->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawAfScore( void )
*		�X�R�A�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfScore( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffafScore , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureafScore );
	
	for( int nCntAfScore = 0; nCntAfScore < AF_SCORE_NUMBER_OF_DIGITS; nCntAfScore++ )
	{
		if( g_aAfScoreUseFlag[nCntAfScore] == true )
		{
			/*�X�R�A�̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfScore * AF_SCORE_VERTEX_MAX , 2 );
		}
	}
	
}

/*******************************************************************************
* �֐���	:void PlusAfScore( unsigned long plusNum )
*		�X�R�A���Z�֐��ł��B
* ����		:unsigned long plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z���܂��B
*******************************************************************************/
void PlusAfScore( long plusNum )
{
	long long buff,buff2;
	
	buff = ( long long )( nowAfScore );
	buff2 = ( long long )( plusNum );
	sucoreChange = 0.0f;

	if( buff + buff2 > AF_SCORE_MAX )
	{
		nowAfScore = AF_SCORE_MAX;
	}	
	else
	{
		if( -buff > buff2 )
		{
			nowAfScore = 0;
		}
		else
		{
			nowAfScore += plusNum;
		}
	}
}

/*******************************************************************************
* �֐���	:resetAfScore( unsigned long resetNum )
*		�X�R�A���Z�b�g�֐��ł��B
* ����		:unsigned long resetNum	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z�b�g���܂��B
*******************************************************************************/
void resetAfScore( unsigned long resetNum )
{
	if( resetNum >= 0 && nowAfScore <= AF_SCORE_MAX )
	{
		nowAfScore = resetNum;
	}
}

/*******************************************************************************
* �֐���	:unsigned long getAfScore( void )
*		�X�R�A�Q�b�g�֐��ł��B
* ����		:void	�����Ȃ�
* �߂�l	:int nowAfScore	���݂̃X�R�A
* ����		:�X�R�A���擾���܂��B
*******************************************************************************/
unsigned long getAfScore( void )
{
	return nowAfScore;
}