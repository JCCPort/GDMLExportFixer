#include <string>
#include <iostream>
#include <regex>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string/find.hpp>
#include <sys/resource.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/utility/string_ref.hpp>
#include <iomanip>

//std::string replace_all_copy(const std::string &s, const std::string &f, const std::string &r) {
//	if (s.empty() || f.empty() || f == r || f.size() > s.size()) {
//		return s;
//	}
//	std::ostringstream buffer;
//	auto start = s.cbegin();
//	while (true) {
//		const auto end = search(start, s.cend(), f.cbegin(), f.cend());
//		copy(start, end, std::ostreambuf_iterator<char>(buffer));
//		if (end == s.cend()) {
//			break;
//		}
//		copy(r.cbegin(), r.cend(), std::ostreambuf_iterator<char>(buffer));
//		start = end + f.size();
//	}
//	return buffer.str();
//}

std::string replace_all_copy(std::string const &original, std::string const &before, std::string const &after) {
	std::string retval;
	std::string::const_iterator end = original.end();
	std::string::const_iterator current = original.begin();
	std::string::const_iterator next = std::search(current, end, before.begin(), before.end());
	while (next != end) {
		retval.append(current, next);
		retval.append(after);
		current = next + before.size();
		next = std::search(current, end, before.begin(), before.end());
	}
	retval.append(current, next);
	return retval;
}

//template<typename Range>
//int expand(Range const & /*key*/) {
//	return rand() % 42; // todo lookup value with key (be sure to stay lean here)
//}
//
//
//std::string fastReplace(const std::string inputString) {
//	std::ostringstream builder;
//	builder.str().reserve(1024); // reserve ample room, not crucial since we reuse it anyways
//
//	for (size_t iterations = 1ul << 14; iterations; --iterations) {
//		builder.str("");
//		std::ostreambuf_iterator<char> out(builder);
//
//		for (auto f(inputString.begin()), l(inputString.end()); f != l;) {
//			switch (*f) {
//				case '{' : {
//					auto s = ++f;
//					size_t n = 0;
//
//					while (f != l && *f != '}')
//						++f, ++n;
//
//					// key is [s,f] now
//					builder << expand(boost::string_ref(&*s, n));
//
//					if (f != l)
//						++f; // skip '}'
//				}
//					break;
//				default:
//					*out++ = *f++;
//			}
//		}
//		// to make it slow, uncomment:
//		// std::cout << builder.str();
//	}
//	return builder.str();
//}


int main(int argc, char *argv[]) {
	if (argc < 2) {
		throw std::invalid_argument("Requires an input and output path as arguments");
	}
	std::string inputFileName = std::string(argv[1]);
	std::string outputFileName = std::string(argv[2]);
	// Opening the file
	std::ifstream t(inputFileName);
	std::stringstream buffer;
	buffer << t.rdbuf();

	// Defining regexes
	boost::regex positionsSolidPairs2("(?<=<define>)([\\s\\S]*)  <\\/define>\n  <solids>([\\s\\S]*)(?=<\\/solids>)");
	boost::regex positionsSolidPairs("(  <[^>]*>(?>\n"
	                                 "    <[^\\/]*\\/>)*\n"
	                                 "  <\\/[^>]*>\n"
	                                 "  <[^>]*>\n"
	                                 "    <[^>]*>(?>\n"
	                                 "      <[^\\/]*\\/>)*\n"
	                                 "    <\\/[^>]*>\\n  <\\/[^>]*>)");
	boost::regex startOfFile("<\\?xml[\\s\\S]+?(?=  <define>)");
	boost::regex endOfFile("\n  <structure>[\\s\\S]*");
	boost::regex positions("position name=\"(\\d+)\" x");

	std::string bufferString = buffer.str();

	// Creating the string that will be filled up and eventually written out
	std::string writeString;

	// Getting start of file text
	boost::smatch startOfFileMatch;
	boost::regex_search(bufferString, startOfFileMatch, startOfFile);
	writeString = writeString + startOfFileMatch[0].str();

	// Getting end of file text
	boost::smatch endOfFileMatch;
	boost::regex_search(bufferString, endOfFileMatch, endOfFile);

	// Loop through position solid pairs
	boost::sregex_token_iterator iter(bufferString.begin(), bufferString.end(), positionsSolidPairs, 0);
	boost::sregex_token_iterator end;

	auto numMatches = std::to_string(boost::distance(iter->begin(), iter->end()));
	int currentMatch = 0;
	int j = 0;
	for (; iter != end; ++iter) {
		currentMatch++;
		// We first need to find the positions match, then get the first group (position number), then find the vertices that match the specific name.
		//  then update the names for both

		std::string solidPositionPairCopy = iter->str();

		std::string bufferString2 = iter->str();
		boost::sregex_token_iterator iter2(bufferString2.begin(), bufferString2.end(), positions, 0);
		boost::sregex_token_iterator end2;

		while (iter2 != end2) {
			std::string posMatch = iter2->str();
			boost::smatch posNumberMatch;
			boost::regex_search(posMatch, posNumberMatch, positions);
			std::string posNumber = posNumberMatch[1].str();

			std::string vertex1RegexString = "vertex1=\"" + posNumber + "\"";
			std::string vertex2RegexString = "vertex2=\"" + posNumber + "\"";
			std::string vertex3RegexString = "vertex3=\"" + posNumber + "\"";
			std::string positionRegexString = "position name=\"" + posNumber + "\" x";
			boost::regex vertices1Regex(vertex1RegexString);
			boost::regex vertices2Regex(vertex2RegexString);
			boost::regex vertices3Regex(vertex3RegexString);
			boost::regex positionRegex(positionRegexString);

			std::string fixedPosName = "POS" + std::to_string(j);
			std::string fixedPosLine = "position name=\"" + fixedPosName + "\" x";
			std::string fixedVert1Line = "vertex1=\"" + fixedPosName + "\"";
			std::string fixedVert2Line = "vertex2=\"" + fixedPosName + "\"";
			std::string fixedVert3Line = "vertex3=\"" + fixedPosName + "\"";

			solidPositionPairCopy = replace_all_copy(solidPositionPairCopy, positionRegexString, fixedPosLine);
			solidPositionPairCopy = replace_all_copy(solidPositionPairCopy, vertex1RegexString, fixedVert1Line);
			solidPositionPairCopy = replace_all_copy(solidPositionPairCopy, vertex2RegexString, fixedVert2Line);
			solidPositionPairCopy = replace_all_copy(solidPositionPairCopy, vertex3RegexString, fixedVert3Line);

			++iter2;
			j++;
		}
		writeString.append(solidPositionPairCopy);
	}
	writeString = writeString + endOfFileMatch[0].str();

	std::string defineStart = "<define>\n    ";
	size_t pos = writeString.find(defineStart);
	if (pos != std::string::npos) {
		writeString.insert(pos + defineStart.size(), "<position name=\"center\" x=\"0.0\" y=\"0.0\" z=\"0.0\" unit=\"mm\"/>");
	}

	std::string solidsStart = "<solids>\n    ";
	size_t pos2 = writeString.find(solidsStart);
	if (pos2 != std::string::npos) {
		writeString.insert(pos2 + solidsStart.size(), "<box aunit=\"radian\" lunit=\"mm\" name=\"worldbox\" x=\"10000\" y=\"10000\" z=\"10000\" />");
	}

	std::ofstream outFile(outputFileName);
	outFile << writeString;
	outFile.close();

	return 0;
}