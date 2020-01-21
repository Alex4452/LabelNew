#include "framework.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

namespace InternalMandatoryAccessControl
{
	class InternalSecurityContext
	{
	public:
		InternalSecurityContext()
		{};
		InternalSecurityContext(const string labelID, const int level, const vector<int>& compartments, const vector<int>& groups) :
			labelID(labelID), level(level), compartments(compartments), groups(groups)
		{}

		~InternalSecurityContext()
		{}

		int getLevel() { return level; };

		vector<int> getCompartments() { return compartments; };

		vector<int> getGroups() { return groups; };

		string getLabel();

		string getLabelID() { return labelID; };

	private:
		string labelID;
		int level;
		vector<int> compartments;
		vector<int> groups;
	};

	string InternalMandatoryAccessControl::InternalSecurityContext::getLabel()
	{
		string temp = labelID + ":" + to_string(level) + ":";
		for (auto it = compartments.begin(); it != compartments.end(); it++)
		{
			temp += to_string(*it) + ",";
		}
		temp.pop_back();
		temp += ":";
		for (auto it = groups.begin(); it != groups.end(); it++)
		{
			temp += to_string(*it) + ",";
		}
		temp.pop_back();
		return temp;
	}
}

