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
	labelTest.createObjectLabel(40, compartments, groups, 1010);
	map<LabelID, SecurityContext*> testObjectLavel = labelTest.getAllObjLabels();

	EXPECT_TRUE(testObjectLavel.find(1010) != testObjectLavel.end());
}

TEST(TestCreateSimpleLabel, TestObjectSimpleLabelStorageNullID) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	vector<ComponentID> compartments = { 85 };
	vector<ComponentID> groups = { 1000 };
	labelTest.createObjectLabel(40, compartments, groups);
	map<LabelID, SecurityContext*> testObjectLavel = labelTest.getAllObjLabels();

	EXPECT_TRUE(testObjectLavel.find(labelTest.hashID(40, compartments, groups)) != testObjectLavel.end());
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

TEST(TestCreateFileLabel, TestObjectFileLabelStorageNullID) {
	string fileLevel = "..\\UnitTestLabel\\TestFiles\\TestLevelLabel.txt";
	string fileCompartment = "..\\UnitTestLabel\\TestFiles\\TestLabelCompartments.txt";
	string fileGroup = "..\\UnitTestLabel\\TestFiles\\TestGroupLabel.txt";
	string fileObjectLabel = "..\\UnitTestLabel\\TestFiles\\TestObjectLabelNullID.txt";

	FileLabelStorage labelFileTest(fileLevel, fileCompartment, fileGroup, fileObjectLabel);

	map<LabelID, SecurityContext*> testObjectLavel = labelFileTest.getAllObjLabels();

	EXPECT_TRUE(testObjectLavel.find(-1) == testObjectLavel.end());
	EXPECT_EQ(testObjectLavel.size(), 3);
}

TEST(TestWorkEngine, TestGetLabelID) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	vector<ComponentID> compartments = { 85 };
	vector<ComponentID> groups = { 1000 };
	labelTest.createObjectLabel(40, compartments, groups, 1010);

	Engine engineTest(labelTest);

	Label testLabel = engineTest.getLabel(1010);

	EXPECT_EQ(engineTest.getLabelID(testLabel), 1010);
}

TEST(TestWorkEngine, TestGetLabel) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createGroup("WESTERN_REGION", "WR", 1000);

	vector<ComponentID> compartments = { 85 };
	vector<ComponentID> groups = { 1000 };
	labelTest.createObjectLabel(40, compartments, groups, 1010);

	Engine engineTest(labelTest);

	EXPECT_EQ(engineTest.getLabel(1010), "1010:40:85:1000");
}

TEST(TestEngineCheckAccessForRead, TestReadAccessCheckForSimpleLabSt) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createLevel("SENSITIVE", "S", 30);

	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createCompartment("CHEMICAL", "CHEM", 65);

	labelTest.createGroup("WESTERN_REGION", "WR", 1000);
	labelTest.createGroup("WR_SALES", "WR_SAL", 1100, 1000);

	vector<int> compartmentsObject = { 85, 65 };
	vector<int> groupsObject = { 1000 };
	labelTest.createObjectLabel(40, compartmentsObject, groupsObject, 1010);

	vector<int> compartmentsSubject = { 65 };
	vector<int> groupsSubject = { 1100 };
	labelTest.createObjectLabel(30, compartmentsSubject, groupsSubject, 2020);

	Engine engineTest(labelTest);
	SecurityContext subjTest = engineTest.getSecurityContext(1010);
	SecurityContext objTest = engineTest.getSecurityContext(2020);

	EXPECT_TRUE(engineTest.checkAccess(subjTest, objTest, READ));
	EXPECT_FALSE(engineTest.checkAccess(objTest, subjTest, READ));
}

TEST(TestEngineCheckAccessForRead, TestReadAccessCheckForFileLabSt) {

	string fileLevel = "..\\UnitTestLabel\\TestFiles\\TestLevelLabel.txt";
	string fileCompartment = "..\\UnitTestLabel\\TestFiles\\TestLabelCompartments.txt";
	string fileGroup = "..\\UnitTestLabel\\TestFiles\\TestGroupLabel.txt";
	string fileObjectLabel = "..\\UnitTestLabel\\TestFiles\\TestObjectLabel.txt";

	FileLabelStorage labelFileTest(fileLevel, fileCompartment, fileGroup, fileObjectLabel);

	Engine engineTest(labelFileTest);
	SecurityContext firstLabelTest = engineTest.getSecurityContext(1010);
	SecurityContext secondLabelTest = engineTest.getSecurityContext(2020);
	SecurityContext thridLabelTest = engineTest.getSecurityContext(3030);

	EXPECT_TRUE(engineTest.checkAccess(firstLabelTest, secondLabelTest, READ));
	EXPECT_FALSE(engineTest.checkAccess(secondLabelTest, firstLabelTest, READ));

	EXPECT_TRUE(engineTest.checkAccess(firstLabelTest, thridLabelTest, READ));
	EXPECT_FALSE(engineTest.checkAccess(thridLabelTest, firstLabelTest, READ));

	EXPECT_FALSE(engineTest.checkAccess(secondLabelTest, thridLabelTest, READ));
	EXPECT_FALSE(engineTest.checkAccess(thridLabelTest, secondLabelTest, READ));
}

