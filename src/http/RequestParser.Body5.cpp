#include "http/RequestParser.hpp"
#include <cctype>
#include <sstream>

bool	HttpRequestParser::parseBody()
{
	/*
		We only implement Content-Length body parsing in this step.

		At this point:
		- Headers are already parsed.
		- parseHeaders() decided there IS a body, and set _state = PS_BODY.
		- _req.hasContentLength / _req.contentLength is already set.

		We need to collect exactly _req.contentLength bytes from _buffer.
		When done, we set PS_DONE.
	*/

	/*
		If request is chunked, we are NOT implementing it yet.
		We will do chunked in Phase 3.5.TODO
	*/
	if (_req.chunked == true)
		return (parseBodyChunked());

	/*
		If there is no Content-Length information, then something is wrong:
		PS_BODY should only happen when we know how much to read.
	*/
	if (_req.hasContentLength == false)
		return (setError(400));

	//	If Content-Length is 0, body is empty and we are done immediately.

	if (_req.contentLength == 0)
	{
		_state = PS_DONE;
		return (true);
	}

	/*
		We may not have all body bytes yet.
		We only take what's available in _buffer and wait for more if needed.
	*/
	{
		size_t	remaining;
		size_t	canTake;

		remaining = _req.contentLength - _req.body.size();
		if (_buffer.size() == 0)
			return (false);
		if (_buffer.size() < remaining)
			canTake = _buffer.size();
		else
			canTake = remaining;

		//	Append body bytes to _req.body.
		_req.body.append(_buffer, 0, canTake);

		//	Remove consumed bytes from _buffer.
		_buffer.erase(0, canTake);
	}
	//	If we collected the full body, we're done.

	if (_req.body.size() == _req.contentLength)
	{
		_state = PS_DONE;
		return (true);
	}
	//	Otherwise, we need more body bytes from feed().
	return (false);
}


/*
	Chunked body format (simplified):

		<hex_size>\r\n
		<data bytes...>\r\n
		<hex_size>\r\n
		<data bytes...>\r\n
		0\r\n
		\r\n

	We must decode the chunks and append the data into _req.body.
*/

/*
	parseChunkSizeLine()

	Reads and parses the next chunk size line.
	Returns:
		- false : need more data
		- true  : progressed (chunkSizeKnown becomes true)
	Also sets error state on malformed input.
*/
bool	HttpRequestParser::parseChunkSizeLine()
{
	std::string			line;
	bool				hasLine;
	std::istringstream	iss;
	size_t				sizeValue;

	hasLine = false;
	line = readLine(hasLine);

	//	If we don't have a full line yet, wait for more data.
	if (hasLine == false)
		return (false);

	/*
		Chunk size is hex (base 16). Example: "4" or "1A".
		There may be optional chunk extensions like: "4;ext=1"
		We ignore extensions by cutting at ';' if present.
	*/
	{
		std::string::size_type	pos;

		pos = line.find(';');
		if (pos != std::string::npos)
			line = line.substr(0, pos);
	}

	line = trim(line);
	if (line.empty() == true)
		return (setError(400));

	/*
		Parse hex number.
		std::istringstream + std::hex lets us read hex into an integer.
	*/
	sizeValue = 0;
	iss.clear();
	iss.str(line);
	iss >> std::hex >> sizeValue;

	if (iss.fail())
		return (setError(400));

	_req.chunkSize = sizeValue;
	_req.chunkSizeKnown = true;

	return (true);
}

/*
	consumeFinalChunkCRLF()

	After a 0-size chunk, we require the final "\r\n".
	Returns:
		- false : need more data
		- true  : consumed CRLF and marked done
*/
bool	HttpRequestParser::consumeFinalChunkCRLF()
{
	if (_buffer.size() < 2)
		return (false);
	if (_buffer.substr(0, 2) != "\r\n")
		return (setError(400));
	_buffer.erase(0, 2);

	_state = PS_DONE;
	return (true);
}

/*
	consumeChunkDataAndCRLF()

	Consumes:
		- _req.chunkSize bytes of data
		- then the trailing "\r\n"
	Returns:
		- false : need more data
		- true  : chunk consumed successfully
*/
bool	HttpRequestParser::consumeChunkDataAndCRLF()
{
	//STEP B: We know the chunk size. We must read exactly that many bytes from _buffer.

	//Not enough data yet to read the full chunk.
	if (_buffer.size() < _req.chunkSize)
		return (false);

	//	Append chunk data to the decoded body.
	_req.body.append(_buffer, 0, _req.chunkSize);
	_buffer.erase(0, _req.chunkSize);

	//	STEP C: After chunk data, the protocol requires "\r\n".
	if (_buffer.size() < 2)
		return (false);
	if (_buffer.substr(0, 2) != "\r\n")
		return (setError(400));
	_buffer.erase(0, 2);

	//	Chunk finished. Next chunk size line must be parsed.
	_req.chunkSize = 0;
	_req.chunkSizeKnown = false;

	return (true);
}

/*
	parseBodyChunked()

	Main loop:
	- read size line (if needed)
	- if size is 0 => consume final CRLF and finish
	- else consume chunk bytes + CRLF
*/
bool	HttpRequestParser::parseBodyChunked()
{
	while (true)
	{
		/*
			STEP A: If we don't know the next chunk size, we must parse the size line.
			The size line ends with "\r\n".
		*/
		if (_req.chunkSizeKnown == false)
		{
			if (parseChunkSizeLine() == false)
				return (false);

			/*
				If chunk size is 0, that means: end of body.
				After 0\r\n there is a final "\r\n" (and optional trailer headers).
				For this project step, we just require the final "\r\n".
			*/
			if (_req.chunkSize == 0)
				return (consumeFinalChunkCRLF());
		}

		if (consumeChunkDataAndCRLF() == false)
			return (false);

		//	Loop again: parse next size line, read next chunk, etc.
	}
}

void	HttpRequestParser::resetKeepBuffer()
{
	/*
		Prepare to parse a NEW request,
		but DO NOT clear _buffer.

		This is important when:
		- we received 2 requests in the same socket read
		- or we received extra bytes after finishing the body
	*/
	_state = PS_START_LINE;
	_req = HttpRequest();
	_errorStatus = 0;
}
