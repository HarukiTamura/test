/*******************************************************************************
* �^�C�g��	:�R���{��.cpp
* �t�@�C����:comboNumber.cpp
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
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define COMBONUMBER_NUMBER_OF_DIGITS	( 9 )	//�R���{������
#define COMBONUMBER_WIDTH				( 50 )	//�R���{��������


#define COMBONUMBER_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define COMBONUMBER_ANIM_MAX	( 1 )	/*�A�j���[�V�����R�}��*/
#define COMBONUMBER_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define COMBONUMBER_ANIM_DIV_X	( 1 )	/*��������*/
#define COMBONUMBER_ANIM_DIV_Y	( 10 )	/*����������*/

#define COMBONUMBER_OUTPUT_SIZE	( 0.38 ) /*�����\���{��*/ 

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffcomboNumber = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturecomboNumber;	/*�R���{���̃e�N�X�`���̃|�C���^*/
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

static int g_ComboNumberPosX, g_ComboNumberPosY;
static bool g_aComboNumberUseFlag[COMBONUMBER_NUMBER_OF_DIGITS];

static int nowComboNumber = 0;	//���݂̃R���{��
static int g_fadeNum;		//�t�F�[�h�A�E�g���
/*******************************************************************************
* �֐���	:HRESULT InitComboNumber( void )
*		�R���{���̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitComboNumber( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * COMBONUMBER_VERTEX_MAX * COMBONUMBER_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffcomboNumber , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/comboNumber.png" , &g_pTexturecomboNumber );
	D3DXGetImageInfoFromFile( "data/TEXTURE/comboNumber.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffcomboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / COMBONUMBER_ANIM_DIV_X ) * COMBONUMBER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / COMBONUMBER_ANIM_DIV_Y ) * COMBONUMBER_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , COMBONUMBER_NUMBER_OF_DIGITS );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffcomboNumber->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	
	//�e���\���t���O������
	for( int cntComboNumber = 0; cntComboNumber < COMBONUMBER_NUMBER_OF_DIGITS; cntComboNumber++ )
	{	
		g_aComboNumberUseFlag[cntComboNumber] = false;
	}

	//�R���{����ʒu�ݒ�
	g_ComboNumberPosX = COMBONUMBER_POS_X + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	g_ComboNumberPosY = COMBONUMBER_POS_Y;
	
	//�R���{��������
	nowComboNumber = 0;
	g_fadeNum = 0;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitComboNumber( void )
*		�R���{���̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitComboNumber( void )
{
	if( g_pVtxBuffcomboNumber != NULL )
	{
		g_pVtxBuffcomboNumber->Release();
		g_pVtxBuffcomboNumber = NULL;
	}
	if( g_pTexturecomboNumber != NULL )
	{
		g_pTexturecomboNumber->Release();
		g_pTexturecomboNumber = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateComboNumber( void )
*		�R���{���̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateComboNumber( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	int comboNumberCpy = nowComboNumber, outputComboNumberNum = 0, nCntComboNumber = COMBONUMBER_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	int posXPlus = 0, posYPlus = 0;
	float fadeDev = 0;
	int colorFade = 255;
	int numDigits = 0;

	//�����擾�ƕ\���ʒu�ݒ�
	while( comboNumberCpy != 0 )
	{
		comboNumberCpy /= 10;
		numDigits++;
	}
	if( numDigits % 2 == 0 )
	{
		g_ComboNumberPosX = COMBONUMBER_POS_X - numDigits / 2 * COMBONUMBER_WIDTH - COMBONUMBER_WIDTH * 0.5 + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	else
	{
		g_ComboNumberPosX = COMBONUMBER_POS_X - ( numDigits - 1 ) / 2 * COMBONUMBER_WIDTH - COMBONUMBER_WIDTH + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	g_ComboNumberPosX -= ( COMBONUMBER_NUMBER_OF_DIGITS - numDigits ) * COMBONUMBER_WIDTH;
	comboNumberCpy = nowComboNumber;

	//�i�X������
	g_fadeNum = ( g_fadeNum > 0 ? g_fadeNum - 1 : 0 );
	if( g_fadeNum == 0 )
	{
		g_fadeNum = 0;
		nowComboNumber = 0;
	}
	fadeDev = ( float )( g_fadeNum ) / ( float )( COMBO_FADE_OUT );	
	fadeDev = sin( D3DX_PI / 2 * fadeDev );

	colorFade *= fadeDev;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffcomboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	//�e���\���t���O������
	nCntComboNumber = COMBONUMBER_NUMBER_OF_DIGITS;
	while( nCntComboNumber-- )
	{	
		g_aComboNumberUseFlag[nCntComboNumber] = false;

		/*�p�^�[���A�j���[�V�������O�v�Z����*/
		if( ++g_nCounterAnim %= COMBONUMBER_ANIM_SPAN )
		{
			++g_nPatternAnim %= COMBONUMBER_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == COMBONUMBER_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / COMBONUMBER_ANIM_DIV_X;
		texHeight = 1.0f / COMBONUMBER_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % COMBONUMBER_ANIM_DIV_X );

		comboNumberCpy = nowComboNumber;
		for( int cnt = 0; cnt < nCntComboNumber; cnt++ )
		{
			comboNumberCpy /= 10;
		}

		outputComboNumberNum = comboNumberCpy % 10;
		if( outputComboNumberNum != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / COMBONUMBER_ANIM_DIV_X ) ) + outputComboNumberNum;
		
		if( numPrintFlag == true )
		{
			g_aComboNumberUseFlag[nCntComboNumber] = true;
		}
		fadeDev = ( float )( g_fadeNum ) / ( float )( COMBO_FADE_OUT );
	//	colorFade *= COLOR_FADE_RATIO;

		polygonMag = 1.0f * fadeDev; 

		g_ComboNumberPosX += COMBONUMBER_WIDTH * polygonMag;
		posXPlus = ( COMBONUMBER_WIDTH * nCntComboNumber - COMBO_OUTPUT_POS_X_ADJUST_PLUS ) * ( 1.0f - polygonMag );
		posYPlus = -SCREEN_HEIGHT * 0.25f * ( 1.0f - polygonMag );

		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( colorFade , colorFade , colorFade , colorFade );

		/*�e�N�X�`�����W����*/
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;
		
		/*���_���W�ݒ�*/
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].pos.x = g_ComboNumberPosX + posXPlus + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].pos.y = g_ComboNumberPosY + posYPlus + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].pos.x = g_ComboNumberPosX + posXPlus + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].pos.y = g_ComboNumberPosY + posYPlus + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].pos.x = g_ComboNumberPosX + posXPlus + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].pos.y = g_ComboNumberPosY + posYPlus + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].pos.x = g_ComboNumberPosX + posXPlus + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].pos.y = g_ComboNumberPosY + posYPlus + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_ComboNumberPosX = COMBONUMBER_POS_X + COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	g_ComboNumberPosY = COMBONUMBER_POS_Y;
	g_aComboNumberUseFlag[0] = true;
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffcomboNumber->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawComboNumber( void )
*		�R���{���̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawComboNumber( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffcomboNumber , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturecomboNumber );
	
	for( int nCntComboNumber = 0; nCntComboNumber < COMBONUMBER_NUMBER_OF_DIGITS; nCntComboNumber++ )
	{
		if( g_aComboNumberUseFlag[nCntComboNumber] == true )
		{
			/*�R���{���̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntComboNumber * COMBONUMBER_VERTEX_MAX , 2 );
		}
	}
}


/*******************************************************************************
* �֐���	:void DrawComboNumber2( void )
*		�R���{���̉��Z�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawComboNumber2( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffcomboNumber , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturecomboNumber );

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffcomboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	for( int nCntComboNumber = 0; nCntComboNumber < COMBONUMBER_NUMBER_OF_DIGITS; nCntComboNumber++ )
	{
		if( g_aComboNumberUseFlag[nCntComboNumber] == true && nowComboNumber != 0 )
		{
			pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntComboNumber * COMBONUMBER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );

			/*�R���{���̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntComboNumber * COMBONUMBER_VERTEX_MAX , 2 );
		}
	}	
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffcomboNumber->Unlock();
}

/*******************************************************************************
* �֐���	:void PlusComboNumber( int plusNum )
*		�R���{�����Z�֐��ł��B
* ����		:int plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�R���{�������Z���܂��B
*******************************************************************************/
void PlusComboNumber( int plusNum )
{
	g_fadeNum = COMBO_FADE_OUT;
	if( nowComboNumber + plusNum > COMBONUMBER_MAX )
	{
		nowComboNumber = COMBONUMBER_MAX;
	}
	else if( nowComboNumber + plusNum < 0 )
	{
		nowComboNumber = 0;
	}
	else
	{
		nowComboNumber += plusNum;
	}
}

/*******************************************************************************
* �֐���	:int GetComboNumber( void )
*		�R���{���擾�֐��ł��B
* ����		:void ��������
* �߂�l	:int g_nowComboNum	���݂̃R���{��
* ����		:�R���{�������Z���܂��B
*******************************************************************************/
int GetComboNumber( void )
{
	return nowComboNumber;
}

/*******************************************************************************
* �֐���	:int GetComboNum( void )
*		�R���{���t�F�[�h�l�擾�֐��ł��B
* ����		:void ��������
* �߂�l	:int g_fadeNum	���݂̃R���{���t�F�[�h�l
* ����		:�R���{�������Z���܂��B
*******************************************************************************/
int GetFadeComboNum( void )
{
	return g_fadeNum;
}

/*******************************************************************************
* �֐���	:resetComboNumber( int resetNum )
*		�R���{�����Z�b�g�֐��ł��B
* ����		:int resetNum	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�R���{�������Z�b�g���܂��B
*******************************************************************************/
void resetComboNumber( int resetNum )
{
	if( resetNum >= 0 && nowComboNumber <= COMBONUMBER_MAX )
	{
		nowComboNumber = resetNum;
	}
}