TEST(TestEngineCheckAccessForWrite, TestWriteAccessCheckForSimpleLabSt) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createLevel("SENSITIVE", "S", 30);

	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createCompartment("CHEMICAL", "CHEM", 65);

	labelTest.createGroup("WESTERN_REGION", "WR", 1000);
	labelTest.createGroup("WR_SALES", "WR_SAL", 1100, 1000);

	vector<int> compartmentsObject = { 85, 65 };
	vector<int> groupsSubject = { 1000 };
	labelTest.createObjectLabel(40, compartmentsObject, groupsSubject, 1010);

	vector<int> compartmentsSubject = { 65 };
	vector<int> groupsObject = { 1100 };
	labelTest.createObjectLabel(40, compartmentsSubject, groupsObject, 2020);

	Engine engineTest(labelTest);
	SecurityContext subjTest = engineTest.getSecurityContext(1010);
	SecurityContext objTest = engineTest.getSecurityContext(2020);

	EXPECT_TRUE(engineTest.checkAccess(subjTest, objTest, WRITE));
	EXPECT_FALSE(engineTest.checkAccess(objTest, subjTest, WRITE));
}

TEST(TestEngineCheckAccessForWrite, TestWriteAccessCheckForFileLabSt) {

	string fileLevel = "..\\UnitTestLabel\\TestFiles\\TestLevelLabel.txt";
	string fileCompartment = "..\\UnitTestLabel\\TestFiles\\TestLabelCompartments.txt";
	string fileGroup = "..\\UnitTestLabel\\TestFiles\\TestGroupLabel.txt";
	string fileObjectLabel = "..\\UnitTestLabel\\TestFiles\\TestObjectLabel.txt";

	FileLabelStorage labelFileTest(fileLevel, fileCompartment, fileGroup, fileObjectLabel);

	Engine engineTest(labelFileTest);
	SecurityContext firstLabelTest = engineTest.getSecurityContext(1010);
	SecurityContext secondLabelTest = engineTest.getSecurityContext(2020);
	SecurityContext thridLabelTest = engineTest.getSecurityContext(3030);

	EXPECT_FALSE(engineTest.checkAccess(firstLabelTest, secondLabelTest, WRITE));
	EXPECT_FALSE(engineTest.checkAccess(secondLabelTest, firstLabelTest, WRITE));

	EXPECT_FALSE(engineTest.checkAccess(firstLabelTest, thridLabelTest, WRITE));
	EXPECT_FALSE(engineTest.checkAccess(thridLabelTest, firstLabelTest, WRITE));

	EXPECT_FALSE(engineTest.checkAccess(secondLabelTest, thridLabelTest, WRITE));
	EXPECT_FALSE(engineTest.checkAccess(thridLabelTest, secondLabelTest, WRITE));
}

TEST(TestEngineCheckAccessWithPolitics, TestReadAccessForFullAccsess) {

	string fileLevel = "..\\UnitTestLabel\\TestFiles\\TestLevelLabel.txt";
	string fileCompartment = "..\\UnitTestLabel\\TestFiles\\TestLabelCompartments.txt";
	string fileGroup = "..\\UnitTestLabel\\TestFiles\\TestGroupLabel.txt";
	string fileObjectLabel = "..\\UnitTestLabel\\TestFiles\\TestObjectLabel.txt";

	FileLabelStorage labelFileTest(fileLevel, fileCompartment, fileGroup, fileObjectLabel);

	Engine engineTest(labelFileTest);
	SecurityContext firstLabelTest = engineTest.getSecurityContext(1010);
	SecurityContext secondLabelTest = engineTest.getSecurityContext(2020);
	SecurityContext thridLabelTest = engineTest.getSecurityContext(3030);

	EXPECT_TRUE(engineTest.checkAccess(firstLabelTest, secondLabelTest, READ, FULL_ACCESS));
	EXPECT_TRUE(engineTest.checkAccess(secondLabelTest, firstLabelTest, READ, FULL_ACCESS));

	EXPECT_TRUE(engineTest.checkAccess(firstLabelTest, thridLabelTest, READ, FULL_ACCESS));
	EXPECT_TRUE(engineTest.checkAccess(thridLabelTest, firstLabelTest, READ, FULL_ACCESS));

	EXPECT_TRUE(engineTest.checkAccess(secondLabelTest, thridLabelTest, READ, FULL_ACCESS));
	EXPECT_TRUE(engineTest.checkAccess(thridLabelTest, secondLabelTest, READ, FULL_ACCESS));
}

