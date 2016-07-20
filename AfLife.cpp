/*******************************************************************************
* �^�C�g��	:���C�t.cpp
* �t�@�C����:afLife.cpp
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
#include "afLife.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"
#include "AfPlayer.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_LIFE_MAX				( 100 )	//���C�t�ő�l
#define AF_LIFE_NUMBER_OF_DIGITS	( 10 )	//���C�t����
#define AF_LIFE_WIDTH				( 54 )	//���C�t������
#define POS_X_PLUS		( -2 )

#define AF_LIFE_POS_X		( SCREEN_WIDTH - ( AF_LIFE_WIDTH * AF_LIFE_NUMBER_OF_DIGITS ) - ( AF_LIFE_WIDTH * 0.6 ) - 46 )	//���C�t�\���ʒu
#define AF_LIFE_POS_Y		( 560 )

#define AF_LIFE_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define AF_LIFE_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define AF_LIFE_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define AF_LIFE_ANIM_DIV_X	( 1 )	/*��������*/
#define AF_LIFE_ANIM_DIV_Y	( 12 )	/*����������*/

#define AF_LIFE_OUTPUT_SIZE	( 0.56 ) /*�����\���{��*/ 

#define MOVE_NUM ( 2.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.20f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.05f )	/*������*/

#define DEFAULT_LIFE	( 0 )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffafLife = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureafLife;	/*���C�t�̃e�N�X�`���̃|�C���^*/
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

static int g_AfLifePosX, g_AfLifePosY;
static unsigned long nowAfLife = 0;
static unsigned long outputAfLife = 0;
static float sucoreChange = 0;
static bool g_aAfLifeUseFlag[AF_LIFE_NUMBER_OF_DIGITS];

