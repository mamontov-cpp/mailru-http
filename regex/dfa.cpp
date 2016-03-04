#include "dfa.h"

regex::DFA::DFA()
{
	
}

regex::MatchResult regex::DFA::match(const std::string& s)
{
	regex::MatchResult result;
	result.Result.push_back(std::string());
	bool error = false;
	int currentstate = 0;
	bool running = true;
	int currentgroupindex = 0;
	size_t index = 0;
	while(running && !error)
	{
		if (index < s.size())
		{
			char input = s[index];
			int matchededge = -1;
			const std::vector<Edge>& edges = Edges[currentstate];
			for(size_t i = 0; i < edges.size(); i++)
			{
				const Edge& edge = edges[i];
				if (((edge.C == input) && (edge.Not == false)) || ((edge.C != input) && (edge.Not == true)))
				{
					matchededge = i;
				}			
			}
			if (matchededge != -1)
			{
				const Edge& edge = edges[matchededge];
				if (edge.Begin)
				{
					currentgroupindex++;
					result.Result.push_back(std::string());
				}
				if (edge.End)
				{
					currentgroupindex--;
				}
				if (edge.Final)
				{
					running = false;
				}
				for(size_t i = 0; i <= currentgroupindex; i++)
				{	
					result.Result[i].push_back(input);
				}
				currentstate = edge.State;
			}
			else
			{
				running = false;
				error = true;
			}
			++index;
		}
		else
		{
			running = false;
		}
	}
	if (error)
	{
		result.Type = regex::MRT_ERROR;
		result.Result.clear();
	}
	else
	{
		if (currentstate != Edges.size() - 1)
		{
			result.Type = regex::MRT_PARTIAL;		
		}
		else
		{
			result.Type = regex::MRT_FULL;
		}
	}
	return result;
}