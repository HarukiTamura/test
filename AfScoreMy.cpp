/*******************************************************************************
* �^�C�g��	:�X�R�A.cpp
* �t�@�C����:afScoreMy.cpp
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
#include "afScoreMy.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_SCORE_MY_MAX				( ULONG_MAX - 1 )	//�X�R�A�ő�l
#define AF_SCORE_MY_NUMBER_OF_DIGITS	( 10 )	//�X�R�A����
#define AF_SCORE_MY_WIDTH				( 50 )	//�X�R�A������

#define AF_SCORE_MY_POS_X		( 80 )	//�X�R�A�\���ʒu
#define AF_SCORE_MY_POS_Y		( 218 )

#define AF_SCORE_MY_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define AF_SCORE_MY_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define AF_SCORE_MY_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define AF_SCORE_MY_ANIM_DIV_X	( 1 )	/*��������*/
#define AF_SCORE_MY_ANIM_DIV_Y	( 10 )	/*����������*/

#define AF_SCORE_MY_OUTPUT_SIZE	( 0.65 ) /*�����\���{��*/ 

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffafScoreMy = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureafScoreMy;	/*�X�R�A�̃e�N�X�`���̃|�C���^*/
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

static int g_AfScoreMyPosX, g_AfScoreMyPosY;
static unsigned long nowAfScoreMy = 0;
static unsigned long outputAfScoreMy = 0;
static float sucoreChange = 0;
static bool g_aAfScoreMyUseFlag[AF_SCORE_MY_NUMBER_OF_DIGITS];
/*******************************************************************************
* �֐���	:HRESULT InitAfScoreMy( void )
*		�X�R�A�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfScoreMy( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_SCORE_MY_VERTEX_MAX * AF_SCORE_MY_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffafScoreMy , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfScore.png" , &g_pTextureafScoreMy );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfScore.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffafScoreMy->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / AF_SCORE_MY_ANIM_DIV_X ) * AF_SCORE_MY_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_SCORE_MY_ANIM_DIV_Y ) * AF_SCORE_MY_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , AF_SCORE_MY_NUMBER_OF_DIGITS );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffafScoreMy->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//�X�R�A������
	nowAfScoreMy = 0;
	outputAfScoreMy = 0;

	//�e���\���t���O������
	for( int cntAfScoreMy = 0; cntAfScoreMy < AF_SCORE_MY_NUMBER_OF_DIGITS; cntAfScoreMy++ )
	{	
		g_aAfScoreMyUseFlag[cntAfScoreMy] = false;
	}

	//�X�R�A��ʒu�ݒ�
	g_AfScoreMyPosX = AF_SCORE_MY_POS_X;
	g_AfScoreMyPosY = AF_SCORE_MY_POS_Y;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitAfScoreMy( void )
*		�X�R�A�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfScoreMy( void )
{
	if( g_pVtxBuffafScoreMy != NULL )
	{
		g_pVtxBuffafScoreMy->Release();
		g_pVtxBuffafScoreMy = NULL;
	}
	if( g_pTextureafScoreMy != NULL )
	{
		g_pTextureafScoreMy->Release();
		g_pTextureafScoreMy = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfScoreMy( void )
*		�X�R�A�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfScoreMy( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	int outputAfScoreMyNum = 0, nCntAfScoreMy = AF_SCORE_MY_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	unsigned long afScoreMyCpy = nowAfScoreMy;

	if( sucoreChange != 1.0f )
	{
		sucoreChange = ( sucoreChange + 0.0025f >= 1.0f ? 1.0f : sucoreChange + 0.0025f );
	}

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffafScoreMy->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	if(	outputAfScoreMy > afScoreMyCpy )
	{
		outputAfScoreMy -= ( outputAfScoreMy - afScoreMyCpy ) * sin( sucoreChange ) - 1;
	}
	else if( outputAfScoreMy < afScoreMyCpy )
	{
		outputAfScoreMy += ( afScoreMyCpy - outputAfScoreMy ) * sin( sucoreChange ) + 1;
	}
	else
	{
		outputAfScoreMy = afScoreMyCpy;
	}

	//�e���\���t���O������
	nCntAfScoreMy = AF_SCORE_MY_NUMBER_OF_DIGITS;
	while( nCntAfScoreMy-- )
	{	
		g_aAfScoreMyUseFlag[nCntAfScoreMy] = false;

		/*�p�^�[���A�j���[�V�������O�v�Z����*/
		if( ++g_nCounterAnim %= AF_SCORE_MY_ANIM_SPAN )
		{
			++g_nPatternAnim %= AF_SCORE_MY_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == AF_SCORE_MY_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / AF_SCORE_MY_ANIM_DIV_X;
		texHeight = 1.0f / AF_SCORE_MY_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % AF_SCORE_MY_ANIM_DIV_X );

		afScoreMyCpy = outputAfScoreMy;
		for( int cnt = 0; cnt < nCntAfScoreMy; cnt++ )
		{
			afScoreMyCpy /= 10;
		}

		outputAfScoreMyNum = afScoreMyCpy % 10;
		if( outputAfScoreMyNum != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_SCORE_MY_ANIM_DIV_X ) ) + outputAfScoreMyNum;
		
		if( numPrintFlag == true )
		{
			g_aAfScoreMyUseFlag[nCntAfScoreMy] = true;
		}

		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		/*�e�N�X�`�����W����*/
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		g_AfScoreMyPosX += AF_SCORE_MY_WIDTH;
		/*���_���W�ݒ�*/
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].pos.x = g_AfScoreMyPosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].pos.y = g_AfScoreMyPosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].pos.x = g_AfScoreMyPosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].pos.y = g_AfScoreMyPosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].pos.x = g_AfScoreMyPosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].pos.y = g_AfScoreMyPosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].pos.x = g_AfScoreMyPosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].pos.y = g_AfScoreMyPosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_AfScoreMyPosX = AF_SCORE_MY_POS_X;
	g_AfScoreMyPosY = AF_SCORE_MY_POS_Y;
	g_aAfScoreMyUseFlag[0] = true;
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffafScoreMy->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawAfScoreMy( void )
*		�X�R�A�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfScoreMy( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffafScoreMy , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureafScoreMy );
	
	for( int nCntAfScoreMy = 0; nCntAfScoreMy < AF_SCORE_MY_NUMBER_OF_DIGITS; nCntAfScoreMy++ )
	{
		if( g_aAfScoreMyUseFlag[nCntAfScoreMy] == true )
		{
			/*�X�R�A�̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfScoreMy * AF_SCORE_MY_VERTEX_MAX , 2 );
		}
	}
	
}

/*******************************************************************************
* �֐���	:void PlusAfScoreMy( unsigned long plusNum )
*		�X�R�A���Z�֐��ł��B
* ����		:unsigned long plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z���܂��B
*******************************************************************************/
void PlusAfScoreMy( long plusNum )
{
	long long buff,buff2;
	
	buff = ( long long )( nowAfScoreMy );
	buff2 = ( long long )( plusNum );
	sucoreChange = 0.0f;

	if( buff + buff2 > AF_SCORE_MY_MAX )
	{
		nowAfScoreMy = AF_SCORE_MY_MAX;
	}	
	else
	{
		if( -buff > buff2 )
		{
			nowAfScoreMy = 0;
		}
		else
		{
			nowAfScoreMy += plusNum;
		}
	}
}

/*******************************************************************************
* �֐���	:resetAfScoreMy( unsigned long resetNum )
*		�X�R�A���Z�b�g�֐��ł��B
* ����		:unsigned long resetNum	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�R�A�����Z�b�g���܂��B
*******************************************************************************/
void resetAfScoreMy( unsigned long resetNum )
{
	if( resetNum >= 0 && nowAfScoreMy <= AF_SCORE_MY_MAX )
	{
		nowAfScoreMy = resetNum;
	}
}

/*******************************************************************************
* �֐���	:unsigned long getAfScoreMy( void )
*		�X�R�A�Q�b�g�֐��ł��B
* ����		:void	�����Ȃ�
* �߂�l	:int nowAfScoreMy	���݂̃X�R�A
* ����		:�X�R�A���擾���܂��B
*******************************************************************************/
unsigned long getAfScoreMy( void )
{
	return nowAfScoreMy;
}