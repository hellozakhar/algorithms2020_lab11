#include <iostream>
#include <vector>
#include <fstream>
#include <string> 
#include <cmath>
#include <random>
#include <chrono>

#include "Timer.cpp"

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

std::string random_text(const char(*src), const size_t len)
{
	const size_t N = strlen(src);
	const size_t n = 1 + len / N;

	std::basic_string<char> text = src;
	for (size_t i = 1; i < n; ++i)
		text += src;

	std::random_shuffle(text.begin(), text.end());
	text.resize(len);
	return text;
}

std::vector<int> prefix_function(std::string const& s) {
	const int n = (int) s.length();
	std::vector<int> pi(n);

	for (int i = 1; i < n; ++i) {
		int j = pi[i - 1];

		while (j > 0 && s[i] != s[j])
			j = pi[j - 1];

		if (s[i] == s[j])  
			++j;

		pi[i] = j;
	}
	return pi;
}

std::string::size_type findKMP(std::string const& str, int const& begin, std::string const& pattern) {
	std::vector<int> pf = prefix_function(pattern);

	for (int k = 0, i = begin; i < str.length(); ++i)
	{
		while ((k > 0) && (pattern[k] != str[i]))
			k = pf[k - 1];

		if (pattern[k] == str[i])
			k++;

		if (k == pattern.length())
			return (i - pattern.length() + 1);
	}

	return (std::string::npos);
}

std::string::size_type findRK(std::string const& str, std::string const& pattern) {

	const size_t str_size = str.length();
	const size_t pattern_size = pattern.length();

	// p = 2^61 - 1 or p = 2^31 - 1
	const unsigned long long p = ((unsigned long long) pow(2, 61)) - 1ULL;
	// a in {0, ..., p - 1}
	const unsigned long long a = 2ULL;

	// precomp positive a: { 1, a^1, a^2, ..., a^n }
	// complexity O(n)
	unsigned long long* apos = new unsigned long long[str_size + 1];
	apos[0] = 1ULL;
	for (int i = 1; i <= str_size; i++) {
		apos[i] = (apos[i - 1] << 1) % p;
		std::cout << "apos[" << i << "]= " << apos[i] << std::endl;
	}

	// precomp H_i, where H0 is empty str
	// H0 = h(T[0,0)) = 0, H1 = h(T[0, 1)), ...
	unsigned long long* H = new unsigned long long[str_size + 1];
	H[0] = 0ULL;
	for (int i = 0; i < str_size; i++) {
	    H[i + 1] = (H[i] + str[i] * apos[i]) % p;
		std::cout << "H[" << i + 1 << "]= " << H[i + 1] << std::endl;
	}

	// calculating hash of pattern
	unsigned long long pattern_hash = 0ULL;
	for (int i = 0; i < pattern_size; i++) {
		//pattern_hash = (pattern_hash + pattern[i] * apos[i]) % p;
		pattern_hash += (pattern[i] << i) % p;
	}

	unsigned long long h = 0ULL;
	for (int i = 0; i < str_size - pattern_size; i++) {
		
		// h = ((unsigned long long)((H[i + pattern_size] - H[i]) / apos[i])) % p;
		h = (((H[i + pattern_size] - H[i]) >> i)) % p;

		if (h == pattern_hash) {
			if (str.substr(i, pattern_size) == pattern) {
				delete[] apos;
				delete[] H;
				return i;
			}
		}
	}
	
	delete[] apos;
	delete[] H;
	return (std::string::npos);
}

std::string::size_type findNaive(std::string const& str, std::string const& pattern) {

	const size_t str_size = str.length();
	const size_t pattern_size = pattern.length();

	for (int i = 0; i <= str_size - pattern_size; ++i) {
		bool correct = true;

		for (int j = 0; j < pattern_size; j++) {
			if (pattern[j] != str[i + j])
				correct = false;
		}

		if (correct)
			return i;
	}

	return (std::string::npos);
}


