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
#include "afResult.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_SCORE3_MAX				( ULONG_MAX )	//�X�R�A�ő�l
#define AF_SCORE3_NUMBER_OF_DIGITS	( 10 )	//�X�R�A����
#define AF_SCORE3_WIDTH				( 37 )	//�X�R�A������

#define AF_SCORE3_POS_X		( 192 )		//�X�R�A�\���ʒu
#define AF_SCORE3_POS_Y		( 558 )

#define AF_SCORE3_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define AF_SCORE3_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define AF_SCORE3_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define AF_SCORE3_ANIM_DIV_X	( 1 )	/*��������*/
#define AF_SCORE3_ANIM_DIV_Y	( 10 )	/*����������*/

#define AF_SCORE3_OUTPUT_SIZE	( 0.48 ) /*�����\���{��*/ 

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

static int g_AfScore3PosX, g_AfScore3PosY;
static unsigned long nowAfScore3 = 0;
static bool g_aAfScore3UseFlag[AF_SCORE3_NUMBER_OF_DIGITS];
/*******************************************************************************
* �֐���	:HRESULT InitAfScore3( void )
*		�X�R�A�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfScore3( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_SCORE3_VERTEX_MAX * AF_SCORE3_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffafScore , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfScore.png" , &g_pTextureafScore );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfScore.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffafScore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / AF_SCORE3_ANIM_DIV_X ) * AF_SCORE3_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_SCORE3_ANIM_DIV_Y ) * AF_SCORE3_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , AF_SCORE3_NUMBER_OF_DIGITS );

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
	nowAfScore3 = 0;
	
	//�e���\���t���O������
	for( int cntAfScore3 = 0; cntAfScore3 < AF_SCORE3_NUMBER_OF_DIGITS; cntAfScore3++ )
	{	
		g_aAfScore3UseFlag[cntAfScore3] = false;
	}

	//�X�R�A��ʒu�ݒ�
	g_AfScore3PosX = AF_SCORE3_POS_X;
	g_AfScore3PosY = AF_SCORE3_POS_Y;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitAfScore3( void )
*		�X�R�A�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfScore3( void )
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
* �֐���	:void UpdateAfScore3( void )
*		�X�R�A�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfScore3( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	int outputAfScore3Num = 0, nCntAfScore3 = AF_SCORE3_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	unsigned long afScoreCpy = nowAfScore3;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffafScore->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	afScoreCpy = 87654321;
	//�e���\���t���O������
	nCntAfScore3 = AF_SCORE3_NUMBER_OF_DIGITS;
	while( nCntAfScore3-- )
	{	
		g_aAfScore3UseFlag[nCntAfScore3] = false;

		/*�p�^�[���A�j���[�V�������O�v�Z����*/
		if( ++g_nCounterAnim %= AF_SCORE3_ANIM_SPAN )
		{
			++g_nPatternAnim %= AF_SCORE3_ANIM_MAX;
		}
		
		texWidth = 1.0f / AF_SCORE3_ANIM_DIV_X;
		texHeight = 1.0f / AF_SCORE3_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % AF_SCORE3_ANIM_DIV_X );

		afScoreCpy = nowAfScore3;
		for( int cnt = 0; cnt < nCntAfScore3; cnt++ )
		{
			afScoreCpy /= 10;
		}

		outputAfScore3Num = afScoreCpy % 10;
		if( outputAfScore3Num != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_SCORE3_ANIM_DIV_X ) ) + outputAfScore3Num;
		
		if( numPrintFlag == true )
		{
			g_aAfScore3UseFlag[nCntAfScore3] = true;
		}

		if( getPlayerAfScoreResult() == nowAfScore3 )
		{
			pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		}
		else
		{
			pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 0 , 182 , 55 );
			pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 0 , 182 , 55 );
			pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 0 , 182 , 55 );
			pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 0 , 182 , 55 );
		}

		/*�e�N�X�`�����W����*/
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		g_AfScore3PosX += AF_SCORE3_WIDTH;
		/*���_���W�ݒ�*/
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 0].pos.x = g_AfScore3PosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 0].pos.y = g_AfScore3PosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 1].pos.x = g_AfScore3PosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 1].pos.y = g_AfScore3PosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 2].pos.x = g_AfScore3PosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 2].pos.y = g_AfScore3PosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 3].pos.x = g_AfScore3PosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 3].pos.y = g_AfScore3PosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScore3 * AF_SCORE3_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_AfScore3PosX = AF_SCORE3_POS_X;
	g_AfScore3PosY = AF_SCORE3_POS_Y;
	g_aAfScore3UseFlag[0] = true;
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffafScore->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawAfScore3( void )
*		�X�R�A�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfScore3( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffafScore , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureafScore );
	
	for( int nCntAfScore3 = 0; nCntAfScore3 <= AF_SCORE3_NUMBER_OF_DIGITS; nCntAfScore3++ )
	{
		if( g_aAfScore3UseFlag[nCntAfScore3] == true )
		{
			/*�X�R�A�̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfScore3 * AF_SCORE3_VERTEX_MAX , 2 );
		}
	}
	
}

/*******************************************************************************
* �֐���	:void PlusAfScore3( unsigned long plusNum )
*		�X�R�A���Z�֐��ł��B
* ����		:unsigned long plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z���܂��B
*******************************************************************************/
void PlusAfScore3( unsigned long plusNum )
{
	
	if( nowAfScore3 + plusNum > AF_SCORE3_MAX )
	{
		nowAfScore3 = AF_SCORE3_MAX;
	}
	else if( nowAfScore3 + plusNum < 0 )
	{
		nowAfScore3 = 0;
	}
	else
	{
		nowAfScore3 += plusNum;
	}
}

/*******************************************************************************
* �֐���	:resetAfScore3( unsigned long resetNum )
*		�X�R�A���Z�b�g�֐��ł��B
* ����		:int resetNum	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z�b�g���܂��B
*******************************************************************************/
void resetAfScore3( unsigned long resetNum )
{
	if( resetNum >= 0 && nowAfScore3 <= AF_SCORE3_MAX )
	{
		nowAfScore3 = resetNum;
	}
}