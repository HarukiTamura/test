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
#define SCORE3_MAX				( ULONG_MAX )	//�X�R�A�ő�l
#define SCORE3_NUMBER_OF_DIGITS	( 10 )	//�X�R�A����
#define SCORE3_WIDTH				( 40 )	//�X�R�A������

#define SCORE3_POS_X		( 300 )		//�X�R�A�\���ʒu
#define SCORE3_POS_Y		( 300 )

#define SCORE3_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define SCORE3_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define SCORE3_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define SCORE3_ANIM_DIV_X	( 1 )	/*��������*/
#define SCORE3_ANIM_DIV_Y	( 10 )	/*����������*/

#define SCORE3_OUTPUT_SIZE	( 0.65 ) /*�����\���{��*/ 

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

static int g_Score3PosX, g_Score3PosY;
static unsigned long nowScore3 = 0;
static bool g_aScore3UseFlag[SCORE3_NUMBER_OF_DIGITS];
/*******************************************************************************
* �֐���	:HRESULT InitScore3( void )
*		�X�R�A�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitScore3( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * SCORE3_VERTEX_MAX * SCORE3_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffscore , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/score.png" , &g_pTexturescore );
	D3DXGetImageInfoFromFile( "data/TEXTURE/score.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffscore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / SCORE3_ANIM_DIV_X ) * SCORE3_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / SCORE3_ANIM_DIV_Y ) * SCORE3_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , SCORE3_NUMBER_OF_DIGITS );

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
	nowScore3 = 0;
	
	//�e���\���t���O������
	for( int cntScore3 = 0; cntScore3 < SCORE3_NUMBER_OF_DIGITS; cntScore3++ )
	{	
		g_aScore3UseFlag[cntScore3] = false;
	}

	//�X�R�A��ʒu�ݒ�
	g_Score3PosX = SCORE3_POS_X;
	g_Score3PosY = SCORE3_POS_Y;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitScore3( void )
*		�X�R�A�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitScore3( void )
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
* �֐���	:void UpdateScore3( void )
*		�X�R�A�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateScore3( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	int outputScore3Num = 0, nCntScore3 = SCORE3_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	unsigned long scoreCpy = nowScore3;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffscore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	scoreCpy = 87654321;
	//�e���\���t���O������
	nCntScore3 = SCORE3_NUMBER_OF_DIGITS;
	while( nCntScore3-- )
	{	
		g_aScore3UseFlag[nCntScore3] = false;

		/*�p�^�[���A�j���[�V�������O�v�Z����*/
		if( ++g_nCounterAnim %= SCORE3_ANIM_SPAN )
		{
			++g_nPatternAnim %= SCORE3_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == SCORE3_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / SCORE3_ANIM_DIV_X;
		texHeight = 1.0f / SCORE3_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % SCORE3_ANIM_DIV_X );

		scoreCpy = nowScore3;
		for( int cnt = 0; cnt < nCntScore3; cnt++ )
		{
			scoreCpy /= 10;
		}

		outputScore3Num = scoreCpy % 10;
		if( outputScore3Num != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / SCORE3_ANIM_DIV_X ) ) + outputScore3Num;
		
		if( numPrintFlag == true )
		{
			g_aScore3UseFlag[nCntScore3] = true;
		}

		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		/*�e�N�X�`�����W����*/
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		g_Score3PosX += SCORE3_WIDTH;
		/*���_���W�ݒ�*/
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 0].pos.x = g_Score3PosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 0].pos.y = g_Score3PosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 1].pos.x = g_Score3PosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 1].pos.y = g_Score3PosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 2].pos.x = g_Score3PosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 2].pos.y = g_Score3PosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 3].pos.x = g_Score3PosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 3].pos.y = g_Score3PosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore3 * SCORE3_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_Score3PosX = SCORE3_POS_X;
	g_Score3PosY = SCORE3_POS_Y;
	g_aScore3UseFlag[0] = true;
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffscore->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawScore3( void )
*		�X�R�A�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawScore3( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffscore , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturescore );
	
	for( int nCntScore3 = 0; nCntScore3 <= SCORE3_NUMBER_OF_DIGITS; nCntScore3++ )
	{
		if( g_aScore3UseFlag[nCntScore3] == true )
		{
			/*�X�R�A�̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntScore3 * SCORE3_VERTEX_MAX , 2 );
		}
	}
	
}

/*******************************************************************************
* �֐���	:void PlusScore3( unsigned long plusNum )
*		�X�R�A���Z�֐��ł��B
* ����		:unsigned long plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z���܂��B
*******************************************************************************/
void PlusScore3( unsigned long plusNum )
{
	
	if( nowScore3 + plusNum > SCORE3_MAX )
	{
		nowScore3 = SCORE3_MAX;
	}
	else if( nowScore3 + plusNum < 0 )
	{
		nowScore3 = 0;
	}
	else
	{
		nowScore3 += plusNum;
	}
}

/*******************************************************************************
* �֐���	:resetScore3( unsigned long resetNum )
*		�X�R�A���Z�b�g�֐��ł��B
* ����		:int resetNum	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z�b�g���܂��B
*******************************************************************************/
void resetScore3( unsigned long resetNum )
{
	if( resetNum >= 0 && nowScore3 <= SCORE3_MAX )
	{
		nowScore3 = resetNum;
	}
}