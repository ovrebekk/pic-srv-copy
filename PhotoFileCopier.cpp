// PhotoFileCopier.cpp : Defines the entry point for the application.
//

#include "PhotoFileCopier.h"
#include "ConfigFileReader.h"
#include <filesystem>
#include <iostream>
#include <string>

using namespace std;
namespace fs = std::filesystem;

// ffmpeg -i INPUT.MP4 -c:v libx264 -preset slow -vf scale=1920:1080 -crf 22 -c:a copy OUTPUT.mp4
string testpath = "E:\\bilder\\fotografi\\2003";

uint64_t filesize_total = 0;
int file_count = 0;

void recursive_dir_search(const fs::path& cur_path, bool in_pic_folder = false)
{
	static int counter = 0;
	for (const fs::directory_entry& dir_entry : fs::directory_iterator(cur_path))
	{
		if (dir_entry.is_directory())
		{
			if (dir_entry.path().filename().string().size() >= 8 &&
				dir_entry.path().filename().string()[2] == '_' && 
				dir_entry.path().filename().string()[5] == '_')
			{
				//cout << "- " << fs::last_write_time(dir_entry) << " " << dir_entry.path().filename() << endl;
				// Valid picture folder found
				recursive_dir_search(dir_entry.path(), true);
				if (ConfigFileReader::configFileExistsInPath(dir_entry))
				{
					ConfigFileReader cfgReader;
					cfgReader.read(dir_entry, fs::last_write_time(dir_entry));
					if (cfgReader.timeForUpdate())
					{
						cout << "Folder change detected! Writing config file at " << dir_entry << endl;
						cfgReader.write(dir_entry, fs::last_write_time(dir_entry));
					}
				}
			}
			else
			{
				recursive_dir_search(dir_entry.path());
			}
		}
		else if (in_pic_folder)
		{

		}
		if (dir_entry.path().extension() == ".jpg" || dir_entry.path().extension() == ".JPG")
		{
			file_count++;
			filesize_total += dir_entry.file_size();
			
		}
	}
}

int main()
{
	fs::current_path(testpath);
	fs::path start_path = fs::path(testpath);
	cout << "PhotoFileCopier started" << endl;

	recursive_dir_search(start_path);
	cout << "Files: " << file_count << ", Size total: " << (filesize_total / 1024 / 1024) << " MB\n";
#if 0
	for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator("."))
	{
		if(dir_entry.is_directory())
			cout << dir_entry << '\n';
	}
#endif
	return 0;
}