void test() {
	std::vector<int> tmp;
	tmp = prefix_function("abcdefghteafes fddsa fmdksamfdm fdlsakmf ewok");

	const std::string str_example_1 = "fkdsaf  o,i.e feojwaofk fndsan  nfdsna kklfdsa neoiwfn qi,f e ewqef  hdffds zakh fmdska ieow ks n cdaddc. dskj fnd;akjnf d;sanf;eiwajnf oeiwja foiewjaf oidsjaf oiejwqfoenwqfjnewq'ojfnewq'lkrnewqkfnjvofi[djsboifjdsgjrnewkgjenr;kgjernw;kfjnew;kfjnresodxbjf'objfr'eowgn";

	const std::string string_example03 = str_example_1.substr(0, 70);
	const std::string pattern_example_03 = str_example_1.substr(60, 6);
	
	std::cout << "string_example_3: " << string_example03 << std::endl;
	std::cout << "pattern_example_3: " << pattern_example_03 << std::endl;
	std::cout << "pattern_length: " << pattern_example_03.size() << std::endl;
	//std::cout << "polyhash: " << polyhash(pattern_example_03) << std::endl;
	
	std::cout << "findRK start ---" << std::endl;
	std::cout << "findRK result: " << findRK(string_example03, pattern_example_03) << std::endl;
	std::cout << "findNaive result: " << findNaive(string_example03, pattern_example_03) << std::endl;
}

