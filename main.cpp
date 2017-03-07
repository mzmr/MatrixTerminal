#include <iostream>
#include <ctime>
#include <windows.h>

typedef struct _CONSOLE_SCREEN_BUFFER_INFOEX {
  ULONG      cbSize;
  COORD      dwSize;
  COORD      dwCursorPosition;
  WORD       wAttributes;
  SMALL_RECT srWindow;
  COORD      dwMaximumWindowSize;
  WORD       wPopupAttributes;
  BOOL       bFullscreenSupported;
  COLORREF   ColorTable[16];
} CONSOLE_SCREEN_BUFFER_INFOEX, *PCONSOLE_SCREEN_BUFFER_INFOEX;

extern "C" BOOL WINAPI GetConsoleScreenBufferInfoEx(
  HANDLE hConsoleOutput,
  PCONSOLE_SCREEN_BUFFER_INFOEX lpConsoleScreenBufferInfoEx
);

extern "C" BOOL WINAPI SetConsoleScreenBufferInfoEx(
  HANDLE hConsoleOutput,
  PCONSOLE_SCREEN_BUFFER_INFOEX lpConsoleScreenBufferInfoEx
);

using namespace std;

HANDLE hOut = GetStdHandle( STD_OUTPUT_HANDLE );

const int _SZER = 79;
const int _WYS = 25;

const int stopnie_jasnosci = 7; // domyslnie 7
const int stopnie_ciemnosci = 6; // domyslnie 3

void KursorPoz(int x, int y);

class animacja {
  char arr_anim[_SZER][_WYS];
  int arr_licznik[_SZER]; // odleglosc poczatku ciagu od gory ekranu w odpowiedniej kolumnie
  int arr_puste[_SZER]; // liczba pustych pol w kolumnie (zanikajacego ciagu)
  int arr_koniec[_SZER]; // liczba wyswietlen znaku z samego dolu
  int arr_sciemnienie[_SZER]; // informuje o poziomie sciemnienia koncowego znaku kolumny

  char LosujZnak();
  void ZerujAnimacje();
public:
  animacja();
  ~animacja();

  void NowaKolumna(int czestosc); // mniejsza wartosc parametru to czesciej, 0 to 100%
  void PrzedluzCiag();
  void ZakonczCiag(int dlugosc); // im wieksza wartosc tym dluzej ciagi beda mogly pozostac
  void SkrocCiag();
  void ZmienIstniejace(int czestosc); // mniejsza wartosc parametru to czesciej, 0 to 100%
  void ZmienPierwsze();
  void Wyswietl();
};

animacja::animacja() {
  ZerujAnimacje();
  system("cls");
}

animacja::~animacja() {
  system("cls");
}

void animacja::NowaKolumna(int czestosc) {
  for (int i = 0; i < _SZER; i++) {
    if (rand() % czestosc == 0 && arr_licznik[i] == 0) {
      arr_anim[i][0] = LosujZnak();
      arr_licznik[i]++;
      arr_puste[i]--;
    }
  }
}

void animacja::PrzedluzCiag() {
  for (int i = 0; i < _SZER; i++) {
    if (arr_licznik[i] > 0 && arr_anim[i][_WYS - 1] == NULL) {
      arr_anim[i][arr_licznik[i]] = LosujZnak();
      arr_licznik[i]++;
      arr_puste[i]--;
    }
  }
}

