/*
 * Программа для стабилизирования устройства в условиях невесомости.
*/

#include <MPU6050.h>
#include "Wire.h"
#include "I2Cdev.h"

MPU6050 mpu;

#define BUFFER_SIZE 100

#define DRV_1 3           // пин драйвера мотора in3
#define DRV_2 5           // пин драйвера мотора in4

int16_t ax, ay, az;
int16_t gx, gy, gz;

//unsigned long dopZnachenie;
int PrevGZ;
int PWM;
// Две доп переменные для расчета И и Д составляющих. Представляют собой текущие значения в двух случаях.
int ZadannoeGZ_pozitive = 100;
int ZadannoeGZ_otricat = -100;
int I;
int D;
int16_t err;
int16_t PrevErr;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  mpu.initialize();
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
  calibration();

  pinMode(DRV_1, OUTPUT);
  pinMode(DRV_2, OUTPUT);

}

void loop() {
  // выводим начальные значения
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.println(gz);

  // Двигаем мотор на драйвере с расчетом ШИМ от угловой скорости
  if (gz > 100) {
    I = I + (gz - ZadannoeGZ_pozitive) * 12;
    err = gz - ZadannoeGZ_pozitive;
    D = (err - PrevErr)*12;
    PrevErr = err;
    I = I / 125;
    D = D / 125;
    PWM = map( gz, 100, 32767, 0, 255); // Преобразую угл.скорость в ШИМ сигнал 
    // Кручу мотором в одну сторону
    analogWrite(DRV_1, PWM - (int)I - (int)D);
    digitalWrite(DRV_2, LOW);
  }
  else if (gz < -100) {
    I = I + (gz - ZadannoeGZ_poloj) * 12;
    err = gz - ZadannoeGZ_poloj;
    D = (err - PrevErr)*12;
    PrevErr = err;
    I = I / 125;
    D = D / 125;
    PWM = map( gz, -100, -32767, 0, 255); // Преобразую угл.скорость в ШИМ сигнал(инвертированно)
    // Кручу мотором в другую сторону
    analogWrite(DRV_2, PWM - (int)I - (int)D);
    digitalWrite(DRV_1, LOW);
  }
  else {
    digitalWrite(DRV_1, LOW);
    digitalWrite(DRV_2, LOW);
  }
}


// ======= ФУНКЦИЯ КАЛИБРОВКИ =======
void calibration() {
  long offsets[6];
  long offsetsOld[6];
  int16_t mpuGet[6];

  // используем стандартную точность
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);

  // обнуляем оффсеты
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
  delay(10);
  
  Serial.println("Calibration start. It will take about 5 seconds");
  for (byte n = 0; n < 10; n++) {     // 10 итераций калибровки
    for (byte j = 0; j < 6; j++) {    // обнуляем калибровочный массив
      offsets[j] = 0;
    }
    for (byte i = 0; i < 100 + BUFFER_SIZE; i++) { // делаем BUFFER_SIZE измерений для усреднения
      mpu.getMotion6(&mpuGet[0], &mpuGet[1], &mpuGet[2], &mpuGet[3], &mpuGet[4], &mpuGet[5]);
      if (i >= 99) {                         // пропускаем первые 99 измерений
        for (byte j = 0; j < 6; j++) {
          offsets[j] += (long)mpuGet[j];   // записываем в калибровочный массив
        }
      }
    }
    for (byte i = 0; i < 6; i++) {
      offsets[i] = offsetsOld[i] - ((long)offsets[i] / BUFFER_SIZE); // учитываем предыдущую калибровку
      if (i == 2) offsets[i] += 16384;                               // если ось Z, калибруем в 16384
      offsetsOld[i] = offsets[i];
    }
    // ставим новые оффсеты
    mpu.setXAccelOffset(offsets[0] / 8);
    mpu.setYAccelOffset(offsets[1] / 8);
    mpu.setZAccelOffset(offsets[2] / 8);
    mpu.setXGyroOffset(offsets[3] / 4);
    mpu.setYGyroOffset(offsets[4] / 4);
    mpu.setZGyroOffset(offsets[5] / 4);
    delay(2);
  }
  
    // выводим в порт
    Serial.println("Calibration end. Your offsets:");
    Serial.println("accX accY accZ gyrX gyrY gyrZ");
    Serial.print(mpu.getZGyroOffset());
}
