
// I dont know any other fun puns

#ifndef MAIN_H
#define MAIN_H
    // Each square is 50x50 pixels

#include <windows.h>
#include <commctrl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <wingdi.h>
#include <tchar.h> // Easier to work with char and wchar_t

#pragma comment(lib, "Comctl32.lib") // Link to common controls library

// Global variables
#define BOARD_SIZE 8       // 8x8 chessboard
#define SQUARE_SIZE 116 
#define BOARD_AREA (BOARD_SIZE * BOARD_SIZE)
#define SCREEN_X (GetSystemMetrics(SM_CXSCREEN))
#define SCREEN_Y (GetSystemMetrics(SM_CYSCREEN))
#define SPRITE_SIZE 116

extern int boardState[BOARD_SIZE][BOARD_SIZE];
extern COLORREF boardColor1;
extern COLORREF boardColor2;
extern COLORREF backgroundColor;
extern HWND hSlider;


// Function prototypes
void CreateSlider(HWND hWnd);
void CreateButton(HWND hWnd);
void ParseFEN(const std::string& fen);
void PlacePiece(HDC hdc, HDC hdcMem, UINT8 pieceData, int row, int col);

#endif // MAIN_H