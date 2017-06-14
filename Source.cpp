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


using namespace boost;
using namespace std;


unsigned int cpuCount = 1;
mutex scopeMutex;

int countFileLine(const vector<string> &filenames, vector<int> &countResult, int start, int end) {
	mutex::scoped_lock lock(scopeMutex);
	fprintf(stderr, "start:%d, end:%d\n", start, end);
	//for (auto &f : filenames) {
	//	//fprintf(stderr, "%s\n", f);
	//	cout << f << '\n';
	//}
	for (int i = start; i <= end; ++i) {
		cout << filenames[i] << '\n';
	}
	return 0; 
}

int getCpuCount() {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int cpuCount = si.dwNumberOfProcessors;
	//fprintf(stderr, "cpuCount: %d/n", cpuCount);
	return cpuCount;
}

char *usage() {
	return "usage: countLine path";
}
int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "%d\n", argc);
		for (int i = 0; i < argc; ++i) {
			fprintf(stderr, "%s\n", argv[i]);
		}
		fprintf(stderr, usage());
		return 1;
	}
	
	vector<string> fileNameList;
	filesystem::path rootPath(argv[1], filesystem::native);


	if (!filesystem::exists(rootPath)) { cout << "path not exist!";  return 2; }
	filesystem::recursive_directory_iterator end_iter;
	for (filesystem::recursive_directory_iterator iter(rootPath); iter != end_iter; iter++) {
		try {
			if (filesystem::is_directory(*iter)) {
				//cout << *iter << "is dir" << endl;
				//fileNameList.push_back(iter->path().string());
			}
			else {
				fileNameList.push_back(iter->path().string());
				//cout << *iter << " is a file" << endl;
			}
		}
		catch (const boost::filesystem::filesystem_error & ex) {
			cerr << ex.what() << endl;
			continue;
		}
	}

	vector<int> lineCountResult(fileNameList.size());

	cpuCount = getCpuCount();
	//cout << cpuCount << '\n';
	int thresh = ceil(fileNameList.size() / cpuCount);
	int threadCount = min(cpuCount, fileNameList.size());
	thread_group group;
	//cout << "threadCount:" << threadCount << " thresh:" << thresh << " fileCount:" << fileNameList.size() << '\n';
	for (int i = 0; i < threadCount; ++i) {
		int start = thresh * i;
		int end   = thresh * (i + 1) - 1;
		if (end >= fileNameList.size())    end = fileNameList.size() - 1;
		group.create_thread(bind(countFileLine, fileNameList, lineCountResult, start, end));
	}
	group.join_all();

	::system("pause");
	return 0;
}