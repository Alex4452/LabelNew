#include "pch.h"
#include "..\LabelNew\LabelNew.h"

using namespace MandatoryAccessControl;

TEST(TestCreateSimpleLabel, TestSimpleLabelStorage) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	// Check stored access label

	EXPECT_EQ(labelTest.getLevel(40), "40:HIGHLY_SENSITIVE:HS");
	EXPECT_EQ(labelTest.getCompartments(85), "85:FINANCIAL:FINCL");
	EXPECT_EQ(labelTest.getGroups(1000), "1000:WESTERN_REGION:WR");
}

TEST(TestCreateSimpleLabel, TestObjectSimpleLabelStorage) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	vector<int> compartments = { 85 };
	vector<int> groups = { 1000 };
	labelTest.createObjectLabel("TestObject", 40, compartments, groups);
	map<string, SecurityContext*> testObjectLavel = labelTest.getAllObjLabels();

	EXPECT_TRUE(testObjectLavel.find("TestObject") != testObjectLavel.end());
}

TEST(TestCreateFileLabel, TestFileLabelStorage) {

	string fileLevel = "..\\UnitTestLabel\\TestFiles\\TestLevelLabel.txt";
	string fileCompartment = "..\\UnitTestLabel\\TestFiles\\TestLabelCompartments.txt";
	string fileGroup = "..\\UnitTestLabel\\TestFiles\\TestGroupLabel.txt";
	string fileObjectLabel = "..\\UnitTestLabel\\TestFiles\\TestObjectLabel.txt";

	FileLabelStorage labelFileTest(fileLevel, fileCompartment, fileGroup, fileObjectLabel);

	EXPECT_EQ(labelFileTest.getLevel(40), "40:HIGHLY_SENSITIVE:HS");
	EXPECT_EQ(labelFileTest.getCompartments(85), "85:FINANCIAL:FINCL");
	EXPECT_EQ(labelFileTest.getGroups(1000), "1000:WESTERN_REGION:WR");

}

TEST(TestCreateFileLabel, TestObjectFileLabelStorage) {
	string fileLevel = "..\\UnitTestLabel\\TestFiles\\TestLevelLabel.txt";
	string fileCompartment = "..\\UnitTestLabel\\TestFiles\\TestLabelCompartments.txt";
	string fileGroup = "..\\UnitTestLabel\\TestFiles\\TestGroupLabel.txt";
	string fileObjectLabel = "..\\UnitTestLabel\\TestFiles\\TestObjectLabel.txt";

	FileLabelStorage labelFileTest(fileLevel, fileCompartment, fileGroup, fileObjectLabel);

	map<string, SecurityContext*> testObjectLavel = labelFileTest.getAllObjLabels();

	EXPECT_TRUE(testObjectLavel.find("TEST_OBJECT") != testObjectLavel.end());
}

TEST(TestWorkEngine, TestGetLabelID) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	vector<int> compartments = { 85 };
	vector<int> groups = { 1000 };
	labelTest.createObjectLabel("TestObject", 40, compartments, groups);

	Engine engineTest(labelTest);

	EXPECT_EQ(engineTest.getSecurityContext("TestObject").getLabelID(), "TestObject");
}

TEST(TestWorkEngine, TestGetLabel) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	vector<int> compartments = { 85 };
	vector<int> groups = { 1000 };
	labelTest.createObjectLabel("TestObject", 40, compartments, groups);

	Engine engineTest(labelTest);

	EXPECT_EQ(engineTest.getReadableLookLabel("TestObject"), "TestObject:40:85:1000");
}

TEST(TestWorkEngine, TestReadAccessCheck) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createLevel("SENSITIVE", "S", 30);

	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createCompartment("CHEMICAL", "CHEM", 65);

	labelTest.createGroup("WESTERN_REGION", "WR", 1000);
	labelTest.createGroup("WR_SALES", "WR_SAL", 1100);

	vector<int> compartmentsObject = { 85, 65 };
	vector<int> groupsObject = { 1000, 1100 };
	labelTest.createObjectLabel("TestSubject", 40, compartmentsObject, groupsObject);

	vector<int> compartmentsSubject = { 65 };
	vector<int> groupsSubject = { 1100 };
	labelTest.createObjectLabel("TestObject", 30, compartmentsSubject, groupsSubject);

	Engine engineTest(labelTest);
	SecurityContext subjTest = engineTest.getSecurityContext("TestSubject");
	SecurityContext objTest = engineTest.getSecurityContext("TestObject");

	EXPECT_TRUE(engineTest.checkAccess(subjTest, objTest, READ));
	EXPECT_FALSE(engineTest.checkAccess(objTest, subjTest, READ));
}

TEST(TestWorkEngine, TestWriteAccessCheck) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createLevel("SENSITIVE", "S", 30);

	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createCompartment("CHEMICAL", "CHEM", 65);

	labelTest.createGroup("WESTERN_REGION", "WR", 1000);
	labelTest.createGroup("WR_SALES", "WR_SAL", 1100);

	vector<int> compartmentsObject = { 85, 65 };
	vector<int> groupsObject = { 1000, 1100 };
	labelTest.createObjectLabel("TestSubject", 40, compartmentsObject, groupsObject);

	vector<int> compartmentsSubject = { 65 };
	vector<int> groupsSubject = { 1100 };
	labelTest.createObjectLabel("TestObject", 40, compartmentsSubject, groupsSubject);

	Engine engineTest(labelTest);
	SecurityContext subjTest = engineTest.getSecurityContext("TestSubject");
	SecurityContext objTest = engineTest.getSecurityContext("TestObject");

	EXPECT_TRUE(engineTest.checkAccess(subjTest, objTest, WRITE));
	EXPECT_TRUE(engineTest.checkAccess(objTest, subjTest, WRITE));
}