/**
* CTBeta R308 Library (GPLv3)
* Support by CTBeta Team http://www.ctbeta.org/
* Created by Jason C.H Nov 2016
*/

#ifndef  CTB_R308_H
#define CTB_R308_H

class CTB_R308

{

public:

	unsigned char FP_SerialRead[10];//读取的返回数据

	CTB_R308();
	bool init();
	bool Cmd_Get_Img();
	bool Cmd_Img_To_Buffer1();
  bool Cmd_Img_To_Buffer2();
  bool Cmd_Reg_Model();
	bool Cmd_Save_Finger();
  bool Cmd_Delete_All_Model();
  void Cmd_Delete_Model(unsigned int ID_temp);
  bool Cmd_Search_Finger();

private:

	unsigned char FP_Init[10]={0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1B};//模块握手验证
	unsigned char FP_Pack_Head[6]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF};//协议包头
	unsigned char FP_Get_Img[6]={0x01,0x00,0x03,0x01,0x0,0x05};//获得指纹图像
	unsigned char FP_Templete_Num[6]={0x01,0x00,0x03,0x1D,0x00,0x21 }; //获得模版总数
	unsigned char FP_Search[11]={0x01,0x0,0x08,0x04,0x01,0x0,0x0,0x01,0xF4,0x0,0xB2};//搜索指纹搜索范围0 - 929
	unsigned char FP_Img_To_Buffer1[7]={0x01,0x0,0x04,0x02,0x01,0x0,0x08};//将图像放入到BUFFER1
	unsigned char FP_Img_To_Buffer2[7]={0x01,0x0,0x04,0x02,0x02,0x0,0x09};//将图像放入到BUFFER2
	unsigned char FP_Reg_Model[6]={0x01,0x0,0x03,0x05,0x0,0x09};//将BUFFER1跟BUFFER2合成特征模版
	unsigned char FP_Delet_All_Model[6]={0x01,0x0,0x03,0x0d,0x00,0x11};//删除指纹模块里所有的模版
	unsigned char FP_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x02,0x00,0x10};//将BUFFER1中的特征码存放到指定的位置
	unsigned char FP_Delete_Model[10]={0x01,0x00,0x07,0x0C,0x0,0x0,0x0,0x1,0x0,0x0};//删除指定的模版
	//unsigned char FP_Search_0_9[11]={0x01,0x0,0x08,0x04,0x01,0x0,0x0,0x0,0x13,0x0,0x21};//搜索0-9号指纹
	//volatile unsigned char FINGER_NUM;

	void SerialClean();
	bool SerialRead();

};

#endif
