//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Definition of Messages and Message Related Utilities
//---------------------------------------------------------------------------
#ifndef MsgImplH
#define MsgImplH

#include "tjsMessage.h"

#ifndef TVP_MSG_DECL
	#define TVP_MSG_DECL(name, msg) extern tTJSMessageHolder name;
#endif

//---------------------------------------------------------------------------
// Message Strings ( these should be localized )
//---------------------------------------------------------------------------
// Japanese localized messages
TVP_MSG_DECL(TVPScriptExceptionRaised,
	TJS_W("�X�N���v�g�ŗ�O���������܂���"));

TVP_MSG_DECL(TVPHardwareExceptionRaised,
	TJS_W("�n�[�h�E�F�A��O���������܂���"));

TVP_MSG_DECL(TVPMainCDPName,
	TJS_W("�X�N���v�g�G�f�B�^ (���C��)"));

TVP_MSG_DECL(TVPExceptionCDPName,
	TJS_W("�X�N���v�g�G�f�B�^ (��O�ʒm)"));

TVP_MSG_DECL(TVPCannnotLocateUIDLLForFolderSelection,
	TJS_W("�t�H���_/�A�[�J�C�u�̑I����ʂ�\�����悤�Ƃ��܂����� ")
		TJS_W("krdevui.dll ��������Ȃ��̂ŕ\���ł��܂���.\n")
		TJS_W("���s����t�H���_/�A�[�J�C�u�̓R�}���h���C���̈�Ƃ��Ďw�肵�Ă�������"));

TVP_MSG_DECL(TVPInvalidUIDLL,
	TJS_W("krdevui.dll ���ُ킩�A�o�[�W��������v���܂���"));

TVP_MSG_DECL(TVPInvalidBPP,
	TJS_W("����ȐF�[�x�ł�"));

TVP_MSG_DECL(TVPCannotLoadPlugin,
	TJS_W("�v���O�C�� %1 ��ǂݍ��߂܂���"));

TVP_MSG_DECL(TVPNotValidPlugin,
	TJS_W("%1 �͗L��ȃv���O�C���ł͂���܂���"));

TVP_MSG_DECL(TVPPluginUninitFailed,
	TJS_W("�v���O�C���̉��Ɏ��s���܂���"));

TVP_MSG_DECL(TVPCannnotLinkPluginWhilePluginLinking,
	TJS_W("�v���O�C���̐ڑ����ɑ��̃v���O�C����ڑ����邱�Ƃ͂ł��܂܂���"));

TVP_MSG_DECL(TVPNotSusiePlugin,
	TJS_W("�ُ�� Susie �v���O�C���ł�"));

TVP_MSG_DECL(TVPSusiePluginError,
	TJS_W("Susie �v���O�C���ŃG���[���������܂���/�G���[�R�[�h %1"));

TVP_MSG_DECL(TVPCannotReleasePlugin,
	TJS_W("�w�肳�ꂽ�v���O�C���͎g�p���̂��߉��ł��܂���"));

TVP_MSG_DECL(TVPNotLoadedPlugin,
	TJS_W("%1 �͓ǂݍ��܂�Ă��܂���"));

TVP_MSG_DECL(TVPCannotAllocateBitmapBits,
	TJS_W("�r�b�g�}�b�v�p���������m�ۂł��܂���/%1(size=%2)"));

TVP_MSG_DECL(TVPScanLineRangeOver,
	TJS_W("�X�L�������C�� %1 �͔͈�(0�`%2)�𒴂��Ă��܂�"));

TVP_MSG_DECL(TVPPluginError,
	TJS_W("�v���O�C���ŃG���[���������܂���/%1"));

TVP_MSG_DECL(TVPInvalidCDDADrive,
	TJS_W("�w�肳�ꂽ�h���C�u�ł� CD-DA ���Đ��ł��܂���"));

TVP_MSG_DECL(TVPCDDADriveNotFound,
	TJS_W("CD-DA ���Đ��ł���h���C�u��������܂���"));

TVP_MSG_DECL(TVPMCIError,
	TJS_W("MCI �ŃG���[���������܂��� : %1"));

TVP_MSG_DECL(TVPInvalidSMF,
	TJS_W("�L��� SMF �t�@�C���ł͂���܂��� : %1"));

