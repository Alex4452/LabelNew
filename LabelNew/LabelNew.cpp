﻿// LabelNew.cpp : Определяет экспортируемые функции для DLL.
//

#include "pch.h"
#include "framework.h"
#include "LabelNew.h"

using namespace MandatoryAccessControl;

FileLabelStorage::FileLabelStorage(string pathLevels, string pathCompartments, string pathGroups, string pathObjectLabel)
{
	parseFile(pathLevels, levels);
	parseFile(pathCompartments, compartments);
	parseFile(pathGroups, groups);

	parseObjLabel(pathObjectLabel);
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

void LabelStorage::clearString(string & str)
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

void LabelStorage::readString(string & temp, string input, int & startInd, int & i)
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

void MandatoryAccessControl::FileLabelStorage::parseObjLabel(string path)
{
	ifstream labelFile(path);
	if (!labelFile.is_open())
	{
		printf("File not found.\n");
		return;
	}

	while (!labelFile.eof())
	{
		string buff;
		getline(labelFile, buff);

		string labelID;
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
				checkColon(temp, labelID, level, compartments, groups, colon);
				colon++;
				break;
			case ',':
				FileLabelStorage::readString(temp, buff, startInd, i);
				checkColon(temp, labelID, level, compartments, groups, colon);
				break;
			default:
				break;
			}
		}
		FileLabelStorage::readString(temp, buff, startInd, i);
		checkColon(temp, labelID, level, compartments, groups, colon);
		objLabels[labelID] = new SecurityContext(labelID, level, compartments, groups);
	}
}

void MandatoryAccessControl::LabelStorage::checkColon(string temp, string & labelID,
	int & level, vector<int>& compartment, vector<int>& group, int colon)
{
	switch (colon)
	{
	case 0:
		labelID = temp;
		break;
	case 1:
		if (levels.find(atoi(temp.c_str())) != levels.end())
			level = atoi(temp.c_str());
		else
			printf("Level not found.\n");
		break;
	case 2:
		if (compartments.find(atoi(temp.c_str())) != compartments.end())
			compartment.push_back(atoi(temp.c_str()));
		else
			printf("Compartments not found.\n");
		break;
	case 3:
		if (groups.find(atoi(temp.c_str())) != groups.end())
			group.push_back(atoi(temp.c_str()));
		else
			printf("Group not found.\n");
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
		printf("Security context not found.\n");
	}
}

bool MandatoryAccessControl::Engine::checkAccess(SecurityContext& subject, SecurityContext& object, AccessVector accessVector)
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

void MandatoryAccessControl::Engine::printAllLabel()
{
	for (auto it = objLabels.begin(); it != objLabels.end(); it++)
	{
		printf((*it).second->getLabel().c_str());
		printf("\n");
	}
}

void MandatoryAccessControl::Engine::printLabel(string labelID)
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

MandatoryAccessControl::SecurityContext::SecurityContext(const string id, const int level,
	const vector<int>& compartments, const vector<int>& groups)
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

void MandatoryAccessControl::SimpleLabelStorage::createLevel(string full, string shortForm, int tag)
{
	string temp = tag + ":" + full + ":" + shortForm;
	levels[tag] = temp;
}

void MandatoryAccessControl::SimpleLabelStorage::createCompartment(string full, string shortForm, int tag)
{
	string temp = tag + ":" + full + ":" + shortForm;
	compartments[tag] = temp;
}

void MandatoryAccessControl::SimpleLabelStorage::createGroup(string full, string shortForm, int tag)
{
	string temp = tag + ":" + full + ":" + shortForm;
	groups[tag] = temp;
}

void MandatoryAccessControl::SimpleLabelStorage::createObjectLabel(string id, int level, vector<int>& compartment, vector<int>& group)
{
	bool lev = (levels.find(level) != levels.end()) ? true : false;

	bool comp;
	for (int i = 0; i < compartment.size(); i++)
	{
		comp = (compartments.find(compartment[i]) != compartments.end()) ? true : false;
	}

	bool gr;
	for (int i = 0; i < compartment.size(); i++)
	{
		gr = (groups.find(group[i]) != groups.end()) ? true : false;
	}

	if (lev && comp && gr)
		objLabels[id] = new SecurityContext(id, level, compartment, group);
	else
		printf("Check entered label.\n");
}
