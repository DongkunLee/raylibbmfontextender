# Raylib Bitmap Font Loader Extender

## Introduction 


![Bitmap Font Extender](https://github.com/DongkunLee/raylibbmfontextender/blob/main/Raylib%20Bitmap%20Font%20Loader%20Extender.png)

This is extender for raylib's internal function LoadBMFont() 

You can load mutiple atlas fnt font by this. 

## Usage 

Just call 
     Font fontnice = LoadBMFontEX("Yourfont.fnt");

rather than 
    Font font= Loadont("Yourfont.fnt");


## License 

Feel free to use. You can use this at any purpose. 

## Further more

[My Homepage Explanation](https://lawwiki.kr/doku.php/raylib:util:raylib_loadbmfont_extender)


## History 
2023. 11. 13. 
1. Modify font rect position 

<pre>
<code>
         font.recs[i] = (Rectangle){ (float)charX, (float)charY + (float)imHeight * pageID, (float)charWidth, (float)charHeight };
</code>
</pre>
  Prior version uses constant 1024 rather than (float)imHeight

2. Dynamic allocation of imPath variable
<pre>
<code>
        char** imPath; 
        imPath = malloc(sizeof(char) * 100);  // imPath Initialization
</code>
</pre>


2023. 11. 2. 
Version 1.0.
Created  By Dongkun Lee 
