/*******************************************************************************
* �^�C�g��	:�R���{�\��.cpp
* �t�@�C����:comboUi.cpp
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
#include "comboNumber.h"
#include "comboUi.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"
#include "player01.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define COMBOUI_POS_X		( COMBONUMBER_POS_X )		//�R���{�\���\���ʒu
#define COMBOUI_POS_Y		( COMBONUMBER_POS_Y )
#define COMBOUI_POS_PLUS_X	( -9 )
#define COMBOUI_POS_PLUS_Y	( 12 )

#define COMBOUI_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define COMBOUI_ANIM_MAX	( 1 )	/*�A�j���[�V�����R�}��*/
#define COMBOUI_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define COMBOUI_ANIM_DIV_X	( 3 )	/*��������*/
#define COMBOUI_ANIM_DIV_Y	( 1 )	/*����������*/

#define COMBOUI_OUTPUT_SIZE	( 1.3 ) /*�����\���{��*/ 

#define MOVE_NUM ( 2.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.20f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.05f )	/*������*/

#define USE_QUOTE	( 3 )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffcomboUi = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturecomboUi;	/*�R���{�\���̃e�N�X�`���̃|�C���^*/
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

static int g_ComboUiPosX, g_ComboUiPosY;

static int g_fadeNum;		//�t�F�[�h�A�E�g���
/*******************************************************************************
* �֐���	:HRESULT InitComboUi( void )
*		�R���{�\���̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitComboUi( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * COMBOUI_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffcomboUi , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/comboUi.png" , &g_pTexturecomboUi );
	D3DXGetImageInfoFromFile( "data/TEXTURE/comboUi.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffcomboUi->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / COMBOUI_ANIM_DIV_X ) * COMBOUI_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / COMBOUI_ANIM_DIV_Y ) * COMBOUI_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffcomboUi->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//�R���{�\����ʒu�ݒ�
	g_ComboUiPosX = COMBOUI_POS_X;
	g_ComboUiPosY = COMBOUI_POS_Y;
	
	//�R���{�\��������
	g_fadeNum = 0;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitComboUi( void )
*		�R���{�\���̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitComboUi( void )
{
	if( g_pVtxBuffcomboUi != NULL )
	{
		g_pVtxBuffcomboUi->Release();
		g_pVtxBuffcomboUi = NULL;
	}
	if( g_pTexturecomboUi != NULL )
	{
		g_pTexturecomboUi->Release();
		g_pTexturecomboUi = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateComboUi( void )
*		�R���{�\���̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateComboUi( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	int comboUiCpy = GetComboNumber(), outputComboUiNum = 0;
	bool numPrintFlag = false;
	int posXPlus = 0, posYPlus = 0;
	float fadeDev = 0;
	int colorFade = 255;
	int numDigits = 0;

	//�����擾�ƕ\���ʒu�ݒ�
	while( comboUiCpy != 0 )
	{
		comboUiCpy /= 10;
		numDigits++;
	}

	if( numDigits % 2 == 0 )
	{
		g_ComboUiPosX = COMBOUI_POS_X + numDigits / 2 * COMBONUMBER_WIDTH + COMBONUMBER_WIDTH * 0.5 + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	else
	{
		g_ComboUiPosX = COMBOUI_POS_X + ( numDigits - 1 ) / 2 * COMBONUMBER_WIDTH + COMBONUMBER_WIDTH + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	//g_ComboUiPosX -= ( COMBONUMBER_NUMBER_OF_DIGITS - numDigits ) * COMBONUMBER_WIDTH;

	//�i�X������
	g_fadeNum = GetFadeComboNum();
	fadeDev = ( float )( g_fadeNum ) / ( float )( COMBO_FADE_OUT );	
	fadeDev = sin( D3DX_PI / 2 * fadeDev );

	colorFade *= fadeDev;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffcomboUi->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	for( int nCntComboUi = 0; nCntComboUi < USE_QUOTE; nCntComboUi++ )
	{
		/*�p�^�[���A�j���[�V�������O�v�Z����*/
		if( ++g_nCounterAnim %= COMBOUI_ANIM_SPAN )
		{
			++g_nPatternAnim %= COMBOUI_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == COMBOUI_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / COMBOUI_ANIM_DIV_X;
		texHeight = 1.0f / COMBOUI_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % COMBOUI_ANIM_DIV_X ) + nCntComboUi;
		nowPatternY = ( float )( ( int )( g_nPatternAnim / COMBOUI_ANIM_DIV_X ) ) + outputComboUiNum;
		
		if( nCntComboUi == 0 )
		{
			fadeDev = ( float )( g_fadeNum ) / ( float )( COMBO_FADE_OUT );
			colorFade *= COLOR_FADE_RATIO;

			polygonMag = 1.0f * fadeDev;
			//polygonMag = COMBOUI_OUTPUT_SIZE;
			g_ComboUiPosX -= COMBOUI_POS_X * 0.05 * fadeDev + COMBOUI_POS_PLUS_X;
			g_ComboUiPosY += COMBOUI_POS_Y * 0.25 * fadeDev + COMBOUI_POS_PLUS_Y;
 			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			g_ComboUiPosX *= fadeDev;
			posYPlus = -SCREEN_HEIGHT * 0.25f * ( 1.0f - polygonMag );
		}
		else
		{
			fadeDev = ( float )( g_fadeNum ) / ( float )( COMBO_FADE_OUT );
			polygonMag = ( 0.5f + sqrt( ( ( float )( GetComboNumber() ) / ( float )( COMBONUMBER_MAX ) ) ) ) * fadeDev;
			g_ComboUiPosX = GetPlayerState01()->posX;
			g_ComboUiPosY = GetPlayerState01()->posY;
 			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
			posYPlus = 0;
		}

		/*�e�N�X�`�����W����*/
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

		/*���_���W�ݒ�*/
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].pos.x = g_ComboUiPosX + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].pos.y = g_ComboUiPosY + posYPlus + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].pos.x = g_ComboUiPosX + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].pos.y = g_ComboUiPosY + posYPlus + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].pos.x = g_ComboUiPosX + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].pos.y = g_ComboUiPosY + posYPlus + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].pos.x = g_ComboUiPosX + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].pos.y = g_ComboUiPosY + posYPlus + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_ComboUiPosX = COMBOUI_POS_X;
	g_ComboUiPosY = COMBOUI_POS_Y;
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffcomboUi->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawComboUi( void )
*		�R���{�\���̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawComboUi( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffcomboUi , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturecomboUi );
	
	for( int nCntComboUi = 0; nCntComboUi < USE_QUOTE; nCntComboUi++ )
	{
		if( GetFadeComboNum() >= 0 && GetComboNumber() != 0 )
		{
			/*�R���{�\���̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntComboUi * COMBOUI_VERTEX_MAX , 2 );
		}
	}
}


/*******************************************************************************
* �֐���	:void DrawComboUi2( void )
*		�R���{�\���̉��Z�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawComboUi2( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffcomboUi , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturecomboUi );
	
	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffcomboUi->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	for( int nCntComboUi = 0; nCntComboUi < USE_QUOTE; nCntComboUi++ )
	{
		if( GetFadeComboNum() >= 0 && GetComboNumber() != 0 )
		{
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );

			if( nCntComboUi != 0 )
			{
				//���Z�����ւ̐؂�ւ�
				GetDevice()->SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_REVSUBTRACT );
				GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_ZERO | D3DBLEND_SRCALPHA );
				GetDevice()->SetRenderState( D3DRS_DESTBLEND ,  D3DBLEND_ONE );

				pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 100 );
				pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 100 );
				pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 100 );
				pVtx[nCntComboUi * COMBOUI_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 100 );

			}
			/*�R���{�\���̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntComboUi * COMBOUI_VERTEX_MAX , 2 );
		}
	}	
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffcomboUi->Unlock();

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*�A���t�@�u�����h�L����*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*�X�N���[�����u�����h�ݒ�*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*�|���S�����u�����h�ݒ�*/
}
