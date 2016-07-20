/*******************************************************************************
* �^�C�g��	:�L�������w�i.cpp
* �t�@�C����:sideCharacter.cpp
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
#include "sideCharacter.h"
#include "input.h"
#include "player01.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define SIDECHARACTER_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define SIDECHARACTER_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define SIDECHARACTER_ANIM_SPAN	( 30 )	/*�A�j���[�V�����Ԋu*/
#define SIDECHARACTER_ANIM_DIV_X	( 6 )	/*��������*/
#define SIDECHARACTER_ANIM_DIV_Y	( 1 )	/*����������*/

#define SIDECHARACTER_OUTPUT_SIZE	( 1 ) /*�����\���{��*/ 

typedef enum
{
	CHARACTER_NULL = -1,
	CHARACTER_NORMAL,
	CHARACTER_DAMAGE,
	CHARACTER_HYPERMODE,
	CHARACTER_MODEMAX
}CHARACTER_MODE;
/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffsideCharacter = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturesideCharacter;	/*�L�������w�i�̃e�N�X�`���̃|�C���^*/
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

static CHARACTER_MODE mode = CHARACTER_NORMAL;
static CHARACTER_MODE pastMode = CHARACTER_NULL;

/*******************************************************************************
* �֐���	:HRESULT InitSideCharacter( void )
*		�L�������w�i�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitSideCharacter( void )
{
	int outputSizeX,outputSizeY;
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * SIDECHARACTER_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffsideCharacter , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/sideCharacter.png" , &g_pTexturesideCharacter );
	D3DXGetImageInfoFromFile( "data/TEXTURE/sideCharacter.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffsideCharacter->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / SIDECHARACTER_ANIM_DIV_X ) * SIDECHARACTER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / SIDECHARACTER_ANIM_DIV_Y ) * SIDECHARACTER_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , 1 );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffsideCharacter->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	mode = CHARACTER_NORMAL;
	pastMode = CHARACTER_NULL;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitSideCharacter( void )
*		�L�������w�i�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitSideCharacter( void )
{
	if( g_pVtxBuffsideCharacter != NULL )
	{
		g_pVtxBuffsideCharacter->Release();
		g_pVtxBuffsideCharacter = NULL;
	}
	if( g_pTexturesideCharacter != NULL )
	{
		g_pTexturesideCharacter->Release();
		g_pTexturesideCharacter = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateSideCharacter( void )
*		�L�������w�i�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateSideCharacter( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffsideCharacter->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

#ifdef _DEBUG
	if( GetKeyboardPress( DIK_1 ) )
	{
		mode = CHARACTER_NORMAL;
	}
	if( GetKeyboardPress( DIK_2 ) )
	{
		mode = CHARACTER_DAMAGE;
	}
	if( GetKeyboardPress( DIK_3 ) )
	{
		mode = CHARACTER_HYPERMODE;
	}
#endif

	//MODE�ύX��
	if( GetPlayerState01()->mode == PLAYER_MODE_HYPERMODE )
	{
		mode = CHARACTER_HYPERMODE;
	}
	else
	{
		if( GetPlayerState01()->mode == PLAYER_MODE_DAMAGE )
		{
			mode = CHARACTER_DAMAGE;
		}
		else
		{
			mode = CHARACTER_NORMAL;
		}
	}

	if( mode != pastMode )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = 0;
	}
	/*�p�^�[���A�j���[�V�������v�Z����*/
	switch( mode )
	{
		case CHARACTER_NORMAL:
			{
				g_nCounterAnim++;
				if( g_nPatternAnim > 2 )
				{
					g_nPatternAnim = 0;
					g_nCounterAnim = 0; 
				}
				if( g_nPatternAnim == 0 && g_nCounterAnim >= 72 )
				{
					g_nPatternAnim++;
					g_nCounterAnim = 0; 
				}
				if( g_nPatternAnim == 1 && g_nCounterAnim >= 5 )
				{
					g_nPatternAnim++;
					g_nCounterAnim = 0; 
				}
				if( g_nPatternAnim == 2 && g_nCounterAnim >= 5 )
				{
					g_nPatternAnim = 0;
					g_nCounterAnim = 0; 
				}
				break;
			}
		case CHARACTER_DAMAGE:
			{
				if( g_nPatternAnim != 1 )
				{
					g_nPatternAnim = 1; 
				}
				g_nCounterAnim++;
				if( g_nCounterAnim > 45 )
				{
					g_nPatternAnim = 1; 
					mode = CHARACTER_NORMAL;
				}
				break;
			}
		case CHARACTER_HYPERMODE:
			{
				/*���[�h�p���ɂ��I�[�o�[�t���[�h�~*/
				g_nCounterAnim += ( g_nCounterAnim > 60 ? g_nCounterAnim : g_nCounterAnim + 1 );
				if( g_nPatternAnim < 3 )
				{
					g_nPatternAnim = 3;
					g_nCounterAnim = 0; 
				}
				if( g_nPatternAnim == 3 && g_nCounterAnim >= 5 )
				{
					g_nPatternAnim++;
					g_nCounterAnim = 0; 
				}
				if( g_nPatternAnim == 4 && g_nCounterAnim >= 5 )
				{
					g_nPatternAnim++;
					g_nCounterAnim = 0; 
				}
				break;
			}
		default :
			{
				break;
			}
	}
				
	texWidth = 1.0f / SIDECHARACTER_ANIM_DIV_X;
	texHeight = 1.0f / SIDECHARACTER_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % SIDECHARACTER_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / SIDECHARACTER_ANIM_DIV_X ) ) + 0;
	
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
	pVtx[0].pos.x = SCREEN_LEFT;
	pVtx[0].pos.y = SCREEN_TOP;
	pVtx[0].pos.z = 0.0f;
		
	pVtx[1].pos.x = SCREEN_RIGHT;
	pVtx[1].pos.y = SCREEN_TOP;
	pVtx[1].pos.z = 0.0f;
		
	pVtx[2].pos.x = SCREEN_LEFT;
	pVtx[2].pos.y = SCREEN_BOTTOM;
	pVtx[2].pos.z = 0.0f;
		 
	pVtx[3].pos.x = SCREEN_RIGHT;
	pVtx[3].pos.y = SCREEN_BOTTOM;
	pVtx[3].pos.z = 0.0f;

	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffsideCharacter->Unlock();

	pastMode = mode;
}

/*******************************************************************************
* �֐���	:void DrawSideCharacter( void )
*		�L�������w�i�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawSideCharacter( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffsideCharacter , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturesideCharacter );

	/*�L�������w�i�̂̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}