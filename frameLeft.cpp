/*******************************************************************************
* �^�C�g��	:�t���[���摜.cpp
* �t�@�C����:frameLeft.cpp
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
#include "frameLeft.h"
#include "input.h"
#include "player01.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define FRAMELEFT_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define FRAMELEFT_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define FRAMELEFT_ANIM_SPAN	( 30 )	/*�A�j���[�V�����Ԋu*/
#define FRAMELEFT_ANIM_DIV_X	( 1 )	/*��������*/
#define FRAMELEFT_ANIM_DIV_Y	( 1 )	/*����������*/

#define FRAMELEFT_OUTPUT_SIZE	( 1 ) /*�����\���{��*/ 

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffframeLeft = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureframeLeft;	/*�t���[���摜�̃e�N�X�`���̃|�C���^*/
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

static int mode;
static int pastMode;

/*******************************************************************************
* �֐���	:HRESULT InitFrameLeft( void )
*		�t���[���摜�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitFrameLeft( void )
{
	int outputSizeX,outputSizeY;
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * FRAMELEFT_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffframeLeft , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/frameLeft.png" , &g_pTextureframeLeft );
	D3DXGetImageInfoFromFile( "data/TEXTURE/frameLeft.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffframeLeft->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / FRAMELEFT_ANIM_DIV_X ) * FRAMELEFT_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / FRAMELEFT_ANIM_DIV_Y ) * FRAMELEFT_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , 1 );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffframeLeft->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	mode = 0;
	pastMode = -1;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitFrameLeft( void )
*		�t���[���摜�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitFrameLeft( void )
{
	if( g_pVtxBuffframeLeft != NULL )
	{
		g_pVtxBuffframeLeft->Release();
		g_pVtxBuffframeLeft = NULL;
	}
	if( g_pTextureframeLeft != NULL )
	{
		g_pTextureframeLeft->Release();
		g_pTextureframeLeft = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateFrameLeft( void )
*		�t���[���摜�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateFrameLeft( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffframeLeft->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	mode = GetPlayerState01()->mode;
				
	texWidth = 1.0f / FRAMELEFT_ANIM_DIV_X;
	texHeight = 1.0f / FRAMELEFT_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % FRAMELEFT_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / FRAMELEFT_ANIM_DIV_X ) ) + 0;
	
	/*�e�N�X�`�����W����*/
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	
	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 215 );

	/*���_���W�ݒ�*/
	pVtx[0].pos.x = FIELD_START_X - SCREEN_WIDTH + ( SCREEN_WIDTH * 0.5 );
	pVtx[0].pos.y = SCREEN_TOP;
	pVtx[0].pos.z = 0.0f;
		
	pVtx[1].pos.x = FIELD_START_X + ( SCREEN_WIDTH * 0.5 );
	pVtx[1].pos.y = SCREEN_TOP;
	pVtx[1].pos.z = 0.0f;
		
	pVtx[2].pos.x = FIELD_START_X - SCREEN_WIDTH + ( SCREEN_WIDTH * 0.5 );
	pVtx[2].pos.y = SCREEN_BOTTOM;
	pVtx[2].pos.z = 0.0f;
		 
	pVtx[3].pos.x = FIELD_START_X + ( SCREEN_WIDTH * 0.5 );
	pVtx[3].pos.y = SCREEN_BOTTOM;
	pVtx[3].pos.z = 0.0f;

	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffframeLeft->Unlock();

	pastMode = mode;
}

/*******************************************************************************
* �֐���	:void DrawFrameLeft( void )
*		�t���[���摜�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawFrameLeft( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffframeLeft , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureframeLeft );

	/*�t���[���摜�̂̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}