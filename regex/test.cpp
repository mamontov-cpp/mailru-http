#include <cstdio>
#include "dfa.h"

int main(int argc, char** argv)
{
	regex::DFA dfa;
	dfa.newState().nextStateOn().match('G');	
	dfa.newState().nextStateOn().match('E');
	dfa.newState().nextStateOn().match('T');
	dfa.newState().nextStateOn().match(' ').startCaptureGroup();
	
	dfa.Edges.push_back(std::vector<regex::Edge>());
	dfa.Edges[4].push_back(regex::Edge(4, false, false, true, '\r'));
	dfa.Edges[4].push_back(regex::Edge(5, false, true, false, '\r'));

	dfa.Edges.push_back(std::vector<regex::Edge>());
	dfa.Edges[5].push_back(regex::Edge(6, false, false, false, '\n'));	
	dfa.Edges[5][0].Final = true;

	dfa.Edges.push_back(std::vector<regex::Edge>());
	
	FILE* f = fopen("1.txt", "rb");
	std::string result;
	while(!feof(f))
	{
		result.push_back(fgetc(f));
	}
	fclose(f);
	regex::MatchResult r = dfa.match(result);
	
	printf("Result: %d\n", r.Type);
	for(size_t i = 0;  i < r.Result.size();i++)
	{
		printf("Captured: %s\n", r.Result[i].c_str());
	}
	return 0;
}
