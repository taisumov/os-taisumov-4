#include <windows.h>
#include <iostream>

using namespace std;

//Функция для асинхронного чтения данных

void WINAPI Callback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
	cout << "Информация получена." << endl;
}

int main()
{
	system("chcp 1251");
	system("cls");

	CHAR buffer[512];										//Буфер для передачи сообщений
	BOOL isConnected = FALSE;								//Проверка подключения
	LPCTSTR Pipename = TEXT("\\\\.\\pipe\\taisumov");		//Название именованного канала

	HANDLE Event = CreateEvent(NULL, FALSE, FALSE, NULL);	//Создание события для организации асинхронного чтения
	HANDLE Pipe = CreateFile(Pipename,						//Создание именованного канала
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL
	);

	int choose;												//Переменная для выбора пункта меню

	OVERLAPPED overlapped = OVERLAPPED();					//Содержит информацию, используемую в асинхронном (или перекрывающем) вводе/выводе данных

	//При успешном создании объектов HANDLE программа начинает свою основную работу

	if (Event != INVALID_HANDLE_VALUE && Pipe != INVALID_HANDLE_VALUE)
	{
		do
		{
			system("cls");

			cout << "+---+---------------------------------------+" << endl;
			cout << "| 1 | Принять сообщение                     |" << endl;
			cout << "+---+---------------------------------------+" << endl;
			cout << "| 2 | Отсоединиться от именованного канала  |" << endl;
			cout << "+---+---------------------------------------+" << endl;
			cout << "| 0 | Выйти                                 |" << endl;
			cout << "+---+---------------------------------------+" << endl;

			cin >> choose;

			switch (choose) {

			case 1:

				overlapped.hEvent = Event;
				/*
								BOOL ReadFileEx(
									HANDLE hFile,													// дескриптор файла
									LPVOID lpBuffer,												// буфер данных
									DWORD nNumberOfBytesToRead,										// число читаемых байтов
									LPOVERLAPPED lpOverlapped,										// смещение
									LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine				// процедура завершения
								);
				*/
				isConnected = ReadFileEx(Pipe, (LPVOID)buffer, 512, &overlapped, Callback);	//Запуск асинхронного чтения

				if (isConnected)
				{
					cout << buffer << endl;
					system("pause");
				}
				else
				{
					cout << "Чтение не удалось!" << endl;
					system("pause");
				}




				break;

			case 2:

				isConnected = CloseHandle(Pipe);	//Закрытие именованного канала

				if (isConnected)
					cout << "Вы были отсоединены от именованного канала!" << endl;
				else
					cout << "Не удалось отсоединиться!" << endl;

				isConnected = FALSE;

				system("pause");

				break;

			case 0:

				break;

			default:

				break;

			}

		} while (choose);
	}
	else
		cout << "Не удалось создать именованный канал, перезапустите программу!" << endl;

	return 0;
}