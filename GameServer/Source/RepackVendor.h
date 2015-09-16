#ifndef REPACK_H
#define REPACK_H

#pragma once

#include <Windows.h>

struct RepackData
{
	//Repack versions
	char temp_ver[10];
	char bin_ver[10];
	char src_ver[10];
	
	//Repo & author
	char repo_url[50];
	char author[15];
};

class RepackVendor
{
public:
	RepackVendor();
	virtual ~RepackVendor();

	void ReadConfig();
	const RepackData getConfig();

private:
	RepackData config;

};

extern RepackVendor objRepack;

#endif