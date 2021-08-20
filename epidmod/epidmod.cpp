﻿// epidmod.cpp : Defines the entry point for the application.
//

#include "epidmod.h"



struct PERSON
{
	unsigned int state = 0;							// 0 - healthy, 1 - exposed, 2 - infectious, 3 - removed
	bool isUpdate = false;
	float chance = 0;		// chance to go to next stage
	std::vector<int> connects;
};

int main()
{

	// reading data from json

	unsigned int PEOPLE_AMOUNT = 10;
	unsigned int CONNECTIONS_AMOUNT = 0;
	unsigned int RAND_CONNECTIONS_AMOUNT = 0;

	float INFECT_PART = 0.0f;
	float EXPOSED_PART = 0.0f;

	float WSG_K = 0.0f;

	unsigned int TIME = 50;

	float CATCH_INFECT = 0.0f;

	float WEIBULL_EXPOSED_A = 0.0f;
	float WEIBULL_EXPOSED_B = 0.0f;
	float WEIBULL_INFECT_A = 0.0f;
	float WEIBULL_INFECT_B = 0.0f;


	int TICK = 17;

	nlohmann::json out;

	try {
		nlohmann::json j;
		std::cin >> j;

		if (j["PEOPLE_AMOUNT"] != nullptr) { PEOPLE_AMOUNT = j["PEOPLE_AMOUNT"].get<unsigned int>(); }
		else { out["error"] = "wrong PEOPLE_AMOUNT value"; std::cout << out << std::endl; return 1; }

		if (j["CONNECTIONS_AMOUNT"] != nullptr) { CONNECTIONS_AMOUNT = j["CONNECTIONS_AMOUNT"].get<unsigned int>(); }
		else { out["error"] = "wrong CONNECTIONS_AMOUNT value"; std::cout << out << std::endl; return 1; }

		if (j["RAND_CONNECTIONS_AMOUNT"] != nullptr) { RAND_CONNECTIONS_AMOUNT = j["RAND_CONNECTIONS_AMOUNT"].get<unsigned int>(); }
		else { out["error"] = "wrong RAND_CONNECTIONS_AMOUNT value"; std::cout << out << std::endl; return 1; }

		if (j["INFECT_PART"] != nullptr) { INFECT_PART = j["INFECT_PART"].get<float>(); }
		else { out["error"] = "wrong INFECT_PART value"; std::cout << out << std::endl; return 1; }

		if (j["EXPOSED_PART"] != nullptr) { EXPOSED_PART = j["EXPOSED_PART"].get<float>(); }
		else { out["error"] = "wrong EXPOSED_PART value"; std::cout << out << std::endl; return 1; }

		if (j["WSG_K"] != nullptr) { WSG_K = j["WSG_K"].get<float>(); }
		else { out["error"] = "wrong WSG_K value"; std::cout << out << std::endl; return 1; }

		if (j["TIME"] != nullptr) { TIME = j["TIME"].get<unsigned int>(); }
		else { out["error"] = "wrong TIME value"; std::cout << out << std::endl; return 1; }

		if (j["CATCH_INFECT"] != nullptr) { CATCH_INFECT = j["CATCH_INFECT"].get<float>(); }
		else { out["error"] = "wrong CATCH_INFECT value"; std::cout << out << std::endl; return 1; }

		if (j["WEIBULL_EXPOSED_TO_INFECT_A"] != nullptr) { WEIBULL_EXPOSED_A = j["WEIBULL_EXPOSED_TO_INFECT_A"].get<float>(); }
		else { out["error"] = "wrong WEIBULL_EXPOSED_TO_INFECT_A value"; std::cout << out << std::endl; return 1; }

		if (j["WEIBULL_EXPOSED_TO_INFECT_B"] != nullptr) { WEIBULL_EXPOSED_B = j["WEIBULL_EXPOSED_TO_INFECT_B"].get<float>(); }
		else { out["error"] = "wrong WEIBULL_EXPOSED_TO_INFECT_B value"; std::cout << out << std::endl; return 1; }

		if (j["WEIBULL_INFECT_TO_REMOVED_A"] != nullptr) { WEIBULL_INFECT_A = j["WEIBULL_INFECT_TO_REMOVED_A"].get<float>(); }
		else { out["error"] = "wrong WEIBULL_INFECT_TO_REMOVED_A value"; std::cout << out << std::endl; return 1; }

		if (j["WEIBULL_INFECT_TO_REMOVED_B"] != nullptr) { WEIBULL_INFECT_B = j["WEIBULL_INFECT_TO_REMOVED_B"].get<float>(); }
		else { out["error"] = "wrong WEIBULL_INFECT_TO_REMOVED_B value"; std::cout << out << std::endl; return 1; }

		if (j["PRINT_TICK"] != nullptr) { TICK = j["PRINT_TICK"].get<int>(); }
		else { out["error"] = "wrong PRINT_TICK value"; std::cout << out << std::endl; return 1; }

	}
	catch (...) {
		out["error"] = "wrong input data";
		std::cout << out << std::endl;
		return 1;
	}



	// end reading data

	int CONN_ONE_SIDE = CONNECTIONS_AMOUNT / 2;



	std::mt19937 gen(time(0));
	std::uniform_real_distribution<float> randFloat(0, 1);
	std::uniform_int_distribution<int> randPeople(0, PEOPLE_AMOUNT - 1);
	std::weibull_distribution<float> randInf(WEIBULL_EXPOSED_A, WEIBULL_EXPOSED_B);
	std::weibull_distribution<float> randRemoved(WEIBULL_INFECT_A, WEIBULL_INFECT_B);

	int exp_amount = static_cast<int>(PEOPLE_AMOUNT * EXPOSED_PART);
	int inf_amount = static_cast<int>(PEOPLE_AMOUNT * INFECT_PART);

	std::vector<PERSON> people = std::vector<PERSON>();

	time_t begin = time(0);

	for (size_t i = 0; i < PEOPLE_AMOUNT; i++)	// creating not randomized graph by connecting N neighbours
	{
		std::vector<int> numbers = std::vector<int>();
		for (int j = -CONN_ONE_SIDE; j <= CONN_ONE_SIDE; j++)
		{
			if (j != 0) {
				int k = i + j;
				if (k < 0) { k += PEOPLE_AMOUNT; }
				k %= PEOPLE_AMOUNT;
				numbers.push_back(k);
			}
		}
		PERSON pers;
		pers.connects = numbers;
		pers.chance = randFloat(gen);
		if (i < exp_amount + inf_amount) {	// exposed
			pers.state++;
			pers.chance = randInf(gen);
		}
		if (i < inf_amount) {				// infectious
			pers.state++;
			pers.chance = randRemoved(gen);
		}
		people.push_back(pers);
	}

	for (size_t i = 0; i < PEOPLE_AMOUNT; i++)	// rerroll connections by wats-strogatz algorythm
	{
		for (size_t ind = 0; ind < people[i].connects.size(); ind++)
		{
			if (randFloat(gen) < WSG_K) {	// if edge need to be replaced
				int new_adr;
				do { new_adr = randPeople(gen); } while (new_adr == i);		// new edge without connecting vertex to itself
				if (std::find(people[new_adr].connects.begin(), people[new_adr].connects.end(), (int)i) == people[new_adr].connects.end()) {		// if edge not exist
					people[new_adr].connects.push_back(i);		// adding edge to 'new' vertex
					people[people[i].connects[ind]].connects.erase(std::find(people[people[i].connects[ind]].connects.begin(), people[people[i].connects[ind]].connects.end(), (int)i));		// remove edge from 'old' vertex
					people[i].connects[ind] = new_adr;	// change edge to initial vertex
				}
			}
		}
	}

	int s0 = 0;		// cout result
	int s1 = 0;
	int s2 = 0;// 0 - healthy, 1 - exposed, 2 - infectious, 3 - removed
	int s3 = 0;
	for (size_t i = 0; i < PEOPLE_AMOUNT; i++)
	{
		if (people[i].state == 0) { s0++; }
		if (people[i].state == 1) { s1++; }
		if (people[i].state == 2) { s2++; }
		if (people[i].state == 3) { s3++; }
	}
	out["results"] = nlohmann::json::array();
	out["results"].push_back({
				{"time", 0},
				{"healthy", s0},
				{"exposed", s1},
				{"infectious", s2},
				{"removed", s3}
		});

	for (size_t t = 1; t <= TIME; t++)	// update states many times
	{

		for (size_t i = 0; i < PEOPLE_AMOUNT; i++)	// count chance of updating states
		{

			std::vector<int> connects = people[i].connects;		// add random edges
			for (size_t idx = 0; idx < RAND_CONNECTIONS_AMOUNT; idx++)
			{
				int conn = randPeople(gen);
				if (std::find(connects.begin(), connects.end(), conn) == connects.end()) { connects.push_back(conn); }
			}



			if (people[i].state == 0) {		// calculating chances
				float ch1 = 1.0f;
				for (size_t j = 0; j < connects.size(); j++)
				{
					if (people[connects[j]].state == 2) { ch1 *= 1 - CATCH_INFECT; }
				}
				if ((1 - ch1) > people[i].chance) { people[i].isUpdate = true; }
			}
			if (people[i].state == 1 || people[i].state == 2) {
				people[i].chance--;
				if (people[i].chance < 0) { people[i].isUpdate = true; }
			}
		}


		for (size_t i = 0; i < PEOPLE_AMOUNT; i++)		// updating
		{
			if (people[i].isUpdate) {
				if (people[i].state == 0) { people[i].chance = randInf(gen); }
				if (people[i].state == 1) { people[i].chance = randRemoved(gen); }
				people[i].state++;
				people[i].isUpdate = false;
			}
			if (people[i].state == 0) { people[i].chance = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }
		}


		if (((TICK != 0) && (t % TICK == 0)) || t == TIME) {
			s0 = 0;		// cout result
			s1 = 0;
			s2 = 0;// 0 - healthy, 1 - exposed, 2 - infectious, 3 - removed
			s3 = 0;
			for (size_t i = 0; i < PEOPLE_AMOUNT; i++)
			{
				if (people[i].state == 0) { s0++; }
				if (people[i].state == 1) { s1++; }
				if (people[i].state == 2) { s2++; }
				if (people[i].state == 3) { s3++; }
			}
			out["results"].push_back({
					{"time", t},
					{"healthy", s0},
					{"exposed", s1},
					{"infectious", s2},
					{"removed", s3}
				});
		}

	}

	time_t end = time(0);
	out["duration"] = end - begin;

	std::cout << out.dump(4) << std::endl;


	return(0);
}
