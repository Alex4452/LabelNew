#include "framework.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

namespace InternalMandatoryAccessControl
{
	typedef int LabelID;
	typedef string Label;
	typedef int ComponentID;

	class InternalSecurityContext
	{
	public:
		InternalSecurityContext()
		{};
		InternalSecurityContext(LabelID labelID, const ComponentID level,
			const vector<ComponentID>& compartments, const vector<ComponentID>& groups) :
			labelID(labelID), level(level), compartments(compartments), groups(groups)
		{}

		~InternalSecurityContext()
		{}

		ComponentID getLevel() { return level; };

		vector<ComponentID> getCompartments() { return compartments; };

		vector<ComponentID> getGroups() { return groups; };

		Label getLabel();

		LabelID getLabelID() { return labelID; };

	private:
		LabelID labelID;
		ComponentID level;
		vector<ComponentID> compartments;
		vector<ComponentID> groups;
	};

	Label InternalMandatoryAccessControl::InternalSecurityContext::getLabel()
	{
		Label temp = "";
		temp = to_string(labelID) + ":" + to_string(level) + ":";
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

