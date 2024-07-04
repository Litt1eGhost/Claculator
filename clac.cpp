#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <Windows.h>
using namespace std;

#define START_IN_DEV_MODE false

#define WINDOW_CREATION_X 900
#define WINDOW_CREATION_Y 750
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 80
#define MARGIN 4
#define STRSIZE 35
#define FONT_PATH "src/monofonto rg.otf"
#define TITLE "Claculator-dev"
#define FONT_RES 64

#define NumberOfCommands 8

bool DEV_MODE, running;
string commands[] = {"HELP", "help", "CLR", "clr", "EXIT", "exit", "DEV", "dev"};
SDL_Color white{255, 255, 255, 255};	SDL_Color red{255, 0, 0, 255};
SDL_Color green{0, 255, 0, 255};		SDL_Color blue{0, 0, 255, 255};
SDL_Color black{0, 0, 0, 255};			SDL_Color yellow{0, 255, 255, 255};
SDL_Color grey{100, 100, 100, 255};		SDL_Color bg{30, 30, 30, 255};

int CountBrackets(char* str, int i)
{
	int index = 0, count = 0;
	while(index < i)
	{
		if(str[index] == '(') count++;
		else if(str[index] == ')' && count > 0) count--;
		index++;
	}
	if(DEV_MODE)
		cout << "found: " << count << " unclosed brackets in: " << str << '\n';
	return count;
}

void CheckCommand(int command, char* str, int pos)
{
	if(DEV_MODE)
		cout << "checking for: " << commands[command] << '\n';
	int n = commands[command].size();
	for(int ch = 0; ch < n; ch++)
	{
		if(commands[command][ch] == str[pos+ch])
			continue;
		return;
	}
	switch(command)
	{
		case 0: case 1:
			if(DEV_MODE)
				cout << "Claculator(dev)\nCommands supported:\n" << 
						"shift + X: exit\tshift + D: Dev Mode\n" << 
						"shift + W: toggle window border\tshift + K: process\n" << 
						"shift + L: undo\tshift + J: clear screen\n";
			break;
		case 2: case 3:
			for(int x = 0; x < STRSIZE; x++) str[x] = '\0'; break;
		case 4: case 5:
			running = false; break;
		case 6: case 7:
			DEV_MODE = !DEV_MODE; break;
	}
}

void CopyStr(char* str1, int i1, char* str2, int* i2)	// 1 to 2
{
	*i2 = i1;
	for(int i = 0; i < STRSIZE; i++)
	{
		str2[i] = '\0';
		if(i < i1) str2[i] = str1[i];
	}
	return;
}

int ParseCommands(char* str, int i)
{
	for(int jj = 0; jj < NumberOfCommands; jj++)
	{
		for(int ii = 0; ii < i; ii++)
		{
			if(str[ii] == commands[jj][0])
				CheckCommand(jj, str, ii);
		}
	}
	return 0;
}

bool HasNum(char* str, int i)
{
	if(((str[i-1] >= '0' && str[i-1] <= '9')) 
		&& 
	((str[i+1] >= '0' && str[i+1] <= '9') || str[i+1] == '-'))return true;
	return false;
}

int ParseNumber(int dir, char* str, int pos, int* end)	//tested
	//dir 1 = left, 2 = right
{
	int res = 0, po10=1, mult = 1;
	if(dir == 2 && str[pos] == '-')
	{
		mult = -1;
		pos++;
	}
	while(str[pos] >= '0' && str[pos] <= '9')
	{
		if(dir == 1)
		{
			res += ((int)(str[pos]-'0'))*po10;
			po10*=10;
			pos--;
		}
		else if(dir == 2)
		{
			res*=10; res+=(int)(str[pos]-'0');
			pos++;
		}
	}
	if(dir == 1 && str[pos] == '-')
	{
		mult = -1;
		pos--;
	}
	if(dir == 1)
	{	*end = pos+1;	}
	else if(dir == 2)
	{	*end = pos-1;	}
	return res * mult;
}

int Write(char* str, int start, int buffer, int* end)	//tested
	//write buffer in str from start and return end position via the pointer
{
	string num = to_string(buffer);
	if(DEV_MODE)
	{	cout << "Write function to write: " << buffer << " to: " << start << '\n';	}
	int digits = num.length();
	*end = start + digits - 1;
	for(int i = 0; i < digits; i++)	str[start+i] = num[i];
	return 0;
}

