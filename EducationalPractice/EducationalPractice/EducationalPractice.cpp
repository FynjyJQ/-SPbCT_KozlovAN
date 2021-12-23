#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>
#include <StringsDll.h>
#include <time.h>
#include <algorithm>

#include "FixDefs.h"
#define BUF_SIZE 256
static VOID cat(LPTSTR, LPTSTR);

std::string INPUT_FILE = "digits.txt";
std::string OUTPUT_FILE = "output.txt";

void log(std::string text)
{
    std::cout << "\tLog: " << text << std::endl << std::endl;
}

std::string replaceCustom(int startIx, int len, int subpos, std::string source, std::string toCopy) {
    if (startIx + len > source.length()) {
        return "Error with copy";
    }

    std::string::iterator it1 = source.begin();
    std::string::iterator it2 = source.begin();

    while (startIx > 0) {
        it1++;
        it2++;
        startIx--;
    }

    while (len > 0) {
        it2++;
        len--;
    }

    source.replace(it1, it2, toCopy);
    return source;
}

int main()
{
    setlocale(LC_ALL, "RUS");

    auto dateTime = std::chrono::system_clock::now();

    std::time_t parsedDateTime = std::chrono::system_clock::to_time_t(dateTime);

    const int bufSize = 255;
    char buffer[bufSize];
    ctime_s(buffer, bufSize, &parsedDateTime);

    std::cout << "ФИО: Горохов Даниил Анатольевич" << std::endl;
    std::cout << "Дата и время: " << buffer << std::endl;

    std::ifstream ifilestream(INPUT_FILE);

    std::ofstream ofilestream(OUTPUT_FILE);
    ofilestream << "ФИО: Горохов Даниил Анатольевич" << std::endl;
    ofilestream << "Дата и время: " << buffer << std::endl;

    std::string digits = "";

    std::ifstream in(INPUT_FILE);
    std::string line;
    if (in.is_open())
    {
        while (getline(in, line))
        {
            digits += line;
        }
    }
    in.close();

	std::string digitsCp = digits;
	std::string digitsCp2 = digits;

    log("Пример с STL replace и строкой цифр");

    std::string strWithReplaced = replaceCustom(15, 10, 3, digits, "!!!some string for example!");
    std::cout << strWithReplaced << std::endl;
    ofilestream << strWithReplaced << std::endl;

    log("Пример с ошибкой в STL replace из-за неправильных границ строки (индекс + длина больше настоящей длины строки)");

    std::string errExample = replaceCustom(100, 200, 4, digits, "");
    std::cout << errExample << std::endl;
    ofilestream << errExample << std::endl;

    log("Пример использования dll - функция из одной строки создаёт несколько");

	std::vector<std::string> strsVec = StringsFunc::MyStringsFunc::getStrs(digitsCp);
	std::string strs[10];

	int ix = 0;
	auto it = strsVec.begin();
	while (it != strsVec.end()) {
		strs[ix] = *it;
		std::cout << *it << std::endl;
		ofilestream << *it << std::endl;

		ix++;
		it++;
	}
	int strsC = ix;

	std::cout << std::endl;
	ofilestream << std::endl;

    log("Пример сортировки строк по убыванию - строки, созданные с помощью dll сортируются по убыванию согласно заданию:");

	std::sort(strsVec.begin(), strsVec.end());

	auto it2 = strsVec.begin();
	while (it2 != strsVec.end()) {
		std::cout << *it2 << std::endl;
		ofilestream << *it2 << std::endl;
		it2++;
	}

	log("Пример параллельных процессов. В виде паралельного процесса используются поиск одной из строк в файлу с символами " + INPUT_FILE);

	HANDLE hTempFile;
	BOOL prov;
	TCHAR outFile[MAX_PATH + 100];
	SECURITY_ATTRIBUTES StdOutSA =

	{ sizeof
	(SECURITY_ATTRIBUTES), NULL, TRUE };
	TCHAR CommandLine[MAX_PATH + 100];
	STARTUPINFO StartUpSearch, StartUp;
	PROCESS_INFORMATION ProcessInfo;
	DWORD iProc, ExCode;
	HANDLE* hProc;

	typedef struct
	{
		TCHAR TempFile[MAX_PATH];
	}
	PROCFILE;
	PROCFILE* ProcFile;

	GetStartupInfo(&StartUpSearch);
	GetStartupInfo(&StartUp);

	ProcFile = (PROCFILE*)malloc((strsC - 2) * sizeof(PROCFILE));
	hProc = (HANDLE*)malloc((strsC - 2) * sizeof(HANDLE));

	for (iProc = 0; iProc < strsC - 2; iProc++)
	{
		_stprintf_s(CommandLine, _T("%s%s %s"),
			_T("FindStr "), strs[1], strs[iProc + 2]);
		_tprintf(_T("%s\n"), CommandLine);
		if (GetTempFileName(_T("."), _T("gtm"), 0,
			ProcFile[iProc].TempFile) == 0);

		hTempFile = CreateFile(ProcFile[iProc].TempFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &StdOutSA, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		StartUpSearch.dwFlags = STARTF_USESTDHANDLES;
		StartUpSearch.hStdOutput = hTempFile;
		StartUpSearch.hStdError = hTempFile;
		StartUpSearch.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

		CreateProcess(NULL, CommandLine, NULL, NULL,
			TRUE, 0, NULL, NULL, &StartUpSearch, &ProcessInfo);

		CloseHandle(hTempFile);
		CloseHandle(ProcessInfo.hThread);

		hProc[iProc] = ProcessInfo.hProcess;
	}

	for (iProc = 0; iProc < strsC - 2; iProc += MAXIMUM_WAIT_OBJECTS)
		WaitForMultipleObjects(min(MAXIMUM_WAIT_OBJECTS, strsC - 2 -
			iProc), &hProc[iProc], TRUE, INFINITE);

	for (iProc = 0; iProc < strsC - 2; iProc++)
	{
		printf("Proc= %d\n", iProc);
		prov = GetExitCodeProcess(hProc[iProc], &ExCode);
		if (ExCode != 0) DeleteFile(ProcFile[iProc].TempFile);
		if (GetExitCodeProcess(hProc[iProc], &ExCode) && ExCode == 0)
		{
			if (strsC > 3) _tprintf(_T("%s : \n"), strs[iProc + 2]);
			fflush(stdout);

			_stprintf_s(outFile, _T("%s"), ProcFile[iProc].TempFile);
			_stprintf_s(CommandLine, _T("%s%s"),
				_T("type "), ProcFile[iProc].TempFile);
			_tprintf(_T("%s\n"), CommandLine);
			CreateProcess(NULL, CommandLine, NULL, NULL,
				TRUE, 0, NULL, NULL, &StartUp, &ProcessInfo);
			WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
			CloseHandle(ProcessInfo.hProcess);
			CloseHandle(ProcessInfo.hThread);
		}
		CloseHandle(hProc[iProc]);
	}
	free(ProcFile);
	free(hProc);

    system("pause");
}

static VOID cat(LPTSTR hInFile, LPTSTR hOutFile)
{
	CopyFile(hInFile, hOutFile, FALSE);
	return;
}