void animacja::ZakonczCiag(int dlugosc) {
  for (int i = 0; i < _SZER; i++) {
    if (rand() % dlugosc == 0 && arr_puste[i] == 0 && arr_sciemnienie[i] == 0) {
      KursorPoz(i, 0);
      SetConsoleTextAttribute( hOut, 8); // 1 poziom sciemnienia
      WriteConsoleA( hOut, &arr_anim[i][0], 1, NULL, NULL );

      arr_sciemnienie[i]++;
    }
    else if (arr_sciemnienie[i] > 0 && arr_sciemnienie[i] < stopnie_ciemnosci) {
      for (int k = 0; k <= arr_sciemnienie[i]; k++) {
        KursorPoz(i, k);
        SetConsoleTextAttribute( hOut, 8 - (arr_sciemnienie[i] - k));
        WriteConsoleA( hOut, &arr_anim[i][k], 1, NULL, NULL );
      }

      arr_sciemnienie[i]++;
    }
    else if (arr_sciemnienie[i] == stopnie_ciemnosci) {
      arr_anim[i][0] = NULL;
      KursorPoz(i, 0);
      WriteConsoleA( hOut, (PCVOID)" ", 1, NULL, NULL );

      for (int k = 0; k < stopnie_ciemnosci; k++) {
        KursorPoz(i, k + 1);
        SetConsoleTextAttribute( hOut, 8 - (stopnie_ciemnosci - 1 - k));
        WriteConsoleA( hOut, &arr_anim[i][k + 1], 1, NULL, NULL );
      }

      arr_puste[i]++;
      arr_licznik[i]--;
      arr_sciemnienie[i] = 0;
    }
  }
}

void animacja::SkrocCiag() {
  for (int i = 0; i < _SZER; i++) {
    if (arr_puste[i] > 0 && arr_koniec[i] > 0) {
      arr_anim[i][arr_puste[i]] = NULL;
      KursorPoz(i, arr_puste[i]);
      WriteConsoleA( hOut, (PCVOID)" ", 1, NULL, NULL );
      arr_puste[i]++;
      arr_licznik[i]--;

      for (int k = 0; k < stopnie_ciemnosci; k++) {
        if (arr_puste[i] + k < _WYS) {
          KursorPoz(i, arr_puste[i] + k);
          SetConsoleTextAttribute( hOut, 8 - (stopnie_ciemnosci - 1 - k));
          WriteConsoleA( hOut, &arr_anim[i][arr_puste[i] + k], 1, NULL, NULL );
        }
      }

      if (arr_licznik[i] == 0) {
        arr_koniec[i] = 0;
      }
    }
  }
}

void animacja::Wyswietl() {
  for (int i = 0; i < _SZER; i++) {
    if (arr_licznik[i] > 0) {

      if (arr_koniec[i] == 0) {
        SetConsoleTextAttribute( hOut, 15 ); // bialy
        KursorPoz(i, arr_licznik[i] - 1);
        WriteConsoleA( hOut, &arr_anim[i][arr_licznik[i] - 1], 1, NULL, NULL );

        for (int k = 1; k < stopnie_jasnosci; k++) {
          if (arr_licznik[i] > k) {
            SetConsoleTextAttribute( hOut, 15 - k ); // od bialego do zwyklej zielenii
            KursorPoz(i, arr_licznik[i] - (1 + k));
            WriteConsoleA( hOut, &arr_anim[i][arr_licznik[i] - (1 + k)], 1, NULL, NULL );
          }
        }

        if (arr_anim[i][_WYS - 1] != NULL) {
          arr_koniec[i] = 1;
        }
      }
      else if (arr_koniec[i] > 0 && arr_koniec[i] < stopnie_jasnosci) {
        for (int k = arr_koniec[i], p = 1; k < stopnie_jasnosci; k++, p++) {
          SetConsoleTextAttribute( hOut, 15 - k);
          KursorPoz(i, _WYS - p);
          WriteConsoleA( hOut, &arr_anim[i][_WYS - p], 1, NULL, NULL );
        }
        arr_koniec[i]++;
      }

      // warunek w zasadzie sie nie przydaje, bo nie wypisuje nic na ostatniej kolumnie
      if (arr_licznik[_SZER - 1] == _WYS) {
        KursorPoz(0,0);
      }
    }
  }
}

void animacja::ZerujAnimacje() {
  for (int i = 0; i < _SZER; i++){
    for (int k = 0; k < _WYS; k++){
      arr_anim[i][k] = NULL;
    }
    arr_licznik[i] = 0;
    arr_puste[i] = _WYS;
    arr_koniec[i] = 0;
    arr_sciemnienie[i] = 0;
  }
}

