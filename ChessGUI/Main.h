
// I dont know any other fun puns

#pragma once
#ifndef MAIN_H
#define MAIN_H
    // Each square is 50x50 pixels

#include <windows.h> // For Windows API
#include <commctrl.h> // For common controls
#include <iostream> // For std::cout
#include <fstream> // For file operations
#include <string> // For std::string
#include <vector> // For std::vector
#include <wingdi.h> // For GDI functions
#include <utility> // For std::pair
#include <tchar.h> // Easier to work with char and wchar_t

#pragma comment(lib, "Comctl32.lib") // Link to common controls library

// Global variables
#define BOARD_SIZE 8       // 8x8 chessboard
#define SQUARE_SIZE 116  // Size of squares to match sprites in pixels (just learned that i can rescale the sprites, might change)
#define BOARD_AREA (BOARD_SIZE * BOARD_SIZE)
#define SCREEN_X (GetSystemMetrics(SM_CXSCREEN))
#define SCREEN_Y (GetSystemMetrics(SM_CYSCREEN))
#define SPRITE_SIZE 116

extern UINT boardState[BOARD_SIZE][BOARD_SIZE];
extern COLORREF boardColor1;
extern COLORREF boardColor2;
extern COLORREF backgroundColor;
extern HWND hSlider;
extern int selectedRow; // selected row
extern int selectedCol; // selected column
extern int colB; // new column
extern int rowB; // new row
extern int colF; // old column
extern int rowF; // old row
extern std::vector<std::pair<int, int>> moves;

extern int mouseXStart;
extern int mouseYStart;

extern bool whiteKingMoved;
extern bool blackKingMoved;
extern bool whiteRookMoved[2];
extern bool blackRookMoved[2];
extern std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> gameHistory; // <from> <to>
extern UINT lastPieceData;
extern char algebraicPieceChar;
extern HWND hTextbox;
extern wchar_t buffer[1024];



// Function prototypes
void CreateSlider(HWND hWnd);
void CreateButton(HWND hWnd);
void ParseFEN(const std::string& fen);
void PlacePiece(HDC hdcBuffer, HDC hdcMem, UINT8 pieceData, int row, int col);
void DrawBoard(HDC hdcBuffer, HDC hdcMem);
std::vector<std::pair<int, int>> GetValidMoves(int pieceData, int row, int col);
void RefreshRect(HWND hWnd, int row, int col);
void RefreshPossibleMoves(HWND hWnd);
void MovePiece(HWND hWnd, int rowA, int rowB, int colA, int colB, UINT pieceData, LPARAM lParam);
void UpdateMoveHistory();

#endif // MAIN_H