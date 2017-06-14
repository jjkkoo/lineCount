#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
#include <typeinfo>
#include <math.h>
#include <iomanip>

using namespace boost;
using namespace std;

unsigned int cpuCount = 1;
mutex scopeMutex;

int getLineNumber(string filename) {
	FILE *fp;
	int n = 0;
	char buffer[4096];

	//if ((fp = fopen(filename.c_str(), "rb")) == NULL)    return -1;
	//fclose(fp);

	if ((fp = fopen(filename.c_str(), "rb")) == NULL)    return -1;
	/* check text file */
	char checkText[100];
	int readCount = fread(checkText, 1, 100, fp);
	if ((readCount = 100) || feof(fp)) {
		int checkZeroCount = 0;
		for (int i = 0; i < readCount; ++i) {
			if (checkText[i] == '\0')    ++checkZeroCount;
		}
		if (checkZeroCount > 2)    return -2;
	}
	else {
		return -1;
	}
	/* calculate line count */
	rewind(fp);
	while (true) {
		if (fgets(buffer, 4096, fp) != NULL) {
			if (buffer[strlen(buffer) - 1] == '\n')
			{
				n++;
			}
		}
		else if (feof(fp)) {
			n++;
			break;
		}
		else if (ferror(fp)) {
			n = -1;
			break;
		}
	}
	fclose(fp);
	return n;
}
int countFileLine(const vector<string> &filenames, int * startIter, int start, int end) {
	//mutex::scoped_lock lock(scopeMutex);
	for (int i = start; i <= end; ++i)    *startIter++ = getLineNumber(filenames[i]);
	return 0;
}

int getCpuCount() {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int cpuCount = si.dwNumberOfProcessors;
	return cpuCount;
}

char *usage() {
	return "usage: countLine path";
}
int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, usage());
		return 1;
	}

	vector<string> fileNameList;
	filesystem::path rootPath(argv[1], filesystem::native);

	if (!filesystem::exists(rootPath))    return -3;
	if (filesystem::is_directory(rootPath)) {
		filesystem::recursive_directory_iterator end_iter;
		for (filesystem::recursive_directory_iterator iter(rootPath); iter != end_iter; iter++) {
			try {
				if (!filesystem::is_directory(*iter))
					fileNameList.push_back(iter->path().string());
			}
			catch (const boost::filesystem::filesystem_error & ex) {
				cerr << ex.what() << endl;
				continue;
			}
		}
	}
	else {
		fileNameList.push_back(rootPath.string());
	}
	vector<int> lineCountResult(fileNameList.size());

	cpuCount = getCpuCount();
	int threadCount = min(cpuCount, fileNameList.size());
	int thresh = fileNameList.size() / threadCount;
	cout << "total files count: " << fileNameList.size() << "; threads count: " << threadCount << '\n';

	thread_group group;
	for (int i = 0; i < threadCount; ++i) {
		int start = thresh * i;
		int end = thresh * (i + 1) - 1;
		if ((i == threadCount - 1) && (end < fileNameList.size() - 1))    end = fileNameList.size() - 1;
		group.create_thread(bind(&countFileLine, fileNameList, lineCountResult.data() + start, start, end));
	}
	group.join_all();

	cout << "-1:cannot open; -2:not text file;" << '\n';
	int totalLineCount = 0;
	for (int i = 0; i < fileNameList.size(); ++i) {
		cout << setw(8) << dec << lineCountResult[i] << " : " << fileNameList[i] << '\n';
		if (lineCountResult[i] > 0)
		    totalLineCount += lineCountResult[i];
	}
	cout << "total lines count: " << totalLineCount << '\n';

	::system("pause");
	return 0;
}