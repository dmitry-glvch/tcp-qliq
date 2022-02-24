# Тестовое задание


## Постановка задачи

Необходимо реализовать клиент-серверное приложение под Linux. Клиент - программа, запускаемая из консоли. Сервер - демон, корректно завершающийся по сигналам SIGTERM и SIGHUP. Клиент должен передать содержимое текстового файла через TCP. Сервер должен принять и сохранить в файл.


## Реализация

### Процесс передачи файла

1. От подключившигося клиента сервер запрашивает размер файла. При наличии возможности принять файл такого размера (есть место на диске, разрешение на запись в рабочую директорию) отправляет клиенту сигнал подтверждения.
2. Клиент отправляет длину названия файла и название файла. Получает сигнал подтверждения от сервера.
3. Клиент передаёт содержимое файла. Получает сигнал подтверждения от сервера в случае успешного получения и записи на диск.


### Зависимости

- Boost.Asio
- CMake


### Сборка проекта

```shell
mkdir build
cd build
cmake ..
cmake --build .
```


### Запуск

#### Сервер

```shell
daemon <listen-ip-address> <listen-port>
```
Для отключения сервера необходимо послать `SIGHUP` или `SIGTERM`.

Пример использования:

```shell
./daemon 192.168.0.3 1337
```

#### Клиент

```shell
client <server-ip-address> <port> <path-to-file>
```

Пример использования:

```shell
./client 192.168.0.3 1337 ~/image.jpg
```