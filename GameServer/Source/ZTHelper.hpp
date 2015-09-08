#pragma once

// import
#include "stdafx.h"

// helpers collection
class ZTHelper {
public:
	// check syntax before broadcast message, scaleform have bug in client side
	static bool isValidChatSyntax(const char* Text) {
		std::string strText(Text);
		if (strText.find("<") != std::string::npos) {
			return false;
		}
		return true;
	}
};