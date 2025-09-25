#pragma once

enum class Color
{
	BLACK,
	WHITE,
	RED,
	GREEN,
	BLUE,
	YELLOW,
};

class ConsoleLog
{
	enum { BUFFER_SIZE = 4096 };
	HANDLE		_stdOut;
	HANDLE		_stdErr;
protected:
	void		SetColor(bool stdOut, Color color);
public:
	ConsoleLog();
	~ConsoleLog();

	void		WriteStdOut(Color color, const WCHAR* str, ...);
	void		WriteStdErr(Color color, const WCHAR* str, ...);
};
