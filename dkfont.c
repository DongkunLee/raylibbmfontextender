/*  ========================================================
        Bitmap Font Loader Extender  

        Author : Dongkun Lee. 
        Created : 2023. 11. 2. 
        Version : 1.0 
        
        Description : 
            This is helper utility to load bitmap font(fnt) that have multiple atlas. 
            Just load fnt font by LadBMFontEx("filename");
            It support up to 10 atlas.
*/

#include "string.h"
#include "stdio.h"
#include <stdlib.h>

#include "dkfont.h"

// Load a BMFont file (AngelCode font file)
// REQUIRES: strstr(), sscanf(), strrchr(), memcpy()
Font LoadBMFontEX(const char *fileName)
{
    #define MAX_BUFFER_SIZE     256

    Font font = { 0 };

    char buffer[MAX_BUFFER_SIZE] = { 0 };
    char *searchPoint = NULL;

    int fontSize = 0;
    int glyphCount = 0;

    int imWidth = 0;
    int imHeight = 0;
    int totalPage = 1;
    int curPage = 0;
    char imFileName[10][129] = { 0 };

    int base = 0;   // Useless data

    char *fileText = LoadFileText(fileName);

    if (fileText == NULL) return font;

    char *fileTextPtr = fileText;

    // NOTE: We skip first line, it contains no useful information
    int lineBytes = GetLine(fileTextPtr, buffer, MAX_BUFFER_SIZE);
    fileTextPtr += (lineBytes + 1);

    // Read line data
    lineBytes = GetLine(fileTextPtr, buffer, MAX_BUFFER_SIZE);
    searchPoint = strstr(buffer, "lineHeight");
    sscanf(searchPoint, "lineHeight=%i base=%i scaleW=%i scaleH=%i pages=%i", &fontSize, &base, &imWidth, &imHeight, &totalPage);
    fileTextPtr += (lineBytes + 1);

    // This is just for debugging
    printf("FONT: [%s] Loaded font info:\n", fileName);
    printf("    > Base size: %i\n", fontSize);
    printf("    > Texture scale: %ix%i\n", imWidth, imHeight);

    for (int i = 0; i < totalPage; i++)
    {
        lineBytes = GetLine(fileTextPtr, buffer, MAX_BUFFER_SIZE);
        searchPoint = strstr(buffer, "id");
        sscanf(searchPoint, "id=%i file=\"%128[^\"]\"", &curPage,  imFileName[i]);

        // This is just for debugging
        printf("    > Texture filename: %s\n", imFileName[i]);

        fileTextPtr += (lineBytes + 1);
    }

    lineBytes = GetLine(fileTextPtr, buffer, MAX_BUFFER_SIZE);
    searchPoint = strstr(buffer, "count");
    sscanf(searchPoint, "count=%i", &glyphCount);
    fileTextPtr += (lineBytes + 1);

    // This is just for debugging
    printf("    > Chars count: %i\n", glyphCount);

    // Compose correct path using route of .fnt file (fileName) and imFileName
    // char *imPath[totalPage] = {0};
    char** imPath; 

    imPath = malloc(sizeof(char) * 100);  // imPath Initialization
    char *lastSlash = NULL;

    for (int i = 0; i< totalPage; i++)
    {
        lastSlash = strrchr(fileName, '/');
        if (lastSlash == NULL) lastSlash = strrchr(fileName, '\\');

        if (lastSlash != NULL)
        {
        // NOTE: We need some extra space to avoid memory corruption on next allocations!
        imPath[i] = (char *)RL_CALLOC(TextLength(fileName) - TextLength(lastSlash) + TextLength(imFileName[i]) + 4, 1);
        memcpy(imPath[i], fileName, TextLength(fileName) - TextLength(lastSlash) + 1);
        memcpy(imPath[i] + TextLength(fileName) - TextLength(lastSlash) + 1, imFileName[i], TextLength(imFileName[i]));
         }
        else imPath[i] = imFileName[i];

        // For debug
        printf("    > Image loading path: %s\n", imPath[i]);
        // TRACELOGD("    > Image loading path: %s", imPath);
    }

    Image imFont[totalPage];
    
    for (int i = 0; i < totalPage; i++)
    {
        imFont[i] =  LoadImage(imPath[i]);
        // For debug
        printf("Image [%i] width : %i\n", i,  imFont[i].width);


        if (imFont[i].format == PIXELFORMAT_UNCOMPRESSED_GRAYSCALE)
        {
            // Convert image to GRAYSCALE + ALPHA, using the mask as the alpha channel
            Image imFontAlpha = {
                .data = RL_CALLOC(imFont[i].width*imFont[i].height, 2),
                .width = imFont[i].width,
                .height = imFont[i].height,
                .mipmaps = 1,
                .format = PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA
            };

            for (int p = 0, pi = 0; p < (imFont[i].width*imFont[i].height*2); p += 2, pi++)
            {
                ((unsigned char *)(imFontAlpha.data))[p] = 0xff;
                ((unsigned char *)(imFontAlpha.data))[p + 1] = ((unsigned char *)imFont[i].data)[pi];
            }

            UnloadImage(imFont[i]);
            imFont[i] = imFontAlpha;
        }

        if (lastSlash != NULL) RL_FREE(imPath[i]);
    }

    // Resize and ReDraw Font Image 
    Image fullFont; 
    fullFont = imFont[0];

    // If multiple atlas, then merge atlas
    if (totalPage > 1)
    {
        ImageResizeCanvas(&fullFont, imWidth, imHeight * totalPage, 0, 0, BLACK); 

        for (int index = 1; index <= totalPage; index++)
        {
            Rectangle srcRec = { 0.0f, 0.0f, (float)imWidth, (float)imHeight};
            Rectangle destRec = { 0.0f, (float)imHeight * (float)index, (float)imWidth, (float)imHeight};
            ImageDraw(&fullFont, imFont[index], srcRec, destRec, WHITE);              
        }
    }

    // Image to Texure
    font.texture = LoadTextureFromImage(fullFont);

    // Fill font characters info data
    font.baseSize = fontSize;
    font.glyphCount = glyphCount;
    font.glyphPadding = 0;
    font.glyphs = (GlyphInfo *)RL_MALLOC(glyphCount*sizeof(GlyphInfo));
    font.recs = (Rectangle *)RL_MALLOC(glyphCount*sizeof(Rectangle));

    int charId, charX, charY, charWidth, charHeight, charOffsetX, charOffsetY, charAdvanceX, pageID;

    for (int i = 0; i < glyphCount; i++)
    {
        lineBytes = GetLine(fileTextPtr, buffer, MAX_BUFFER_SIZE);
        sscanf(buffer, "char id=%i x=%i y=%i width=%i height=%i xoffset=%i yoffset=%i xadvance=%i page=%i",
                       &charId, &charX, &charY, &charWidth, &charHeight, &charOffsetX, &charOffsetY, &charAdvanceX, &pageID);
        fileTextPtr += (lineBytes + 1);

        // Get character rectangle in the font atlas texture
       font.recs[i] = (Rectangle){ (float)charX, (float)charY + (float)imHeight * pageID, (float)charWidth, (float)charHeight };
        // Save data properly in sprite font
        font.glyphs[i].value = charId;
        font.glyphs[i].offsetX = charOffsetX;
        font.glyphs[i].offsetY = charOffsetY;
        font.glyphs[i].advanceX = charAdvanceX;

        // Fill character image data from imFont data
        font.glyphs[i].image = ImageFromImage(fullFont, font.recs[i]);
    }

    // Unload 
    UnloadImage(fullFont);
    UnloadFileText(fileText);

    if (font.texture.id == 0)
    {
        UnloadFont(font);
        font = GetFontDefault();
       // TRACELOG(LOG_WARNING, "FONT: [%s] Failed to load texture, reverted to default font", fileName);
       printf("FONT: [%s] Failed to load texture, reverted to default font\n", fileName);
    }
    else // TRACELOG(LOG_INFO, "FONT: [%s] Font loaded successfully (%i glyphs)", fileName, font.glyphCount);
        printf("FONT: [%s] Font loaded successfully (%i glyphs)", fileName, font.glyphCount);

    return font;
}

int GetLine(const char *origin, char *buffer, int maxLength)
{
    int count = 0;
    for (; count < maxLength; count++) if (origin[count] == '\n') break;
    memcpy(buffer, origin, count);
    return count;
}
