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
	typedef int LabelID;
	typedef string Label;

	enum LABELNEW_API AccessVectorFlag {
		READ = 1,
		WRITE = 2,
		CREATE = 4,
		DROP = 8,
		//...
	};

	enum LABELNEW_API SecurityPolicy {
		NO_POLICY = 0,
		FULL_ACCESS = 1,
		READ_ACCESS = 2,
		COMPACCESS = 3,
		//...
	};

	/******************************************************************************
	 *
	 *	Used for processing and storing component labels and object labels
	 */

	class LABELNEW_API LabelStorage
	{
	protected:
		struct Group;
		struct Component;
	public:
		LabelStorage() {};

		// Used to get label components and object labels
		Component& getLevel(ComponentID id);
		Component& getCompartments(ComponentID id);
		Component& getGroups(ComponentID id);
		map<LabelID, SecurityContext*>& getAllObjLabels() { return objLabels; }

		// Used for clearing strings from spaces and extra characters
		static void clearString(string& str);
		// Used to trim a string at given indices
		static void readString(string& temp, string input, int& startInd, int& i);

		// Considers a colon when parsing object labels
		bool checkColon(string temp, LabelID& labelID, int& level,
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

			// Used for storing parent group
			Component* parentComp;
		};

	protected:
		// Component labels
		map<ComponentID, Component> levels;
		map<ComponentID, Component> compartments;
		map<ComponentID, Component> groups;

		// Object labels
		map<LabelID, SecurityContext*> objLabels;
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

		// Used for setting components label
		void createLevel(string full, string shortForm, int numForm);
		void createCompartment(string full, string shortForm, int numForm);
		void createGroup(string full, string shortForm, int numForm, ComponentID parent = -1);

		// Used for setting objects label
		void createObjectLabel(LabelID id, ComponentID level, vector<ComponentID>& compartments, vector<ComponentID>& groups);
	};

	/******************************************************************************
	 *
	 *	Handles object labels, works with access rights
	 */

	class LABELNEW_API Engine
	{
	public:
		Engine(LabelStorage& storage) : label(storage)
		{};

		// Used for getting objects label
		SecurityContext& getSecurityContext(LabelID labelID);

		// Used to verify access of one label to another
		bool checkAccess(SecurityContext& subject, SecurityContext& object, 
			AccessVector accessVector, SecurityPolicy policy = NO_POLICY);
		bool compareLevel(SecurityContext& subject, SecurityContext& object, AccessVector accessVector);
		bool compareCompartments(SecurityContext& subject, SecurityContext& object);
		bool compareGroups(SecurityContext& subject, SecurityContext& object);

		Label getAllLabel();
		Label getLabel(LabelID labelID);
		LabelID getLabelID(Label labelRead);

	private:
		LabelStorage& label;
	};


	class LABELNEW_API SecurityContext
	{
	public:
		SecurityContext() {};
		SecurityContext(LabelID id, ComponentID level, const vector<ComponentID>& compartments, const vector<ComponentID>& groups);

		~SecurityContext()
		{
			delete objLabel;
		}

		Label getLabel();
		LabelID getLabelID();
		ComponentID getLevelLabel();
		vector<ComponentID> getCompartmentsLabel();
		vector<ComponentID> getGroupsLabel();

	private:
		InternalMandatoryAccessControl::InternalSecurityContext* objLabel;
	};
}