void animacja::ZmienIstniejace(int czestosc) {
  for (int i = 0; i < _SZER; i++) {
    for (int k = 0; k < arr_licznik[i] - arr_puste[i]; k++) {
      if (rand() % czestosc == 0) {
        KursorPoz(i, k + arr_puste[i]);
        arr_anim[i][k + arr_puste[i]] = LosujZnak();

        SetConsoleTextAttribute( hOut, 9 ); // zielony zwykly
        WriteConsoleA( hOut, &arr_anim[i][k + arr_puste[i]], 1, NULL, NULL );

        if (i == _SZER - 1 && k + arr_puste[i] == _WYS - 1) {
          KursorPoz(0,0);
        }
      }
    }
  }
}

void animacja::ZmienPierwsze() {
  for (int i = 0; i < _SZER; i++) {
    if (arr_licznik[i] > 0 && arr_koniec[i] < 2) {
      arr_anim[i][arr_licznik[i] - 1] = LosujZnak();
      KursorPoz(i, arr_licznik[i] - 1);
      SetConsoleTextAttribute( hOut, 15 ); // bialy
      WriteConsoleA( hOut, &arr_anim[i][arr_licznik[i] - 1], 1, NULL, NULL );
    }
  }
}

char animacja::LosujZnak() {
  return rand() % 241 + 14;
}

//-----------------------------------------------------------------------------

void KursorPoz(int x, int y) {
  SetConsoleCursorPosition( hOut, (COORD){x, y} );
}

//-----------------------------------------------------------------------------

int main() {
  // nadanie tytulu oknu konsoli
  SetConsoleTitleA("Matrix Raining Code - Copyright (c) Maciej Znamirowski 2014");

  // ukrycie kursora
  CONSOLE_CURSOR_INFO hCCI;
  GetConsoleCursorInfo( hOut, & hCCI );
  hCCI.bVisible = FALSE;
  SetConsoleCursorInfo( hOut, & hCCI );

  // ustawienie palety kolorow
  CONSOLE_SCREEN_BUFFER_INFOEX bi;
  bi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
  GetConsoleScreenBufferInfoEx(hOut, &bi);
  bi.bFullscreenSupported = true;

  // Ustawienie wartosci nowych kolorow. Format: 0x00bbggrr
  bi.ColorTable[15] = 0xffffff; // bialy
  bi.ColorTable[14] = 0xaaffaa; // zielony jasnosc poziom 5 (najjasniejszy)
  bi.ColorTable[13] = 0x55ff55; // zielony jasnosc poziom 4
  bi.ColorTable[12] = 0x00ff00; // zielony jasnosc poziom 3
  bi.ColorTable[11] = 0x00c000; // zielony jasnosc poziom 2
  bi.ColorTable[10] = 0x00a000; // zielony jasnosc poziom 1
  bi.ColorTable[9] = 0x008000; // zielony zwykly
  bi.ColorTable[8] = 0x006000; // zielony ciemnosc poziom 1
  bi.ColorTable[7] = 0x005000; // zielony ciemnosc poziom 2
  bi.ColorTable[6] = 0x004000; // zielony ciemnosc poziom 3
  bi.ColorTable[5] = 0x003000; // zielony ciemnosc poziom 4
  bi.ColorTable[4] = 0x002000; // zielony ciemnosc poziom 5
  bi.ColorTable[3] = 0x001000; // zielony ciemnosc poziom 6 (najciemniejszy)
  bi.ColorTable[2] = 0xff77ff;
  bi.ColorTable[1] = 0xff77ff;
  bi.ColorTable[0] = 0x000000;

  // zapis nowych kolorow
  SetConsoleScreenBufferInfoEx(hOut, &bi);

  // zmiana rozmiaru okna konsoli
  SMALL_RECT windowSize = {0, 0, 79, 24};
  SetConsoleWindowInfo(hOut, TRUE, &windowSize);

  srand(time(NULL));

  animacja matrix;

  while (1) {
    matrix.PrzedluzCiag();
    matrix.NowaKolumna(80);
    matrix.ZmienIstniejace(20);
    matrix.Wyswietl();
    matrix.SkrocCiag();
    matrix.ZakonczCiag(10);

    for (int x = 0; x < 8; x++) {
      matrix.ZmienPierwsze();
      Sleep(10);
    }
  }

  return 0;
}
