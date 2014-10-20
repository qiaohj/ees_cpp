/*
 * CommonFun.h
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */
using namespace std;
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <float.h>

#include <boost/algorithm/string/join.hpp>

#include "../JsonPaster/include/json/json.h"

class CommonFun {
public:
	static string readFile(const char* path);
	static string removeSuffix(const string& path,
	                                const string& extension);
	static Json::Value readJson(const char* path);
	static void writeFile(const string s, const char* path);
	static void writeFile(const vector<string> s, const char* path);
	template <typename T> static bool AlmostEqualRelative(T a, T b);
};


