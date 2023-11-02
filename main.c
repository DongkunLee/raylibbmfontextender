#include "raylib.h" 
#include "dkfont.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

int main(void) 
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1200;
    const int screenHeight = 900;

    // 게임창 만들기 
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    InitWindow(screenWidth, screenHeight, "BM Font Extender"); 

    // Font 
    Font fontnice = LoadBMFontEX("Assets/font/elandnice.fnt");
    Font fontstrawberry = LoadBMFontEX("Assets/font/gostrawberry.fnt");
    Font fontgukdae = LoadBMFontEX("Assets/font/gukdae.fnt");

    const char* beautifulhangul = "한글은 아름다워요. 예뻐요. 그림 같아요"; 
    const char* hun[7]; 
    hun[0] = "나랏말싸미 듕귁에 달아 문자와로 서로 사맛디 아니할새"; 
    hun[1] = "이런 젼차로 어린백성이 니르고자 할배 있어도";
    hun[2] = "마참내 그 뜻을 시려펴디 못할 놈이 하노라"; 
    hun[3] = "내 이를 위하여 어여삐 여겨"; 
    hun[4] = "새로 스물여덟자를 맹그나니"; 
    hun[5] = "사람마다 해여 수비녀겨 날로 쓰매";
    hun[6] = "편한케 하고져 할 따르미니라"; 
    const char* author = "대한민국 변호사, 이동근이 만들었어요. 2023. 11. 2.";

    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose())    // 게임 루프
    {
        BeginDrawing();
        ClearBackground(BLACK); 
        DrawTextEx(fontnice, beautifulhangul  , (Vector2) { 100, 110 }, 48, 5, DARKGREEN);

        for (int i =0; i < 7; i++)
        {
            Vector2 length = MeasureTextEx(fontstrawberry, hun[i], 36, 5);     // font height is 36 
            Vector2 pos = { screenWidth / 2 - length.x  / 2, (float)260 + (float)i * (float)40 }; // middle alignment
            DrawTex tEx(fontstrawberry, hun[i], pos, 36, 5, RAYWHITE ); 
        }

 
        DrawTextEx(fontgukdae, author, (Vector2) { 360, 800 }, 32, 5, VIOLET );
        EndDrawing();
    }

   UnloadFont(fontnice);
   UnloadFont(fontstrawberry);
   UnloadFont(fontgukdae);
   CloseWindow();                  // Close window and OpenGL context
   return 0;

};
