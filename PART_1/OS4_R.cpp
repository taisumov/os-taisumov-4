#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <Windows.h>

const int N = 19;

int main()
{
    HANDLE freeSem = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, L"freeSem");
    HANDLE usedSem = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, L"usedSem");
    HANDLE fileMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"fileMutex");

    std::srand(std::time(nullptr));

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    LONG page = -1;
    DWORD written = 0;
    std::string outputString = "";

    const wchar_t mapName[] = L"MAPPING";
    HANDLE mapHandle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, mapName);
    if (mapHandle)
    {
        for (int i = 0; i < 3; ++i)
        {
/*            
            BOOL WriteFile(
                HANDLE hFile,                    // дескриптор файла
                LPCVOID lpBuffer,                // буфер данных
                DWORD nNumberOfBytesToWrite,     // число байтов для записи
                LPDWORD lpNumberOfBytesWritten,  // число записанных байтов
                LPOVERLAPPED lpOverlapped        // асинхронный буфер
            );
*/

            // Ждём сигнального состояния семафора

            WaitForSingleObject(usedSem, INFINITE);
            outputString = std::to_string(GetTickCount()) + " | TAKE | USED SEMAPHORE\n";
            WriteFile(hStdout, outputString.data(), outputString.length(), &written, NULL);

            // Ждём, когда можно будет перехватить мьютекс

            WaitForSingleObject(fileMutex, INFINITE);
            outputString = std::to_string(GetTickCount()) + " | TAKE | MUTEX\n";
            WriteFile(hStdout, outputString.data(), outputString.length(), &written, NULL);

            Sleep(std::rand() % 1000 + 500);

            // Освобождаем мьютекс

            if (ReleaseMutex(fileMutex))
            {
                outputString = std::to_string(GetTickCount()) + " | FREE | MUTEX\n";
                WriteFile(hStdout, outputString.data(), outputString.length(), &written, NULL);
            }
            else
            {
                std::string errorString = std::to_string(GetLastError()) + " CODE (mutex)\n";
                WriteFile(hStdout, errorString.data(), errorString.length(), &written, NULL);
            }

            // Освобождаем семафор

            if (ReleaseSemaphore(freeSem, 1, &page))
            {
                outputString = std::to_string(GetTickCount()) + " | FREE | FREE SEMAPHORE\n";
                WriteFile(hStdout, outputString.data(), outputString.length(), &written, NULL);
                std::string str = std::to_string(GetTickCount()) + " | PAGE | NUMBER = " + std::to_string(N - page) + "\n\n";
                WriteFile(hStdout, str.data(), str.length(), &written, NULL);
            }
            else
            {
                std::string errorString = std::to_string(GetLastError()) + " CODE (semaphore)\n";
                WriteFile(hStdout, errorString.data(), errorString.length(), &written, NULL);
            }
        }
    }
    else
    {
        WriteFile(hStdout, "Mapping creation failed\n", strlen("Mapping creation failed\n"), &written, NULL);
    }

    CloseHandle(hStdout);

    return 0;
}

