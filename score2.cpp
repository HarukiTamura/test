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
#define SCORE2_MAX				( ULONG_MAX )	//�X�R�A�ő�l
#define SCORE2_NUMBER_OF_DIGITS	( 10 )	//�X�R�A����
#define SCORE2_WIDTH				( 40 )	//�X�R�A������

#define SCORE2_POS_X		( 300 )		//�X�R�A�\���ʒu
#define SCORE2_POS_Y		( 200 )

#define SCORE2_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define SCORE2_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define SCORE2_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define SCORE2_ANIM_DIV_X	( 1 )	/*��������*/
#define SCORE2_ANIM_DIV_Y	( 10 )	/*����������*/

#define SCORE2_OUTPUT_SIZE	( 0.65 ) /*�����\���{��*/ 

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

static int g_Score2PosX, g_Score2PosY;
static unsigned long nowScore2 = 0;
static bool g_aScore2UseFlag[SCORE2_NUMBER_OF_DIGITS];
/*******************************************************************************
* �֐���	:HRESULT InitScore2( void )
*		�X�R�A�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitScore2( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * SCORE2_VERTEX_MAX * SCORE2_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffscore , NULL ) ) )
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
	outputSizeX = ( int )( ( info.Width / SCORE2_ANIM_DIV_X ) * SCORE2_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / SCORE2_ANIM_DIV_Y ) * SCORE2_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , SCORE2_NUMBER_OF_DIGITS );

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
	nowScore2 = 0;
	
	//�e���\���t���O������
	for( int cntScore2 = 0; cntScore2 < SCORE2_NUMBER_OF_DIGITS; cntScore2++ )
	{	
		g_aScore2UseFlag[cntScore2] = false;
	}

	//�X�R�A��ʒu�ݒ�
	g_Score2PosX = SCORE2_POS_X;
	g_Score2PosY = SCORE2_POS_Y;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitScore2( void )
*		�X�R�A�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitScore2( void )
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
* �֐���	:void UpdateScore2( void )
*		�X�R�A�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateScore2( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	int outputScore2Num = 0, nCntScore2 = SCORE2_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	unsigned long scoreCpy = nowScore2;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffscore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	scoreCpy = 87654321;
	//�e���\���t���O������
	nCntScore2 = SCORE2_NUMBER_OF_DIGITS;
	while( nCntScore2-- )
	{	
		g_aScore2UseFlag[nCntScore2] = false;

		/*�p�^�[���A�j���[�V�������O�v�Z����*/
		if( ++g_nCounterAnim %= SCORE2_ANIM_SPAN )
		{
			++g_nPatternAnim %= SCORE2_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == SCORE2_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / SCORE2_ANIM_DIV_X;
		texHeight = 1.0f / SCORE2_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % SCORE2_ANIM_DIV_X );

		scoreCpy = nowScore2;
		for( int cnt = 0; cnt < nCntScore2; cnt++ )
		{
			scoreCpy /= 10;
		}

		outputScore2Num = scoreCpy % 10;
		if( outputScore2Num != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / SCORE2_ANIM_DIV_X ) ) + outputScore2Num;
		
		if( numPrintFlag == true )
		{
			g_aScore2UseFlag[nCntScore2] = true;
		}

		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		/*�e�N�X�`�����W����*/
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		g_Score2PosX += SCORE2_WIDTH;
		/*���_���W�ݒ�*/
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 0].pos.x = g_Score2PosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 0].pos.y = g_Score2PosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 1].pos.x = g_Score2PosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 1].pos.y = g_Score2PosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 2].pos.x = g_Score2PosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 2].pos.y = g_Score2PosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 3].pos.x = g_Score2PosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 3].pos.y = g_Score2PosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntScore2 * SCORE2_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_Score2PosX = SCORE2_POS_X;
	g_Score2PosY = SCORE2_POS_Y;
	g_aScore2UseFlag[0] = true;
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffscore->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawScore2( void )
*		�X�R�A�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawScore2( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffscore , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturescore );
	
	for( int nCntScore2 = 0; nCntScore2 <= SCORE2_NUMBER_OF_DIGITS; nCntScore2++ )
	{
		if( g_aScore2UseFlag[nCntScore2] == true )
		{
			/*�X�R�A�̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntScore2 * SCORE2_VERTEX_MAX , 2 );
		}
	}
	
}

/*******************************************************************************
* �֐���	:void PlusScore2( unsigned long plusNum )
*		�X�R�A���Z�֐��ł��B
* ����		:unsigned long plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z���܂��B
*******************************************************************************/
void PlusScore2( unsigned long plusNum )
{
	
	if( nowScore2 + plusNum > SCORE2_MAX )
	{
		nowScore2 = SCORE2_MAX;
	}
	else if( nowScore2 + plusNum < 0 )
	{
		nowScore2 = 0;
	}
	else
	{
		nowScore2 += plusNum;
	}
}

/*******************************************************************************
* �֐���	:resetScore2( unsigned long resetNum )
*		�X�R�A���Z�b�g�֐��ł��B
* ����		:unsigned long resetNum	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z�b�g���܂��B
*******************************************************************************/
void resetScore2( unsigned long resetNum )
{
	if( resetNum >= 0 && nowScore2 <= SCORE2_MAX )
	{
		nowScore2 = resetNum;
	}
}