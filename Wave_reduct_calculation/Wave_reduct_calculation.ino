/*
 * Программа для вычисления точек для построения внешнего колеса волнового редуктора с ПТК
*/

float N = 23; //- передаточное число, количество шариков
float n = 0; //- количество впадин, НЕ ТРОГАТЬ
float Dsh = 4.4; //- диаметр шариков
float De = 37; //- диаметр эксцентрика
float e = 0; //- смещение оси эксцентрика
float f = 0; //- угол
float fMax = 0; //- макс угол
float Step = 0.2; //- шаг угла
float R = 0; //- промежуточная переменная
float Rsum = 0; // - промежуточная переменная


void setup() {
  Serial.begin(9600);

  n = N + 1;
  Rsum = (Dsh + De)/ 2; // Промежуточная переменная
  e = Rsum / N;

  Serial.println(e);
}

void loop() {
  fMax = 360 / n;
  fMax = fMax / 2;

  while(f <= fMax){
    R = sqrt((Rsum + e * sin(f)) * (Rsum - e * sin(f)));
    float Alfa = atan((e * n * sin(f)) / R); // - inbetween variable
    float Y = e * cos(f) + R; // - inbetween variable

    float Xn = Y * sin(f / n) + (Dsh * sin(Alfa + f / n)) / 2;
    float Yn = Y * cos(f / n) + (Dsh * cos(Alfa + f / n)) / 2; 
    Serial.print("X = ");
    Serial.print(Xn);
    Serial.print("  ");
    Serial.print("Y = ");
    Serial.print(Yn);

    Serial.print("  ");
    Serial.print("Angle = ");
    Serial.println(f);
    f = f + Step;
  }

}