void testFind(int const& times) {
	std::cout << "\n------------ TEST STARTED! ------------" << std::endl;
	std::cout << "Contains timings for find-substrs for random strs with different length." << std::endl;
	std::cout << "Running " << times << " times." << std::endl;

	double* timeKMP = new double[times];
	double* timeRK = new double[times];
	double* timeNaive = new double[times];
	
	bool isSuccess = true;
	const char alphabet[]
		= "qwertyuiopasdfghjklzxcvbnm  ,.?!";
	int minlen = 0;
	int maxlen = 0;
	std::string generatedstr = "";
	std::string pattern = "";
	size_t len_generatedstr = 0;
	size_t len_pattern = 0;
	int lower = 0;
	//int upper = 0;
	int resultKMP = 0;
	int resultRK = 0;
	int resultNaive = 0;

	int count = 15;
	int* lengthArr = new int[count];
	int* lengthPatternArr = new int[count];
	double* avgtimeKMP = new double[count];
	double* avgtimeRK = new double[count];
	double* avgtimeNaive = new double[count];

	for (int i = 0; i < count; i++) {
		minlen = (i + 1) * 120;
		maxlen = minlen;

		generatedstr = random_text(alphabet, random(minlen, maxlen));
		len_generatedstr = generatedstr.size();
		len_pattern = random((int)len_generatedstr / 5, (int)len_generatedstr / 4);

		lengthArr[i] = minlen;
		lengthPatternArr[i] = len_pattern;

		std::cout << "\nlen_generatedstr = " << len_generatedstr << std::endl;
		std::cout << "len_pattern = " << len_pattern << std::endl;

		lower = random(0, len_generatedstr - len_pattern - 2);
		//upper = random(lower, len_generatedstr - len_pattern - 1);
		// was pattern = generatedstr.substr(lower, upper); and that was a BIG MISTAKE!
		pattern = generatedstr.substr(lower, len_pattern);

		for (int k = 0; k < times; k++) {
			Timer t1;
			resultKMP = findKMP(generatedstr, 0, pattern);
			timeKMP[k] = t1.elapsed();

			Timer t2;
			resultRK = findRK(generatedstr, pattern);
			timeRK[k] = t2.elapsed();

			Timer t3;
			resultNaive = findNaive(generatedstr, pattern);
			timeNaive[k] = t3.elapsed();

			if (k == 0) {
				if (resultKMP != resultRK || resultRK != resultNaive) {
					std::cout << "Wrong result! Results: {" << resultKMP
						      << ", " << resultRK << ", " << resultNaive << '}' << std::endl;
					/*
					std::cout << "\nString: " << generatedstr << std::endl;
					std::cout << "\nPattern: " << pattern << std::endl;
					std::cout << "\npolyhash(Pattern): " << polyhash(pattern) << std::endl;
					std::cout << "\npolyhash(generatedstr, resultKMP, resultKMP + pattern.size()): " << polyhash(generatedstr, resultKMP, resultKMP + pattern.size()) << std::endl;
					isSuccess = false;

					std::ofstream out;
					out.open("testresult.txt", std::ios::app);
					if (out.is_open())
					{
						out << "Wrong result! Results: {" << resultKMP
							<< ", " << resultRK << ", " << resultNaive << '}' << std::endl;
						out << "\nString: " << generatedstr << std::endl;
						out << "\nPattern: " << pattern << std::endl;
						out << "\npolyhash(Pattern): " << polyhash(pattern) << std::endl;
						out << "\npolyhash(generatedstr, resultKMP, resultKMP + pattern.size()): " << polyhash(generatedstr, resultKMP, resultKMP + pattern.size()) << std::endl;
					}
					out.close();*/
				}
			}
		}

		if (isSuccess) {
			double avgKMP = 0.0;
			double minKMP = timeKMP[0];
			double maxKMP = timeKMP[0];

			double avgRK = 0.0;
			double minRK = timeRK[0];
			double maxRK = timeRK[0];

			double avgNaive = 0.0;
			double minNaive = timeNaive[0];
			double maxNaive = timeNaive[0];

			for (int i = 0; i < times; i++) {

				avgKMP += timeKMP[i];
				avgRK += timeRK[i];
				avgNaive += timeNaive[i];

				if (minKMP > timeKMP[i]) minKMP = timeKMP[i];
				if (maxKMP < timeKMP[i]) maxKMP = timeKMP[i];

				if (minRK > timeRK[i]) minRK = timeRK[i];
				if (maxRK < timeRK[i]) maxRK = timeRK[i];

				if (minNaive > timeNaive[i]) minNaive = timeNaive[i];
				if (maxNaive < timeNaive[i]) maxNaive = timeNaive[i];
			}
			avgKMP /= times;
			avgRK /= times;
			avgNaive /= times;

			avgtimeKMP[i] = avgKMP;
			avgtimeRK[i] = avgRK;
			avgtimeNaive[i] = avgNaive;


			std::cout << "------TIME------"
				<< "\nKMP   -- AVG: " << avgKMP << " MIN: " << minKMP << " MAX: " << maxKMP
				<< "\nRK    -- AVG: " << avgRK << " MIN: " << minRK << " MAX: " << maxRK
				<< "\nNaive -- AVG: " << avgNaive << " MIN: " << minNaive << " MAX: " << maxNaive << std::endl;
		}
	}

	std::ofstream o;
	o.open("randstr_prepared_results.txt", std::ios::app);
	if (o.is_open()) {
		// Список длин
		o << "length of strings" << std::endl;
		for (int i = 0; i < count; i++) {
			o << lengthArr[i] << std::endl;
		}
		o << std::endl;

		o << "length of patterns" << std::endl;
		for (int i = 0; i < count; i++) {
			o << lengthPatternArr[i] << std::endl;
		}
		o << std::endl;

		o << "KMP" << std::endl;
		for (int i = 0; i < count; i++) {
			o << avgtimeKMP[i] << std::endl;
		}
		o << std::endl;

		// пока не исправлю реализацию
		/*o << "RK" << std::endl;
		for (int i = 0; i < count; i++) {
			o << avgtimeRK[i] << std::endl;
		}
		o << std::endl;*/

		o << "Naive" << std::endl;
		for (int i = 0; i < count; i++) {
			o << avgtimeNaive[i] << std::endl;
		}
		o << std::endl;
	}
	o.close();

	delete[] timeKMP;
	delete[] timeRK;
	delete[] timeNaive;
	delete[] avgtimeKMP;
	delete[] avgtimeRK;
	delete[] avgtimeNaive;
	delete[] lengthArr;
	delete[] lengthPatternArr;

	std::cout << "\n------------  TEST ENDED!  ------------" << std::endl;
}

