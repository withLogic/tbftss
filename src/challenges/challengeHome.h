/*
Copyright (C) 2015-2018 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "../common.h"

#define CHALLENGES_PER_PAGE		14
#define SHOW_CHALLENGES 		0
#define SHOW_FIGHTER_DB			5
#define SHOW_MENU 				1
#define SHOW_OPTIONS 			2
#define SHOW_STATS 				3
#define SHOW_TROPHIES			4

extern void autoSizeWidgetButtons(char *group, int recenter);
extern void awardChallengeTrophies(void);
extern void awardStatsTrophies(void);
extern void blit(AtlasImage *atlasImage, int x, int y, int centered);
extern void clearInput(void);
extern int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
extern void doFighterDatabase(void);
extern void doStars(float dx, float dy);
extern void doWidgets(void);
extern void drawBackground(SDL_Texture *texture);
extern void drawFighterDatabase(void);
extern void drawOptions(void);
extern void drawStars(void);
extern void drawStats(void);
extern void drawText(int x, int y, int size, int align, SDL_Color c, const char *format, ...);
extern void drawTrophies(void);
extern void drawWidgets(char *groupName);
extern void endSectionTransition(void);
extern AtlasImage *getAtlasImage(const char *filename);
extern char *getBackgroundTextureName(int n);
extern char *getChallengeDescription(Challenge *c);
extern char *getPlanetTextureName(int n);
extern SDL_Texture *getTexture(char *filename);
extern Widget *getWidget(const char *name, const char *group);
extern void initBackground(void);
extern void initBattle(void);
extern void initFighterDatabaseDisplay(void);
extern void initOptions(void (*returnFromOptions)(void));
extern void initStatsDisplay(void);
extern void initTitle(void);
extern void initTrophiesDisplay(void);
extern void loadMission(char *filename);
extern void playMusic(char *filename, int loop);
extern void playSound(int sound);
extern void scrollBackground(float x, float y);
extern void selectWidget(const char *name, const char *group);
extern void startSectionTransition(void);
extern void stopMusic(void);
extern char *timeToString(long millis, int showHours);
extern void updateAllMissions(void);

extern App app;
extern Battle battle;
extern Colors colors;
extern Dev dev;
extern Game game;
