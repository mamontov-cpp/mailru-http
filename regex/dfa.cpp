#include "dfa.h"

regex::DFA::DFA()
{
	
}

regex::MatchResult regex::DFA::match(const std::vector<char>& s)
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
                    if (edge.Not2)
                    {
                        if (edge.C2 != input)
                        {
                            matchededge = i;
                        }
                    }
                    else
                    {
					    matchededge = i;
                    }
				}			
			}
			if (matchededge != -1)
			{
				const Edge& edge = edges[matchededge];				
				if (edge.End)
				{
					currentgroupindex--;
				}
				if (edge.Final)
				{
					running = false;
				}
				for(int i = 0; i <= currentgroupindex; i++)
				{	
					result.Result[i].push_back(input);
				}
				if (edge.Begin)
				{
					currentgroupindex++;
					result.Result.push_back(std::string());
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
        result.Result.push_back(std::string());
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

regex::DFA& regex::DFA::newState()
{
	Edges.push_back(std::vector<regex::Edge>());
	return *this;
}

regex::Edge& regex::DFA::nextStateOn()
{
	std::vector<regex::Edge>& edges = Edges[Edges.size() - 1];
	edges.push_back(regex::Edge());
	regex::Edge& e = edges[edges.size() - 1];
	e.State = Edges.size();
	return e;
}


regex::Edge& regex::DFA::loopOn()
{
	std::vector<regex::Edge>& edges = Edges[Edges.size() - 1];
	edges.push_back(regex::Edge());
	regex::Edge& e = edges[edges.size() - 1];
	e.State = Edges.size() - 1;
	return e;
}

regex::Edge& regex::DFA::stringShouldContain(const std::string& s)
{
    regex::Edge* result = NULL;
    for(size_t i = 0; i < s.size(); i++)
    {
        result = &(this->newState().nextStateOn().match(s[i]));
    }
    return *result;
}

void regex::DFA::stringMustEndWidth(char c)
{
    this->newState().nextStateOn().match(c).Final = true;
    this->newState();
}

regex::Edge& regex::DFA::zeroOrInfinityNot(char c)
{
    this->newState();

    std::vector<regex::Edge>& edges = Edges[Edges.size() - 1];
    edges.push_back(regex::Edge());
    edges[0].State = Edges.size() - 1;
    edges[0].notMatch(c);

    edges.push_back(regex::Edge());
    edges[1].State = Edges.size();
    edges[1].match(c);
    
    return edges[1];
}

regex::Edge& regex::DFA::zeroOrInfinityNot(char c, char c2)
{
    this->newState();

    std::vector<regex::Edge>& edges = Edges[Edges.size() - 1];
    edges.push_back(regex::Edge());
    edges[0].State = Edges.size() - 1;
    edges[0].notMatch(c);
    edges[0].Not2 = true;
    edges[0].C2 = c2;

    edges.push_back(regex::Edge());
    edges[1].State = Edges.size();
    edges[1].match(c);
    
    return edges[1];
}
