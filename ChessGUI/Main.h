
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
#include <tchar.h> // Easier to work with char and wchar_t

#pragma comment(lib, "Comctl32.lib") // Link to common controls library

// Global variables
#define BOARD_SIZE 8       // 8x8 chessboard
#define SQUARE_SIZE 50 
#define BOARD_AREA (BOARD_SIZE * SQUARE_SIZE)
extern char boardState[BOARD_SIZE][BOARD_SIZE];
extern COLORREF boardColor1;
extern COLORREF boardColor2;
extern COLORREF backgroundColor;
extern HWND hSlider;

// Function prototypes
void CreateSlider(HWND hWnd);
void CreateButton(HWND hWnd);
void ParseFEN(const std::string& fen);

#endif // MAIN_H