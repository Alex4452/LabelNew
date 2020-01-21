// ClientLabel.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "LabelNew.h"

using namespace MandatoryAccessControl;


int main()
{
	FileLabelStorage accessLabel("labelLevel.txt", "labelCompartments.txt", "labelGroups.txt", "createLabel.txt");
	Engine engineLabel(accessLabel);
	bool check = engineLabel.checkAccess(engineLabel.getSecurityContext("OBJECT"), engineLabel.getSecurityContext("SUBJECT"), READ);
	engineLabel.printAllLabel();
}