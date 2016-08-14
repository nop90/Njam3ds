//-----------------------------------------------------------------------------
//  Filename:   njam.cpp
//  Created:    30. May 2003. by Milan Babuskov
//
//  Purpose:    Initialization of the game
//              Managing resources
//              Handling Main Menu
//
//  The order of functions in this file is alphabetical
//  Except for main() and NjamEngine's ctor and destructor.
//
//  Tabs should be at 4 spaces.
//  Each section is separated with line: //-------...
/*-----------------------------------------------------------------------------
Copyright 2003 Milan Babuskov

This file is part of Njam (http://njam.sourceforge.net).

Njam is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Njam is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Njam in file COPYING; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
-----------------------------------------------------------------------------*/
// needed for chdir
#ifdef __linux__
/* this must be done before the first include of unistd.h for setresgid */
#define _GNU_SOURCE
#include <defaults.h>
#endif
#include <unistd.h>


#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "3ds/SDL_3ds.h"

#include "njamutils.h"
#include "njamfont.h"
#include "njammap.h"
#include "njam.h"

static FILE* highscore_fp;

//-----------------------------------------------------------------------------
int main()
{
    bool Fullscreen = true;
    bool SWSurface = true;
    bool UseDGA = false;
    highscore_fp = fopen("/3ds/Njam3ds/njam.hs", "r+");

    // cd to game directory
    char path[300];    // should be enough
    sprintf(path, "%s/%s", "3ds", "Njam3ds");
//    printf("%s",path);
    chdir(path);

    NjamEngine Engine;
    if (!Engine.Init(Fullscreen, SWSurface, UseDGA))
    {
        return 1;
    }

//    try
//    {
        Engine.Start();
/*    }
    catch (...)
    {
        printf("Caught exception.\n");
    }
 */   return 0;
}
//-----------------------------------------------------------------------------
NjamEngine::NjamEngine()
{
    m_LastHiscore = -1;

    m_Skins = NULL;
    m_ScriptDelay = -350;   // when it's less than 100, the InfoWindow is empty
    script_file = NULL;
    m_ScriptFilePos = 0;
    m_ScriptLastPos = 0;
    m_ScrollOffset = 810;
    m_Screen = NULL;
    m_Screen2 = NULL;
    m_FontBlue = NULL;
    m_FontYellow = NULL;
    m_AudioAvailable = false;
    m_NetworkAvailable = false;
//    m_ServerSocket = NULL;
    m_SDL = false;

    m_Icon = NULL;
    m_MainMenuImage = NULL;
    m_OptionsMenuImage = NULL;
    m_StatsImage = NULL;
    m_SpriteImage = NULL;
    m_HiScoresImage = NULL;
    m_LevelsetImage = NULL;
    m_NetSendImage = NULL;
    m_NetEnterImage = NULL;
    m_NetLobbyImage = NULL;
    m_GameOverImage = NULL;

    m_MainMenuMusic = NULL;
    m_GameMusic1 = NULL;
    m_GameMusic2 = NULL;

    // default values for game options (Init() overrides if njam.conf file exists)
    m_GameOptions.PlayMusic = true;
    m_GameOptions.PlaySound = true;
    m_GameOptions.UsedSkin = 0;
    m_GameOptions.ServerIP[0] = 0;
}
//-----------------------------------------------------------------------------
NjamEngine::~NjamEngine()
{

    if (script_file)
        fclose(script_file);

    if (m_Skins)
    {
        for (int i=0; i<m_NumberOfSkins; i++)
            SDL_FreeSurface(m_Skins[i]);
        delete[] m_Skins;
    }

 //   if (m_Icon)
 //       SDL_FreeSurface(m_Icon);
    if (m_MainMenuImage)
        SDL_FreeSurface(m_MainMenuImage);
    if (m_OptionsMenuImage)
        SDL_FreeSurface(m_OptionsMenuImage);
    if (m_OptionsMenuImage)
        SDL_FreeSurface(m_Background);
    if (m_GameOverImage)
        SDL_FreeSurface(m_GameOverImage);
    if (m_StatsImage)
        SDL_FreeSurface(m_StatsImage);
    if (m_SpriteImage)
        SDL_FreeSurface(m_SpriteImage);
    if (m_HiScoresImage)
        SDL_FreeSurface(m_HiScoresImage);
    if (m_LevelsetImage)
        SDL_FreeSurface(m_LevelsetImage);
    if (m_NetSendImage)
        SDL_FreeSurface(m_NetSendImage);
    if (m_NetEnterImage)
        SDL_FreeSurface(m_NetEnterImage);
    if (m_NetLobbyImage)
        SDL_FreeSurface(m_NetLobbyImage);

    if (m_FontBlue)
        delete m_FontBlue;
    if (m_FontYellow)
        delete m_FontYellow;

    for (int i=0; i<17; i++)
        if (m_Sounds[i])
            Mix_FreeChunk(m_Sounds[i]);

    if (m_MainMenuMusic)
        Mix_FreeMusic(m_MainMenuMusic);
    if (m_GameMusic1)
        Mix_FreeMusic(m_GameMusic1);
    if (m_GameMusic2)
        Mix_FreeMusic(m_GameMusic2);

    if (m_AudioAvailable)
        Mix_CloseAudio();

//    if (m_NetworkAvailable)
//        SDLNet_Quit();

    // write configuration options
    char *home = getenv("HOME");
    std::string njamconf("njam.conf");
    if (home)
        njamconf = std::string(home) + "/.njamconf";
    FILE *fp = fopen(njamconf.c_str(), "w+");
    if (fp)
    {
        fprintf(fp, "M=%d\n", (m_GameOptions.PlayMusic ? 1 : 0));
        fprintf(fp, "S=%d\n", (m_GameOptions.PlaySound ? 1 : 0));
        fprintf(fp, "U=%d\n", m_GameOptions.UsedSkin);
        fprintf(fp, "I=%s\n", m_GameOptions.ServerIP);
        fclose(fp);
    }

        // format: NAME#POINTS#LEVEL#
    if (highscore_fp)
    {
        rewind(highscore_fp);
        for (int i=0; i<10; i++)
            fprintf(highscore_fp, "%s#%d#%d#\n", TopTenScores[i].name, TopTenScores[i].points, TopTenScores[i].level);
        fclose(highscore_fp);
    }

    if (m_SDL)
        SDL_Quit();
}
//---------------------------------------------------------------------------
void NjamEngine::DoScript(void)     // loads script.txt file, and show data on main-menu screen
{
    if (m_ScriptDelay < -100)   // initial delay, so screen isn't too full when the game is loaded
    {
        m_FontBlue->WriteTextColRow(m_Screen2, 1, 1, "NJAM-3DS  ***  PORTED TO 3DS BY NOP90  ***");
        m_ScriptDelay++;
        return;
    }

    m_ScriptDelay--;
    if (m_ScriptDelay <= 0)     // change the script
    {
        m_ScriptDelay = 400;    // delay change for 400 cycles
        m_ScriptFilePos = m_ScriptLastPos;
    }

    if (script_file == NULL)
        script_file = fopen("romfs:/data/script.txt", "rb");
    if (script_file == NULL)
        return;

    fseek(script_file, m_ScriptFilePos, SEEK_SET);

    char buffer[52];
    for (int i=0; i<52; i++)    // empty buffer
        buffer[i] = 0;

    // first line of text
    int Row = 75 / m_FontBlue->GetCharHeight();
    int Column = 36 / m_FontBlue->GetCharWidth();

    while (true)
    {
        fgets(buffer, 51, script_file);
        if (feof(script_file))
        {
            printf("njam.cpp: ERROR: Script.txt not terminated with -- line.");
            m_ScriptLastPos = 0;
            break;
        }

        if (buffer[0] == '-' && buffer[1] == '-')   // -- end of script
        {
            m_ScriptLastPos = 0;
            break;
        }

        if (buffer[0] == '-')                   // end of part
        {
            m_ScriptLastPos = ftell(script_file);
            break;
        }

        if (buffer[0] == '<')                   // image
        {
            int i = (int)(buffer[2] - '0');     // image index

            if (buffer[1] == 'G')               // image of a ghost
            {
                SDL_Rect src, dest;
                NjamSetRect(dest, 36, 20);
                NjamSetRect(src, i*25,   0, 25, 25);
                SDL_BlitSurface(m_SpriteImage, &src, m_Screen2, &dest);
            }
            else                                // image of a map-data
            {
                i+=2;

                for (int j=0; j<m_NumberOfSkins; j++)
                {
                    SDL_Rect src, dest;
                    NjamSetRect(dest, 36 + j*30, 20);
                    NjamSetRect(src,        i*25,   0, 25, 25);
                    SDL_BlitSurface(m_Skins[j], &src, m_Screen2, &dest);
                }
            }
        }
        else    // text
        {
            m_FontBlue->WriteTextColRow(m_Screen2, Column, Row, buffer);
            Row++;
        }
    }
}
//-----------------------------------------------------------------------------
bool NjamEngine::Init(bool Fullscreen, bool SoftwareSurface, bool UseDGA)
{
    char buf[512];
    snprintf(buf, sizeof(buf), "romfs:/levels");
//    NjamCheckAndCreateDir(buf);

    if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER ) < 0 )
    {
        LogFile("Error initializing SDL\n");
        return false;
    }
    else
        m_SDL = true;

    Uint32 flags = SDL_ANYFORMAT;

    if (Fullscreen)
        flags |= SDL_FULLSCREEN;

    if (SoftwareSurface)
        flags |= SDL_SWSURFACE;
    else
        flags |= SDL_HWSURFACE|SDL_DOUBLEBUF;

    m_Screen = SDL_SetVideoMode(400, 240, 32, flags);
    m_Screen2 = SDL_SetVideoMode(320, 240, 32, flags);
    if ( m_Screen == NULL )
    {
        LogFile("Couldn't set video mode\n");
        return false;
    }
    else

    // Loading various resources...
    m_FontBlue = new NjamFont("romfs:/data/font-blue.png", 6*2.5, 9*2.5);
    m_FontYellow = new NjamFont("romfs:/data/font-yellow.png", 10*2.5, 15*2.5);
   // load .bmp for main menu
    m_MainMenuImage = IMG_Load("romfs:/data/mainmenu.png");
    m_OptionsMenuImage = IMG_Load("romfs:/data/options.png");
    m_Background = IMG_Load("romfs:/data/background.png");
    m_NetSendImage = IMG_Load("romfs:/data/netsend.png");
    m_HiScoresImage = IMG_Load("romfs:/data/hiscore.png");
    m_LevelsetImage = IMG_Load("romfs:/data/levelset.png");
    m_GameOverImage = IMG_Load("romfs:/data/gameover.png");
    m_NetLobbyImage = IMG_Load("romfs:/data/network.png");
    m_NetEnterImage = IMG_Load("romfs:/data/enter-ip.png");
    m_StatsImage = IMG_Load("romfs:/data/stats.png");
    m_SpriteImage = IMG_Load("romfs:/data/sprites.png");
 
    // loading skins
    char Filename[30];
    m_NumberOfSkins = 8;
    if (m_NumberOfSkins > 0)        // alloc memory for pointers
        m_Skins = new SDL_Surface *[m_NumberOfSkins];

    for (int i=0; i<m_NumberOfSkins; i++)
    {
		snprintf(Filename, 30, "romfs:/skins/Back00%i.png",i);
 		m_Skins[i] = IMG_Load(Filename);
    }

     // Load user settings from .conf file
    std::string njamconf("/3ds/Njam3ds/njam.conf");
    FILE *fp = fopen(njamconf.c_str(), "r");
    if (fp)
    {
		LogFile("Loading user settings from .conf file\n");
        char buff[20];
        while (true)
        {
            fgets(buff, 20, fp);
            if (feof(fp))
                break;

            if (buff[1] == '=')
            {
                int i;
                switch (buff[0])
                {
                    case 'M':   m_GameOptions.PlayMusic = (buff[2] == '1');     break;
                    case 'S':   m_GameOptions.PlaySound = (buff[2] == '1');     break;
                    case 'U':   m_GameOptions.UsedSkin = (buff[2] - '0');   break;
                    case 'I':   // serverIP
                        for (i=2; buff[i] != '\n' && buff[i] && i<17; i++)
                            m_GameOptions.ServerIP[i-2] = buff[i];
                        m_GameOptions.ServerIP[i] = '\0';
                        break;
                    default:
                        printf("Unknown configuration file option: %c\n", buff[0]);
                }
            }
        }
        fclose(fp);
    } 
 

    // create default hiscore
    char DefaultNames[10][10] = {
        "MILAN",        "TANJA",
        "DULIO",        "DJORDJE",
        "CLAUS",        "ENZO",
        "JOLAN",        "JAAP",
        "MARTIN",       "RICHARD"   };
    for (int i=0; i<10; i++)
    {
        TopTenScores[i].points = (int)(18 - 1.8*i) * 220;
        TopTenScores[i].level = 18 - (int)(1.9*i);
        sprintf(TopTenScores[i].name, "%s", DefaultNames[i]);
    }

    // load hiscore from file (if any)
    // format: NAME#POINTS#LEVEL#
    if (highscore_fp)
    {
        char buff[40];
        int number = 0;
        while (!feof(highscore_fp) && number < 10)
        {
            fgets(buff, 40, highscore_fp);
            int i = 0, last;

            // name
            while (buff[i] != '#' && i < 9)
            {
                TopTenScores[number].name[i] = buff[i];
                i++;
            }
            TopTenScores[number].name[i] = '\0';

            i++;
            last = i;
            while (buff[i] != '#' && i < 40)
                i++;
            if (i >= 40)
                break;
            buff[i] = '\0';
            TopTenScores[number].points = atoi(buff+last);
            i++;
            last = i;
            while (buff[i] != '#' && i < 40)
                i++;
            if (i >= 40)
                break;
            buff[i] = '\0';
            TopTenScores[number].level = atoi(buff+last);

            number++;
        }
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 1, 2048) == -1)
    {
        m_GameOptions.PlayMusic = false;
        m_GameOptions.PlaySound = false;
    }
    else
    {
        m_AudioAvailable = true;

        printf("Reserving 4 channels for sfx...\n");
        int reserved = 4;

        printf("Loading audio data ... ");
        m_MainMenuMusic = Mix_LoadMUS("romfs:/data/satisfy.wav");
        m_GameMusic1 = Mix_LoadMUS("romfs:/data/ritam.wav");;
        m_GameMusic2 = Mix_LoadMUS("romfs:/data/dali.wav");;
        m_Sounds[0] = Mix_LoadWAV("romfs:/data/dead.wav");
        m_Sounds[1] = Mix_LoadWAV("romfs:/data/dead2.wav");
        m_Sounds[2] = Mix_LoadWAV("romfs:/data/dead3.wav");
        m_Sounds[3] = Mix_LoadWAV("romfs:/data/kill.wav");
        m_Sounds[4] = Mix_LoadWAV("romfs:/data/kill2.wav");
        m_Sounds[5] = Mix_LoadWAV("romfs:/data/kill3.wav");
        m_Sounds[6] = Mix_LoadWAV("romfs:/data/killply.wav");
        m_Sounds[7] = Mix_LoadWAV("romfs:/data/mapend.wav"); 
        m_Sounds[8] = Mix_LoadWAV("romfs:/data/mapend2.wav");
        m_Sounds[9] = Mix_LoadWAV("romfs:/data/juice.wav");
        m_Sounds[10] = Mix_LoadWAV("romfs:/data/teleport.wav");
        m_Sounds[11] = Mix_LoadWAV("romfs:/data/invisible.wav"); 
        m_Sounds[12] = Mix_LoadWAV("romfs:/data/trapdoor.wav");
        m_Sounds[13] = Mix_LoadWAV("romfs:/data/freeze.wav");
        m_Sounds[14] = Mix_LoadWAV("romfs:/data/tripleding.wav"); 
        m_Sounds[15] = Mix_LoadWAV("romfs:/data/bonus.wav");
        m_Sounds[16] = Mix_LoadWAV("romfs:/data/50pts.wav");
    }

    printf("Done!\n");
 
    return true;
}
//-----------------------------------------------------------------------------
bool NjamEngine::MenuItemSelected(int& SelectedMenuItem)
{
    if (SelectedMenuItem == m_NumberOfMenuItems-1)      // last menu item = EXIT
    {
        m_ActiveMenu = mtMainMenu;                      // return to main menu...
        m_NumberOfMenuItems = 3; // 8;                        // ...which has 7 items in network enabled game, only 2 at the moment on 3ds
        SelectedMenuItem = 0;
        return true;
    }

    if (m_ActiveMenu == mtMainMenu)                     // process all items in main menu
    {
/*
        if (SelectedMenuItem == 6)                      // "LevelEditor" selected
        {
//            LevelEditor();
            return true;
        }

        if (SelectedMenuItem == 5)                      // "Options" menu item selected
*/
        if (SelectedMenuItem == 1)                      // "Options" menu item selected
        {
            m_ActiveMenu = mtOptionsMenu;
            m_NumberOfMenuItems = 4;                    // Options menu has 4 items
            SelectedMenuItem = 0;
            return true;
        }
        else                                            // some of the games is selected
        {                                               // also other menu items can get here...
            // play the game
            StartGame((GameType)SelectedMenuItem);
            if (m_NetworkAvailable)
//                Disconnect();       // safe to call - checks everthing

            // play music again (if not playing already)
            if (m_AudioAvailable && m_MainMenuMusic && m_GameOptions.PlayMusic && !Mix_PlayingMusic() && Mix_PlayMusic(m_MainMenuMusic, -1)==-1)
                printf("ERROR: njam.cpp: MenuItemSelected: Mix_PlayMusic: %s\n", Mix_GetError());

            return true;
        }
    }

    if (m_ActiveMenu == mtOptionsMenu)                  // process all items in Options menu
    {
        if (SelectedMenuItem == 0)      // music on/off
        {
            m_GameOptions.PlayMusic = !m_GameOptions.PlayMusic;
            if (!m_AudioAvailable)
                m_GameOptions.PlayMusic = false;

            if (m_GameOptions.PlayMusic)
            {
                if (m_AudioAvailable && m_MainMenuMusic && Mix_PlayMusic(m_MainMenuMusic, -1)==-1)
                    printf("ERROR: njam.cpp: MenuItemSelected: Mix_PlayMusic: %s\n", Mix_GetError());
            }
            else if (m_AudioAvailable && m_MainMenuMusic && Mix_PlayingMusic())
                Mix_HaltMusic();
        }
        else if (SelectedMenuItem == 1)     // sound on/off
        {
            m_GameOptions.PlaySound = !m_GameOptions.PlaySound;
            if (!m_AudioAvailable)
                m_GameOptions.PlaySound = false;
        }
        else if (SelectedMenuItem == 2)     // skins on/off
        {
            m_GameOptions.UsedSkin++;
            if (m_GameOptions.UsedSkin > m_NumberOfSkins)
                m_GameOptions.UsedSkin = 0;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NjamEngine::ScrollText()
{
    const char scroll_text[] = "NJAM BY MILAN BABUSKOV               USE ARROW KEYS TO NAVIGATE MENU            HIT A TO SELECT MENU ITEM               PORTED TO 3DS BY NOP90                NJAM IS A FREE GAME      ";
    m_ScrollOffset-=2;

    if (m_ScrollOffset < -3600)     // end of message
        m_ScrollOffset = 820;

    m_FontYellow->WriteTextXY(m_Screen, m_ScrollOffset, 540, scroll_text);
}
//-----------------------------------------------------------------------------
void NjamEngine::Start()
{
    // play main menu .mod
    if (m_AudioAvailable && m_GameOptions.PlayMusic && Mix_PlayMusic(m_MainMenuMusic, -1) == -1)
        printf("ERROR: njam.cpp: Mix_PlayMusic\n");

    int SelectedMenuItem = 0;
    m_NumberOfMenuItems = 3;// only 2 options in mainm enu on 3ds atm.     8;    // 7 options in main menu (can be different for other menus, but last option must be: Exit or Back)
    m_ActiveMenu = mtMainMenu;  // MainMenu is active (I used enum to make code readable)

    while (true)    // loop main menu
    {
        // this screen has animations, so we want it to look the same on all machines
        Uint32 StartTime = SDL_GetTicks();

        if (0 != SDL_BlitSurface(m_MainMenuImage, NULL, m_Screen, NULL))    // draw main image
        {
             return;
        }

        // if options -- draw options
        if (m_ActiveMenu == mtOptionsMenu)
        {
            SDL_Rect src,dest;
/*          dest.x = 50;
            dest.y = 120;
            if (0 != SDL_BlitSurface(m_OptionsMenuImage, NULL, m_Screen, &dest))
*/
            if (0 != SDL_BlitSurface(m_OptionsMenuImage, NULL, m_Screen, NULL))
            {
                return;
            }

            // draw boxes for selected options
            for (int i=0; i<2; i++)
            {
                bool option;
                switch (i)
                {
                    case 0:     option = m_GameOptions.PlayMusic;   break;
                    case 1:     option = m_GameOptions.PlaySound;   break;
                }

                NjamSetRect(dest, 350, 200+58*i, 80, 50);
                if (!option)
                {
                    dest.x = 450;
//                    dest.w = 80;
                }
                SDL_FillRect(m_Screen, &dest, SDL_MapRGB(0, 255, 255, 125));
                dest.x += 6;
                dest.y += 6;
                NjamSetRect(src, dest.x, dest.y, dest.w - 12, dest.h - 12);
                SDL_BlitSurface(m_OptionsMenuImage, &src, m_Screen, &dest);
            }

            // draw selected skin, or random
            if (m_GameOptions.UsedSkin == 0)
            {
//                NjamSetRect(dest, 110, 202, 302, 27);
//                SDL_FillRect(m_Screen, &dest, SDL_MapRGB(0, 200, 200, 20));
//                NjamSetRect(dest, 220, 400, 300, 25);
//                SDL_FillRect(m_Screen, &dest, SDL_MapRGB(0, 0, 0, 0));
                m_FontYellow->WriteTextXY(m_Screen, 350, 325, "R A N D O M");
            }
            else
            {
//                NjamSetRect(dest, 110, 202, 302, 27);
//                SDL_Rect(m_Screen, &dest, SDL_MapRGB(0, 0, 0, 0));
                NjamSetRect(dest, 350, 335);
                SDL_BlitSurface(m_Skins[m_GameOptions.UsedSkin - 1], NULL, m_Screen, &dest);
            }
        } else {
		
			// draw hiscore table
			m_FontBlue->WriteTextXY(m_Screen, 534, 160, "LEVEL SCORE");
			for (int i=0; i<10; i++)
			{
				if (i == m_LastHiscore)
				{
					SDL_Rect dst;
					NjamSetRect(dst, 394, 185+i*25, 124*2.5, 30);
					SDL_FillRect(m_Screen, &dst, SDL_MapRGB(0, 0, 0, 100));
				}

				char text[100];
				sprintf(text, "%-11s %2d %5d", TopTenScores[i].name, TopTenScores[i].level, TopTenScores[i].points);
				m_FontBlue->WriteTextXY(m_Screen, 400, 185+i*25, text);
			}
		}

        // draw menu selector - at pos: SelectedMenuItem
        SDL_Rect rsrc, rdest;
        NjamSetRect(rsrc, 100, 0, 25, 25);
        NjamSetRect(rdest, 73, 217 + 58 * SelectedMenuItem);
        if (0 != SDL_BlitSurface(m_SpriteImage, &rsrc, m_Screen, &rdest))
        {
            return;
        }

        ScrollText();   // update text that scrolls at the top of screen

        // draw a version number
        m_FontBlue->WriteTextXY(m_Screen, 75, 160, "VERSION 1.25");

        // handle a keypress
        SDL_Event event;
        SDL_PollEvent(event);    

        DoScript();     // update info window
 
		u32 keys = SDL_GetKeyState(NULL);

                    if (keys&SDLK_ESCAPE)
                    {
                        if (mtMainMenu == m_ActiveMenu)             // ESC - exit game
                            return;

                        SelectedMenuItem = m_NumberOfMenuItems - 1;
                        if (!MenuItemSelected(SelectedMenuItem))    // as if last menu item is selected (= Exit from submenu)
                            return;
                    }
                    else if (keys&SDLK_UP)         // up arrow
                    {
                        if (SelectedMenuItem > 0)
                            SelectedMenuItem--;
                    }
                    else if (keys&SDLK_DOWN)       // down arrow
                    {
                        if (SelectedMenuItem < m_NumberOfMenuItems-1)
                            SelectedMenuItem++;
                    }
                    else if (keys&SDLK_RETURN)  // ENTER
                    {
                        if (SelectedMenuItem == m_NumberOfMenuItems-1 && m_ActiveMenu == mtMainMenu)        // Exit in main = exit game
                            return;

                        if (!MenuItemSelected(SelectedMenuItem))    // show options/run game/whatever, returns false if fatal error
                            return;
                    }
 
        // About 30fps. So that it doesn't fly on fast machines...
        while (StartTime + 30 > SDL_GetTicks())
            SDL_Delay(2);

        SDL_Flip(m_Screen);
    }
}
//---------------------------------------------------------------------------
