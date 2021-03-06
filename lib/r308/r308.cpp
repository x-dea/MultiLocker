/**
 * @brief A library for R308 fingerprint module.
 *
 * @file r308.cpp
 * @author Jason C.H
 * @date 2018-09-02
 */

#include <r308.h>

const uint8_t R308::packHead[6] PROGMEM = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t R308::packInit[10] PROGMEM = {0x01, 0x00, 0x07, 0x13, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x1B};
const uint8_t R308::packGetImg[6] PROGMEM = {0x01, 0x00, 0x03, 0x01, 0x0, 0x05};
const uint8_t R308::packToBuffer1[7] PROGMEM = {0x01, 0x0, 0x04, 0x02,
                                                0x01, 0x0, 0x08};
const uint8_t R308::packToBuffer2[7] PROGMEM = {0x01, 0x0, 0x04, 0x02,
                                                0x02, 0x0, 0x09};
const uint8_t R308::packRegModel[6] PROGMEM = {0x01, 0x0, 0x03,
                                               0x05, 0x0, 0x09};
const uint8_t R308::packEmpty[6] PROGMEM = {0x01, 0x0, 0x03, 0x0d, 0x00, 0x11};

/**
 * @brief Send package (PGM)
 * @brief 发送包 (PGM)
 *
 * @param pack Package to send.需要发送的包
 * @param length 包长度
 */
inline void R308::sendPack(const uint8_t *pack, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) SerPort.write(pgm_read_byte(pack + i));
}

/**
 * @brief Clean serial port cache and packSerialRead
 * @brief 清空串口缓存和上次接收的数据
 */
inline void R308::serialClean() {
  for (int8_t i = 0; i < 10; i++) packSerialRead[i] = 0xFF;
  while (SerPort.read() >= 0) {
  }
}

/**
 * @brief Read data from serial port.
 * @brief 从串口读取数据
 *
 * @return true Finished.收包完成
 * @return false Failed.收包或包头校验失败
 */
bool R308::serialRead() {
  // Wait for data stream.等待数据流
  unsigned long timeStart = millis();
  while (SerPort.available() <= 0 && millis() - timeStart >= 0 &&
         millis() - timeStart < 3000) {
  }

  // Verify pack head & sign.校验包头与标识
  if (SerPort.read() == 0xEF && SerPort.read() == 0x01) {
    for (uint8_t i = 0; i < 4; i++) {
      if (SerPort.read() != packHead[i + 2]) return false;
    }
    if (SerPort.read() != 0x07) return false;
  } else
    return false;

  // Receive pack.接收包
  packSerialRead[0] = ((SerPort.read() << 8) | SerPort.read());
  for (uint8_t i = 1; i <= packSerialRead[0]; i++)
    packSerialRead[i] = SerPort.read();
  return true;
}

R308::R308() {}

/**
 * @brief Initialize module
 * @brief 初始化函数库，对指纹模块进行握手验证
 */
void R308::init() {
  SerPort.begin(57600);
  serialClean();
  sendPack(packHead, 6);
  sendPack(packInit, 10);
  delay(10);
}

/**
 * @brief Get fingerprint image.
 * @brief 获得指纹图像
 *
 * @return int8_t -1:Failed.收包失败
 *                 0:Succeeded.成功
 *                 1:Pack error.收包有误
 *                 2:No finger.传感器无手指
 *                 3:Scan Failed.录入失败
 */
int8_t R308::cmdGetImg() {
  serialClean();
  sendPack(packHead, 6);
  sendPack(packGetImg, 6);
  delay(300);
  if (serialRead() == true)
    return packSerialRead[2];
  else
    return -1;
}

/**
 * @brief Put fingerprint image to buffer1
 * @brief 将图像转换成特征码存放在缓冲区 1 中
 *
 * @return int8_t -1:Failed.收包失败
 *                 0:Succeeded.成功
 *                 1:Pack error.收包有误
 *                 6:Bad image.图像太乱
 *                 7:Few features.特征点过少
 *                 15:No image in buffer.没有图像
 */
int8_t R308::cmdToBuffer1() {
  serialClean();
  sendPack(packHead, 6);
  sendPack(packToBuffer1, 7);
  delay(500);
  if (serialRead() == true)
    return packSerialRead[2];
  else
    return -1;
}

/**
 * @brief Put fingerprint image to buffer2
 * @brief 将图像转换成特征码存放在缓冲区 2 中
 *
 * @return int8_t -1:Failed.收包失败
 *                 0:Succeeded.成功
 *                 1:Pack error.收包有误
 *                 6:Bad image.图像太乱
 *                 7:Few features.特征点过少
 *                 15:No image in buffer.没有图像
 */
int8_t R308::cmdToBuffer2() {
  serialClean();
  sendPack(packHead, 6);
  sendPack(packToBuffer2, 7);
  delay(500);
  if (serialRead() == true)
    return packSerialRead[2];
  else
    return -1;
}

