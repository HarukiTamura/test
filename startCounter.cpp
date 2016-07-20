/*******************************************************************************
* ���U���g	:�X�^�[�g�J�E���g.cpp
* �t�@�C����:StartCounter.cpp
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
#include "game.h"
#include "StartCounter.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define STARTCOUNTER_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define ONE_COUNT_FPS		( 60 )
#define COUNT_COUNTDOWN_FPS	( ONE_COUNT_FPS * 3 )	/*�����\��FPS*/
#define COUNT_FADEOUT_FPS	( 20 )	/*GO�\���c��FPS*/
#define COUNT_START_FPS	( COUNT_COUNTDOWN_FPS + COUNT_FADEOUT_FPS )	/*�J�E���g�[���܂ł�FPS*/

#define STARTCOUNTER_OUTPUT_POS_X	( SCREEN_WIDTH * 0.5 )		/*�\��X���W*/
#define STARTCOUNTER_OUTPUT_POS_Y	( SCREEN_HEIGHT * 0.5 )		/*�\��Y���W*/

#define STARTCOUNTER_ANIM_MAX		( 4 )	/*�A�j���[�V�����R�}��*/
#define STARTCOUNTER_ANIM_SPAN		( 60 )	/*�A�j���[�V�����Ԋu*/
#define STARTCOUNTER_ANIM_DIV_X	( 4 )	/*��������*/
#define STARTCOUNTER_ANIM_DIV_Y	( 1 )	/*����������*/

#define STARTCOUNTER_OUTPUT_SIZE	( 1 ) /*�����\���{��*/ 

#define MOVE_NUM ( 1.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.10f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN	( 0.05f )	/*������*/
/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffStartCounter = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureStartCounter;	/*�X�^�[�g�J�E���g�e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;		/*�摜���*/

static D3DXVECTOR3 g_movePolygon;	/*�|���S���̈ʒu*/
static D3DXVECTOR3 g_posPolygon;	/*�|���S���̈ړ���*/
static D3DXVECTOR3 g_rotPolygon;	/*�|���S���̉�]�p*/
static float g_fLengthPolygon;		/*�|���S���̑Ίp���̒���*/
static float g_fAnglePolygon;		/*�|���S���̑Ίp���̊p�x*/
static float polygonWidth , polygonHeight;	/*�|���S���̕��^�����i�S�̂̔����j*/

static float polygonMag;	/*�\���{��*/

static int g_nCounterAnim = 0;
static int g_nPatternAnim = 0;

static int outputSizeX , outputSizeY;

