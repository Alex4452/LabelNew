// LabelNew.cpp : Определяет экспортируемые функции для DLL.
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

void FileLabelStorage::parseFile(string path, map<ComponentID, LabelStorage::Component>& labels)
{
	ifstream labelFile;
	labelFile.open(path);
	if (!labelFile.is_open()) 
	{
		printf("File not found.\n");
		return;
	}

	while (!labelFile.eof())
	{
		string buff;
		getline(labelFile, buff);

		if (strlen(buff.c_str()) == 0)
			continue;

		ComponentID id = 0;
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
					id = atoi(temp.c_str());
					labels[id].idComponent = atoi(temp.c_str());
					labels[id].numForm = atoi(temp.c_str());
				}
				else if (colon == 1)
				{
					labels[id].longForm = temp;
				}
				else if (colon == 2)
				{
					labels[id].shortForm = temp;
				}
				colon++;
				break;
			default:
				break;
			}
		}
		readString(temp, buff, startInd, i);
		if (colon == 2)
			labels[id].shortForm = temp;
		else if (colon == 3)
			labels[id].parentComp = &getGroups(atoi(temp.c_str()));
		else
			labels[id].parentComp = NULL;
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

LabelStorage::Component& LabelStorage::getLevel(ComponentID id)
{
	map<ComponentID, LabelStorage::Component>::iterator it = levels.find(id);
	if (it != levels.end())
		return it->second;
	else
		return *(new LabelStorage::Component());
}

LabelStorage::Component& LabelStorage::getCompartments(ComponentID id)
{
	map<ComponentID, LabelStorage::Component>::iterator it = compartments.find(id);
	if (it != compartments.end())
		return it->second;
	else
		return *(new LabelStorage::Component());
}

LabelStorage::Component& LabelStorage::getGroups(ComponentID id)
{
	map<ComponentID, Component>::iterator it = groups.find(id);
	if (it != groups.end())
		return it->second;
	else
		return *(new Component());
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

		LabelID labelID;
		ComponentID level = 0;
		vector<ComponentID> compartments;
		vector<ComponentID> groups;

		bool accessCreateLabel;
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
				accessCreateLabel = checkColon(temp, labelID, level, compartments, groups, colon);
				if (!accessCreateLabel)
					break;
				colon++;
				break;
			case ',':
				FileLabelStorage::readString(temp, buff, startInd, i);
				accessCreateLabel = checkColon(temp, labelID, level, compartments, groups, colon);
				if (!accessCreateLabel)
					break;
				break;
			default:
				break;
			}
		}
		FileLabelStorage::readString(temp, buff, startInd, i);
		if(accessCreateLabel)
			accessCreateLabel = checkColon(temp, labelID, level, compartments, groups, colon);

		if (labelID == -1)
			labelID = hashID(level, compartments, groups);

		if(accessCreateLabel)
			objLabels[labelID] = new SecurityContext(labelID, level, compartments, groups);
		else
			printf("Check entered label.\n");
	}
}

/******************************************************************************
 *
 *	Checks which colon and depending on this determines a particular label field
 */

bool MandatoryAccessControl::LabelStorage::checkColon(string temp, LabelID & labelID,
	int & level, vector<int>& compartment, vector<int>& group, int colon)
{
	string type = "Default";
	switch (colon)
	{
	case 0:
		type = "ID";
		if (strlen(temp.c_str()) != 0)
		{
			labelID = atoi(temp.c_str());
			return true;
		}
		else
		{
			labelID = -1;
			return true;
		}
		break;
	case 1:
		type = "Level";
		if (levels.find(atoi(temp.c_str())) != levels.end())
		{
			level = atoi(temp.c_str());
			return true;
		}
		break;
	case 2:
		type = "Compartments";
		if (compartments.find(atoi(temp.c_str())) != compartments.end())
		{
			compartment.push_back(atoi(temp.c_str()));
			return true;
		}
		break;
	case 3:
		type = "Groups";
		if (groups.find(atoi(temp.c_str())) != groups.end())
		{
			group.push_back(atoi(temp.c_str()));
			return true;
		}
		break;
	default:
		type = "Collon number";
		break;
	}
	printf((type + " not found.\n").c_str());
	return false;
}

