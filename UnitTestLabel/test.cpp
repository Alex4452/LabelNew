#include "pch.h"
#include "..\LabelNew\LabelNew.h"

using namespace MandatoryAccessControl;

TEST(TestCaseName, TestName) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	vector<int> compartments = {85};
	vector<int> groups = {1000};
	labelTest.createObjectLabel("TestObject", 40, compartments, groups);

	Engine engineTest(labelTest);

  EXPECT_EQ(engineTest.getSecurityContext("TestObject").getLabelID(), "TestObject");
  EXPECT_TRUE(true);
}