void testFindFiction(int times) {
	const std::string text = "Pierre, after all, had not managed to choose a career for himself in Petersburg, and had been expelled from there for riotous conduct and sent to Moscow. The story told about him at Count Rostóv’s was true. Pierre had taken part in tying a policeman to a bear. He had now been for some days in Moscow and was staying as usual at his father’s house. Though he expected that the story of his escapade would be already known in Moscow and that the ladies about his father—who were never favorably disposed toward him—would have used it to turn the count against him, he nevertheless on the day of his arrival went to his father’s part of the house. Entering the drawing room, where the princesses spent most of their time, he greeted the ladies, two of whom were sitting at embroidery frames while a third read aloud. It was the eldest who was reading—the one who had met Anna Mikháylovna. The two younger ones were embroidering: both were rosy and pretty and they differed only in that one had a little mole on her lip which made her much prettier. Pierre was received as if he were a corpse or a leper. The eldest princess paused in her reading and silently stared at him with frightened eyes; the second assumed precisely the same expression; while the youngest, the one with the mole, who was of a cheerful and lively disposition, bent over her frame to hide a smile probably evoked by the amusing scene she foresaw. She drew her wool down through the canvas and, scarcely able to refrain from laughing, stooped as if trying to make out the pattern. How do you do, cousin ? said Pierre.You don’t recognize me ?  I recognize you only too well, too well. How is the count ? Can I see him ?  asked Pierre, awkwardly as usual, but unabashed. The count is suffering physically and mentally, and apparently you have done your best to increase his mental sufferings. Can I see the count ?  Pierre again asked.";

	std::cout << "\n------------ TEST STARTED! ------------" << std::endl;
	std::cout << "Contains timings of find-substrs for string from Tolstoy's book 'War and peace' with different length of str." << std::endl;
	std::cout << "Running " << times << " times." << std::endl;

	double* timeKMP = new double[times];
	double* timeRK = new double[times];
	double* timeNaive = new double[times];

	int minlen = 0;
	int maxlen = 0;
	std::string str = "";
	std::string pattern = "";
	int len_str = 0;
	int len_pattern = 0;
	int lower = 0;
	//int upper = 0;
	int resultKMP = 0;
	int resultRK = 0;
	int resultNaive = 0;

	int count = 15;
	int* lengthArr = new int[count];
	int* lengthPatternArr = new int[count];
	double* avgtimeKMP = new double[count];
	double* avgtimeRK = new double[count];
	double* avgtimeNaive = new double[count];

	for (int i = 0; i < count; i++) {
		minlen = (i + 1) * 120;
		maxlen = minlen;

		str = text.substr(0, minlen);
		len_str = str.size();

		len_pattern = random((int)len_str / 5, (int)len_str / 4);

		lengthArr[i] = minlen;
		lengthPatternArr[i] = len_pattern;

		std::cout << "\nlen_str = " << len_str << std::endl;
		std::cout << "len_pattern = " << len_pattern << std::endl;
		lower = random(0, len_str - len_pattern - 2);
		//upper = random(lower, len_str - len_pattern - 1);
		// was pattern = generatedstr.substr(lower, upper); and that was a BIG MISTAKE!
		pattern = str.substr(lower, len_pattern);

		for (int k = 0; k < times; k++) {
			Timer t1;
			resultKMP = findKMP(str, 0, pattern);
			timeKMP[k] = t1.elapsed();

			Timer t2;
			resultRK = findRK(str, pattern);
			timeRK[k] = t2.elapsed();

			Timer t3;
			resultNaive = findNaive(str, pattern);
			timeNaive[k] = t3.elapsed();

			if (k == 0) {
				if (resultKMP != resultRK || resultRK != resultNaive) {
					std::cout << "Wrong result! Results: {" << resultKMP
						<< ' ' << resultRK << ' ' << resultNaive << '}' << std::endl;
				}
			}
		}

		double avgKMP = 0.0;
		double minKMP = timeKMP[0];
		double maxKMP = timeKMP[0];

		double avgRK = 0.0;
		double minRK = timeRK[0];
		double maxRK = timeRK[0];

		double avgNaive = 0.0;
		double minNaive = timeNaive[0];
		double maxNaive = timeNaive[0];

		for (int i = 0; i < times; i++) {

			avgKMP += timeKMP[i];
			avgRK += timeRK[i];
			avgNaive += timeNaive[i];

			if (minKMP > timeKMP[i]) minKMP = timeKMP[i];
			if (maxKMP < timeKMP[i]) maxKMP = timeKMP[i];

			if (minRK > timeRK[i]) minRK = timeRK[i];
			if (maxRK < timeRK[i]) maxRK = timeRK[i];

			if (minNaive > timeNaive[i]) minNaive = timeNaive[i];
			if (maxNaive < timeNaive[i]) maxNaive = timeNaive[i];
		}
		avgKMP /= times;
		avgRK /= times;
		avgNaive /= times;

		avgtimeKMP[i] = avgKMP;
		avgtimeRK[i] = avgRK;
		avgtimeNaive[i] = avgNaive;

		std::cout << "------TIME------"
			<< "\nKMP   -- AVG: " << avgKMP << " MIN: " << minKMP << " MAX: " << maxKMP
			<< "\nRK    -- AVG: " << avgRK << " MIN: " << minRK << " MAX: " << maxRK
			<< "\nNaive -- AVG: " << avgNaive << " MIN: " << minNaive << " MAX: " << maxNaive << std::endl;
	}

	std::ofstream o;
	o.open("book_prepared_results.txt", std::ios::app);
	if (o.is_open()) {
		// Список длин
		o << "length of strings" << std::endl;
		for (int i = 0; i < count; i++) {
			o << lengthArr[i] << std::endl;
		}
		o << std::endl;

		o << "length of patterns" << std::endl;
		for (int i = 0; i < count; i++) {
			o << lengthPatternArr[i] << std::endl;
		}
		o << std::endl;

		o << "KMP" << std::endl;
		for (int i = 0; i < count; i++) {
			o << avgtimeKMP[i] << std::endl;
		}
		o << std::endl;

		// пока не исправлю реализацию
		/*o << "RK" << std::endl;
		for (int i = 0; i < count; i++) {
			o << avgtimeRK[i] << std::endl;
		}
		o << std::endl;*/

		o << "Naive" << std::endl;
		for (int i = 0; i < count; i++) {
			o << avgtimeNaive[i] << std::endl;
		}
		o << std::endl;
	}
	o.close();

	delete[] timeKMP;
	delete[] timeRK;
	delete[] timeNaive;
	delete[] avgtimeKMP;
	delete[] avgtimeRK;
	delete[] avgtimeNaive;
	delete[] lengthArr;
	delete[] lengthPatternArr;

	std::cout << "\n------------  TEST ENDED!  ------------" << std::endl;
}