LabelID MandatoryAccessControl::LabelStorage::hashID(ComponentID level, vector<ComponentID>& compartments, vector<ComponentID>& groups)
{
	LabelID hashLabel;
	ComponentID comp = 0;
	ComponentID gr = 0;

	for (int i = 0; i < compartments.size(); i++)
	{
		comp += compartments[i];
	}

	for (int i = 0; i < groups.size(); i++)
	{
		gr += groups[i];
	}

	hashLabel = (level + comp + gr);
	std::hash<LabelID> hashF;
	hashLabel = hashF(hashLabel);
	if (hashLabel < 0)
		hashLabel = ~hashLabel;
	return hashLabel;
}

SecurityContext& MandatoryAccessControl::Engine::getSecurityContext(LabelID labelID)
{
	map<LabelID, SecurityContext*> temp = label.getAllObjLabels();
	if (temp.find(labelID) != temp.end())
	{
		return *temp.find(labelID)->second;
	}
	else
	{
		printf("Security context not found.\n");
	}
}

bool MandatoryAccessControl::Engine::checkAccess(SecurityContext& subject, SecurityContext& object, 
	AccessVector accessVector, SecurityPolicy policy)
{
	bool level = false;
	bool compartments = false;
	bool groups = false;

	// Check levels
	level = compareLevel(subject, object, accessVector);

	// Check groups
	if (policy != COMPACCESS)
		groups = compareGroups(subject, object);
	else
		groups = true;

	// Check compartmets
	compartments = compareCompartments(subject, object);

	// Check policy
	switch (policy)
	{
	case MandatoryAccessControl::FULL_ACCESS:
		level = true;
		compartments = true;
		groups = true;
		break;
	case MandatoryAccessControl::READ_ACCESS:
		if (accessVector == READ)
		{
			level = true;
			compartments = true;
			groups = true;
		}
		break;
	default:
		break;
	}

	if (level && compartments && groups)
		return true;
	else
		return false;
}

bool MandatoryAccessControl::Engine::compareLevel(SecurityContext & subject, SecurityContext & object, AccessVector accessVector)
{
	switch (accessVector)
	{
	case READ:
		if (subject.getLevelLabel() >= object.getLevelLabel())
			return true;
		break;
	case WRITE:
		if (subject.getLevelLabel() == object.getLevelLabel())
			return true;
		break;
	default:
		return false;
		break;
	}
	return false;
}

bool MandatoryAccessControl::Engine::compareCompartments(SecurityContext & subject, SecurityContext & object)
{
	if (object.getCompartmentsLabel().size() == NULL)
		return true;
	else if (subject.getCompartmentsLabel().size() == NULL)
		return false;
	else
	{
		map<int, bool> objInclude;
		for (int i = 0; i < object.getCompartmentsLabel().size(); i++)
		{
			objInclude[object.getCompartmentsLabel()[i]] = false;
		}

		for (int i = 0; i < subject.getCompartmentsLabel().size(); i++)
		{
			if (objInclude.find(subject.getCompartmentsLabel()[i]) != objInclude.end())
				objInclude[objInclude.find(subject.getCompartmentsLabel()[i])->first] = true;
		}

		for (auto it = objInclude.begin(); it != objInclude.end(); it++)
		{
			if (it->second == false)
				return false;
		}
		return true;
	}
}

bool MandatoryAccessControl::Engine::compareGroups(SecurityContext & subject, SecurityContext & object)
{
	if (object.getGroupsLabel().size() == NULL)
		return true;
	else if (subject.getGroupsLabel().size() == NULL)
		return false;
	else
	{
		for (int i = 0; i < subject.getGroupsLabel().size(); i++)
		{
			for (int j = 0; j < object.getGroupsLabel().size(); j++)
			{
				if (subject.getGroupsLabel()[i] == object.getGroupsLabel()[j])
					return true;
				else
				{
					LabelStorage::Component* obj = &label.getGroups(object.getGroupsLabel()[j]);
					while (obj->parentComp)
					{	
						if (subject.getGroupsLabel()[i] == obj->parentComp->numForm)
							return true;
						else
							obj = obj->parentComp;
					}
				}
			}
		}
	}
	return false;
}

