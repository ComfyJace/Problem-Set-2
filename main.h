// Created by: Jason Erwin Clyde V. Dimalanta (ID: 12198331)
#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <exception>
#include <algorithm>
#include <unordered_map>

#include "matchmaker.h"


void printHeader();

void readConfig(std::string fileName, std::unordered_map<std::string, long long int>& configValues);

void printConfig(std::unordered_map<std::string, long long int>& configValues);

std::string trim(const std::string& str);

void testThreadPool(std::unordered_map<std::string, long long int>& configValues);