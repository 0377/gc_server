#include "WindowsConsole.h"
#include "BaseServer.h"

#if defined(_DEBUG) && defined(PLATFORM_WINDOWS)
WindowsConsole::WindowsConsole()
	: h_out_(nullptr)
	, h_in_(nullptr)
	, show_cur_(false)
	//, old_code_page_(0)
	, save_old_mode_(0)
	, line_buf_(nullptr)
	, line_buf_size_(0)
{
	h_out_ = GetStdHandle(STD_OUTPUT_HANDLE);
	h_in_ = GetStdHandle(STD_INPUT_HANDLE);

	//old_code_page_ = GetConsoleOutputCP();
	//SetConsoleOutputCP(65001); // UTF-8
	hide_cursor();
	
	GetConsoleMode(h_in_, &save_old_mode_);
	SetConsoleMode(h_in_, ENABLE_WINDOW_INPUT);

	GetConsoleScreenBufferInfo(h_out_, &info_);

	line_buf_size_ = info_.dwSize.X;
	line_buf_ = (char*)malloc(line_buf_size_ + 1);
	line_buf_[line_buf_size_] = '\0';
}

WindowsConsole::~WindowsConsole()
{
	//SetConsoleOutputCP(old_code_page_);
	SetConsoleMode(h_in_, save_old_mode_);

	CloseHandle(h_out_);
	CloseHandle(h_in_);

	free(line_buf_);
}

void WindowsConsole::read_console_input()
{
	DWORD cNumRead;
	ReadConsoleInput(h_in_, ir_in_buf_, 128, &cNumRead);

	for (DWORD i = 0; i < cNumRead; i++) 
	{
		switch(ir_in_buf_[i].EventType) 
		{ 
		case KEY_EVENT: // keyboard input 
			key_event_proc(i);
			break; 

		case MOUSE_EVENT: // mouse input 
			break; 

		case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
			resize_event_proc(i);
			break; 

		case FOCUS_EVENT:  // disregard focus events 

		case MENU_EVENT:   // disregard menu events 
			break; 

		default: 
			break; 
		} 
	}
}

void WindowsConsole::key_event_proc(DWORD i)
{
	auto& ev = ir_in_buf_[i].Event.KeyEvent;

	if (!ev.bKeyDown)
	{
		return;
	}

	COORD crPos;
	DWORD dwNumber;

	// ֻ�ڰ���ʱ�жϣ�����ʱ���ж�
	if (show_cur_)
	{
		switch (ev.wVirtualKeyCode)
		{
		case VK_RETURN:         // ���س�ʱִ������
			GetConsoleScreenBufferInfo(h_out_, &info_);
			crPos = info_.dwCursorPosition;
			crPos.X = 0;
			ReadConsoleOutputCharacterA(h_out_, line_buf_, line_buf_size_, crPos, &dwNumber);
			
			// gm cmd
			BaseServer::instance()->on_gm_command(line_buf_);

			printf("\n");

			hide_cursor();
			show_cur_ = false;
			return;

		case VK_SPACE:          // ���ո�ʱ���һ���ո�  
			printf(" ");
			return;

		case VK_BACK:           // ��ɾ��ʱɾ��һ���ַ�(ֻ�ܵ�ǰ�в���)  
			GetConsoleScreenBufferInfo(h_out_, &info_);
			crPos = info_.dwCursorPosition;
			if (crPos.X != 0)
			{
				crPos.X -= 1;
			}
			SetConsoleCursorPosition(h_out_, crPos);
			printf(" ");
			SetConsoleCursorPosition(h_out_, crPos);
			return;

		case VK_ESCAPE:         // ��ESC��ʱȡ��
			GetConsoleScreenBufferInfo(h_out_, &info_);
			crPos = info_.dwCursorPosition;
			crPos.X = 0;
			SetConsoleCursorPosition(h_out_, crPos);
			clear_cur_line();
			SetConsoleCursorPosition(h_out_, crPos);

			hide_cursor();
			show_cur_ = false;
			return;

		default:
			break;
		}

		// -- ��ӡ�ַ�  
		char ch = ev.uChar.AsciiChar;

		// ������Դ�ӡ���ַ������ASCII��  
		if (ch > 0x20 && ch < 0x7e)
		{
			putchar(ch);
		}
	}
	else if (ev.wVirtualKeyCode == VK_RETURN) // ���س�ʱ
	{
		show_cursor();
		show_cur_ = true;
	}
	else if (0 != (ev.dwControlKeyState & SHIFT_PRESSED) && ev.wVirtualKeyCode == VK_ESCAPE)
	{
		// ��SHIFT+ESC��ʱ��������
		if (BaseServer::instance())
			BaseServer::instance()->stop();
	}
}

void WindowsConsole::resize_event_proc(DWORD i)
{
	auto& ev = ir_in_buf_[i].Event.WindowBufferSizeEvent;

	if (line_buf_size_ == ev.dwSize.X)
		return;

	line_buf_size_ = ev.dwSize.X;
	line_buf_ = (char*)realloc(line_buf_, line_buf_size_ + 1);
	line_buf_[line_buf_size_] = '\0';
}

void WindowsConsole::hide_cursor()
{
	CONSOLE_CURSOR_INFO curInfo;

	curInfo.dwSize = 1;             // ���û��ֵ�Ļ������ع����Ч  
	curInfo.bVisible = FALSE;

	SetConsoleCursorInfo(h_out_, &curInfo);

	SetConsoleTextAttribute(h_out_, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void WindowsConsole::show_cursor()
{
	CONSOLE_CURSOR_INFO curInfo;

	curInfo.dwSize = 25;
	curInfo.bVisible = TRUE;

	SetConsoleCursorInfo(h_out_, &curInfo);

	SetConsoleTextAttribute(h_out_, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
}

void WindowsConsole::clear_cur_line()
{
	for (SHORT i = 0; i < line_buf_size_; i++)
	{
		printf(" ");
	}
}

void WindowsConsole::write_console(WORD clr, const char* str)
{
	SHORT old_x = 0;

	if (show_cur_)
	{
		COORD crPos;
		GetConsoleScreenBufferInfo(h_out_, &info_);
		crPos = info_.dwCursorPosition;
		old_x = crPos.X;
		if (old_x > 0)
		{
			DWORD dwNumber;
			crPos.X = 0;
			ReadConsoleOutputCharacterA(h_out_, line_buf_, line_buf_size_, crPos, &dwNumber);

			SetConsoleCursorPosition(h_out_, crPos);
			clear_cur_line();
			SetConsoleCursorPosition(h_out_, crPos);
		}
	}

	SetConsoleTextAttribute(h_out_, clr);
	printf(str);

	if (show_cur_)
	{
		SetConsoleTextAttribute(h_out_, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
		if (old_x > 0)
		{
			line_buf_[old_x] = '\0';
			printf(line_buf_);
		}
	}
}
#endif
