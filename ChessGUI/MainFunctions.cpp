
// Putting the fun in functions

#include "Main.h"

COLORREF boardColor1 = RGB(224, 224, 224); // Brown
COLORREF boardColor2 = RGB(204, 102, 0);       // White
COLORREF backgroundColor = RGB(50, 50, 50); // Gray
HWND hSlider;
HWND hButton;

COLORREF transparentColor = RGB(127, 127, 127); // Color I want transparent

enum PieceType
{
	KING = 0b00000001,
	QUEEN = 0b00000010,
	BISHOP = 0b00000011,
	KNIGHT = 0b00000100,
	ROOK = 0b00000101,
	PAWN = 0b00000110
};

enum PieceColor
{
	WHITE = 0b00000000,
	BLACK = 0b00001000
};

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
		L"Reset Board",        // Button text
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
	std::fill(&boardState[0][0], &boardState[0][0] + BOARD_AREA, 0); // fill from 0,0 to AREA, AREA with 0
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

			int pieceData = isupper(c) ? 0b00000000 : 0b00001000; // White : Black
			
			// Convert piece character to index
			switch (tolower(c))
			{
			case 'k': pieceData |= KING; break; // King
			case 'q': pieceData |= QUEEN; break; // Queen
			case 'b': pieceData |= BISHOP; break; // Bishop
			case 'n': pieceData |= KNIGHT; break; // Knight
			case 'r': pieceData |= ROOK; break; // Rook
			case 'p': pieceData |= PAWN; break; // Pawn
			default: return;
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

void DrawBoard(HDC hdcBuffer, HDC hdcMem)
{
	for (UINT row = 0; row < BOARD_SIZE; ++row)
	{
		for (UINT col = 0; col < BOARD_SIZE; ++col)
		{
			int x = 300 + col * SQUARE_SIZE;
			int y = 50 + row * SQUARE_SIZE;

			HBRUSH brush = (row + col) % 2 == 0 ? CreateSolidBrush(boardColor1) : CreateSolidBrush(boardColor2);

			if (BOARD_SIZE - row == selectedRow && col == selectedCol)
			{
				brush = CreateSolidBrush(RGB(50, 200, 0)); // Highlight color (red)
			}
			else if (row == rowB && col == colB)
			{
				brush = CreateSolidBrush(RGB(10, 100, 50));
			}

			RECT rect = { x, y, x + SQUARE_SIZE, y + SQUARE_SIZE };
			FillRect(hdcBuffer, &rect, brush);
			DeleteObject(brush);

			for (const auto& move : moves)
			{
				//std::cout << "Row: " << move.first << ", Col: " << move.second << std::endl;

				HBRUSH pieceBrush = CreateSolidBrush(RGB(0, 0, 50));
				SelectObject(hdcBuffer, pieceBrush);

				Ellipse(
					hdcBuffer,
					(300 + move.second * SQUARE_SIZE) + SQUARE_SIZE / 4, // Left
					(50 + move.first * SQUARE_SIZE) + SQUARE_SIZE / 4, // Top
					(300 + move.second * SQUARE_SIZE) + 3 * SQUARE_SIZE / 4, // Right
					(50 + move.first * SQUARE_SIZE) + 3 * SQUARE_SIZE / 4 // Bottom
				);
				DeleteObject(pieceBrush);

			}
		

			if (boardState[row][col] != 0) 
			{
				PlacePiece(hdcBuffer, hdcMem, boardState[row][col], 7-row, col);
			}
		}
	}
}

bool IsValidPos(int row, int col)
{
	return row >= 0 && row < BOARD_SIZE && col >=0 && col < BOARD_SIZE;
}

std::vector<std::pair<int, int>> GetValidMoves(int pieceData, int row, int col)
{
	std::vector<std::pair<int, int>> moves;

	switch (pieceData & 0b00000111)
	{
		case KING:
		{
			const std::vector<std::pair<int, int>> directions = {
				{1, 0}, // Down
				{-1, 0}, // Up
				{0, 1},  // Right
				{0, -1},  // Left
				{1, 1},  // Down-Right
				{1, -1}, // Down-Left
				{-1, 1}, // Up-Right
				{-1, -1} // Up-Left
			};

			algebraicPieceChar = 'K';

			if (!(pieceData & 0b00001000))
			{
				if (!whiteKingMoved && !whiteRookMoved[1] 
					 && boardState[row][col + 1] == 0 && boardState[row][col + 2] == 0 
					// && !IsSquareUnderAttack(row, col, WHITE) &&
					//!IsSquareUnderAttack(row, col + 1, WHITE) &&
					//!IsSquareUnderAttack(row, col + 2, WHITE)
					)
				{
					moves.emplace_back(row, col + 2); // Castling move
				}
				if (!whiteKingMoved && !whiteRookMoved[0]
					&& boardState[row][col - 1] == 0 && boardState[row][col - 2] == 0 && boardState[row][col - 3] == 0
					// && !IsSquareUnderAttack(row, col, WHITE) &&
					//!IsSquareUnderAttack(row, col - 1, WHITE) &&
					//!IsSquareUnderAttack(row, col - 2, WHITE)
					)
				{
					moves.emplace_back(row, col - 2); // Castling move
				}
			}
			else if (pieceData & 0b00001000)
			{
				if (!blackKingMoved && !blackRookMoved[1]
					&& boardState[row][col + 1] == 0 && boardState[row][col + 2] == 0
					// && !IsSquareUnderAttack(row, col, WHITE) &&
					//!IsSquareUnderAttack(row, col + 1, WHITE) &&
					//!IsSquareUnderAttack(row, col + 2, WHITE)
					)
				{
					moves.emplace_back(row, col + 2); // Castling move
				}
				if (!blackKingMoved && !blackRookMoved[0]
					&& boardState[row][col - 1] == 0 && boardState[row][col - 2] == 0 && boardState[row][col - 3] == 0
					// && !IsSquareUnderAttack(row, col, WHITE) &&
					//!IsSquareUnderAttack(row, col - 1, WHITE) &&
					//!IsSquareUnderAttack(row, col - 2, WHITE)
					)
				{
					moves.emplace_back(row, col - 2); // Castling move
				}
			}

			for (const auto& direction : directions)
			{
				int rowOffset = direction.first;
				int colOffset = direction.second;

				for (int i = 1; i <= 1; ++i)
				{
					int newRow = row + i * rowOffset;
					int newCol = col + i * colOffset;

					if (!IsValidPos(newRow, newCol))
						break; // Stop

					if (boardState[newRow][newCol] == 0) // Empty
					{
						moves.emplace_back(newRow, newCol);
					}
					else
					{
						// Check enemy
						if ((boardState[newRow][newCol] >> 3) != (pieceData >> 3))
						{
							moves.emplace_back(newRow, newCol); // Add this move
						}
						break; // Stop 
					}
				}
			}
			break;
		}

		case QUEEN:
		{
			const std::vector<std::pair<int, int>> directions = {
				{1, 0}, // Down
				{-1, 0}, // Up
				{0, 1},  // Right
				{0, -1},  // Left
				{1, 1},  // Down-Right
				{1, -1}, // Down-Left
				{-1, 1}, // Up-Right
				{-1, -1} // Up-Left
			};

			algebraicPieceChar = 'Q';

			for (const auto& direction : directions)
			{
				int rowOffset = direction.first;
				int colOffset = direction.second;

				for (int i = 1; i < BOARD_SIZE; ++i)
				{
					int newRow = row + i * rowOffset;
					int newCol = col + i * colOffset;

					if (!IsValidPos(newRow, newCol))
						break; // Stop

					if (boardState[newRow][newCol] == 0) // Empty
					{
						moves.emplace_back(newRow, newCol);
					}
					else
					{
						// Check enemy
						if ((boardState[newRow][newCol] >> 3) != (pieceData >> 3))
						{
							moves.emplace_back(newRow, newCol); // Add this move
						}
						break; // Stop 
					}
				}
			}
			break;
		}

		case BISHOP: 
		{
			const std::vector<std::pair<int, int>> directions = {
				{1, 1},  // Down-Right
				{1, -1}, // Down-Left
				{-1, 1}, // Up-Right
				{-1, -1} // Up-Left
			};

			algebraicPieceChar = 'B';

			for (const auto& direction : directions)
			{
				int rowOffset = direction.first;
				int colOffset = direction.second;

				for (int i = 1; i < BOARD_SIZE; ++i)
				{
					int newRow = row + i * rowOffset;
					int newCol = col + i * colOffset;

					if (!IsValidPos(newRow, newCol))
						break; // Stop

					if (boardState[newRow][newCol] == 0) // Empty
					{
						moves.emplace_back(newRow, newCol);
					}
					else
					{
						// Check enemy
						if ((boardState[newRow][newCol] >> 3) != (pieceData >> 3))
						{
							moves.emplace_back(newRow, newCol); // Add this move
						}
						break; // Stop 
					}
				}
			}
			break;
		}
		
		case KNIGHT:
		{
			const std::vector<std::pair<int, int>> directions = {
					{2, 1}, {2, -1},
					{-2, 1}, {-2, -1},
					{1, 2}, {1, -2},
					{-1, 2}, {-1, -2}
			};

			algebraicPieceChar = 'N';

			for (const auto& direction : directions)
			{
				int rowOffset = direction.first;
				int colOffset = direction.second;

				for (int i = 1; i <= 1; ++i)
				{
					int newRow = row + i * rowOffset;
					int newCol = col + i * colOffset;

					if (!IsValidPos(newRow, newCol))
						break; // Stop

					if (boardState[newRow][newCol] == 0) // Empty
					{
						moves.emplace_back(newRow, newCol);
					}
					else
					{
						// Check enemy
						if ((boardState[newRow][newCol] >> 3) != (pieceData >> 3))
						{
							moves.emplace_back(newRow, newCol); // Add this move
						}
						break; // Stop 
					}
				}
			}
			break;
		}

		case ROOK:
		{
			// Directions: {rowOffset, colOffset} for down, up, right, left
			const std::vector<std::pair<int, int>> directions = {
				{1, 0},  // Down
				{-1, 0}, // Up
				{0, 1},  // Right
				{0, -1}  // Left
			};

			algebraicPieceChar = 'R';

			for (const auto& direction : directions)
			{
				int rowOffset = direction.first;
				int colOffset = direction.second;

				for (int i = 1; i < BOARD_SIZE; ++i)
				{
					int newRow = row + i * rowOffset;
					int newCol = col + i * colOffset;

					if (!IsValidPos(newRow, newCol))
						break; // Stop

					if (boardState[newRow][newCol] == 0) // Empty
					{
						moves.emplace_back(newRow, newCol);
					}
					else
					{
						// Check enemy
						if ((boardState[newRow][newCol] >> 3) != (pieceData >> 3))
						{
							moves.emplace_back(newRow, newCol); // Add this move
						}
						break; // Stop 
					}
				}
			}
			break;
			}

		case PAWN:
		{
			const std::vector<std::pair<int, int>> directions = {
					{1, 0},
					{1, 1},
					{1, -1},
					{2, 0}
			};

			algebraicPieceChar = NULL;

			for (const auto& direction : directions)
			{

				int rowOffset = direction.first;
				int colOffset = direction.second;

				if (!(pieceData & 0b00001000)) // Had to do NOT black, as I am assuming you cant compare to 0 ( LO volt AND LO volt != HI volt)
				{ // Also this is for different pawn directions based on color
					rowOffset *= -1;
				}

				if (direction.first == 2)
				{
					if (!(row == 1 || row == 6))
					{
						break; // Only move 2 squares from starting position
					}
					else
					{
						if (((boardState[row + rowOffset][col] != 0) || (boardState[row + (rowOffset / 2)][col] != 0))) // Check if empty
						{
							continue; // Can't move if not empty
						}
					}
				}

				for (int i = 1; i <= 1; ++i)
				{
					int newRow = row + i * rowOffset;
					int newCol = col + i * colOffset;

					if (!IsValidPos(newRow, newCol))
						break; // Stop

					if ((boardState[newRow][newCol] == 0) && (colOffset == 0)) // Empty and cannot move diagonally on non-captures
					{
						moves.emplace_back(newRow, newCol);
					}
					else
					{
						// Check enemy
						if (((boardState[newRow][newCol] >> 3) != (pieceData >> 3)) && (boardState[newRow][newCol] != 0) && (colOffset != 0))
						{
							moves.emplace_back(newRow, newCol); // Add this move
						}
						break; // Stop 
					}
				}
			}

			break;
		}
	}

	return moves;
}

void RefreshRect(HWND hWnd, int row, int col)
{
	RECT newRect = {
			300 + col * SQUARE_SIZE,
			50 + row * SQUARE_SIZE,
			300 + (col + 1) * SQUARE_SIZE,
			50 + (row + 1) * SQUARE_SIZE
	};

	InvalidateRect(hWnd, &newRect, FALSE);
}

void RefreshPossibleMoves(HWND hWnd) { // doesnt have a use just learned. 
	for (const auto& move : moves)
	{
		RefreshRect(hWnd, move.first, move.second);

		std::cout << "Refreshed cell " << move.first << move.second << std::endl;
	}
}

void MovePiece(HWND hWnd, int rowA, int rowB, int colA, int colB, UINT pieceData, LPARAM lParam)
{

		RECT originalRect = {
		300 + colA * SQUARE_SIZE,
		50 + rowA * SQUARE_SIZE,
		300 + (colA + 1) * SQUARE_SIZE,
		50 + (rowA + 1) * SQUARE_SIZE
		};

		RECT newRect = {
		300 + colB * SQUARE_SIZE,
		50 + rowB * SQUARE_SIZE,
		300 + (colB + 1) * SQUARE_SIZE,
		50 + (rowB + 1) * SQUARE_SIZE
		};

		InvalidateRect(hWnd, &originalRect, FALSE);
		InvalidateRect(hWnd, &newRect, FALSE);

		//std::cout << "Drag stopped at: " << mouseXEnd << ", " << mouseYEnd << " (" << static_cast<char>('A' + colB) << BOARD_SIZE - rowB << ")" << std::endl;

		boardState[rowA][colA] = 0;

		if (rowB >= 0 && rowB < BOARD_SIZE && colB >= 0 && colB < BOARD_SIZE)
		{
			boardState[rowB][colB] = pieceData; // Move piece
			//gameHistory.emplace_back(std::make_pair(rowA, colA), std::make_pair(rowB, colB)); // Store move history
			gameHistory.push_back({ {rowA, colA}, {rowB, colB} }); // Store move history
			UpdateMoveHistory();
			lastPieceData = pieceData;
		}

		if ((pieceData & 0b00000111) == KING)
		{
			if (pieceData & 0b00001000) // Black
			{
				if (colA == 4 && colB == 6 && !blackKingMoved && !blackRookMoved[1]) // Short castling
				{
					boardState[0][5] = boardState[0][7]; // Move rook
					boardState[0][7] = 0; // Clear old rook position
					RefreshRect(hWnd, 0, 7);
				}
				else if (colA == 4 && colB == 2 && !blackKingMoved && !blackRookMoved[0]) // Long castling
				{
					boardState[0][5] = boardState[0][0]; // Move rook
					boardState[0][7] = 0; // Clear old rook position
					RefreshRect(hWnd, 0, 7);
				}
				blackKingMoved = true;
			}
			else
			{
				if (colA == 4 && colB == 6 && !whiteKingMoved && !whiteRookMoved[1]) // Short castling
				{
					boardState[7][5] = boardState[7][7]; // Move rook
					boardState[7][7] = 0; // Clear old rook position
					RefreshRect(hWnd, 7, 7);
				}
				else if (colA == 4 && colB == 2 && !whiteKingMoved && !whiteRookMoved[0]) // Long castling
				{
					boardState[7][3] = boardState[7][0]; // Move rook
					boardState[7][0] = 0; // Clear old rook position
					RefreshRect(hWnd, 7, 0);
				}
				whiteKingMoved = true;
			}
		}
		else if ((pieceData & 0b00000111) == ROOK)
		{
			if (pieceData & 0b00001000) // Black
			{
				if (colA == 0)
					blackRookMoved[0] = true; 
				else if (colA == BOARD_SIZE - 1)
					blackRookMoved[1] = true; 
			}
			else
			{
				if (colA == 0)
					whiteRookMoved[0] = true; 
				else if (colA == BOARD_SIZE - 1)
					whiteRookMoved[1] = true; 
			}
		}
		else if ((pieceData & 0b00000111) == PAWN)
		{
				if ((rowB == 0 || rowB == BOARD_SIZE - 1))
				{
					// Promote
					std::cout << "Promotion!!!" << std::endl;
				}

				std::cout << (static_cast<char>('a' + gameHistory.end()[-1].first.first) + gameHistory.end()[-1].first.second
					+ static_cast<char>('a' + gameHistory.end()[-1].second.first) + gameHistory.end()[-1].second.second) << std::endl;
				
		}
		else  
		{
				std::cout << algebraicPieceChar << static_cast<char>('a' + gameHistory.end()[-1].first.first) << gameHistory.end()[-1].first.second
					<< static_cast<char>('a' + gameHistory.end()[-1].second.first) << gameHistory.end()[-1].second.second << std::endl;
		}
	
}

void UpdateMoveHistory()
{
	wchar_t buffer[4096];
	GetWindowText(hTextbox, buffer, 4096);

	auto LastMove = gameHistory.back();
	int rowA = LastMove.first.first;
	int colA = LastMove.first.second;
	int rowB = LastMove.second.first;
	int colB = LastMove.second.second;

	std::wstring move = L"";
	move += static_cast<wchar_t>('a' + colA);
	move += std::to_wstring(BOARD_SIZE - rowA);
	
	move += static_cast<wchar_t>('a' + colB);
	move += std::to_wstring(BOARD_SIZE - rowB);

	move += L"\r\n";

	std::wstring updatedMoveList = buffer + move;

	SetWindowText(hTextbox, updatedMoveList.c_str());
}