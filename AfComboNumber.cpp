/*******************************************************************************
* �^�C�g��	:�R���{��.cpp
* �t�@�C����:afComboNumber.cpp
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
#include "afComboNumber.h"
#include "input.h"
#include "unoMath.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_COMBONUMBER_NUMBER_OF_DIGITS	( 9 )	//�R���{������
#define AF_COMBONUMBER_WIDTH			( 83 )	//�R���{��������

#define AF_COMBONUMBER_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define AF_COMBONUMBER_SWING	( 3 )

#define AF_COMBONUMBER_ANIM_MAX	( 1 )	/*�A�j���[�V�����R�}��*/
#define AF_COMBONUMBER_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define AF_COMBONUMBER_ANIM_DIV_X	( 1 )	/*��������*/
#define AF_COMBONUMBER_ANIM_DIV_Y	( 10 )	/*����������*/

#define AF_COMBONUMBER_OUTPUT_SIZE	( 0.60 ) /*�����\���{��*/ 

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffafComboNumber = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureafComboNumber;	/*�R���{���̃e�N�X�`���̃|�C���^*/
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

static int g_AfComboNumberPosX, g_AfComboNumberPosY;
static bool g_aAfComboNumberUseFlag[AF_COMBONUMBER_NUMBER_OF_DIGITS];

