
// Taking the fun out of functional

#include "Main.h"

// Main window class name
static TCHAR szWindowClass[] = _T("DesktopApp");

// String that appears in title bar
static TCHAR szTitle[] = _T("Board? Try Chess!");

// FEN file
std::string filepath = "C:\\Temp\\ChessFen.txt";

// Stored instance handle for Win32 API calls
HINSTANCE hInst;

// Forward declaration of functions
LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);

// Track selected cell
int selectedRow = -1;
int selectedCol = -1;

// Board state
char boardState[BOARD_SIZE][BOARD_SIZE] = { 0 };

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
		1000, 1000, // Initial size (width, length)
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

	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Awesome Chess");

	switch (message)
	{

	case WM_CREATE:
	{
		// Starting position
		std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
		ParseFEN(fen);

    
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
			

			std::string line;
			std::ifstream ifs(filepath);
			while (getline(ifs, line))
			{
				std::cout << line << "\n";
				ParseFEN(line);
			}
			ifs.close();

			InvalidateRect(hWnd, NULL, TRUE); // Force a repaint

			break;
		}
		break;
	}

	case WM_LBUTTONDOWN: 
	{
		// Get mouse position
		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);

		// Calculate the clicked row and column
		const int boardStartX = 300;
		const int boardStartY = 300;
		const int squareSize = 50;

		if (xPos >= boardStartX && yPos >= boardStartY)
		{
			int col = (xPos - boardStartX) / squareSize;
			int row = (yPos - boardStartY ) / squareSize;

			if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE)
			{
				selectedRow = row;
				selectedCol = col;

				// Force a repaint to update the board
				InvalidateRect(hWnd, NULL, TRUE);
				

				std::cout << "Selected cell " << static_cast<char>('A' + col) << BOARD_SIZE - row << std::endl;
			}
		}
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

		// Background color
		HBRUSH bgBrush = CreateSolidBrush(backgroundColor);
		FillRect(hdc, &ps.rcPaint, bgBrush);
		DeleteObject(bgBrush);

		// Draw the board
		const int boardSize = BOARD_SIZE;
		const int squareSize = SQUARE_SIZE;
		const int boardStartX = 300;
		const int boardStartY = 300;

		// Draw board
		for (int row = 0; row < boardSize; ++row)
		{
			for (int col = 0; col < boardSize; ++col)
			{
				int x = boardStartX + col * squareSize;
				int y = boardStartY + row * squareSize;

				// Check if this is the selected cell
				HBRUSH brush;
				if (row == selectedRow && col == selectedCol)
				{
					brush = CreateSolidBrush(RGB(255, 0, 0)); // Highlight color (red)
				}
				else
				{
					brush = (row + col) % 2 == 0
						? CreateSolidBrush(boardColor1)
						: CreateSolidBrush(boardColor2);
				}

				RECT rect = { x, y, x + squareSize, y + squareSize };
				FillRect(hdc, &rect, brush);
				DeleteObject(brush);

				// Draw circle if piece
				if (boardState[row][col] != 0)
				{
					HBRUSH pieceBrush = CreateSolidBrush(RGB(0, 0, 255));
					SelectObject(hdc, pieceBrush);

					Ellipse(
						hdc,
						x + squareSize / 4, // Left
						y + squareSize / 4, // Top
						x + 3 * squareSize / 4, // Right
						y + 3 * squareSize / 4 // Bottom
					);
					DeleteObject(brush);
				}
			}
		}

		//// Lay out application
		//TextOut(hdc, // Handle to device context
		//	5, 5,
		//	greeting, static_cast<int>(_tcslen(greeting)));
		// End app-specific layout

		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY: // Post quit message and return
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}