# Netmodule
## Скачать
```
git clone https://github.com/alex-1-tech/net-linux-model/
```

## Быстрый старт
```
make clean && make
make test-all
make test-reset
```
  
## Сборка

make  
Собирает kernel-модуль netmodule.ko

## Загрузка модуля

make install  
Загружает модуль в ядро (insmod)

make info  
Показывает информацию о модуле (modinfo)

## Удаление модуля

make remove  
Выгружает модуль из ядра (rmmod)

## Настройка интерфейса

make test-setup  
Поднимает интерфейс netmodule0 и назначает IP 10.0.0.1/24

## Назначение IP через /proc

make test-proc  
Записывает IP в /proc/netmodule_ip и применяет его к интерфейсу

(вручную)
echo "10.0.0.2" | sudo tee /proc/netmodule_ip  
Назначает новый IP интерфейсу

## Проверка

make test-ping  
Пингует интерфейс (проверка работы передачи пакетов)

## Очистка

make test-clean  
Удаляет IP и выключает интерфейс

## Полный тест

make test-all  
Загружает модуль, настраивает интерфейс, задаёт IP и выполняет ping

make test-reset  
Очищает интерфейс и выгружает модуль
