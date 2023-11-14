#include "ConfigFileReader.h"
#include <fstream>

namespace fs = std::filesystem;
using namespace std;

static std::string trim(const std::string& str,
	const std::string& whitespace = " \t")
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

static std::time_t fromFileTimeType(const fs::file_time_type& time)
{
	const auto systemTime = std::chrono::clock_cast<std::chrono::system_clock>(time);
	return std::chrono::system_clock::to_time_t(systemTime);
}

ConfigFileReader::ConfigFileReader(void)
{
	mTimesModified = 0;
	mLastChangedTime = 0;
	mFileDateMatch = false;
	mForceUpdate = false;
}

bool ConfigFileReader::read(const std::filesystem::path& path, const fs::file_time_type & time)
{
	fstream	cfg_file;
	fs::path cfg_file_path = path / CONFIG_FILE_NAME;
	cfg_file.open(cfg_file_path, ios::in);
	if (cfg_file.is_open())
	{
		string cfg_file_line;
		CfgFileParamTypes currentType = PARAM_NONE;
		while (getline(cfg_file, cfg_file_line))
		{
			cfg_file_line = trim(cfg_file_line);
			
			// Skip empty lines
			if (cfg_file_line == "") continue;
			
			// Check for a parameter identifier
			if (cfg_file_line == PARAM_DATE_LAST_MODIFIED_STR) {
				currentType = PARAM_DATE_LAST_MODIFIED;
			}
			if (cfg_file_line == PARAM_DATE_LAST_MODIFIED_INT_STR) {
				currentType = PARAM_DATE_LAST_MODIFIED_INT;
			}
			else if (cfg_file_line == PARAM_TIMES_MODIFIED_STR) {
				currentType = PARAM_TIMES_MODIFIED;
			}
			else if (cfg_file_line == PARAM_FORCE_UPDATE_STR) {
				currentType = PARAM_FORCE_UPDATE;
			}
			else if (cfg_file_line == PARAM_USERS_STR) {
				currentType = PARAM_USERS;
			}
			else
			{
				int timesModified;
				std::time_t fileTime;
				switch (currentType)
				{
				case PARAM_DATE_LAST_MODIFIED:
					break;
				case PARAM_DATE_LAST_MODIFIED_INT:
					fileTime = std::stoll(cfg_file_line);
					if (fileTime == fromFileTimeType(time)) {
						mFileDateMatch = true;
					}
					currentType = PARAM_NONE;
					break;
				case PARAM_TIMES_MODIFIED:
					timesModified = stoi(cfg_file_line);
					if (timesModified > 0)
					{
						mTimesModified = timesModified;
					}
					currentType = PARAM_NONE;
					break;
				case PARAM_FORCE_UPDATE:
					if (std::stoi(cfg_file_line) > 0) mForceUpdate = true;
					currentType = PARAM_NONE;
					break;
				case PARAM_USERS:
					mUserList.push_back(cfg_file_line);
					break;
				case PARAM_NONE:
					break;
				default:
					cout << "ERROR unexpected type\n";
					break;
				}
			}		
		}
		cfg_file.close();
	}
	return true;
}

bool ConfigFileReader::write(const std::filesystem::path& path, const fs::file_time_type& ftime)
{
	fstream cfg_file;
	fs::path cfg_file_path = path / CONFIG_FILE_NAME;
	if (fs::is_regular_file(cfg_file_path))
	{
		cfg_file.open(cfg_file_path, ios::out | ios::trunc);
		if (cfg_file.is_open())
		{
			mTimesModified++;
			cfg_file << PARAM_FORCE_UPDATE_STR << endl;
			cfg_file << "0" << endl;
			cfg_file << PARAM_DATE_LAST_MODIFIED_STR << endl;
			cfg_file << ftime << endl;
			cfg_file << PARAM_DATE_LAST_MODIFIED_INT_STR << endl;
			cfg_file << fromFileTimeType(ftime) << endl;
			cfg_file << PARAM_TIMES_MODIFIED_STR << endl;
			cfg_file << mTimesModified << endl;
			if (mUserList.size() > 0)
			{
				cfg_file << PARAM_USERS_STR << endl;
				for(string user : mUserList)
				{
					cfg_file << user << endl;
				}
			}
			cfg_file.close();
			return true;
		}
	}
	return false;
}

bool ConfigFileReader::timeForUpdate(void)
{
	return !mFileDateMatch || mForceUpdate;
}

bool ConfigFileReader::configFileExistsInPath(const std::filesystem::path& path)
{
	fs::path new_path = path / CONFIG_FILE_NAME;
	return fs::is_regular_file(new_path);
}