static int fade;
/*******************************************************************************
* �֐���	:HRESULT InitAfLife( void )
*		���C�t�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfLife( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_LIFE_VERTEX_MAX * AF_LIFE_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffafLife , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfGameScore.png" , &g_pTextureafLife );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfGameScore.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffafLife->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / AF_LIFE_ANIM_DIV_X ) * AF_LIFE_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_LIFE_ANIM_DIV_Y ) * AF_LIFE_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , AF_LIFE_NUMBER_OF_DIGITS );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffafLife->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//���C�t������
	nowAfLife = DEFAULT_LIFE;
	outputAfLife = DEFAULT_LIFE;

	//�e���\���t���O������
	for( int cntAfLife = 0; cntAfLife < AF_LIFE_NUMBER_OF_DIGITS; cntAfLife++ )
	{	
		g_aAfLifeUseFlag[cntAfLife] = false;
	}

	fade = 255;

	//���C�t��ʒu�ݒ�
	g_AfLifePosX = AF_LIFE_POS_X;
	g_AfLifePosY = AF_LIFE_POS_Y;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitAfLife( void )
*		���C�t�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfLife( void )
{
	if( g_pVtxBuffafLife != NULL )
	{
		g_pVtxBuffafLife->Release();
		g_pVtxBuffafLife = NULL;
	}
	if( g_pTextureafLife != NULL )
	{
		g_pTextureafLife->Release();
		g_pTextureafLife = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfLife( void )
*		���C�t�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfLife( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	int outputAfLifeNum = 0, nCntAfLife = AF_LIFE_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	unsigned long afLifeCpy = nowAfLife;

	sucoreChange = ( sucoreChange + 0.0025f >= 1.0f ? 1.0f : sucoreChange + 0.0025f );

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffafLife->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	if(	outputAfLife > afLifeCpy )
	{
		outputAfLife -= ( outputAfLife - afLifeCpy ) * sin( sucoreChange ) - 1;
	}
	else if( outputAfLife < afLifeCpy )
	{
		outputAfLife += ( afLifeCpy - outputAfLife ) * sin( sucoreChange ) + 1;
	}
	else
	{
		outputAfLife = afLifeCpy;
	}

	//�e���\���t���O������
	nCntAfLife = AF_LIFE_NUMBER_OF_DIGITS;
	while( nCntAfLife-- )
	{	
		g_aAfLifeUseFlag[nCntAfLife] = false;

		/*�p�^�[���A�j���[�V�������O�v�Z����*/
		if( ++g_nCounterAnim %= AF_LIFE_ANIM_SPAN )
		{
			++g_nPatternAnim %= AF_LIFE_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == AF_LIFE_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / AF_LIFE_ANIM_DIV_X;
		texHeight = 1.0f / AF_LIFE_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % AF_LIFE_ANIM_DIV_X );

		afLifeCpy = outputAfLife;
		for( int cnt = 0; cnt < nCntAfLife; cnt++ )
		{
			afLifeCpy /= 10;
		}

		outputAfLifeNum = afLifeCpy % 10;
		if( outputAfLifeNum != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_LIFE_ANIM_DIV_X ) ) + outputAfLifeNum;
		
		if( numPrintFlag == true )
		{
			g_aAfLifeUseFlag[nCntAfLife] = true;
		}

		
		if( GetPlayerState()->posStatus.DrawPosY >= AF_PLAYER_DOWN_LIMIT &&
			GetPlayerState()->posStatus.DrawPosX >= AF_PLAYER_RIGHT_LIMIT )
		{
			fade = ( fade - 1 > 0 ? fade - 1 : 0 );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		}
		else
		{
			fade = ( fade + 1 < 255 ? fade + 1 : 255 );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
			pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		}

		/*�e�N�X�`�����W����*/
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		g_AfLifePosX += AF_LIFE_WIDTH;
		if( nCntAfLife <= 1 )
		{
			g_AfLifePosX += POS_X_PLUS * AF_LIFE_NUMBER_OF_DIGITS * nCntAfLife;
		}
		/*���_���W�ݒ�*/
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].pos.x = g_AfLifePosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].pos.y = g_AfLifePosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].pos.x = g_AfLifePosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].pos.y = g_AfLifePosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].pos.x = g_AfLifePosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].pos.y = g_AfLifePosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].pos.x = g_AfLifePosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].pos.y = g_AfLifePosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfLife * AF_LIFE_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_AfLifePosX = AF_LIFE_POS_X;
	g_AfLifePosY = AF_LIFE_POS_Y;
	g_aAfLifeUseFlag[0] = true;
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffafLife->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawAfLife( void )
*		���C�t�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfLife( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffafLife , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureafLife );
	
	for( int nCntAfLife = 0; nCntAfLife < AF_LIFE_NUMBER_OF_DIGITS; nCntAfLife++ )
	{
		if( g_aAfLifeUseFlag[nCntAfLife] == true )
		{
			/*���C�t�̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfLife * AF_LIFE_VERTEX_MAX , 2 );
		}
	}
	
}

/*******************************************************************************
* �֐���	:void PlusAfLife( unsigned long plusNum )
*		���C�t���Z�֐��ł��B
* ����		:unsigned long plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:���C�t�����Z���܂��B
*******************************************************************************/
void PlusAfLife( long plusNum )
{
	long long buff,buff2;
	
	buff = ( long long )( nowAfLife );
	buff2 = ( long long )( plusNum );
	sucoreChange = 0.0f;

	if( buff + buff2 > AF_LIFE_MAX )
	{
		nowAfLife = AF_LIFE_MAX;
	}	
	else
	{
		if( -buff > buff2 )
		{
			nowAfLife = 0;
		}
		else
		{
			nowAfLife += plusNum;
		}
	}
}

/*******************************************************************************
* �֐���	:resetAfLife( unsigned long resetNum )
*		���C�t���Z�b�g�֐��ł��B
* ����		:unsigned long resetNum	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:���C�t�����Z�b�g���܂��B
*******************************************************************************/
void resetAfLife( unsigned long resetNum )
{
	if( resetNum >= 0 && nowAfLife <= AF_LIFE_MAX )
	{
		nowAfLife = resetNum;
	}
}

/*******************************************************************************
* �֐���	:unsigned long getAfLife( void )
*		���C�t�Q�b�g�֐��ł��B
* ����		:void	�����Ȃ�
* �߂�l	:int nowAfLife	���݂̃��C�t
* ����		:���C�t���擾���܂��B
*******************************************************************************/
unsigned long getAfLife( void )
{
	return nowAfLife;
}