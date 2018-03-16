#include "LoadList.h"

int LoadList(char *fname, std::vector <std::string> &list)
{
	FILE *fp = fopen(fname,"r");
	if (fp)
	{
		list.clear();
		char line[512];
		while (1)
		{
			char *p = fgets(line, 1024, fp);
			if (p == NULL)
				break;
			int len = strlen(p);
			if (len >= 2)
			{
				while (p[len - 1] == '\n')
				{
					p[len - 1] = '\0';
					len--;
				}
				list.push_back(p);
			}
		}
		fclose(fp);
		return 0;
	} else
		return 1;
}

int find_name(std::vector <std::string> &list, char *name)
{
	for (int i = 0; i < list.size(); i++)
	{
		if (strcmp(list[i].c_str(), name) == 0)
			return i;
	}
	return -1;
}
int find_name(std::vector <std::string> &list, std::string name)
{
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i] == name)
			return i;
	}
	return -1;
}

int SaveList(char *fname, std::vector <std::string> &list)
{
	FILE *fp = fopen(fname, "w");
	if (fp)
	{
		char line[512];
		for (int i = 0; i < (int)list.size(); i++)
		{
			fprintf(fp, "%s\n", list[i].c_str());
		}
		fclose(fp);
		return 0;
	}
	else
		return 1;
}