#include "StdAfx.h"
#include "RepackVendor.h"
#include "dirpath.h"
#include "Define.h"
#include "logproc.h"
#include "LogToFile.h"

RepackVendor objRepack;

RepackVendor::RepackVendor() 
{
	this->ReadConfig();
}

RepackVendor::~RepackVendor() {}

void RepackVendor::ReadConfig()
{
	CDirPath path;
	path.SetFirstPath(FOLDER_ROOT);
	LPSTR pStr	=	path.GetNewPath(FILE_REPACK_CONF);

	//Load repack version strings
	GetPrivateProfileString("Version", "Template", "1.0.0", this->config.temp_ver, sizeof(this->config.temp_ver), pStr);
	GetPrivateProfileString("Version", "Binaries", "1.0.0", this->config.bin_ver, sizeof(this->config.bin_ver), pStr);
	GetPrivateProfileString("Version", "Source", "1.0.0", this->config.src_ver, sizeof(this->config.src_ver), pStr);

	//Load repack vendor info
	GetPrivateProfileString("Repository", "URL", "github.com/denkers", this->config.repo_url, sizeof(this->config.repo_url), pStr);
	GetPrivateProfileString("Repository", "Author", "denker", this->config.author, sizeof(this->config.author), pStr);

	CLogToFile logger = CLogToFile(LOG_NAME_DEBUG, LOG_PATH_DEBUG, true);
	//logger.Output("version: %s, url: %s, author: %s", config.src_ver, config.author);
	logger.Output("File: %s", pStr);
}


const RepackData RepackVendor::getConfig()
{
	return this->config;
}