int main()
{
	int times = 7;
	test();
	//testFind(times);
	//testFindFiction(times);
	std::cout << pow(2, 61);
    std::cout << "\nHello World!\n";
}

//int pos = 61;
//h = ((unsigned long long) ((H[pos + pattern_size] - H[pos]) / apos[pos])) % p;
////h = (((H[pos + pattern_size] - H[pos]) >> pos)) % p;
//
//std::cout << "\nH[26]: " << H[26] << std::endl;
//std::cout << "\nH[21]: " << H[21] << std::endl;
//std::cout << "\nH[26] - H[21]: " << (H[26] - H[21]) << std::endl;
//
//std::cout << "\nh: " << h << std::endl;
//std::cout << "\nRK3-pattern_hash: " << pattern_hash << std::endl;
//
//unsigned long long tmp = 330911306547813444ULL;
//unsigned long long rlt = tmp >> 5;
//std::cout << "tmp: " << tmp << std::endl;
//std::cout << "rlt: " << rlt << std::endl;
//std::cout << 'b' + 0 << std::endl;
//
//unsigned long long testr = 0;
//char* eptr = new char('b');
//testr = strtoull(eptr, &eptr, 1);
//std::cout << "testr: " << testr << std::endl;
//

//long long polyhash(std::string const& s, int const& lower, int const& upper) {
//	const int a = 31;
//	const int p = 1e9 + 9;
//	long long hash_value = 0;
//	long long a_pow = 1;
//
//	//std::cout << s.substr(lower, upper - lower) << std::endl;
//	
//	for (int i = lower; i <= upper; i++) {
//		// hash_value = (hash_value + (s[i] - 'a' + 1) * a_pow) % p;
//		hash_value = (hash_value + s[i] * a_pow) % p;
//		
//		//std::cout << "s[" << i << "] = " << s[i] << std::endl;
//		a_pow = (a_pow * a) % p;
//	}
//	//std::cout << std::endl;
////	std::cout << "polyhash(" << s.substr(lower, upper - lower) << ")" << " = " << hash_value << std::endl;
//	return hash_value;
//}
//
//long long polyhash(std::string const& s) {
//	return polyhash(s, 0, s.size() - 1);
//}
//
//long long polyhash(std::string const& s, int const& lower, int const& upper, long long const& previous_hash) {
//	
//	if (previous_hash == 0) {
//		return polyhash(s, lower, upper);
//	}
//	
//	const int a = 31;
//	const int p = 1e9 + 9;
//	long long a_pow = 1;
//	long long hash_value = 0;
//
//	// delete hash of first char from previous string
//	// (s[lower-1] - 'a' + 1)  <--- hash of first char
//	// example
//	// A [B C D E]
//	// ^
//	// |
//	// A - first char from previous string ("[A B C D]")
//	
//	hash_value = (previous_hash - (s[lower-1] - 'a' + 1));
//	hash_value /= a;
//
//	// calculating a^(...) for right char of string
//	for (int i = 0; i < upper - lower; i++) {
//		/*a_pow *= a;
//		a_pow %= p;*/
//		a_pow = (a_pow * a) % p;
//	}
//
//	//  A [B C D E]
//	//           ^
//	//           |
//	// add hash right char
//	//              |
//	hash_value += ((s[upper] - 'a' + 1) * a_pow) % p;
//
//	return hash_value;
//}
//		
//std::string::size_type findRK(std::string const& str, std::string const& pattern) {
//
//	const int str_size = str.length();
//	const int pattern_size = pattern.length();
//	const long long pattern_hash = polyhash(pattern);
//	long long current_hash = 0;
//
//	std::ofstream o;
//	o.open("findRK.log", std::ios::app);
//	o << "\nstring = <" << str << ">" << ", length = " << str.length() << std::endl;
//	o << "pattern = <" << pattern << ">" << ", length = " << pattern.length() << std::endl;
//	   
//	for (int i = 0; i < str_size - pattern_size + 1; i++) {
//
//		current_hash = polyhash(str, i, i + pattern_size - 1, current_hash);
//		
//		std::cout << "i: " << i << "\ns[lower] = " << str[i] << ", s[upper] = "
//			<< str[i + pattern_size - 1] << ", " << std::endl;
//		std::cout << "current_string = <" << str.substr(i, pattern_size) << ">" << std::endl;
//		std::cout << "current_hash = " << current_hash << std::endl;
//		std::cout << "expectedhash = " << polyhash(str, i, i + pattern_size - 1, 0) << '\n' << std::endl;
//
//		o << "i: " << i << "\ns[lower] = " << str[i] << ", s[upper] = "
//			<< str[i + pattern_size - 1] << ", " << std::endl;
//		o << "current_string = <" << str.substr(i, pattern_size) << ">" << std::endl;
//		o << "current_hash = " << current_hash << std::endl;
//		o << "expectedhash = " << polyhash(str, i, i + pattern_size - 1, 0) << '\n' << std::endl;
//		/*std::cout << "current_hash_expected = " << polyhash(str, i, i + pattern_size - 1) << std::endl;
//		std::cout << "current_hash_expected = " << polyhash(str.substr(i, pattern_size)) << std::endl;*/
//
//		if (current_hash == pattern_hash) {
//			 //and now checking strings
//			if (str.substr(i, pattern_size) == pattern) {
//				 //substr found!
//				return i;
//			}
//		}
//	}
//	o.close();
//
//	return (std::string::npos);
//}

