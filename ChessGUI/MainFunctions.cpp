
// Putting the fun in functions

#include "Main.h"

COLORREF boardColor1 = RGB(224, 224, 224); // Brown
COLORREF boardColor2 = RGB(204, 102, 0);       // White
COLORREF backgroundColor = RGB(50, 50, 50); // Gray
HWND hSlider;
HWND hButton;

void CreateSlider(HWND hWnd)
{
    hSlider = CreateWindowEx(
        0, TRACKBAR_CLASS, NULL,
        WS_CHILD | WS_VISIBLE | TBS_HORZ,
        10, 10, 300, 30, // Position and size
        hWnd, NULL, GetModuleHandle(NULL), NULL);

    // Set slider range
    SendMessage(hSlider, TBM_SETRANGE, TRUE, MAKELPARAM(1, 255)); // 1 - 255
    SendMessage(hSlider, TBM_SETPOS, TRUE, 128); // Default position
}

void CreateButton(HWND hWnd)
{
	hButton = CreateWindow(
		L"BUTTON",          // Button class
		L"Insert Pieces",        // Button text
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Styles
		50,                 // x position
		50,                 // y position
		100,                // Button width
		30,                 // Button height
		hWnd,               // Parent window
		(HMENU)1,           // Button ID
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);              // Pointer not needed
}

void ParseFEN(const std::string& fen)
{
	for (int row = 0; row < BOARD_SIZE; ++row)
	{
		for (int col = 0; col < BOARD_SIZE; ++col)
		{
			boardState[row][col] = 0; // Reset each cell to empty
		}
	}

	int row = 0, col = 0;

	for (char c : fen)
	{
		if (c == '/') // Next row indicator 
		{
			row++;
			col = 0;
		}
		else if (isdigit(c)) // Empty square
		{
			col += c - '0';
		}
		else if (isalpha(c)) // Piece
		{
			boardState[row][col] = c;
			col++;
		}
		else
		{
			break; // Invalid so stop
		}
	}
}