#pragma once
#ifndef STU_CONSOLE_INCLUDED
#define STU_CONSOLE_INCLUDED
/*
Author: Stuart Milne
Date: February 2013
Wrapper for windows console output providing manipulation functions for placing the cursor,
printing text, adjusting text and background colours.
Will open a console window and redirect stdin/out/err to it in windows gui applications
Implements singleton pattern: Explicit call to create required before accessing
and explicit call to Destroy to clean up.
Example usage: stu::Console::Get().Print(10, 10, "A string");
*/

#include <string>
#include <exception>
#include <windows.h>

namespace stu
{
	class Console
	{
	public:
		static Console &Get()
		{
			if (mInstance) { return (*mInstance);}
			else throw(std::exception("Console accessed before creation."));
		}

		static void Create(const int width, const int height, const int bg_colour = 0);
		static void Destroy();

		// Print text to console, can specify where in console to print
		void Print(const std::string& text);
		void Print(const int x, const int y, const std::string& text);
		// Move cursor relatively and absolutely
		void MoveCursorTo(const int x, const int y);
		void MoveCursorBy(const int x_inc, const int y_inc, bool wrap = false);
		// Clear screen or a single line
		void Clear(const int bg_colour = -1);
		void ClearLine(const int line_no, const std::string& clear_char = "");
		// Set colour for future print/clear commands
		void SetTextColour(const int colour);
		void SetBackgroundColour(const int colour);
		// Switch colour of buffer text
		void SwitchTextColour(const int colour);
		void SwitchBackgroundColour(const int colour);

		// Colour constants for SetColour methods
		static const int BLACK = 0;
		static const int DARK_BLUE = 1;
		static const int DARK_GREEN = 2;
		static const int DARK_CYAN = 3;
		static const int DARK_RED = 4;
		static const int DARK_PURPLE = 5;
		static const int DARK_YELLOW = 6;
		static const int DARK_WHITE = 7;
		static const int GREY = 8;
		static const int BLUE = 9;
		static const int GREEN = 10;
		static const int CYAN = 11;
		static const int RED = 12;
		static const int PURPLE = 13;
		static const int YELLOW = 14;
		static const int WHITE = 15;

	private:
		Console(const int width, const int height, const int bg_colour);
		Console(const  Console&);	// No copy constructor
		~Console();

		static Console*					mInstance;		// Instance handle for singleton object	
		HANDLE							mConsoleHandle;
		CONSOLE_SCREEN_BUFFER_INFO		mConsoleInfo;
		CONSOLE_CURSOR_INFO				mCursor;

		void RedirectStandardIO();	// Redirect stdin/out/err to this console window

		short mForeGroundColour;
		short mBackgroundColour;
	};
}

#endif // STU_CONSOLE_INCLUDED