/*******************************************************************************
* �^�C�g��	:�^�C�}�[.cpp
* �t�@�C����:timer.cpp
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
#include "timer.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define TIMER_MAX				( 80000000 )	//�^�C�}�[�ő�l
#define TIMER_WIDTH				( 25 )	//�^�C�}�[������
#define TIMER_SPACE_WIDTH		( 25 )	//�^�C�}�[������

#define TIMER_POS_X		( FIELD_RIGHT - ( TIMER_WIDTH * 0.60 ) )		//�^�C�}�[�\���ʒu
#define TIMER_POS_Y		( 200 )

#define TIMER_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define TIMER_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define TIMER_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define TIMER_ANIM_DIV_X	( 1 )	/*��������*/
#define TIMER_ANIM_DIV_Y	( 12 )	/*����������*/

#define TIMER_OUTPUT_SIZE	( 0.35 ) /*�����\���{��*/ 

#define MOVE_NUM ( 2.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.20f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.05f )	/*������*/

#define USE_QUOTE	( 8 )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBufftimer = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturetimer;	/*�^�C�}�[�̃e�N�X�`���̃|�C���^*/
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

static int g_TimerPosX, g_TimerPosY;
static int nowTimer = 0;
/*******************************************************************************
* �֐���	:HRESULT InitTimer( void )
*		�^�C�}�[�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitTimer( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * TIMER_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBufftimer , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/timer.png" , &g_pTexturetimer );
	D3DXGetImageInfoFromFile( "data/TEXTURE/timer.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBufftimer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / TIMER_ANIM_DIV_X ) * TIMER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / TIMER_ANIM_DIV_Y ) * TIMER_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBufftimer->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//�^�C�}�[������
	nowTimer = 0;

	//�^�C�}�[��ʒu�ݒ�
	g_TimerPosX = TIMER_POS_X;
	g_TimerPosY = TIMER_POS_Y;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitTimer( void )
*		�^�C�}�[�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitTimer( void )
{
	if( g_pVtxBufftimer != NULL )
	{
		g_pVtxBufftimer->Release();
		g_pVtxBufftimer = NULL;
	}
	if( g_pTexturetimer != NULL )
	{
		g_pTexturetimer->Release();
		g_pTexturetimer = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateTimer( void )
*		�^�C�}�[�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateTimer( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	int timerCpy = nowTimer, outputTimerNum = 0,nCntTimer = USE_QUOTE, divideNum = 0;
	int outputNum[USE_QUOTE];
	bool numPrintFlag = false;
	
	//���Ԍv�Z
	int timers; 
	timers = timerCpy;
	
	
	for( int nCntTimer = 0; nCntTimer < USE_QUOTE; nCntTimer++ )
	{
		outputNum[nCntTimer] = 0;
	}

	
	timers %= 3600000;
	outputNum[7] = timers / 600000;		timers = timers - ( outputNum[7] * 600000 );
	outputNum[6] = timers / 60000;		timers = timers - ( outputNum[6] * 60000 );
	outputNum[5] = 10;
	outputNum[4] = timers / 10000;		timers = timers - ( outputNum[4] * 10000 );
	outputNum[3] = timers / 1000;		timers = timers - ( outputNum[3] * 1000 );
	outputNum[2] = 10;
	outputNum[1] = timers / 100;		timers = timers - ( outputNum[1] * 100 ); 
	outputNum[0] = timers / 10;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBufftimer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	g_TimerPosX = TIMER_POS_X + TIMER_SPACE_WIDTH * USE_QUOTE;
	g_TimerPosY = TIMER_POS_Y;

	for( int nCntTimer = 0; nCntTimer < USE_QUOTE; nCntTimer++ )
	{	
		/*�p�^�[���A�j���[�V�������O�v�Z����*/
		if( ++g_nCounterAnim %= TIMER_ANIM_SPAN )
		{
			++g_nPatternAnim %= TIMER_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == TIMER_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}

		outputTimerNum = outputNum[nCntTimer];

		texWidth = 1.0f / TIMER_ANIM_DIV_X;
		texHeight = 1.0f / TIMER_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % TIMER_ANIM_DIV_X );

		nowPatternY = ( float )( ( int )( g_nPatternAnim / TIMER_ANIM_DIV_X ) ) + outputTimerNum;

		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		
		if( nCntTimer == 5 || nCntTimer == 2 )
		{
			g_TimerPosX += ( TIMER_SPACE_WIDTH * 0.30 );
		}
		if( nCntTimer == 6 || nCntTimer == 3 )
		{
			g_TimerPosX += ( TIMER_SPACE_WIDTH * 0.22 );
		}

		/*�e�N�X�`�����W����*/
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		/*���_���W�ݒ�*/
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].pos.x = g_TimerPosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].pos.y = g_TimerPosY + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].pos.x = g_TimerPosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].pos.y = g_TimerPosY + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].pos.x = g_TimerPosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].pos.y = g_TimerPosY + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].pos.x = g_TimerPosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].pos.y = g_TimerPosY + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntTimer * TIMER_VERTEX_MAX + 3].pos.z = 0.0f;

		g_TimerPosX -= TIMER_SPACE_WIDTH;
	}
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBufftimer->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawTimer( void )
*		�^�C�}�[�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawTimer( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBufftimer , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturetimer );
	
	for( int nCntTimer = 0; nCntTimer < USE_QUOTE; nCntTimer++ )
	{
		/*�^�C�}�[�̂̕`��*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntTimer * TIMER_VERTEX_MAX , 2 );
	}
}

/*******************************************************************************
* �֐���	:void PlusTimer( int plusNum )
*		�^�C�}�[���Z�֐��ł��B
* ����		:int plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�^�C�}�[�����Z���܂��B
*******************************************************************************/
void PlusTimer( int plusNum )
{
	
	if( nowTimer + plusNum > TIMER_MAX )
	{
		nowTimer = TIMER_MAX;
	}
	else if( nowTimer + plusNum < 0 )
	{
		nowTimer = 0;
	}
	else
	{
		nowTimer += plusNum;
	}
}

/*******************************************************************************
* �֐���	:resetTimer( int resetNum )
*		�^�C�}�[���Z�b�g�֐��ł��B
* ����		:int resetNum	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�^�C�}�[�����Z�b�g���܂��B
*******************************************************************************/
void resetTimer( int resetNum )
{
	if( resetNum >= 0 && nowTimer <= TIMER_MAX )
	{
		nowTimer = resetNum;
	}
}

/*******************************************************************************
* �֐���	:int getTimer( void )
*		�^�C�}�[�Q�b�g�֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:int nowTimer	���݂̃~���b
* ����		:�^�C�}�[���Q�b�g���܂��B
*******************************************************************************/
int getTimer( void )
{
	return nowTimer;
}
