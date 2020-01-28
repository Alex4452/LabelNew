#ifdef LABELNEW_EXPORTS
#define LABELNEW_API __declspec(dllexport)
#else
#define LABELNEW_API __declspec(dllimport)
#endif

#include "framework.h"
#include "InternalSecurityContext.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

using namespace std;

/*Есть объект движка который реализован в Engine его задача работа с метками и правами.
Хранение меток делегируется классу LabelStorage, а точнее его потомкам, в которых например реализуется
загрузка меток. Примерами реализаций LabelStorage могут быть FileLabelStorage, который будет грузить
метки из файла(ов), SimpleLabelStorage в который можно добавить метки вручную, например для юнит тестов
и например какой нибудь DatabaseLabelStorage, который будет хранить метки в СУБД.

Работа с метками разделяется на три уровня: метка-строка Label, для представления читабельного вида
для пользователя; метка-идентификатор - LabelID, для компактного хранения и возможности сослаться на метку
например в строке таблицы; публичный и внутреннй контекс безопасности набор идентификаторов уровень+подразделения+группы
непосредственно для сравнения при checkAccess.

Внешний контекст безопасности SecurityContext - просто идентификатор, и ссылки на некоторые данные из
внутреннего контекста, внутренний - InternalSecurityContext - класс с полями. Пользователь может получить
и пользоваться из движка только внешним. В момент создания внешнего контекста создается идентификатор
контекста и внутренний контекст, который и использует Engine во время обращения по внешнему.
Это гарантирует корректность контекста, при его использовании (он или есть созданный в движке, или его нет).*/

namespace MandatoryAccessControl
{
	class SecurityContext;

	typedef int ComponentID;
	typedef int AccessVector;

	enum LABELNEW_API AccessVectorFlag {
		READ = 1,
		WRITE = 2,
		CREATE = 4,
		DROP = 8,
		//...
	};

	class LABELNEW_API LabelStorage
	{
	protected:
		struct Group;
		struct Component;
	public:
		LabelStorage() {};

		Component& getLevel(ComponentID id);
		Component& getCompartments(ComponentID id);
		Component& getGroups(ComponentID id);
		map<string, SecurityContext*>& getAllObjLabels() { return objLabels; }

		static void clearString(string& str);
		static void readString(string& temp, string input, int& startInd, int& i);

		// Considers a colon when parsing object labels
		bool checkColon(string temp, string& labelID, int& level,
			vector<int>& compartments, vector<int>& groups, int colon);

		struct Component
		{
			Component () : parentComp(NULL)
			{}
			Component (int numF, string longF, string shortF, ComponentID id, Component& parentComp) : 
				numForm(numF), longForm(longF), shortForm(shortF), idComponent(id), parentComp(&parentComp)
			{}
			int numForm;
			string longForm;
			string shortForm;
			ComponentID idComponent;

			Component* parentComp;
		};

	protected:
		map<ComponentID, Component> levels;
		map<ComponentID, Component> compartments;
		map<ComponentID, Component> groups;

		map<string, SecurityContext*> objLabels;
	};

/******************************************************************************
 *
 *	Loading labels from a file
 */

	class LABELNEW_API FileLabelStorage : public LabelStorage
	{
	public:
		FileLabelStorage(string pathLevels, string pathCompartments, string pathGroups, string pathObjectLabel);

		// Function for parsing files containing levels, compartments and groups
		void parseFile(string path, map<ComponentID, Component>& labels);
		void parseFile(string path, map<ComponentID, Group>& labels);

		// Function for parsing files containing objects labels
		void parseObjLabel(string path);
	};

/******************************************************************************
 *
 *	Manual label loading
 */

	class LABELNEW_API SimpleLabelStorage : public LabelStorage
	{
	public:
		SimpleLabelStorage() {}

		void createLevel(string full, string shortForm, int numForm);

		void createCompartment(string full, string shortForm, int numForm);

		void createGroup(string full, string shortForm, int numForm, int parent = -1);

		void createObjectLabel(string id, int level, vector<int>& compartments, vector<int>& groups);
		void createObjectLabel(string id, int level, vector<int>& compartments, vector<int>& groups, long tag);
	};

	class LABELNEW_API Engine
	{
	public:
		Engine(LabelStorage& storage) : label(storage)
		{};

		SecurityContext& getSecurityContext(string labelID);

		bool checkAccess(SecurityContext& subject, SecurityContext& object, AccessVector accessVector);
		bool compareGroups(SecurityContext& subject, SecurityContext& object);

		string getAllLabel();
		string getReadableLookLabel(string labelID);

	private:
		LabelStorage& label;
	};


	class LABELNEW_API SecurityContext
	{
	public:
		SecurityContext() {};
		SecurityContext(string id, int level, const vector<int>& compartments, const vector<int>& groups);
		SecurityContext(string id, int level, const vector<int>& compartments, const vector<int>& groups, long tag);

		~SecurityContext()
		{
			delete objLabel;
		}

		string getLabel();
		string getLabelID();
		int getLevelLabel();
		vector<int> getCompartmentsLabel();
		vector<int> getGroupsLabel();
		long getTagLabel();

	private:
		InternalMandatoryAccessControl::InternalSecurityContext* objLabel;
	};
}

