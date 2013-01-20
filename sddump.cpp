/*
BURPLEX(C) WAITMAN GOBBLE

clang++ -I/usr/local/include -L/usr/local/lib -lboost_filesystem -lboost_date_time -o sddump sddump.cpp


*/
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <math.h>

namespace fs = boost::filesystem;
namespace pt = boost::posix_time;
namespace gr = boost::gregorian;

std::string logtime(void)
{
char outstr[200] = {0};
time_t t;
struct tm *tmp;
t = time(NULL);
tmp = localtime(&t);
strftime(outstr, sizeof(outstr), "%a, %d %b %Y %T %z", tmp);
std::string ret = outstr;
return ret; 
}

int
main(int argc, char* argv[]) {

if (argc<2) {
	exit (-1);
}

pt::ptime now = pt::second_clock::local_time();
//std::cout << now.date() << std::endl;

std::ofstream logfile;
logfile.open("/var/log/beenie",std::ios::app);



std::string dp = std::string("/u1/Comedy/") + gr::to_simple_string(now.date());
fs::path dest_path = fs::path(dp);
fs::create_directories(dest_path);

boost::uintmax_t bytes = 0;

//do this twice to calculate completion bytes
for ( fs::recursive_directory_iterator end, dir(argv[1]);
       dir != end; ++dir ) {
        if (boost::iends_with(dir->path().filename().string(),".MOV")) {
		bytes+=fs::file_size(dir->path());
	}
}

logfile << logtime() << ": [" << bytes << "] Begin Copy of " << argv[1] << std::endl;

boost::uintmax_t progress = 0;

for ( fs::recursive_directory_iterator end, dir(argv[1]); 
       dir != end; ++dir ) {
	if (boost::iends_with(dir->path().filename().string(),".MOV")) {

if (fs::exists(dest_path / dir->path().filename())) {
	logfile << logtime() << ": Error! File Exists! " << dest_path / dir->path().filename() << std::endl;
} else {
	progress+=fs::file_size(dir->path());
	double per = ceil(((double)progress/bytes)*100);

       logfile << logtime() << ": [" << per << "%/" << progress << "/" << bytes << "] Copied " << *dir << " to " << dest_path / dir->path().filename() << std::endl;   
	//copy
	fs::copy_file(*dir,dest_path / dir->path().filename());
}
	
	}                                 
}
logfile << logtime() << ": Completed Copy of " << argv[1] << std::endl;
logfile.close();
return 0;
}