int Shift(char* str, int to, int from, int* i)	//tested	//tested again
	//shift str starting from 'from' till i, to 'to' and update the pointer i to end pos
{
	if(DEV_MODE)
	{
		cout << "\n\nBefore shifting: " << str << '\n';
		printf("Called as: Shift(str, %d, %d, %d)\n", to, from, *i);
	}
	int till = *i;
	*i -= (from - to);
	for(int x = from; x < STRSIZE; x++)
	{
		if(DEV_MODE && str[x] != '\0')
			{	cout << "shifting: " << str[to] << " to " << str[x] << '\n';	}
		str[to] = str[x]; str[x] = '\0'; to++;
	}
	if(DEV_MODE) cout << "After shifting: " << str << "\n\n";
	return 0;
}

void Process(char* str1, int a, int b, int* i1)
{
	if(DEV_MODE)
		cout << "Processing: " << str1 << '\n';
	for(int pos = a+1; pos <= b; pos++)
	{
		if(str1[pos] == '(')
		{
			if(DEV_MODE)
				cout << "found ( at index: " << pos << '\n';
			for(int i = b-1; i>pos; i--)
			{
				if(str1[i] == ')')
				{	
					if(DEV_MODE)
						cout << "found ) at index: " << i << '\n';
					Process(str1, pos, i, i1);
					break;
				}
			}
		}
	}
	int n1=0, n2=0, n3=0, buffer=0; bool found = false;
	for(int pos = a+1; pos <= b; pos++)
	{
		found = false; n1=0, n2=0, n3=0;
		if(!HasNum(str1, pos)) continue;
		switch(str1[pos])
		{
			case '+':
				found = true;
				buffer = 
					ParseNumber(1, str1, pos-1, &n1) + ParseNumber(2, str1, pos+1, &n2);
				break;
			case '-':
				found = true;
				buffer = 
					ParseNumber(1, str1, pos-1, &n1) - ParseNumber(2, str1, pos+1, &n2);
				break;
			case '*':
				found = true;
				buffer = 
					ParseNumber(1, str1, pos-1, &n1) * ParseNumber(2, str1, pos+1, &n2);
				break;
			case '/':
				found = true;
				buffer = 
					ParseNumber(1, str1, pos-1, &n1) / ParseNumber(2, str1, pos+1, &n2);
				break;
			case '%':
				found = true;
				buffer = 
					ParseNumber(1, str1, pos-1, &n1) % ParseNumber(2, str1, pos+1, &n2);
				break;
		}
		
		if(found)
		{
			if(Write(str1, n1, buffer, &n3) || Shift(str1, n3+1, n2+1, i1))
				cerr << "Text moving error\n";
			pos = n3;
		}
	}
	if(str1[a] == '(' && str1[b] == ')')
	{
		Shift(str1, b, b+1, i1);
		Shift(str1, a, a+1, i1);
	}
}

