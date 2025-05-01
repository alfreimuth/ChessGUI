
// Putting the fun in functions

#include "Main.h"

COLORREF boardColor1 = RGB(224, 224, 224); // Brown
COLORREF boardColor2 = RGB(204, 102, 0);       // White
COLORREF backgroundColor = RGB(50, 50, 50); // Gray
HWND hSlider;
HWND hButton;

COLORREF transparentColor = RGB(127, 127, 127); // Color I want transparent

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

void PlacePiece(HDC hdc, HDC hdcMem, UINT8 pieceData, int row, int col)
{
	int spriteX = ((pieceData & 0b00000111) - 1) * SPRITE_SIZE;
	int spriteY = ((pieceData & 0b00001000) >> 3) * SPRITE_SIZE;
	TransparentBlt(
		hdc,                // Device context
		col * SQUARE_SIZE + 300,  // x coordinate
		(7 - row) * SQUARE_SIZE + 50,  // y coordinate
		SPRITE_SIZE,         // Width
		SPRITE_SIZE,         // Height
		hdcMem,             // Source device context
		spriteX,            // Source x coordinate
		spriteY,            // Source y coordinate
		SPRITE_SIZE,         // Source width
		SPRITE_SIZE,         // Source height
		transparentColor    // Color to treat as transparent
	);
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
		// I want to try some bit stuff, since there are 2 colors, 6 pieces, and (possibly work with) 64 squares
		// 1 bit for color, 3 bits for piece type, rest are wasted unfortunately (for now)
		// Also logic is just plain fun

			int pieceData = 0;
			
			if (isupper(c)) // White piece
			{
				pieceData = 0b00000000;
			}
			else // Black piece
			{
				pieceData = 0b00001000;
			}
			// Convert piece character to index
			switch (tolower(c))
			{
			case 'k': pieceData |= 0b00000001; break; // King
			case 'q': pieceData |= 0b00000010; break; // Queen
			case 'b': pieceData |= 0b00000011; break; // Bishop
			case 'n': pieceData |= 0b00000100; break; // Knight
			case 'r': pieceData |= 0b00000101; break; // Rook
			case 'p': pieceData |= 0b00000110; break; // Pawn
			}
		
			boardState[row][col] = pieceData;
			col++;
		}
		else
		{
			break; // Invalid so stop
		}
	}
}