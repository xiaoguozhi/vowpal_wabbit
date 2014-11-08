// vw_explore.cpp : Defines the entry point for the console application.
//

#include "MWTExplorer.h"
#include <chrono>
#include <tuple>

using namespace std;
using namespace std::chrono;
using namespace MultiWorldTesting;

class MyContext
{

};

class MyPolicy : public IPolicy<MyContext>
{
public:
	u32 Choose_Action(MyContext& context)
	{
		return (u32)1;
	}
};

class MyScorer : public IScorer<MyContext>
{
public:
	MyScorer(u32 num_actions) : m_num_actions(num_actions)
	{
	
	}
	vector<float> Score_Actions(MyContext& context)
	{
		vector<float> scores;
		for (size_t i = 0; i < m_num_actions; i++)
		{
			scores.push_back(.1f);
		}
		return scores;
	}
private:
	u32 m_num_actions;
};

class MyRecorder : public IRecorder<MyContext>
{
public:
	virtual void Record(MyContext& context, u32 action, float probability, string unique_key)
	{

	}
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cerr << "arguments: {greedy,tau-first,bagging,softmax,generic} [stateful]" << endl;
		exit(1);
	}
	
	bool stateful = false;
	if (argc == 3) 
	{
		if (strcmp(argv[2], "stateful") == 0)
		{
			stateful = true;
		}
		else
		{
			cerr << "unknown policy type: " << argv[2] << endl;
			exit(1);
		}
	}
	      
	//arguments for individual explorers
	if (strcmp(argv[1], "greedy") == 0)
	{
		int num_actions = 10;
		float epsilon = .2f;
		string unique_key = "sample";

		//Initialize Epsilon-Greedy explore algorithm using MyPolicy
		MWT<MyRecorder> mwt("salt", MyRecorder());
		EpsilonGreedyExplorer<MyPolicy> explorer(MyPolicy(), epsilon, num_actions);
		u32 action = mwt.Choose_Action(explorer, unique_key, MyContext());
	}
	else if (strcmp(argv[1], "tau-first") == 0)
	{
		int num_actions = 10;
		u32 tau = 5;
		string unique_key = "sample";

		//Initialize Tau-First explore algorithm using MyPolicy
		MWT<MyRecorder> mwt("salt", MyRecorder());
		TauFirstExplorer<MyPolicy> explorer(MyPolicy(), tau, num_actions);
		u32 action = mwt.Choose_Action(explorer, unique_key, MyContext());
	}
	else if (strcmp(argv[1], "bagging") == 0)
	{
		int num_actions = 10;
		u32 num_bags = 2;
		string unique_key = "sample";

		//Initialize Bagging explore algorithm using MyPolicy
		MWT<MyRecorder> mwt("salt", MyRecorder());
		vector<MyPolicy> policy_functions;
		for (size_t i = 0; i < num_bags; i++)
		{
			policy_functions.push_back(MyPolicy());
		}
		BaggingExplorer<MyPolicy> explorer(policy_functions, num_bags, num_actions);
		u32 action = mwt.Choose_Action(explorer, unique_key, MyContext());
	}
	else if (strcmp(argv[1], "softmax") == 0)
	{
		int num_actions = 10;
		float lambda = 0.5f;
		string unique_key = "sample";

		//Initialize Softmax explore algorithm using MyScorer 
		MWT<MyRecorder> mwt("salt", MyRecorder());
		SoftmaxExplorer<MyScorer> explorer(MyScorer(num_actions), lambda, num_actions);
		u32 action = mwt.Choose_Action(explorer, unique_key, MyContext());
	}
	else if (strcmp(argv[1], "generic") == 0)
	{
		int num_actions = 10;
		string unique_key = "sample";

		//Initialize Generic explore algorithm using MyScorer 
		MWT<MyRecorder> mwt("salt", MyRecorder());
		GenericExplorer<MyScorer> explorer(MyScorer(num_actions), num_actions);
		u32 action = mwt.Choose_Action(explorer, unique_key, MyContext());
	}
	else
	{
		cerr << "unknown exploration type: " << argv[1] << endl;
		exit(1);
	}

	return 0;
}
