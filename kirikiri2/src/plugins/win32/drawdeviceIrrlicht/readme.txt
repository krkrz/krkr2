������͂ȂɁH

�g���g������ Irrlicht ����舵�����߂̃v���O�C���ł��B

�������̓��L����

�EDirectX9 ��p�ō\�z����Ă��܂��B�R���p�C���ɂ� DirectX9 SDK ���K�v�ł��B

  ��DirectX9 ���g���Ȃ��ꍇ�� drawdevice �ݒ莞�ɗ�O��������܂�

�EIrrlicht ����̃t�@�C���A�N�Z�X�͋g���g���̃t�@�C����Ԃɑ΂��čs���܂�

���g����

1. Window �� drawDevice �ɑ΂��Ďw��\�ł�

-------------------------------------------
Plugins.link("krrlicht.dll");
var WIDTH=800;
var HEIGHT=600;
class MyWindow extends Window {
  var base;
  function MyWindow() {
    super.Window();
    setInnerSize(WIDTH, HEIGHT);
    // drawdevice �������ւ�
    drawDevice = new Irrlicht.DrawDevice(WIDTH,HEIGHT);
     // �v���C�}�����C������
    base = new Layer(this,null);
    base.setSize(WIDTH,HEIGHT);
    add(base);
  }
};
-------------------------------------------

2. ���C���ɑ΂��ĕ`�悪���s�ł��܂�

-------------------------------------------
var win = new Window();
win.visible = true;
var irr = new Irrlicht.SimpleDevice(win, 100, 100);
var layer = new Layer(win, null);

// XXX ��x�C�x���g���[�v�ɓ���ăf�o�C�X�����̉����Ă�����s����K�v����
irr.updateToLayer(layer);
-------------------------------------------

3. �q�E�C���h�E�Ƃ��� Irrlicht ��z�u�ł��܂�

-------------------------------------------
var win = new Window();
win.visible = true;
var irr = new Irrlicht.Window(win, 10, 10, 100, 100);
irr.visible = true;
-------------------------------------------

���ꂼ��̋@�\�ɂ��Ă� manual.tjs ���Q�Ƃ��Ă�������

������̗\��

�E�V�[�������n�̎���
�E�������V�[�����䏈��
�E�e��v���~�e�B�u����̎���

�EIrrlicht �Ǘ����ł̃��[�r�[�Đ������̎���
�@���ł���� krmovie �Ƃ��܂��A�g�ł���悤�ȂƗ��z�����ǁc

�����C�Z���X

Irrlicht �� zlib/libpng �X�^�C���̃��C�Z���X�ł��B

  The Irrlicht Engine License
  ===========================

  Copyright (C) 2002-2009 Nikolaus Gebhardt

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgement in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be clearly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

���̃v���O�C�����̂̃��C�Z���X�͋g���g���{�̂ɏ������Ă��������B
