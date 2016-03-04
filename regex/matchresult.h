/**
 * Define a matching result for matching items
 */
#pragma once
#include <vector>
#include <string>

namespace regex
{
/** A type of match
 */
enum MatchResultType
{
	MRT_ERROR = 0, //!< Input string, given for executing expression on does not comply to regex requirements
	MRT_PARTIAL = 1, //!< String matches partially
	MRT_FULL = 2  //!< String matches fully
};

/** A result of match
 */
struct MatchResult
{
	regex::MatchResultType Type;
	std::vector<std::string> Result; //!< Result with captured groups
	
	// Construct default result
	inline MatchResult() : Type(regex::MRT_ERROR)
	{
		
	}
};

}	