

#include <config.h>

#include<libmutil/FileSystem.h>
#include<fstream>

#include<sys/stat.h>
#include<sys/types.h>

#ifdef WIN32
#include<io.h>
#endif

using namespace std;

class LocalFile : public File {
	public:
		LocalFile(string path);
		virtual void read(void *buf, int32_t count);
		virtual void write(void *buf, int32_t count);
		virtual void seek(int32_t pos );
		virtual int64_t size();
		virtual void flush();

	private:
		fstream file;
		
};


LocalFile::LocalFile(string path) {
	cerr << "EEEE: trying to open <"<<path<<">"<<endl;
	file.open( path.c_str(), ios::out | ios::in | ios::binary );
	if (!file.is_open()){
		throw FileException("Could not open file");
	}
}

void LocalFile::read(void *buf, int32_t count){
	file.read( (char*)buf, count );
}

void LocalFile::write(void *buf, int32_t count){
	file.write( (char*)buf, count);
}

void LocalFile::seek(int32_t pos ){
	file.seekg( pos, ios::beg );
}

int64_t LocalFile::size(){
	int64_t tmp = file.tellg();		// save position
	file.seekg(0, std::ios_base::beg);
	std::ifstream::pos_type begin_pos = file.tellg();
	file.seekg(0, std::ios_base::end);
	int64_t s = static_cast<int64_t>( file.tellg() - begin_pos );
	file.seekg( tmp, ios::beg ); 		// restore position
	return s;
}

void LocalFile::flush(){
	file.flush();
}

void LocalFileSystem::mkdir( const std::string & name ){
#ifdef WIN32
	if ( _mkdir( name.c_str() ) != 0 ){
		throw FileSystemException("Could not create directory");
	}

#else
	if ( ::mkdir( name.c_str(), 0 ) != 0 ){
		throw FileSystemException("Could not create directory");
	}
#endif
}

MRef<File*> LocalFileSystem::open( const std::string & name, bool createIfNotExist ){
	string tmp=name;
	if ( name.size()>0  &&  name[0]!='/' ){
		tmp = defPrefix+name;
	}
	return new LocalFile(tmp);
}

FileException::FileException( string why ) : Exception(why){ }

FileSystemException::FileSystemException( string why ) : Exception(why){ }

void FileSystem::setDefaultPath(std::string dp){
	if (dp[dp.size()-1]=='/')
		defPrefix = dp;
	else
		defPrefix = dp+'/';

}

std::string FileSystem::getDefaultPath(){
	return defPrefix;
}
