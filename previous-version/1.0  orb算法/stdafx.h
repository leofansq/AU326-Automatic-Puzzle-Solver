
//*************************************************************//
//                                                             //
//                   CyberDip ��������v1.3.1                   //
//                                                             //
//             ���� Qt5.7 OpenCV2.4.9  VS2013 �ȸ��߰汾        //
//             ���� FFmpeg 2.2.2                               //
//             ���� bbqScreenClient ��Դ����                    //
//             ���� grbl v0.8c (Baud 9600)                     //
//             ���� grbl v0.9j (Baud 115200)                   //
//                                                             //
//                       CVPRʵ���� ��Ʒ                        //
//                   �ص㣺����Ⱥ¥ 2 - 302B                    //
//                                                             //
//*************************************************************//
//����ı߿��ܱ����������������ǶԲ���ģ�ǿ��֢�Ƿ�����..

//*************************��ض���*****************************//
//************************Definitions***************************//

#define VIA_OPENCV
#define RANGE_X 25
#define RANGE_Y 43.5
#define UP_CUT 0

//*************************ʹ��˵��*****************************//
//************************Instruction***************************//

//STEP0:���Ϸ�QT�˵�������QT�汾(���û����˵��QT��VS�����װʧ��)
//STEP0:Configure QT version

//STEP1: ע�͵�#define VIA_OPENCV��������OpenCVǰ���Ա��뱾���� ����ͨ����������OpenCV����
//STEP1: Comment #define VIA_OPENCV and build -> Test your QT and FFmpeg settings.

//STEP2: ������������豸�ĳ����Լ����ڵ��ϱ�Ե
//RANGE  ��ʾ�豸��Ļ��CyberDIP�еĳߴ�
//UP_CUT ��ʾCaptureʱ��ȥ����ͷ���ı����Ĵ�С������ʹ��screenCaptureʱʹ�ã�ʹ��bbq��ʱ��Ӧ����Ϊ0.
//STEP2: Measure the range of your device in CyberDIP.
//RANGE  means the screen size of device in CyberDIP coordinates.
//UP_CUT is used to chop the title of video window.

//STEP3: ����OpenCV��ȡ��VIA_OPENCV��ע�� ���Ա��뱾����
//STEP3: Config OpenCV and uncomment VIA_OPENCV

//STEP4: ��usrGameController.h��usrGameController.cpp���޸���Ӧ��ͼ�������
//STEP4: Modify codes in usrGameController.h and usrGameController.cpp for image processing.

//ע�⣡����
//Ϊ�˷������ģ�����ֻ�޸�usrGameController.h��usrGameController.cpp�����ļ�

//���²��ظ���
#include <QtWidgets/QtWidgets>
