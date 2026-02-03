#include "configParser/Tokeniser.hpp"

Tokeniser::Tokeniser(std::ifstream& fileName) : cfgFile(fileName) {}

/**
 * @brief Detects special characters `;` `{` and `}`
 * @return The respective `TokenType`
 */
TokenType	Tokeniser::identifyDelimiter(const char& delimiter)
{
	if (delimiter == '{')
		return (TOKEN_LBRACE);
	if (delimiter == '}')
		return (TOKEN_RBRACE);
	if (delimiter == ';')
		return (TOKEN_SEMICOLON);

	//here for safety purposes in case a wrong char is sent
	std::cout << "[DEBUG]: identifyDelimiter() expected delimiter but got: "
		<< delimiter << std::endl;
	return (TOKEN_EOF);
}


/**
 * @brief Creates a Token from the current contents of `buf`, adds it to `tokens`
 * and clears `buf`
 * @param tokens vector containing all tokens
 */
void	Tokeniser::pushAndFlush(std::vector<Token>& tokens, TokenType t,
				std::string& buf, size_t line, size_t col)
{
	tokens.push_back(Token(t, buf, line, col));
	buf.clear();
}


/**
 * @brief reads file line-by-line, filling up `buf` character by character until
 * 1) it encounters a special character 2) it encounters a whitespace.
 * At which point it creates a token from the current contents of `buf`, clears it
 * and then adds the token to `tokens`. If it's a special character it also adds a
 * special character token before continuing.
 */
void	Tokeniser::tokenise(std::vector<Token>& tokens)
{
	std::string currentLine;
	size_t lineNum = 0;

	while (std::getline(cfgFile, currentLine))
	{
		lineNum++;
		std::string	tokenBuf;
		size_t		tokenStartCol = 0;

		for (size_t col = 0; col < currentLine.length(); ++col)
		{
			char c = currentLine[col];
			// Check if comment start
			if (c == '#')
				break; // ignore rest of line

			// Check if special character
			if (c == '{' || c == '}' || c == ';')
			{
				if (!tokenBuf.empty())
					pushAndFlush(tokens, TOKEN_WORD, tokenBuf, lineNum, tokenStartCol);
				TokenType	type = identifyDelimiter(c);
				tokens.push_back(Token(type, std::string(1, c), lineNum, col));
			}

			// Check if whitespace
			else if (std::isspace(c))
			{
				if (!tokenBuf.empty())
					pushAndFlush(tokens, TOKEN_WORD, tokenBuf, lineNum, tokenStartCol);
			}

			// regular character - add to buffer
			else
			{
				if (tokenBuf.empty()) // new word - set tokenStartCol to (curent)col
					tokenStartCol = col;
				tokenBuf += c;
			}
		}
		// End of line - flush buffer if not empty
		if (!tokenBuf.empty())
			pushAndFlush(tokens, TOKEN_WORD, tokenBuf, lineNum, tokenStartCol);
	}
	if (tokens.empty())
		throw (std::runtime_error("Config file is Empty!"));
}

//utility function to check proper token creation
void	printTokens(const std::vector<Token>& tokens)
{
	for (size_t i=0; i < tokens.size(); ++i)
		std::cout << "[" << tokens[i].value << "]" << std::setw(25 - tokens[i].value.size()) << "type: " << tokens[i].type << std::endl;
}