TEST(TestEngineCheckAccessWithPolitics, TestWriteAccessForFullAccsess) {

	string fileLevel = "..\\UnitTestLabel\\TestFiles\\TestLevelLabel.txt";
	string fileCompartment = "..\\UnitTestLabel\\TestFiles\\TestLabelCompartments.txt";
	string fileGroup = "..\\UnitTestLabel\\TestFiles\\TestGroupLabel.txt";
	string fileObjectLabel = "..\\UnitTestLabel\\TestFiles\\TestObjectLabel.txt";

	FileLabelStorage labelFileTest(fileLevel, fileCompartment, fileGroup, fileObjectLabel);

	Engine engineTest(labelFileTest);
	SecurityContext firstLabelTest = engineTest.getSecurityContext(1010);
	SecurityContext secondLabelTest = engineTest.getSecurityContext(2020);
	SecurityContext thridLabelTest = engineTest.getSecurityContext(3030);

	EXPECT_TRUE(engineTest.checkAccess(firstLabelTest, secondLabelTest, WRITE, FULL_ACCESS));
	EXPECT_TRUE(engineTest.checkAccess(secondLabelTest, firstLabelTest, WRITE, FULL_ACCESS));

	EXPECT_TRUE(engineTest.checkAccess(firstLabelTest, thridLabelTest, WRITE, FULL_ACCESS));
	EXPECT_TRUE(engineTest.checkAccess(thridLabelTest, firstLabelTest, WRITE, FULL_ACCESS));

	EXPECT_TRUE(engineTest.checkAccess(secondLabelTest, thridLabelTest, WRITE, FULL_ACCESS));
	EXPECT_TRUE(engineTest.checkAccess(thridLabelTest, secondLabelTest, WRITE, FULL_ACCESS));
}

TEST(TestEngineCheckAccessWithPolitics, TestReadAccessForReadAccess) {

	string fileLevel = "..\\UnitTestLabel\\TestFiles\\TestLevelLabel.txt";
	string fileCompartment = "..\\UnitTestLabel\\TestFiles\\TestLabelCompartments.txt";
	string fileGroup = "..\\UnitTestLabel\\TestFiles\\TestGroupLabel.txt";
	string fileObjectLabel = "..\\UnitTestLabel\\TestFiles\\TestObjectLabel.txt";

	FileLabelStorage labelFileTest(fileLevel, fileCompartment, fileGroup, fileObjectLabel);

	Engine engineTest(labelFileTest);
	SecurityContext firstLabelTest = engineTest.getSecurityContext(1010);
	SecurityContext secondLabelTest = engineTest.getSecurityContext(2020);
	SecurityContext thridLabelTest = engineTest.getSecurityContext(3030);

	EXPECT_TRUE(engineTest.checkAccess(firstLabelTest, secondLabelTest, READ, READ_ACCESS));
	EXPECT_TRUE(engineTest.checkAccess(secondLabelTest, firstLabelTest, READ, READ_ACCESS));

	EXPECT_TRUE(engineTest.checkAccess(firstLabelTest, thridLabelTest, READ, READ_ACCESS));
	EXPECT_TRUE(engineTest.checkAccess(thridLabelTest, firstLabelTest, READ, READ_ACCESS));

	EXPECT_TRUE(engineTest.checkAccess(secondLabelTest, thridLabelTest, READ, READ_ACCESS));
	EXPECT_TRUE(engineTest.checkAccess(thridLabelTest, secondLabelTest, READ, READ_ACCESS));
}

