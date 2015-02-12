
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <list>
#include <map>

#include "game.h"
#include "player.h"

namespace std {
	class bidHint {
	public:
		int probability;
		int action;
		string value;
	};
	
	class EA {
	public:
		const static int bidBID = 1; // used for bidExecute
		const static int bidEA = 2; // used for bidExecute
		const static int SETTING_IMMEDIATE = 1;
		const static int SETTING_LOOKUP = 2;
		const static int PL_LEAD = 1;
		const static int PL_RESPONSE = 2;
		const static int PL_A_BLOCK = 3;
		const static int PL_B_BLOCK = 4;
	private:
		Game *theGame;
		Player *me;
		class rule {
			public:
			class settingItem {
			public:
				string value;
				int type;
			};
			class ruleLine {
			public:
				static const int SET=1;
				static const int LT=2;
				static const int GT=3;
				static const int NOT=4;
				int iOperator;
				string valueToCheck;
				string currentValue;
				int type;
				bool checkRuleMatch(int variable, int iOperator, int value);
			};
			map<string, settingItem> ruleSettings;
			list<ruleLine> ruleLines;
			//list<executeLine> executeLines;
			// Execute parameters
            bidHint execute;
		};

		char eaRuleFile[FILENAME_MAX];
		char eaBidRuleFile[FILENAME_MAX];

		map<std::string, string> settings;
		list<rule> rules;
		ifstream bidrulesfile;

		class oneHand {
			float eaNB;
			float eavalues[9];
			float probability[9];
			float eaNBs[4];

		public:
			void set_eaNB(float value) { eaNB = value; }
			float get_eaNB() { return eaNB; };
			float *get_eavalues() { return eavalues; };
			float get_eavalue(int bid);
			float *get_probabilities() { return probability; };
			float *get_eaNBall() { return eaNBs; };
			void set_eaValues(float v1,float v2,float v3,float v4,float v5,float v6,float v7,float v8,float v9) { 
				eavalues[0] = v1; 
				eavalues[1] = v2; 
				eavalues[2] = v3; 
				eavalues[3] = v4; 
				eavalues[4] = v5; 
				eavalues[5] = v6; 
				eavalues[6] = v7; 
				eavalues[7] = v8; 
				eavalues[8] = v9; 
			}
			void set_probability(float v1,float v2,float v3,float v4,float v5,float v6,float v7,float v8,float v9) { 
				probability[0] = v1; 
				probability[1] = v2; 
				probability[2] = v3; 
				probability[3] = v4; 
				probability[4] = v5; 
				probability[5] = v6; 
				probability[6] = v7; 
				probability[7] = v8; 
				probability[8] = v9; 
			}
			void set_eaNBs(float v1,float v2,float v3,float v4) { 
				eaNBs[0] = v1; 
				eaNBs[1] = v2; 
				eaNBs[2] = v3; 
				eaNBs[3] = v4; 
			}
		};

		class cardcount{
		public:
			int numOfCards;
			int nonImportant;
			std::map<std::string, oneHand> cardCombos;
			std::map<std::string, oneHand>::iterator cardCombosIterator;
		};

		std::list<cardcount> cardcounts;
		bool readRuleFile(std::string str);	
		bool readBidRuleFile(string filename);
		float getEA(int cardsCount, int nonImportant, string cardsConfig, int bid);
		oneHand *getEAstruct(int cardsCount, int nonImportant, string cardsConfig);
		void ruleProcessing(ifstream& infile);


	public:
		EA(void);
		EA(string rulefile);
		EA(string rulefile, string bidRuleFile);
		EA(Game *game, Player *player, string rulefile, string bidRuleFile);
		bidHint getBidHint();
	};
}