TVP_MSG_DECL(TVPMalformedMIDIMessage,
	TJS_W("�w�肳�ꂽ���b�Z�[�W�� MIDI ���b�Z�[�W�Ƃ��ėL��Ȍ`���ł͂���܂���"));

TVP_MSG_DECL(TVPCannotInitDirectSound,
	TJS_W("DirectSound ������ł��܂��� : %1"));

TVP_MSG_DECL(TVPCannotCreateDSSecondaryBuffer,
	TJS_W("DirectSound �Z�J���_���o�b�t�@���쐬�ł��܂��� : %1/%2"));

TVP_MSG_DECL(TVPInvalidLoopInformation,
	TJS_W("���[�v��� %1 �ُ͈�ł�"));

TVP_MSG_DECL(TVPNotChildMenuItem,
	TJS_W("�w�肳�ꂽ���j���[���ڂ͂��̃��j���[���ڂ̎q�ł͂���܂���"));

TVP_MSG_DECL(TVPCannotInitDirectDraw,
	TJS_W("DirectDraw ������ł��܂��� : %1"));

TVP_MSG_DECL(TVPCannotFindDisplayMode,
	TJS_W("�K�������ʃ��[�h��������܂��� : %1"));

TVP_MSG_DECL(TVPCannotSwitchToFullScreen,
	TJS_W("�t���X�N���[���ɐ؂�ւ����܂��� : %1"));

TVP_MSG_DECL(TVPInvalidPropertyInFullScreen,
	TJS_W("�t���X�N���[�����ł͑���ł��Ȃ��v���p�e�B��ݒ肵�悤�Ƃ��܂���"));

TVP_MSG_DECL(TVPInvalidMethodInFullScreen,
	TJS_W("�t���X�N���[�����ł͑���ł��Ȃ����\�b�h���Ăяo�����Ƃ��܂���"));

TVP_MSG_DECL(TVPCannotLoadCursor,
	TJS_W("�}�E�X�J�[�\�� %1 �̓ǂݍ��݂Ɏ��s���܂���"));

TVP_MSG_DECL(TVPCannotLoadKrMovieDLL,
	TJS_W("�r�f�I/Shockwave Flash ���Đ����邽�߂ɂ� krmovie.dll / krflash.dll ")
		TJS_W("���K�v�ł��� �ǂݍ��ނ��Ƃ��ł��܂���"));

TVP_MSG_DECL(TVPInvalidKrMovieDLL,
	TJS_W("krmovie.dll/krflash.dll ���ُ킩 �Ή��ł��Ȃ��o�[�W�����ł�"));

TVP_MSG_DECL(TVPErrorInKrMovieDLL,
	TJS_W("krmovie.dll/krflash.dll ���ŃG���[���������܂���/%1"));

TVP_MSG_DECL(TVPWindowAlreadyMissing,
	TJS_W("�E�B���h�E�͂��łɑ��݂��Ă��܂���"));

TVP_MSG_DECL(TVPPrerenderedFontMappingFailed,
	TJS_W("�����_�����O�ς݃t�H���g�̃}�b�s���O�Ɏ��s���܂��� : %1"));

TVP_MSG_DECL_CONST(TVPConfigFailOriginalFileCannotBeRewritten,
	TJS_W("%1 �ɏ������݂ł��܂���B�\�t�g�E�F�A�����s���̂܂܂ɂȂ��Ă��Ȃ����A���邢��")
		TJS_W("�������݌��������邩�ǂ������m�F���Ă�������"));

TVP_MSG_DECL(TVPConfigFailTempExeNotErased,
	TJS_W("%1 �̏I�����m�F�ł��Ȃ����߁A������폜�ł��܂���ł���(���̃t�@�C���͍폜���Č��\�ł�)"));

TVP_MSG_DECL_CONST(TVPExecutionFail,
	TJS_W("%1 �����s�ł��܂���"));

TVP_MSG_DECL(TVPPluginUnboundFunctionError,
	TJS_W("�v���O�C������֐� %1 ��v������܂������A���̊֐��͖{�̓��ɑ��݂��܂���B�v���O�C����")
			TJS_W("�{�̂̃o�[�W�������������Ή����Ă��邩�m�F���Ă�������"));

TVP_MSG_DECL(TVPExceptionHadBeenOccured,
	TJS_W(" = (��O����)"));

TVP_MSG_DECL(TVPConsoleResult,
	TJS_W("�R���\�[�� : "));


//---------------------------------------------------------------------------



#endif
