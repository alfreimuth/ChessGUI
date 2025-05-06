
/* This is a side project that I wanted to do, as to learn visual c++ better
	Things will be all over the place but hopefully eventually more organized,*/

// Taking the fun out of functional

#include "Main.h"

#pragma comment(lib, "Msimg32.lib")

// DISGUSTING global variables - remove later

// Main window class name
static TCHAR szWindowClass[] = _T("DesktopApp");

// String that appears in title bar
static TCHAR szTitle[] = _T("Board? Try Chess!");

// FEN file
std::string filepath = "C:\\Temp\\ChessFen.txt";

// Stored instance handle for Win32 API calls
HINSTANCE hInst;

bool isDragging = false; //If user is dragging
bool isLClicked = false;

bool whiteKingMoved = false, blackKingMoved = false;
bool whiteRookMoved[2] = { false, false };
bool blackRookMoved[2] = { false, false };

std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> gameHistory; // from - to
UINT lastPieceData;

UINT pieceData;

HWND hTextbox;

// Forward declaration of functions
LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);


int selectedRow = -1; // selected row
int selectedCol = -1; // selected column
int colB = -1; // new column
int rowB = -1; // new row
int colF = -1; // old column
int rowF = -1; // old row

char algebraicPieceChar;


// Board state
UINT boardState[BOARD_SIZE][BOARD_SIZE] = { 0 };

int mouseXStart;
int mouseYStart;
int mouseXEnd;
int mouseYEnd;

std::vector<std::pair<int, int>> moves;

void RedirectIOToConsole()
{
	// Alocate aconsole
	AllocConsole();

	// Redirect input output and error streams to console
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	freopen_s(&fp, "CONIN$", "r", stdin);

	// Write a message to the console
	std::cout << "Console initialized!" << std::endl;

}

// Windows descktop Main function
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
)
{

	RedirectIOToConsole();

	std::ifstream ifs(filepath); // Didnt want to override the file if it exists
	if (ifs.good())
	{
		std::cout << "File exists at " << filepath << std::endl;
	}
	else
	{
		std::ofstream ofs(filepath); // Make sure file is real
		ofs.close();
		std::cout << "File created at " << filepath << std::endl;
	}

	// Information about main window
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); // Size in bytes
	wcex.style = CS_HREDRAW | CS_VREDRAW; // Redraw on resize
	wcex.lpfnWndProc = WndProc; // Function to call when event occurs
	wcex.cbClsExtra = 0; // Extra bytes after window class
	wcex.cbWndExtra = 0; // Extra bytes after window
	wcex.hInstance = hInstance; // Instance handle
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION); // Load default icon
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); // Load default arrow cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Window background color
	wcex.lpszMenuName = NULL; // Menu name
	wcex.lpszClassName = szWindowClass; // Window class name
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION); // Load small icon

	// Register to Windows so it gets information
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			NULL,
			NULL);

		return 1;
	}

	// Store instance handle
	hInst = hInstance;

	// Create the window

	HWND hWnd = CreateWindowEx( // HWND = Handle to a window (Somewhat like pointer)
		WS_EX_OVERLAPPEDWINDOW, // Extended window style
		szWindowClass, // Window class
		szTitle, // Title bar text
		WS_OVERLAPPEDWINDOW, // Type of window
		CW_USEDEFAULT, CW_USEDEFAULT, // Position (x,y)
		SCREEN_X, SCREEN_Y, // Initial size (width, length)
		NULL, // Parent window
		NULL, // Menu bar
		hInstance, // First parameter from WinMain
		NULL // Pointer to window data
	);



	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindowEx failed!"),
			NULL,
			NULL);

		return 1;
	}

	// Tell Windows to make window visible
	ShowWindow(
		hWnd, // Value returned from CreateWindow
		nCmdShow // Fourth parameter from WinMain
	);
	UpdateWindow(hWnd);

	SetWindowPos(GetConsoleWindow(), HWND_TOP, 1, 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Message loop
	// Listens for messages from Windows
	// Sends to WndProc to be handled
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;

}