TEST(TestEngineCheckAccessWithPolitics, TestWriteAccessForReadAccess) {

	string fileLevel = "..\\UnitTestLabel\\TestFiles\\TestLevelLabel.txt";
	string fileCompartment = "..\\UnitTestLabel\\TestFiles\\TestLabelCompartments.txt";
	string fileGroup = "..\\UnitTestLabel\\TestFiles\\TestGroupLabel.txt";
	string fileObjectLabel = "..\\UnitTestLabel\\TestFiles\\TestObjectLabel.txt";

	FileLabelStorage labelFileTest(fileLevel, fileCompartment, fileGroup, fileObjectLabel);

	Engine engineTest(labelFileTest);
	SecurityContext firstLabelTest = engineTest.getSecurityContext(1010);
	SecurityContext secondLabelTest = engineTest.getSecurityContext(2020);
	SecurityContext thridLabelTest = engineTest.getSecurityContext(3030);

	EXPECT_FALSE(engineTest.checkAccess(firstLabelTest, secondLabelTest, WRITE, READ_ACCESS));
	EXPECT_FALSE(engineTest.checkAccess(secondLabelTest, firstLabelTest, WRITE, READ_ACCESS));

	EXPECT_FALSE(engineTest.checkAccess(firstLabelTest, thridLabelTest, WRITE, READ_ACCESS));
	EXPECT_FALSE(engineTest.checkAccess(thridLabelTest, firstLabelTest, WRITE, READ_ACCESS));

	EXPECT_FALSE(engineTest.checkAccess(secondLabelTest, thridLabelTest, WRITE, READ_ACCESS));
	EXPECT_FALSE(engineTest.checkAccess(thridLabelTest, secondLabelTest, WRITE, READ_ACCESS));
}

TEST(TestEngineCheckAccessWithPolitics, TestReadAccessCheckForCompaccess) {

	SimpleLabelStorage labelTest;

	labelTest.createLevel("HIGHLY_SENSITIVE", "HS", 40);
	labelTest.createLevel("SENSITIVE", "S", 30);

	labelTest.createCompartment("FINANCIAL", "FINCL", 85);
	labelTest.createCompartment("CHEMICAL", "CHEM", 65);
	labelTest.createCompartment("OPERATIONAL", "OP", 45);

	labelTest.createGroup("WESTERN_REGION", "WR", 1000);
	labelTest.createGroup("WR_SALES", "WR_SAL", 1100, 1000);
	labelTest.createGroup("WR_FINANCE", "WR_FIN", 1300, 1000);

	vector<int> compartmentsSubject = { 65, 45 };
	vector<int> groupsSubject = { 1100 };
	labelTest.createObjectLabel(40, compartmentsSubject, groupsSubject, 1010);

	vector<int> compartmentsObject = { 65 };
	vector<int> groupsObject = { 1300 };
	labelTest.createObjectLabel(30, compartmentsObject, groupsObject, 2020);

	Engine engineTest(labelTest);
	SecurityContext subjTest = engineTest.getSecurityContext(1010);
	SecurityContext objTest = engineTest.getSecurityContext(2020);

	EXPECT_TRUE(engineTest.checkAccess(subjTest, objTest, READ, COMPACCESS));
	EXPECT_FALSE(engineTest.checkAccess(objTest, subjTest, READ, COMPACCESS));

	vector<int> compartmentsNewObject = { 45 };
	vector<int> groupsNewObject = { 1300 };
	labelTest.createObjectLabel(30, compartmentsNewObject, groupsNewObject, 3030);

	SecurityContext newObjTest = engineTest.getSecurityContext(3030);

	EXPECT_TRUE(engineTest.checkAccess(subjTest, newObjTest, READ, COMPACCESS));
	EXPECT_FALSE(engineTest.checkAccess(newObjTest, objTest, READ, COMPACCESS));
	EXPECT_FALSE(engineTest.checkAccess(newObjTest, subjTest, READ, COMPACCESS));

	vector<int> compartmentsNewSubject;
	vector<int> groupsNewSubject = { 1300 };
	labelTest.createObjectLabel(30, compartmentsNewSubject, groupsNewSubject, 4040);

	SecurityContext newSubTest = engineTest.getSecurityContext(4040);

	EXPECT_TRUE(engineTest.checkAccess(subjTest, newSubTest, READ, COMPACCESS));
	EXPECT_FALSE(engineTest.checkAccess(subjTest, newSubTest, READ));

	EXPECT_TRUE(engineTest.checkAccess(newObjTest, newSubTest, READ, COMPACCESS));
	EXPECT_TRUE(engineTest.checkAccess(newObjTest, newSubTest, READ));

	EXPECT_FALSE(engineTest.checkAccess(newSubTest, newObjTest, READ, COMPACCESS));
	EXPECT_FALSE(engineTest.checkAccess(newSubTest, newObjTest, READ));
}