Label MandatoryAccessControl::Engine::getAllLabel()
{
	Label temp = "";
	map<LabelID, SecurityContext*> tempLabel = label.getAllObjLabels();
	for (auto it = tempLabel.begin(); it != tempLabel.end(); it++)
	{
		temp += (*it).second->getLabel() + "\n";
	}
	return temp;
}

Label MandatoryAccessControl::Engine::getLabel(LabelID labelID)
{
	Label temp = "";
	map<LabelID, SecurityContext*> tempLabel = label.getAllObjLabels();
	if (tempLabel.find(labelID) != tempLabel.end())
	{
		temp += (*tempLabel.find(labelID)).second->getLabel();
	}
	else
	{
		printf("Label not found.");
	}
	return temp;
}

LabelID MandatoryAccessControl::Engine::getLabelID(Label labelRead)
{
	map<LabelID, SecurityContext*> tempLabel = label.getAllObjLabels();

	for (auto it = tempLabel.begin(); it != tempLabel.end(); it++)
	{
		if (it->second->getLabel() == labelRead)
		{
			return it->second->getLabelID();
		}
	}
	return NULL;
}

MandatoryAccessControl::SecurityContext::SecurityContext(const LabelID id, const ComponentID level,
	const vector<ComponentID>& compartments, const vector<ComponentID>& groups)
{
	objLabel = new InternalMandatoryAccessControl::InternalSecurityContext(id, level, compartments, groups);
}

Label MandatoryAccessControl::SecurityContext::getLabel()
{
	return objLabel->getLabel(); 
}

LabelID MandatoryAccessControl::SecurityContext::getLabelID()
{
	return objLabel->getLabelID();
}

ComponentID MandatoryAccessControl::SecurityContext::getLevelLabel()
{
	return objLabel->getLevel();
}

vector<ComponentID> MandatoryAccessControl::SecurityContext::getCompartmentsLabel()
{
	return objLabel->getCompartments();
}

vector<ComponentID> MandatoryAccessControl::SecurityContext::getGroupsLabel()
{
	return objLabel->getGroups();
}

void MandatoryAccessControl::SimpleLabelStorage::createLevel(string full, string shortForm, int numForm)
{
	levels[numForm].idComponent = numForm;
	levels[numForm].numForm = numForm;
	levels[numForm].longForm = full;
	levels[numForm].shortForm = shortForm;
}

void MandatoryAccessControl::SimpleLabelStorage::createCompartment(string full, string shortForm, int numForm)
{
	compartments[numForm].idComponent = numForm;
	compartments[numForm].numForm = numForm;
	compartments[numForm].longForm = full;
	compartments[numForm].shortForm = shortForm;
}

void MandatoryAccessControl::SimpleLabelStorage::createGroup(string full, string shortForm, int numForm, ComponentID parent)
{
	groups[numForm].idComponent = numForm;
	groups[numForm].numForm = numForm;
	groups[numForm].longForm = full;
	groups[numForm].shortForm = shortForm;
	groups[numForm].parentComp = &getGroups(parent);
}

void MandatoryAccessControl::SimpleLabelStorage::createObjectLabel(ComponentID level, 
	vector<ComponentID>& compartment, vector<ComponentID>& group, LabelID id)
{
	bool lev = false;
	lev = (levels.find(level) != levels.end()) ? true : false;

	bool comp = false;
	if (compartment.size() == NULL)
		comp = true;
	else
	{
		for (int i = 0; i < compartment.size(); i++)
		{
			comp = (compartments.find(compartment[i]) != compartments.end()) ? true : false;
		}
	}

	bool gr = false;
	if (groups.size() == NULL)
		gr = true;
	else
	{
		for (int i = 0; i < group.size(); i++)
		{
			gr = (groups.find(group[i]) != groups.end()) ? true : false;
		}
	}

	if (id == -1)
		id = hashID(level, compartment, group);

	if (lev && comp && gr)
		objLabels[id] = new SecurityContext(id, level, compartment, group);
	else
		printf("Check entered label.\n");
}