int main(int argc, char* argv[])
{
	if(argc != 1)
	{	cerr << "No arguments needed/accepted\n"; return -1;	}
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{	cerr << "SDL2 Init error\n" << SDL_GetError() << "\n"; return -1;	}
	if(TTF_Init() < 0)
	{	cerr << "TTF Init error\n" << TTF_GetError() << "\n"; return -1;	}
	auto font = TTF_OpenFont(FONT_PATH, FONT_RES);
	if(!font)
	{	cerr << "Font loading failed\n"; return -1;	}
	
	
	running = true; bool clr = false; DEV_MODE = START_IN_DEV_MODE;
	int char_width = (WINDOW_WIDTH-20)/STRSIZE;
	char str1[STRSIZE] = " ", str2[STRSIZE] = " ", pstr[STRSIZE] = " "; 
	int i1 = 0, i2 = 0, pi = 0, bra = 0;
	
	
	HWND WindowHandle = GetConsoleWindow();
	if(DEV_MODE) ShowWindow(WindowHandle, SW_SHOW);
	else ShowWindow(WindowHandle, SW_HIDE);
	
	
	SDL_Event e; SDL_bool state = SDL_TRUE;
	SDL_Rect
		l1 = {MARGIN, MARGIN,
			WINDOW_WIDTH - MARGIN, WINDOW_HEIGHT/2 - 2*MARGIN},
		l2 = {MARGIN, WINDOW_HEIGHT/2 + MARGIN/2,
			WINDOW_WIDTH - MARGIN, WINDOW_HEIGHT/2 - 2*MARGIN};
	auto window = SDL_CreateWindow(
		TITLE, WINDOW_CREATION_X, WINDOW_CREATION_Y, 
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if(!window)
	{	cerr << "Window creation failed\n"; return -1;	}
	SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
	auto renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer)
	{	cerr << "Renderer creation failed\n"; return -1;	}
	auto text_surface = TTF_RenderText_Shaded(font, str1, white, bg);
	if(!text_surface)
	{	cerr << "Surface creation failed\n"; return -1;	}
	auto texture1 = SDL_CreateTextureFromSurface(renderer, text_surface);
	text_surface = TTF_RenderText_Shaded(font, str2, grey, bg);
	if(!text_surface)
	{	cerr << "Surface creation failed\n"; return -1;	}
	auto texture2 = SDL_CreateTextureFromSurface(renderer, text_surface);
	SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
	if(!texture1 || !texture2)
	{	cerr << "Texture creation failed\n"; return -1;	}
	
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, nullptr);
	SDL_StartTextInput();
	
	while(running)
	{
		if(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
				case SDL_QUIT:
					running = false; break;
					
				case SDL_KEYDOWN:
					if(DEV_MODE)	cout << "Got: " << e.key.keysym.sym << '\n';
					if(e.key.keysym.sym == SDLK_BACKSPACE)
						{	if(i1 >= 1)
							{	str1[i1] = '\0'; str1[--i1] = '\0';	}
						}
					break;
					
				case SDL_TEXTINPUT:
					if(*(e.text.text) == 'X')
						{	running = false; break;	}
					if(*(e.text.text) == 'W')
						{	if(state == SDL_TRUE) state = SDL_FALSE;
							else state = SDL_TRUE;
							SDL_SetWindowBordered(window, state); break;	}
					if(*(e.text.text) == 'D')
						{	DEV_MODE = !DEV_MODE;
							cout << "Dev Mode toggled to " << DEV_MODE << '\n';
							if(DEV_MODE) ShowWindow(WindowHandle, SW_SHOW);
							else ShowWindow(WindowHandle, SW_HIDE);
							break;	}
					if(*(e.text.text) == 'K')
						{	CopyStr(str1, i1, pstr, &pi);
							Process(str1, 0, i1, &i1);
							ParseCommands(str1, i1);
							SDL_Delay(20);
							break;	}
					if(*(e.text.text) == 'L')
						{	CopyStr(pstr, pi, str1, &i1); break;	}
					if(*(e.text.text) == 'J')
						{	
							if(DEV_MODE)
							{
								for(int i = 0; i < STRSIZE; i++)
									cout << (char)str1[i] << " ";
								cout << '\n';
							}
							clr = true;
							break;
						}
					
					
					if(clr)	break;
					if(i1 < STRSIZE-2) str1[i1++] = (char)(*(e.text.text));
					if('(' == (*(e.text.text)) || ')' == (*(e.text.text))) bra = CountBrackets(str1, i1);
					
					
					if(DEV_MODE)
						cout << "got " << (*(e.text.text)) << '\n';
					break;
			}
			CopyStr(str1, i1, str2, &i2);
			Process(str2, 0, i2, &i2);
		}
		
		
		if(clr)
		{
			if(DEV_MODE)	cout << "clearing screen\n";
			for(int x = 0; x < STRSIZE; x++) str1[x] = '\0';
			for(int x = 0; x < STRSIZE; x++) str2[x] = '\0';
//			str1[0] = ' ', str2[0] = ' ';
			i1 = 0, i2 = 0; clr = false;
		}
		
		
		str1[i1] = ' ';
		str2[i2] = ' ';
		if(i1 < STRSIZE-2)
		{
			str1[i1] = '_';
			if(bra)	str1[i1] = '!';
			str2[i2] = ' ';
		}
		
		
		l1.w = (i1+1)*(WINDOW_WIDTH-2*MARGIN)/(STRSIZE-1);
		l2.w = (i2+1)*(WINDOW_WIDTH-2*MARGIN)/(STRSIZE-1);
		
		text_surface = TTF_RenderText_Shaded(font, str1, white, bg);
		texture1 = SDL_CreateTextureFromSurface(renderer, text_surface);
		//SDL_BlitSurface(window, nullptr, text_surface, nullptr);
		text_surface = TTF_RenderText_Shaded(font, str2, grey, bg);
		texture2 = SDL_CreateTextureFromSurface(renderer, text_surface);
		//SDL_BlitSurface(window, nullptr, text_surface, nullptr);
		
		//SDL_UpdateWindowSurface(window);
		
		SDL_RenderCopy(renderer, texture1, nullptr, &l1);
		SDL_RenderCopy(renderer, texture2, nullptr, &l2);
		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);
		SDL_Delay(20);
		
		
	}
	SDL_StopTextInput();
	SDL_FreeSurface(text_surface);
	SDL_DestroyTexture(texture1);
	SDL_DestroyTexture(texture2);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	return 0;
}