/*******************************************************************************
* �֐���	:HRESULT InitStartCounter( void )
*		�X�^�[�g�J�E���g�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitStartCounter( void )
{
	g_nCounterAnim = COUNT_START_FPS;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * STARTCOUNTER_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffStartCounter , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/startCounter.png" , &g_pTextureStartCounter );
	D3DXGetImageInfoFromFile( "data/TEXTURE/startCounter.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffStartCounter->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / STARTCOUNTER_ANIM_DIV_X ) * STARTCOUNTER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / STARTCOUNTER_ANIM_DIV_Y ) * STARTCOUNTER_OUTPUT_SIZE );

	g_posPolygon.x = STARTCOUNTER_OUTPUT_POS_X;
	g_posPolygon.y = STARTCOUNTER_OUTPUT_POS_Y;
	g_posPolygon.z = 0.0f;

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	/*���_���W�ݒ�*/
	pVtx[0].pos = D3DXVECTOR3( SCREEN_LEFT	, SCREEN_TOP , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( SCREEN_RIGHT	, SCREEN_TOP , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( SCREEN_LEFT	, SCREEN_BOTTOM , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( SCREEN_RIGHT , SCREEN_BOTTOM , 0.0f );

	/*���W�ϊ��p�W���ݒ�*/
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;
	
	/*���_�J���[�ݒ�*/
	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	/*�e�N�X�`�����W�ݒ�*/
	pVtx[0].tex = D3DXVECTOR2( 0 , 0 );
	pVtx[1].tex = D3DXVECTOR2( 1 , 0 );
	pVtx[2].tex = D3DXVECTOR2( 0 , 1 );
	pVtx[3].tex = D3DXVECTOR2( 1 , 1 );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffStartCounter->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = STARTCOUNTER_OUTPUT_SIZE;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitStartCounter( void )
*		�X�^�[�g�J�E���g�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitStartCounter( void )
{
	if( g_pVtxBuffStartCounter != NULL )
	{
		g_pVtxBuffStartCounter->Release();
		g_pVtxBuffStartCounter = NULL;
	}
	if( g_pTextureStartCounter != NULL )
	{
		g_pTextureStartCounter->Release();
		g_pTextureStartCounter = NULL;
	}
}

/*******************************************************************************
* �֐���	:bool UpdateStartCounter( void )
*		�X�^�[�g�J�E���g�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:bool true �J�E���g��	false �J�E���g�[��
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
bool UpdateStartCounter( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	
	if( g_nCounterAnim > COUNT_FADEOUT_FPS )
	{
		int cnt = 1;
		while( g_nCounterAnim > ONE_COUNT_FPS * cnt + COUNT_FADEOUT_FPS )
		{
			cnt++;
		} 
		g_nPatternAnim = cnt;
	}
	else
	{
		g_nPatternAnim = 0;
	}
	
	if( g_nCounterAnim >= 0 )
	{
		g_nCounterAnim--;
	}
	//polygon�T�C�Y�ύX
	if( ( g_nCounterAnim - COUNT_FADEOUT_FPS ) % ONE_COUNT_FPS == 0 )
	{
		polygonMag = STARTCOUNTER_OUTPUT_SIZE;
	}
	else
	{
		if( ( g_nCounterAnim - COUNT_FADEOUT_FPS ) % ONE_COUNT_FPS > ONE_COUNT_FPS * 0.5 || g_nCounterAnim < COUNT_FADEOUT_FPS )
		{
			polygonMag += 0.0025f;
		}
		else
		{
			polygonMag -= 0.0025f;
		}
	}
	//�A�j���[�V����
	texWidth = 1.0f / STARTCOUNTER_ANIM_DIV_X;
	texHeight = 1.0f / STARTCOUNTER_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % STARTCOUNTER_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / STARTCOUNTER_ANIM_DIV_X ) );
	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffStartCounter->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;

	/*�ړ��ʔ��f*/
	g_posPolygon.x += g_movePolygon.x;
	g_posPolygon.y += g_movePolygon.y;

	if( g_posPolygon.x < SCREEN_LEFT )
	{
		g_posPolygon.x = SCREEN_LEFT;
	}
	if( g_posPolygon.x > SCREEN_RIGHT )
	{
		g_posPolygon.x = SCREEN_RIGHT;
	}
	if( g_posPolygon.y < SCREEN_TOP )
	{
		g_posPolygon.y = SCREEN_TOP;
	}
	if( g_posPolygon.y > SCREEN_HEIGHT )
	{
		g_posPolygon.y = SCREEN_HEIGHT;
	}
	/*�ړ��ʌ���*/
	g_movePolygon.x += ( DEFAULT_MOVE - g_movePolygon.x ) * MOVE_ATTEN;
	g_movePolygon.y += ( DEFAULT_MOVE - g_movePolygon.y ) * MOVE_ATTEN;	 	
	
	if( g_rotPolygon.z > D3DX_PI )
	{
		g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
	}
	if( g_rotPolygon.z < -D3DX_PI )
	{
		g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
	}
	//���_�J���[�ݒ�
	if( g_nPatternAnim == 0 )
	{
		int fade = ( int )( 215.0f * ( ( float )( g_nCounterAnim ) / ( float )(  COUNT_FADEOUT_FPS ) ) );
		pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
	}
	else
	{
		pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
		pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
		pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
		pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
	}
	/*���_���W�ݒ�*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	*/
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffStartCounter->Unlock();

	return ( g_nCounterAnim > COUNT_FADEOUT_FPS );
}

/*******************************************************************************
* �֐���	:void DrawStartCounter( void )
*		�X�^�[�g�J�E���g�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawStartCounter( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	if( g_nCounterAnim >= 0 )
	{
		/*���_�t�H�[�}�b�g�̐ݒ�*/
		pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
		/*�e�N�X�`���̐ݒ�*/
		pD3DDevice->SetTexture( 0 , g_pTextureStartCounter );
	
		/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
		pD3DDevice->SetStreamSource( 0 , g_pVtxBuffStartCounter , 0 , sizeof( VERTEX_2D ) );

		/*�X�^�[�g�J�E���g�̕`��*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
	}
}


/*******************************************************************************
* �֐���	:void ResetStartCounter( void )
*		�X�^�[�g�J�E���g�����Z�b�g����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�X�^�[�g�J�E���g�����Z�b�g����֐��ł��B
*******************************************************************************/
void ResetStartCounter( void )
{
	g_nCounterAnim = COUNT_START_FPS;
}