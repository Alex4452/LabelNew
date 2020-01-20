// LabelNew.cpp : Определяет экспортируемые функции для DLL.
//

#include "pch.h"
#include "framework.h"
#include "LabelNew.h"

using namespace MandatoryAccessControl;

FileLabelStorage::FileLabelStorage(string pathLevels, string pathCompartments, string pathGroups)
{
	parseFile(pathLevels, levels);
	parseFile(pathCompartments, compartments);
	parseFile(pathGroups, groups);
}

void FileLabelStorage::parseFile(string path, map<int, string>& labels)
{
	ifstream labelFile(path);
	if (!labelFile.is_open()) 
	{
		printf("File not found.");
		return;
	}

	while (!labelFile.eof())
	{
		string buff;
		getline(labelFile, buff);
		int labelID = 0;

		int startInd = 0;
		int colon = 0;
		int i = 0;
		string temp;
		for (; i < buff.length(); i++)
		{
			switch (buff[i])
			{
			case ':':
				readString(temp, buff, startInd, i);
				if (colon == 0)
				{
					labelID = atoi(temp.c_str());
					labels[atoi(temp.c_str())] = temp + ":";
				}
				else
				{
					labels[labelID] += temp + ":";
				}
				colon++;
				break;
			default:
				break;
			}
		}
		readString(temp, buff, startInd, i);
		labels[labelID] += temp;
	}
}

void FileLabelStorage::clearString(string & str)
{
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == ' ' || str[i] == ':' || str[i] == ',')
		{
			str.erase(i, 1);
			i--;
		}
	}
}

void FileLabelStorage::readString(string & temp, string input, int & startInd, int & i)
{
	temp = input.substr(startInd, i - startInd);
	clearString(temp);
	startInd = i;
}

string LabelStorage::getLevel(int id)
{
	map<int, string>::iterator it = levels.find(id);
	if (it != levels.end())
		return it->second;
	else
		return NULL;
}

string LabelStorage::getCompartments(int id)
{
	map<int, string>::iterator it = compartments.find(id);
	if (it != compartments.end())
		return it->second;
	else
		return NULL;
}

string LabelStorage::getGroups(int id)
{
	map<int, string>::iterator it = groups.find(id);
	if (it != groups.end())
		return it->second;
	else
		return NULL;
}

Engine::Engine(LabelStorage* storage, string pathObjectLabel) : storageAccLabel(storage)
{
	//storageAccLabel = &storage;
	parseObjLabel(pathObjectLabel);
}

void MandatoryAccessControl::Engine::parseObjLabel(string path)
{
	ifstream labelFile(path);
	if (!labelFile.is_open())
	{
		printf("File not found.");
		return;
	}

	while (!labelFile.eof())
	{
		string buff;
		getline(labelFile, buff);

		string* labelID = new string();
		int level = 0;
		vector<int> compartments;
		vector<int> groups;

		int startInd = 0;
		int colon = 0;
		string temp;
		int i = 0;
		for (; i < buff.length(); i++)
		{
			switch (buff[i])
			{
			case ':':
				FileLabelStorage::readString(temp, buff, startInd, i);
				checkColon(temp, *labelID, level, compartments, groups, colon);
				colon++;
				break;
			case ',':
				FileLabelStorage::readString(temp, buff, startInd, i);
				checkColon(temp, *labelID, level, compartments, groups, colon);
				break;
			default:
				break;
			}
		}
		FileLabelStorage::readString(temp, buff, startInd, i);
		checkColon(temp, *labelID, level, compartments, groups, colon);
		objLabels[*labelID] = new SecurityContext(*labelID, level, compartments, groups);
	}
}

void MandatoryAccessControl::Engine::checkColon(string temp, string & labelID, 
	int & level, vector<int>& compartments, vector<int>& groups, int colon)
{
	switch (colon)
	{
	case 0:
		labelID = temp;
		break;
	case 1:
		level = atoi(temp.c_str());
		break;
	case 2:
		compartments.push_back(atoi(temp.c_str()));
		break;
	case 3:
		groups.push_back(atoi(temp.c_str()));
	default:
		break;
	}
}

SecurityContext& MandatoryAccessControl::Engine::getSecurityContext(string labelID)
{
	if (objLabels.find(labelID) != objLabels.end())
	{
		return *objLabels.find(labelID)->second;
	}
	else
	{
		printf("Security context not found.");
	}
}

bool MandatoryAccessControl::Engine::checkAccess(SecurityContext subject, SecurityContext object, AccessVector accessVector)
{
	bool level = false;
	bool compartments = false;
	bool groups = false;
	switch (accessVector)
	{
	case READ:
		if (subject.getLevelLabel() >= object.getLevelLabel())
			level = true;
		break;
	case WRITE:
		if (subject.getLevelLabel() == object.getLevelLabel())
			level = true;
		break;
	default:
		level = false;
		break;
	}

	for (int i = 0; i < subject.getCompartmentsLabel().size(); i++)
	{
		for (int j = 0; j < object.getCompartmentsLabel().size(); j++)
		{
			if (subject.getCompartmentsLabel()[i] == object.getCompartmentsLabel()[j])
			{
				compartments = true;
				break;
			}
		}
	}

	for (int i = 0; i < subject.getGroupsLabel().size(); i++)
	{
		for (int j = 0; j < object.getGroupsLabel().size(); j++)
		{
			if (subject.getGroupsLabel()[i] == object.getGroupsLabel()[j])
			{
				groups = true;
				break;
			}
		}
	}

	if (level && compartments && groups)
		return true;
	else
		return false;

	
}

void MandatoryAccessControl::Engine::getAllLabel()
{
	for (auto it = objLabels.begin(); it != objLabels.end(); it++)
	{
		printf((*it).second->getLabel().c_str());
	}
}

void MandatoryAccessControl::Engine::getLabel(string labelID)
{
	if ((*objLabels.find(labelID)).second != NULL)
	{
		printf((*objLabels.find(labelID)).second->getLabel().c_str());
	}
	else
	{
		printf("Label not found.");
	}
}

MandatoryAccessControl::SecurityContext::SecurityContext(string id, int level, vector<int> compartments, vector<int> groups)
{
	objLabel = new InternalMandatoryAccessControl::InternalSecurityContext(id, level, compartments, groups);
}

string MandatoryAccessControl::SecurityContext::getLabel()
{
	return objLabel->getLabel(); 
}

string MandatoryAccessControl::SecurityContext::getLabelID()
{
	return objLabel->getLabelID();
}

int MandatoryAccessControl::SecurityContext::getLevelLabel()
{
	return objLabel->getLevel();
}

vector<int> MandatoryAccessControl::SecurityContext::getCompartmentsLabel()
{
	return objLabel->getCompartments();
}

vector<int> MandatoryAccessControl::SecurityContext::getGroupsLabel()
{
	return objLabel->getGroups();
}

string InternalMandatoryAccessControl::InternalSecurityContext::getLabel()
{
	string temp = labelID + ":" + to_string(level) + ":";
	for (auto it = compartments.begin(); it != compartments.end(); it++)
	{
		temp += *it + ",";
	}
	temp.pop_back();
	temp += ":";
	for (auto it = groups.begin(); it != groups.end(); it++)
	{
		temp += *it + ",";
	}
	temp.pop_back();
	return temp;
}
