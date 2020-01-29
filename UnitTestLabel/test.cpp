#include "pch.h"
#include "..\LabelNew\LabelNew.h"

using namespace MandatoryAccessControl;

TEST(TestCreateSimpleLabel, TestSimpleLabelStorage) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	// Check stored access label

	EXPECT_EQ(labelTest.getLevel(40).longForm, "HIGHLY_SENSITIVE");
	EXPECT_EQ(labelTest.getCompartments(85).shortForm, "FINCL");
	EXPECT_EQ(labelTest.getGroups(1000).longForm, "WESTERN_REGION");
}

TEST(TestCreateSimpleLabel, TestObjectSimpleLabelStorage) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	vector<ComponentID> compartments = { 85 };
	vector<ComponentID> groups = { 1000 };
	labelTest.createObjectLabel(1010, 40, compartments, groups);
	map<LabelID, SecurityContext*> testObjectLavel = labelTest.getAllObjLabels();

	EXPECT_TRUE(testObjectLavel.find(1010) != testObjectLavel.end());
}

TEST(TestCreateFileLabel, TestFileLabelStorage) {

	string fileLevel = "..\\UnitTestLabel\\TestFiles\\TestLevelLabel.txt";
	string fileCompartment = "..\\UnitTestLabel\\TestFiles\\TestLabelCompartments.txt";
	string fileGroup = "..\\UnitTestLabel\\TestFiles\\TestGroupLabel.txt";
	string fileObjectLabel = "..\\UnitTestLabel\\TestFiles\\TestObjectLabel.txt";

	FileLabelStorage labelFileTest(fileLevel, fileCompartment, fileGroup, fileObjectLabel);

	EXPECT_EQ(labelFileTest.getLevel(40).longForm, "HIGHLY_SENSITIVE");
	EXPECT_EQ(labelFileTest.getCompartments(85).shortForm, "FINCL");
	EXPECT_EQ(labelFileTest.getGroups(1000).longForm, "WESTERN_REGION");
}

TEST(TestCreateFileLabel, TestObjectFileLabelStorage) {
	string fileLevel = "..\\UnitTestLabel\\TestFiles\\TestLevelLabel.txt";
	string fileCompartment = "..\\UnitTestLabel\\TestFiles\\TestLabelCompartments.txt";
	string fileGroup = "..\\UnitTestLabel\\TestFiles\\TestGroupLabel.txt";
	string fileObjectLabel = "..\\UnitTestLabel\\TestFiles\\TestObjectLabel.txt";

	FileLabelStorage labelFileTest(fileLevel, fileCompartment, fileGroup, fileObjectLabel);

	map<LabelID, SecurityContext*> testObjectLavel = labelFileTest.getAllObjLabels();

	EXPECT_TRUE(testObjectLavel.find(1010) != testObjectLavel.end());
}

TEST(TestWorkEngine, TestGetLabelID) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	vector<ComponentID> compartments = { 85 };
	vector<ComponentID> groups = { 1000 };
	labelTest.createObjectLabel(1010, 40, compartments, groups);

	Engine engineTest(labelTest);

	EXPECT_EQ(engineTest.getSecurityContext(1010).getLabelID(), 1010);
}

TEST(TestWorkEngine, TestGetLabel) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	vector<ComponentID> compartments = { 85 };
	vector<ComponentID> groups = { 1000 };
	labelTest.createObjectLabel(1010, 40, compartments, groups);

	Engine engineTest(labelTest);

	EXPECT_EQ(engineTest.getReadableLookLabel(1010), "1010:40:85:1000");
}

TEST(TestWorkEngine, TestReadAccessCheck) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createLevel("SENSITIVE", "S", 30);

	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createCompartment("CHEMICAL", "CHEM", 65);

	labelTest.createGroup("WESTERN_REGION", "WR", 1000);
	labelTest.createGroup("WR_SALES", "WR_SAL", 1100, 1000);

	vector<int> compartmentsObject = { 85, 65 };
	vector<int> groupsObject = { 1000 };
	labelTest.createObjectLabel(1010, 40, compartmentsObject, groupsObject);

	vector<int> compartmentsSubject = { 65 };
	vector<int> groupsSubject = { 1100 };
	labelTest.createObjectLabel(2020, 30, compartmentsSubject, groupsSubject);

	Engine engineTest(labelTest);
	SecurityContext subjTest = engineTest.getSecurityContext(1010);
	SecurityContext objTest = engineTest.getSecurityContext(2020);

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
	labelTest.createGroup("WR_SALES", "WR_SAL", 1100, 1000);

	vector<int> compartmentsObject = { 85, 65 };
	vector<int> groupsSubject = { 1000 };
	labelTest.createObjectLabel(1010, 40, compartmentsObject, groupsSubject);

	vector<int> compartmentsSubject = { 65 };
	vector<int> groupsObject = { 1100 };
	labelTest.createObjectLabel(2020, 40, compartmentsSubject, groupsObject);

	Engine engineTest(labelTest);
	SecurityContext subjTest = engineTest.getSecurityContext(1010);
	SecurityContext objTest = engineTest.getSecurityContext(2020);

	EXPECT_TRUE(engineTest.checkAccess(subjTest, objTest, WRITE));
	EXPECT_FALSE(engineTest.checkAccess(objTest, subjTest, WRITE));
}