/**
 * @brief Merge buffers and generate model.
 * @brief 将缓冲区中的特征码合并成指纹模版
 *
 * @return int8_t -1:Failed.收包失败
 *                 0:Succeeded.成功
 *                 1:Pack error.收包有误
 *                 A:Merge error:Not same finger.合并错误:非同一手指
 */
int8_t R308::cmdRegModel() {
  serialClean();
  sendPack(packHead, 6);
  sendPack(packRegModel, 6);
  delay(50);
  if (serialRead() == true)
    return packSerialRead[2];
  else
    return -1;
}

/**
 * @brief Delete all models.
 * @brief 删除指纹模块里的所有指纹模版
 *
 * @return int8_t -1:Failed.收包失败
 *                 0:Succeeded.成功
 *                 1:Pack error.收包有误
 *                11:Empty failed.清空失败
 */
int8_t R308::cmdEmpty() {
  serialClean();
  sendPack(packHead, 6);
  sendPack(packEmpty, 6);
  delay(1000);
  if (serialRead() == true)
    return packSerialRead[2];
  else
    return -1;
}

/**
 * @brief Save fingerprint from buffer to page.
 * @brief 将缓冲区中的特征码存放到指定的位置
 *
 * @param bufferID 缓冲区号
 * @param pageID 指纹库位置
 * @return int8_t -1:Failed.收包失败
 *                 0:Succeeded.成功
 *                 1:Pack error.收包有误
 *                 B:PageID out of range.超出指纹库范围
 *                 18:Flash error.写 Flash 出错
 */
int8_t R308::cmdSaveFinger(uint8_t bufferID, uint16_t pageID) {
  volatile unsigned int sum = 0;

  packSaveFinger[4] = bufferID;
  packSaveFinger[5] = (pageID & 0xFF00) >> 8;
  packSaveFinger[6] = pageID & 0x00FF;

  for (int8_t i = 0; i < 7; i++) sum += packSaveFinger[i];

  packSaveFinger[7] = (sum & 0xFF00) >> 8;
  packSaveFinger[8] = sum & 0x00FF;

  serialClean();
  sendPack(packHead, 6);
  SerPort.write(&packSaveFinger[0], 9);
  delay(100);
  if (serialRead() == true)
    return packSerialRead[2];
  else
    return -1;
}

/**
 * @brief Search fingerprint among pages.
 * @brief 从指纹库中搜索指纹
 *
 * @param bufferID 缓冲区号
 * @param startPageID 指纹库起始页
 * @param pageNum 页数
 * @return int8_t -1:Failed.收包失败
 *                 0:Finished.搜索完成
 *                 1:Pack error.收包有误
 *                 9:Nothing matched.未搜索到
 */
int8_t R308::cmdSearch(uint8_t bufferID, uint16_t startPageID,
                       uint16_t pageNum) {
  volatile unsigned int sum = 0;

  packSearch[4] = bufferID;
  packSearch[5] = (startPageID & 0xFF00) >> 8;
  packSearch[6] = (startPageID & 0x00FF);
  packSearch[7] = (pageNum & 0xFF00) >> 8;
  packSearch[8] = pageNum & 0x00FF;

  for (int8_t i = 0; i < 9; i++) sum += packSearch[i];

  packSearch[9] = (sum & 0xFF00) >> 8;
  packSearch[10] = sum & 0x00FF;

  serialClean();
  sendPack(packHead, 6);
  SerPort.write(&packSearch[0], 11);
  delay(100);
  if (serialRead() == true)
    return packSerialRead[2];
  else
    return -1;
}

/**
 * @brief Delete models.
 * @brief 删除指纹模块里的指定指纹模版
 *
 * @param startPageID 指纹库起始页
 * @param pageNum 页数
 * @return int8_t -1:Failed.收包失败
 *                 0:Succeeded.成功
 *                 1:Pack error.收包有误
 *                10:Delete failed.删除失败
 */
int8_t R308::cmdDeleteModel(uint16_t startPageID, uint16_t pageNum) {
  volatile unsigned int sum = 0;

  packDeleteModel[4] = (startPageID & 0xFF00) >> 8;
  packDeleteModel[5] = startPageID & 0x00FF;
  packDeleteModel[6] = (pageNum & 0xFF00) >> 8;
  packDeleteModel[7] = pageNum & 0x00FF;

  for (int8_t i = 0; i < 8; i++) sum += packDeleteModel[i];

  packDeleteModel[8] = (sum & 0xFF00) >> 8;
  packDeleteModel[9] = sum & 0x00FF;

  serialClean();
  sendPack(packHead, 6);
  SerPort.write(&packDeleteModel[0], 10);
  delay(100);
  if (serialRead() == true)
    return packSerialRead[2];
  else
    return -1;
}