static int nowAfComboNumber = 0;	//���݂̃R���{��
static int g_fadeNum;		//�t�F�[�h�A�E�g���
static int g_fadeColor;		//�t�F�[�h�A�E�g���
/*******************************************************************************
* �֐���	:HRESULT InitAfComboNumber( void )
*		�R���{���̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfComboNumber( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_COMBONUMBER_VERTEX_MAX * AF_COMBONUMBER_NUMBER_OF_DIGITS , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffafComboNumber , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfCcomboNumber.png" , &g_pTextureafComboNumber );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfCcomboNumber.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffafComboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / AF_COMBONUMBER_ANIM_DIV_X ) * AF_COMBONUMBER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_COMBONUMBER_ANIM_DIV_Y ) * AF_COMBONUMBER_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , AF_COMBONUMBER_NUMBER_OF_DIGITS );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffafComboNumber->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	
	//�e���\���t���O������
	for( int cntAfComboNumber = 0; cntAfComboNumber < AF_COMBONUMBER_NUMBER_OF_DIGITS; cntAfComboNumber++ )
	{	
		g_aAfComboNumberUseFlag[cntAfComboNumber] = false;
	}

	//�R���{����ʒu�ݒ�
	g_AfComboNumberPosX = AF_COMBONUMBER_POS_X + AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	g_AfComboNumberPosY = AF_COMBONUMBER_POS_Y;
	
	//�R���{��������
	nowAfComboNumber = 0;
	g_fadeNum = 0;
	g_fadeColor = 0;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitAfComboNumber( void )
*		�R���{���̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfComboNumber( void )
{
	if( g_pVtxBuffafComboNumber != NULL )
	{
		g_pVtxBuffafComboNumber->Release();
		g_pVtxBuffafComboNumber = NULL;
	}
	if( g_pTextureafComboNumber != NULL )
	{
		g_pTextureafComboNumber->Release();
		g_pTextureafComboNumber = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfComboNumber( void )
*		�R���{���̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfComboNumber( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	int afComboNumberCpy = nowAfComboNumber, outputAfComboNumberNum = 0, nCntAfComboNumber = AF_COMBONUMBER_NUMBER_OF_DIGITS, divideNum = 0;
	bool numPrintFlag = false;
	int posXPlus = 0, posYPlus = 0;
	float fadeDev = 0;
	int colorFade = 255;
	int numDigits = 0;
	int centerNum = 0;
	bool EvenOd = false;	//����� true�������@false���

	//�����擾�ƕ\���ʒu�ݒ�
	/*while( afComboNumberCpy != 0 )
	{
		afComboNumberCpy /= 10;
		numDigits++;
	}
	if( numDigits % 2 == 0 )
	{
		centerNum = numDigits / 2;
		EvenOd = true;
		g_AfComboNumberPosX = AF_COMBONUMBER_POS_X - numDigits / 2 * AF_COMBONUMBER_WIDTH - AF_COMBONUMBER_WIDTH * 0.5 + AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	else
	{
		if( numDigits - 1 != 0 )
		{
			centerNum = ( numDigits - 1 ) / 2;
		}
		else
		{
			centerNum = 0;
		}
		EvenOd = false;
		g_AfComboNumberPosX = AF_COMBONUMBER_POS_X - ( numDigits - 1 ) / 2 * AF_COMBONUMBER_WIDTH - AF_COMBONUMBER_WIDTH + AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	}
	*/
	g_AfComboNumberPosX -= ( AF_COMBONUMBER_NUMBER_OF_DIGITS - numDigits ) * AF_COMBONUMBER_WIDTH;

	afComboNumberCpy = nowAfComboNumber;

	//�i�X������
	g_fadeNum = ( g_fadeNum > 0 ? g_fadeNum - 1 : 0 );
	if( g_fadeNum == 0 )
	{
		g_fadeNum = 0;
		nowAfComboNumber = 0;
	}
	fadeDev = ( float )( g_fadeNum ) / ( float )( AF_COMBO_FADE_OUT );	
	fadeDev = sin( D3DX_PI / 2 * fadeDev );

	colorFade *= fadeDev;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffafComboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	//�e���\���t���O������
	nCntAfComboNumber = AF_COMBONUMBER_NUMBER_OF_DIGITS;
	while( nCntAfComboNumber-- )
	{	
		g_aAfComboNumberUseFlag[nCntAfComboNumber] = false;

		/*�p�^�[���A�j���[�V�������O�v�Z����*/
		if( ++g_nCounterAnim %= AF_COMBONUMBER_ANIM_SPAN )
		{
			++g_nPatternAnim %= AF_COMBONUMBER_ANIM_MAX;
			//g_nPatternAnim = ( ++g_nPatternAnim == AF_COMBONUMBER_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
		
		texWidth = 1.0f / AF_COMBONUMBER_ANIM_DIV_X;
		texHeight = 1.0f / AF_COMBONUMBER_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % AF_COMBONUMBER_ANIM_DIV_X );

		afComboNumberCpy = nowAfComboNumber;
		for( int cnt = 0; cnt < nCntAfComboNumber; cnt++ )
		{
			afComboNumberCpy /= 10;
		}

		outputAfComboNumberNum = afComboNumberCpy % 10;
		if( outputAfComboNumberNum != 0 )
		{
			numPrintFlag = true;
		}
		nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_COMBONUMBER_ANIM_DIV_X ) ) + outputAfComboNumberNum;
		
		if( numPrintFlag == true )
		{
			g_aAfComboNumberUseFlag[nCntAfComboNumber] = true;
		}
		fadeDev = ( float )( g_fadeNum ) / ( float )( AF_COMBO_FADE_OUT );
	//	colorFade *= COLOR_FADE_RATIO;

		polygonMag = 1.0f;// * ( fadeDev >= 0.9f ? fadeDev : 0.9f ); 
		colorFade = 255 * fadeDev;
		g_fadeColor = colorFade;
		if( g_aAfComboNumberUseFlag[nCntAfComboNumber] == true )
		{
			if( nCntAfComboNumber < centerNum )
			{
		//		g_AfComboNumberPosX += ( AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS + AF_COMBONUMBER_WIDTH ) * nCntAfComboNumber * ( 1.0f - fadeDev );
			}
			if( nCntAfComboNumber > centerNum )
			{
			//	g_AfComboNumberPosX -= ( AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS + AF_COMBONUMBER_WIDTH ) * nCntAfComboNumber * ( 1.0f - fadeDev );
			}
		}
		g_AfComboNumberPosX += AF_COMBONUMBER_WIDTH;

		
		g_AfComboNumberPosX = g_AfComboNumberPosX + ( rand() % AF_COMBONUMBER_SWING - rand() % AF_COMBONUMBER_SWING );
		g_AfComboNumberPosY = g_AfComboNumberPosY + ( rand() % AF_COMBONUMBER_SWING - rand() % AF_COMBONUMBER_SWING );
	//	posXPlus = ( AF_COMBONUMBER_WIDTH * nCntAfComboNumber - AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS ) * ( 1.0f - polygonMag );
	//	posYPlus = -SCREEN_HEIGHT * 0.25f * ( 1.0f - polygonMag );

		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , colorFade );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , colorFade );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , colorFade );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , colorFade );

		/*�e�N�X�`�����W����*/
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;
		
		/*���_���W�ݒ�*/
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].pos.x = g_AfComboNumberPosX + posXPlus + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].pos.y = g_AfComboNumberPosY + posYPlus + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].pos.x = g_AfComboNumberPosX + posXPlus + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].pos.y = g_AfComboNumberPosY + posYPlus + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].pos.x = g_AfComboNumberPosX + posXPlus + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].pos.y = g_AfComboNumberPosY + posYPlus + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].pos.z = 0.0f;   
										      			   
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].pos.x = g_AfComboNumberPosX + posXPlus + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].pos.y = g_AfComboNumberPosY + posYPlus + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
		pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].pos.z = 0.0f;
	}
	
	g_AfComboNumberPosX = AF_COMBONUMBER_POS_X + AF_COMBO_OUTPUT_POS_X_ADJUST_PLUS;
	g_AfComboNumberPosY = AF_COMBONUMBER_POS_Y;
	g_aAfComboNumberUseFlag[0] = true;
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffafComboNumber->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawAfComboNumber( void )
*		�R���{���̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfComboNumber( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffafComboNumber , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureafComboNumber );
	
	for( int nCntAfComboNumber = 0; nCntAfComboNumber < AF_COMBONUMBER_NUMBER_OF_DIGITS; nCntAfComboNumber++ )
	{
		if( g_aAfComboNumberUseFlag[nCntAfComboNumber] == true )
		{
			/*�R���{���̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX , 2 );
		}
	}
}


/*******************************************************************************
* �֐���	:void DrawAfComboNumber2( void )
*		�R���{���̉��Z�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfComboNumber2( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffafComboNumber , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureafComboNumber );

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffafComboNumber->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	for( int nCntAfComboNumber = 0; nCntAfComboNumber < AF_COMBONUMBER_NUMBER_OF_DIGITS; nCntAfComboNumber++ )
	{
		if( g_aAfComboNumberUseFlag[nCntAfComboNumber] == true && nowAfComboNumber != 0 )
		{
			pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );
			pVtx[nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 200 );

			/*�R���{���̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfComboNumber * AF_COMBONUMBER_VERTEX_MAX , 2 );
		}
	}	
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffafComboNumber->Unlock();
}

/*******************************************************************************
* �֐���	:void PlusAfComboNumber( int plusNum )
*		�R���{�����Z�֐��ł��B
* ����		:int plusNum	���Z�l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�R���{�������Z���܂��B
*******************************************************************************/
void PlusAfComboNumber( int plusNum )
{
	g_fadeNum = AF_COMBO_FADE_OUT;
	if( nowAfComboNumber + plusNum > AF_COMBONUMBER_MAX )
	{
		nowAfComboNumber = AF_COMBONUMBER_MAX;
	}
	else if( nowAfComboNumber + plusNum < 0 )
	{
		nowAfComboNumber = 0;
	}
	else
	{
		nowAfComboNumber += plusNum;
	}
}

/*******************************************************************************
* �֐���	:int GetAfComboNumber( void )
*		�R���{���擾�֐��ł��B
* ����		:void ��������
* �߂�l	:int g_nowAfComboNum	���݂̃R���{��
* ����		:�R���{�������Z���܂��B
*******************************************************************************/
int GetAfComboNumber( void )
{
	return nowAfComboNumber;
}

/*******************************************************************************
* �֐���	:int GetAfComboNum( void )
*		�R���{���t�F�[�h�l�擾�֐��ł��B
* ����		:void ��������
* �߂�l	:int g_fadeNum	���݂̃R���{���t�F�[�h�l
* ����		:�R���{�������Z���܂��B
*******************************************************************************/
int GetFadeAfComboNum( void )
{
	return g_fadeColor;
}

/*******************************************************************************
* �֐���	:resetAfComboNumber( int resetNum )
*		�R���{�����Z�b�g�֐��ł��B
* ����		:int resetNum	���Z�b�g��̒l
* �߂�l	:void �߂�l�Ȃ�
* ����		:�R���{�������Z�b�g���܂��B
*******************************************************************************/
void resetAfComboNumber( int resetNum )
{
	if( resetNum >= 0 && nowAfComboNumber <= AF_COMBONUMBER_MAX )
	{
		nowAfComboNumber = resetNum;
	}
}