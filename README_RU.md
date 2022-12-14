# POLIVATOR by AlexanderAni

Arduino, AVR, C++

[English](/README.md)

## Поливатор самостоятелен
`Как устроена работа Поливатора`

Это устройство для полива до 8 различных цветов по заданному расписанию.
Поливатор можно оставить самостоятельно поливать цветы во время отпуска или просто круглогодично поливать цветы в домашней оражерее. Он сам проверяет наличие воды в баке и управляет поливом каждого цветка по расписанию. При этом учитывается температура и влажность окружающей среды. На дисплее можно проверить текущее состояние и изменить настройки полива.
Особенностями Поливатора является расширяемая модульная система, сохранение истории полива, и работа без пауз на полив конкретного растения. В процессе полива продолжают опрашиваться все датчики и проверяется расписание. Можно смотреть график или изменять настройки не дожидаясь окончания полива.
Питание всей системы от одного блока usb 5v. Режим энергосбережения позволяет работать от аккумулятора.

[Facebook. Премьера](https://www.facebook.com/aanishchenko/posts/pfbid034qEHsVyJWHPmqgtMWfscPb4xsddgqDzVarsc6yDjJoVNN26u15hXLRgtBCGTqUbWl)
[Видео. Полный рассказ об устройстве](https://www.facebook.com/100003967933465/videos/pcb.2457044897771084/1915227252002448)
[Видео. Пример полива](https://www.facebook.com/100003967933465/videos/3267990133452066/)
[Видео. Пример меню](https://www.facebook.com/100003967933465/videos/pcb.2457044897771084/1137887480090560)

## Поливатор и его помощники
`Что входит в комплекс полива`

1. Основной блок
- Блок с 1.3 дюймовым LCD дисплеем SSH1106 под управлением arduino pro mini 5v
- Управление всеми действиями через энкодер
- Модуль часов DS3231
Размеры:

Основной блок разведён на 2х-сторонней плате размером 50х80мм. С задней части находятся разъёмы для подключения датчиков, модуля часов и разъём для подключения 4-жильного провода I2c.

2. Коннекторы для цветов. 4 шт
- Небольшой коннектор на 2 цветка на основе разширителя портов PCF8574
- 2 нормально-закрытых клапана для воды
- 2 датчика воды
Размеры:

Коннекторы разведены на двухсторонней плате размером 20х50мм. На плате расположены отверстия для соединения "бутербродом" платы pcf8574. Каждый коннектор имеет проходное подключение к проводу I2c.

3. Бак с водой
- Бутыль от кулера на 19 литров
- Насос c питанием usb 5v
- Сенсорный датчик наличия воды

4. Трубопровод
- Силиконовые шланги внутренним диаметром 6мм и 4мм

## Собираем сами
`Ссылки на ресурсы для самостоятельной сборки устройства`

- [GitHub. Код Поливатора для arduino pro mini](https://github.com/AlexanderAni/polivator)
- [EasyEDA. Polivator Main. Основной блок. Схема, разводка платы](https://oshwlab.com/alexani/flowers_copy)
- [EasyEDA. Polivator Connector Type A. Схема, разводка платы](https://oshwlab.com/alexani/polivator-connector)
- [GitHub. Water Level T13. Датчик уровня воды в баке. Код для Attiny13](https://github.com/AlexanderAni/water_level_t13)
- [EasyEDA. Water Level T13. Датчик уровня воды в баке. Схема, разводка платы](https://oshwlab.com/alexani/water-sensor)

## Структура кода
`Принципиальная структура кода`

Описание будет добавлено позже

## А что можно ещё?
`Перспективы, возможности`

### Ограничение до двух разновидностей цветов на один коннектор
Коннектор PCF8574 имеет 8 пинов ввода/вывода.
<!-- Для надёжного подключения клапанов или датчиков используется npn mosfet. -->
Для одного цветка используется 3 пина:
- Клапан открытия воды
- Включение датчика воды
- Измерение датчика воды
Датчик воды включается периодически только для опроса состояния. В остальное время он выключен. Такой режим позволяет сохранить контакты датчиков, которые долго находятся в воде, от гальванического окисления контактов.

### Ограничение 4 коннектора и 8 цветов
Ограничения в первую очередь связаны с размером оперативной памяти для ATmega328p
Можно изменять количество коннекторов и количество ячеек хранения истории `FLOWER_SCHEDULE_COUNT`. Сохраняя при этом баланс используемой оперативной памяти, увеличив одно, уменьшив второе.

Расчёт выделяеемой памяти в platformio:
RAM:   [========  ]  82.4% (used 1687 bytes from 2048 bytes)
Flash: [========= ]  93.8% (used 28830 bytes from 30720 bytes)
Максимальная расчётная оперативная память RAM, при которой не возникало ошибок во время тестирования: 84.8% (used 1736 bytes).

Теоретически, можно использовать большое количество коннекторов с микроконтроллерами atmega с большей оперативной памятью. Например ATmega64 с оперативной памятью 4 килобайт.

### Todo list

- Выбор названия цветка через меню
