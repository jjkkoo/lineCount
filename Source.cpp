#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <Windows.h>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
//#include <boost/thread/mutex.hpp>

using namespace boost;
using namespace std;

unsigned int cpuCount = 1;

int countFileLine(const list<string> &filenames, list<int> &countResult) { return 0; }

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
	
	list<filesystem::path> fileNameList;
	filesystem::path inputPath(argv[1]);
	filesystem::path::iterator pathIter = inputPath.begin();
	while (pathIter != inputPath.end())
	{
		fileNameList.push_back(*pathIter);
		std::cout << *pathIter << std::endl;
		++pathIter;
	}

	cpuCount = getCpuCount();
	cout << cpuCount << '\n';
	
	//thread_group group;
	//for (int i = 0; i<cpuCount; i++)
	//	group.create_thread(bind(&fileNameList.subList(0,1), " ", " "));
	//group.join_all();

	::system("pause");
	return 0;
}