//long long h(std::string const& str) {
//	const int a = 29;
//	const int p = 31;
//	long long hash = 0;
//
//	for (int i = 0; i < str.size(); i++) {
//		hash += str[i] * pow(a, i);
//	}
//	hash = hash % p;
//	return hash;
//}

//long long h(std::string const& str, long long& prev_hash) {
//	if (prev_hash == 0) {
//		return h(str);
//	}
//
//	const int a = 29;
//	const int p = 31;
//	long long hash = 0;
//
//	hash = prev_hash + str[str.size() - 1];
//
//}


//std::string::size_type findRK2(std::string const& str, std::string const& pattern) {
//	const int str_size = str.length();
//	const int pattern_size = pattern.length();
//	const long long pattern_hash = 0;
//	const int hashCount = str_size - pattern_size;
//	long long* hashArr = new long long[hashCount];
//
//	for (int i = 0; i < hashCount; i++) {
//		if (i == 0) {
//			hashArr[i] = h(str.substr(i, pattern_size));
//		}
//		else {
//			hashArr[i] = h(str.substr(i, pattern_size), hashArr[i-1]);
//		}
//		
//	}
//
//	for (int i = 0; i < hashCount; i++) {
//		if (hashArr[i] == pattern_hash) {
//			if (str.substr(i, pattern_size) == pattern) {
//				delete[] hashArr;
//				return i;
//			}
//		}
//	}
//
//	delete[] hashArr;
//	return (std::string::npos);
//}