// Window procedure function.
// Handles messages when events occur
LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	static HBITMAP hSpritesheet = NULL; // Handle to spritesheet

	PAINTSTRUCT ps;
	HDC hdc = nullptr;
	TCHAR greeting[] = _T("Awesome Chess");

	switch (message)
	{

	case WM_CREATE:
	{
		FILE* file;
		if (_wfopen_s(&file, L"Content\\Textures\\ChessSprites.bmp", L"rb") != 0)
		{
			MessageBox(hWnd, L"File not found or inaccessiable!", L"Error", MB_OK | MB_ICONERROR);
			PostQuitMessage(0);
		}
		else if (file != nullptr)
		{
			fclose(file);
		}

		// Load spritesheet
		hSpritesheet = (HBITMAP)LoadImage(
			NULL, L"Content\\Textures\\ChessSprites.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE
		);
		if (!hSpritesheet)
		{
			DWORD error = GetLastError();
			std::cout << "Error loading spritesheet: " << error << std::endl;
			MessageBox(hWnd, L"Failed to load spritesheet!",
				L"Error",
				MB_OK | MB_ICONERROR
			);
			PostQuitMessage(0);
		}

		// Starting position
		std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
		ParseFEN(fen);

		hTextbox = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			L"EDIT",
			NULL,
			WS_CHILD | WS_VISIBLE |
			ES_MULTILINE | ES_AUTOVSCROLL,
			SCREEN_X - 400,
			50,
			350,
			SCREEN_Y - 150,
			hWnd,
			NULL,
			hInst,
			NULL
		);

		if (hTextbox == NULL)
		{
			MessageBox(hWnd, L"Textbox creation failed!", L"Error", MB_OK | MB_ICONERROR);
		}

		SetWindowText(hTextbox, std::wstring(fen.begin(), fen.end()).c_str());

    
		// Objects added
		//CreateSlider(hWnd); // Create slider when window is made

		CreateButton(hWnd); // Create button when window is made

		/*if (!hButton) 
		{
			MessageBox(hWnd, L"Button creation failed!", L"Error", MB_OK | MB_ICONERROR);
		}*/
		break;
	}

	case WM_COMMAND: // Menu selections
	{
		switch (LOWORD(wParam))
		{
		case 1: // Button ID
			//MessageBox(hWnd, L"Button clicked!", L"Notification", MB_OK | MB_ICONINFORMATION);
			

			/*std::string line;
			std::ifstream ifs(filepath);
			while (getline(ifs, line))
			{
				std::cout << line << "\n";
				ParseFEN(line);
			}
			ifs.close();*/

			wchar_t buffer[1024];
			GetWindowText(hTextbox, buffer, 1024);

			std::wcout << L"Textbox content: " << buffer << std::endl;

			selectedRow = -1;
			rowF = -1;
			rowB = -1;

			std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
			SetWindowText(hTextbox, std::wstring(fen.begin(), fen.end()).c_str()); // this is the dumbest, goofiest transition
			ParseFEN(fen);

			RECT boardRect = { 300, 50, 300 + BOARD_SIZE * SQUARE_SIZE, 50 + BOARD_SIZE * SQUARE_SIZE };

			InvalidateRect(hWnd, &boardRect, TRUE); // Force a repaint
			 
			moves.clear(); // Clear moves

			break;
		}
		
		break;
	}

	case WM_LBUTTONDOWN: 
	{
		rowB = -1;

		// Get mouse position
		mouseXStart = LOWORD(lParam);
		mouseYStart = HIWORD(lParam);

		// Calculate the clicked row and column
		const int boardStartX = 300;
		const int boardStartY = 50;

		if (mouseXStart >= boardStartX && mouseYStart >= boardStartY)
		{
			isLClicked = true;

			int col = (mouseXStart - boardStartX) / SQUARE_SIZE;
			int row = (mouseYStart - boardStartY ) / SQUARE_SIZE;

			if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE)
			{
				if (selectedRow == BOARD_SIZE - row && selectedCol == col)
				{
					// Same square clicked
					selectedRow = -1;
					selectedCol = -1;
					rowB = -1;
					colB = -1;
					isDragging = false;
					pieceData = 0;
					moves.clear(); // Clear moves

				} else 
				{
					SetCapture(hWnd); // Always follow mouse input and location even if on different window
					isDragging = true; // Start dragging

					std::cout << "Drag started at: " << mouseXStart << ", " << mouseYStart << " (" << static_cast<char>('a' + col) << BOARD_SIZE - row << ")" << std::endl;
					selectedRow = BOARD_SIZE - row;
					selectedCol = col;

					pieceData = boardState[row][col];

					moves = GetValidMoves(boardState[row][col], row, col);
					
				}

					std::cout << "Selected cell " << static_cast<char>('a' + col) << selectedRow << " has a value of " << boardState[row][col] << std::endl;

					// Force a repaint to update the board
					RECT boardRect = { 300, 50, 300 + BOARD_SIZE * SQUARE_SIZE, 50 + BOARD_SIZE * SQUARE_SIZE };
					InvalidateRect(hWnd, &boardRect, FALSE); // Force a repaint	
			}
		}
		
		break;
	}

	case WM_LBUTTONUP:
	{
		isLClicked = false;

		if (isDragging) // Dont like calling this dragging, thinking of better names
		{
			if (pieceData == 0)
			{
				std::cout << "No piece selected!" << std::endl;
				break;
			}

			ReleaseCapture(); // Stop following mouse 

			isDragging = false; 

			// first
			int colA = (mouseXStart - 300) / SQUARE_SIZE;
			int rowA = (mouseYStart - 50) / SQUARE_SIZE;

			// new
			int mouseXEnd = LOWORD(lParam);
			int mouseYEnd = HIWORD(lParam);
			colB = (mouseXEnd - 300) / SQUARE_SIZE;
			rowB = (mouseYEnd - 50) / SQUARE_SIZE;

			if (count(moves.begin(), moves.end(), std::make_pair(rowB, colB)) > 0)
			{
				MovePiece(hWnd, rowA, rowB, colA, colB, pieceData, lParam);
				RefreshPossibleMoves(hWnd);
				moves.clear(); // Clear moves
			}
			else
			{
				//selectedRow = -1;
				//selectedCol = -1;
				//rowN = -1;
				//colN = -1;
				//isDragging = false;
				//pieceData = 0;
				//moves.clear(); // Clear moves
			}
			
		}
		
		break;
	}

	case WM_MOUSEMOVE: 
	{
		isLClicked ? isDragging = true : isDragging = false;
		/*if(isDragging)
		{
			int xPos = LOWORD(lParam);
			int yPos = HIWORD(lParam);

			std::cout << "Dragged from: " << mouseXStart << ", " << mouseYStart << " to: " << xPos << ", " << yPos << std::endl;

			break;
		}*/
		break;
	}
	

	//case WM_HSCROLL:
	//	if ((HWND)lParam == hSlider)
	//	{
	//		// Get slider position
	//		int pos = SendMessage(hSlider, TBM_GETPOS, 0, 0);

	//		// Update colors based on slider position
	//		boardColor1 = RGB(204, 102, 0);
	//		boardColor2 = RGB( 224, 224, 224);
	//		backgroundColor = RGB(pos,-pos,pos);
	//		std::cout << "Slider position: " << pos << std::endl;

	//		// Redraw window
	//		InvalidateRect(hWnd, NULL, TRUE);
	//		Sleep(20);
	//	}

	case WM_PAINT: // Paint main window
	{
		
		hdc = BeginPaint(hWnd, &ps);

		// Double buffer shenanigans
		HDC hdcBuffer = CreateCompatibleDC(hdc);
		HBITMAP hBitmap = CreateCompatibleBitmap(hdc, SCREEN_X, SCREEN_Y);
		HBITMAP hOldBitMap = (HBITMAP)SelectObject(hdcBuffer, hBitmap);

		// Background color
		HBRUSH bgBrush = CreateSolidBrush(backgroundColor);
		FillRect(hdcBuffer, &ps.rcPaint, bgBrush);
		DeleteObject(bgBrush);

		// Board essentials
		HDC hdcMem = CreateCompatibleDC(hdc); // Memory device context
		SelectObject(hdcMem, hSpritesheet);
		
		
		

		DrawBoard(hdcBuffer, hdcMem);
		DeleteDC(hdcMem);

		//// Lay out application
		//TextOut(hdc, // Handle to device context
		//	5, 5,
		//	greeting, static_cast<int>(_tcslen(greeting)));
		// End app-specific layout

		

		// Buffer to screen
		BitBlt(hdc, 0, 0, SCREEN_X, SCREEN_Y, hdcBuffer, 0, 0, SRCCOPY);

		SelectObject(hdcBuffer, hOldBitMap); 
		DeleteObject(hBitmap);
		DeleteDC(hdcBuffer);

		EndPaint(hWnd, &ps);
		break;
	}
	

	case WM_DESTROY: // Post quit message and return
	{
		DeleteObject(hSpritesheet);
		PostQuitMessage(0);
		break;
	}

	default:
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	}
}

