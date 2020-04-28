
// Автоматическое восстановление питания - AutoRecoverySupply
// v1
// Включение электродвигателя после восстановления питания.
// Контроль причины отключения. Если отключение было вызвано срабатыванием теплового реле, то АВП выводится из работы, до восстановления работоспособности схемы.
// Если была ручная остановка электродвигаетля, то АВП не должно реагировать.
// Фиксация успешного восстановления питания
// Сброс запрета работы АВП. Реализован удержанием кнопки включения в течение 7с
// Индикация аварии и запрет на работу АВП.


// == Первый канал
#define START1 1    // Кнопка запуска схемы и включения автоматического режима. Ручное управление отключается!!!
#define KM1 4       // Реле для запуска схемы
#define UKK1 2      // Контроль напряжения между контактами теплового реле
#define IKM1 3      // Контроль тока в цепи или напряжения на катушке магнитного пускателя. Не нужно, т.к. если работает
#define LED_AUTO1 5 // Индикация включения АВП. Можно сделать миганием светодиода

#include <Arduino.h>
#include <GyverButton.h>
GButton start1(START1, HIGH_PULL, NORM_OPEN);

bool flagOn, flagOnKM1 = false, flagError, flagWriteKM1 = false, flagWriteLED_AUTO1 = false;
unsigned int needPause;

void setup()
{
  pinMode(START1, INPUT);
  pinMode(UKK1, INPUT);
  pinMode(KM1, OUTPUT);
  pinMode(IKM1, INPUT);
  pinMode(LED_AUTO1, OUTPUT);
  start1.setTimeout(7000); // Время для удержания в мс
}

void button()
{
  if (start1.isClick())
  {
    flagOn = !flagOn;
  }
  if (start1.isHolded())
  {
    flagError = false;
    flagOn = false;
  }
}

void control()
{
  if (flagOn == true)
  {
    if (flagWriteKM1 == false && flagError == false)
    {
      flagWriteKM1 = true;
      digitalWrite(KM1, HIGH);
    }
    if (flagWriteLED_AUTO1 == false && flagError == false)
    {
      flagWriteLED_AUTO1 = true;
      digitalWrite(LED_AUTO1, HIGH);
    }
  }

  if (flagOn == false && flagError == false)
  {
    if (flagWriteLED_AUTO1 == true)
    {
      flagWriteLED_AUTO1 = false;
      digitalWrite(LED_AUTO1, LOW);
      digitalWrite(KM1, LOW);
    }
  }
}

void stopMotor()
{
  if (digitalRead(UKK1) == false && digitalRead(IKM1) == false) // Блокировка или отключение автоматического режима если двигатель не работает или отключили вручную.
  {
    flagOn = false;
  }
}

void statusError()
{
  if (digitalRead(UKK1) == true) // Сработало тепловое реле. Включили запрет на работу до устранения неполадки.
  {
    flagError = true;
  }
  if (flagError == true)
  {
    if ((millis() - needPause > 500) && flagWriteLED_AUTO1 == false)
    {
      flagWriteLED_AUTO1 == true;
      digitalWrite(LED_AUTO1, HIGH);
      needPause = millis();
    }
    if ((millis() - needPause > 500) && flagWriteLED_AUTO1 == true)
    {
      flagWriteLED_AUTO1 == true;
      digitalWrite(LED_AUTO1, LOW);
      needPause = millis();
    }
  }
}

void loop()
{
}