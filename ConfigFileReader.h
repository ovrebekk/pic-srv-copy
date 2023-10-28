#pragma once

#include <filesystem>
#include <iostream>

#define CONFIG_FILE_NAME "_psc.txt"

#define PARAM_DATE_LAST_MODIFIED_STR "date_last_modified:"
#define PARAM_DATE_LAST_MODIFIED_INT_STR "date_last_modified_int:"
#define PARAM_TIMES_MODIFIED_STR "times_modified:"
#define PARAM_FORCE_UPDATE_STR "force_update:"

typedef enum {PARAM_NONE, PARAM_DATE_LAST_MODIFIED, PARAM_DATE_LAST_MODIFIED_INT, PARAM_TIMES_MODIFIED, PARAM_FORCE_UPDATE} CfgFileParamTypes;

class ConfigFileReader
{
private:
	int mTimesModified;
	std::time_t mLastChangedTime;
	bool mFileDateMatch;
	bool mForceUpdate;
public:
	ConfigFileReader(void);
	bool read(const std::filesystem::path& path, const std::filesystem::file_time_type& time);
	bool write(const std::filesystem::path& path, const std::filesystem::file_time_type& time);
	bool timeForUpdate(void);
	static bool configFileExistsInPath(const std::filesystem::path& path);
};