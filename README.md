qemu2sv
=======

RTL Simulator(�ȉ�SV Sim)�ƃI�[�v���\�[�X�̃G�~�����[�^�ł���QEMU��ڑ���
�ꏏ�ɃV�~�����[�V������������ł��B  
QEMU���͂��炩���ߗp�ӂ���Ă���uxilinx-zynq-a9�v�̃A�[�L�e�N�`���ɒʐM�p���W���[����g�ݍ��݂܂����B
SV Sim����DPI-C��p����QEMU�ƒʐM�����Ă��܂��B  
�ʐM�ɂ̓\�P�b�g�ʐM���g�p���Ă��܂�(IP:127.0.0.1 �|�[�g�ԍ�:15785)  

QEMU��SV Sim�ԂŃT�|�[�g���Ă���]����
�V���O��Write/Read�݂̂Ńo�[�X�g�]���⊄�荞�݂ɂ͑Ή����Ă��܂���B

�܂�QEMU��SV Sim�Ԃ̃V�~�����[�V�������Ԃ𓯊����Ă��Ȃ���
SV Sim���猩���QEMU��0���Ԃŉ������Ă���l�Ɍ����܂��B

�V�X�e���S�̂́��̗l�ɂȂ��Ă��܂��B
![qemu2sv_01](images/qemu2sv_01.jpg "qemu2sv_01")

#�g����
##QEMU�r���h
1. [http://wiki.qemu.org/Download](http://wiki.qemu.org/Download)����uqemu-2.1.2.tar.bz2�v���_�E�����[�h  
2. qemu-2.1.2.tar.bz2���𓀂���qemu2sv/qemu�t�H���_�̒��g���㏑��  (hw��default-configs���㏑������܂�)  
3. �ȉ��R�}���h�����s���ăr���h  
>cd qemu-2.1.2
>mkdir build  
>cd build  
>../configure --target-list="arm-softmmu,microblazeel-softmmu" --enable-fdt --disable-kvm  
>make  

4.`make install`����`build/arm-softmmu/qemu-system-arm`
     ��Path��ݒ肵�Ď��s�\��Ԃɂ��܂��B  
5.`qemu-system-arm -M xilinx-zynq-a9 -nographic`
�����s���҂��󂯏�ԂɂȂ邩�m�F���܂�  
(15�b�o�߂�TimeOut���܂��B)
```
>qemu-system-arm -M xilinx-zynq-a9 -nographic
[hw_if] Socket is listening for connections... Please start the rtl simulation.
        Timeout after 15 sec
```
##SV Sim ����
�t����```qemu2sv/sv/Makefile```��Riviera-PRO�AModelSim�p�ł��B

1.qemu2sv/sv/Makefile��ҏW  
* MODELSIM_HOME�̃p�X�����ɍ��킹�C��(sv_dpi.h������p�X�ɕύX)  
* 64bit���̏ꍇ-m32��-m64�ɕύX����  
    ��ModelSim Altera Starter Edition���g���ꍇ��64bit���ł�-m32�ɂ��ĉ�����

2.```make```�ŃV�~�����[�V���������s�ł��܂��B  
  ���s��```Start server connect...```�ƕ\������A�������Ȃ����15�b��TimeOut���܂��B

##SV Sim ����
QEMU���ʐM�҂��󂯏�Ԃ̎���SV Sim�����s�����QEMU��SV Sim���ڑ����܂��B  
����Ŋm�F�͊����ł��B  
ZynqLinux����p�ӂ��A�h���X`0xE0004000`�փ��[�hor���C�g���s����SV Sim�������O���o�͂��܂��B
