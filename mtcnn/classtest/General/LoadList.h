#ifndef __LOAD_LIST_H__
#define __LOAD_LIST_H__

#include <vector>
//using namespace std;

int LoadList(char *fname, std::vector <std::string> &list);
int SaveList(char *fname, std::vector <std::string> &list);

int find_name(std::vector <std::string> &list, char *name);
int find_name(std::vector <std::string> &list, std::string name);

#endif
