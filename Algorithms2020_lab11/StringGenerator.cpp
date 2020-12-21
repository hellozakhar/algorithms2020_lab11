#pragma once
#include <algorithm> 
#include <iostream>  
#include <fstream>  
#include <cassert> 
#include <codecvt>
#include <string> 

// =======================================================================
// =======================================================================

#include <random>
#include <chrono>

int random(const int a, const int b)
{
	static std::default_random_engine gen(
		static_cast<unsigned>(
			std::chrono::system_clock::now().time_since_epoch().count()
			)
	);
	std::uniform_int_distribution<int> distribution(a, b);
	return distribution(gen);
}

// =======================================================================
// =======================================================================

// src - массив допустимых символов
// len - длина итогового слова

template<typename CharT, size_t N>
std::basic_string<CharT>
random_text(const CharT(&src)[N], const size_t len)
{
	const size_t n = 1 + len / N;

	std::basic_string<CharT> text = src;
	for (size_t i = 1; i < n; ++i)
		text += src;

	std::random_shuffle(text.begin(), text.end());
	text.resize(len);
	return text;
}

// =======================================================================
// =======================================================================



void clear_input()
{
	std::wcin.clear(),
		std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
	assert(std::wcin);
}

template<typename Data, class Checker>
void input(const wchar_t* msg, const wchar_t* help, const Checker& checker, Data& dst)
{
	for (;;)
	{
		assert(std::wcout);
		assert(std::wcin);

		std::wcout << msg;
		std::wcin >> dst;

		if (!std::wcin || !checker(dst))
			std::wcout << L"ошибка: " << help,
			clear_input();
		else
			break;
	}
}

template<typename Data >
void input(const wchar_t* msg, const wchar_t* help, Data& dst)
{
	const auto lambda = [](const Data&) { return true; };
	input(msg, help, lambda, dst);
}


std::locale get_utf8_locale_(const bool with_bom)
{
	typedef std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>
		bom_codec_t;
	typedef std::codecvt_utf8<wchar_t>
		no_bom_codec_t;

	return with_bom ?
		std::locale(std::locale::empty(), new bom_codec_t) :
		std::locale(std::locale::empty(), new no_bom_codec_t);
}

bool open(const std::wstring& filename, std::wofstream& stream, const bool with_BOM)
{
#ifdef _MSC_VER
	stream.open(filename, std::ios::out | std::ios::trunc, _SH_DENYRW);
#else
	stream.open(filename, std::ios::out | std::ios::trunc);
#endif

	stream.imbue(get_utf8_locale_(with_BOM));
	return stream ? true : false;
}


bool valid(const std::wofstream& stream)
{
	if (!stream)
	{
		std::wcout << L"\n\nошибка записи в файл!\n"
			L"убедитесь, что на диске достаточно мета\n"
			L"и что файл не занят другими приложениями\n";
		return false;
	}
	return true;
}

void writerRandomStringsInFile() {
	setlocale(LC_ALL, "");

	/*const wchar_t arr[]
		= L"йцукенгшщзхъфывапролджэячсмитьбю"
		L"qwertyuiopasdfghjklzxcvbnm";*/

	const wchar_t arr[]
		= L"qwertyuiopasdfghjklzxcvbnm  ,.?!";

	std::wcout << L"Generate strings!\n";

	std::wofstream wos;
	const auto checker = [&wos](const std::wstring& filename)
	{
		const bool success = open(filename, wos, true);
		if (!success)
			wos.clear(),
			wos.close();
		return success;
	};

	std::wstring filename;
	input(L"\nFilename: randomstringsfile.txt",
		L"путь к файлу задан ошибочно",
		checker,
		filename
	);

	int number_words = 0;
	input(L"\nвведите количество слов\n"
		L"которое нужно сгенерировать: ",
		L"ожидается число больше нуля\n\n",
		[](const int v) { return v > 0; },
		number_words
	);

	int minlen = 0;
	input(L"\nвведите минимальную длину генерируемого слова: ",
		L"ожидается число больше нуля\n\n",
		[](const int v) { return v > 0; },
		minlen
	);

	int maxlen = 0;
	input(L"\nвведите максимальную длину генерируемого слова: ",
		L"ожидается число больше нуля\n\n",
		[](const int v) { return v > 0; },
		maxlen
	);

	std::wcout << L"\n\nнастройки:\n"
		<< L"имя файла ............. " << filename << L'\n'
		<< L"количество слов ....... " << number_words << L'\n'
		<< L"минимальная длина ..... " << minlen << L'\n'
		<< L"максимальная длина .... " << maxlen << L'\n';

	for (size_t n = 0; n < static_cast<size_t>(number_words); ++n)
	{
		const auto generated = random_text(arr, random(minlen, maxlen));
		//std::wcout << L"сгенерированно: '" << generated << L"'\n";
		//wos << generated << L'\n';
		wos << generated;
		if (n % 100 == 0 || n == static_cast<size_t>(number_words) - 1) {
			wos << L'\n';
		}
		if (!valid(wos))
			return;
	}

	std::wcout << L"\n\nзапись данных в файл успешно завершена\n";
}