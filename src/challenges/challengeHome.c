/*
Copyright (C) 2015-2016 Parallel Realities

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

#include "challengeHome.h"

static void logic(void);
static void draw(void);
static void handleKeyboard(void);
static void drawChallenges(void);
static void doChallenges(void);
static void startChallengeMission(void);
static void drawMenu(void);
static void resume(void);
static void stats(void);
static void options(void);
static void statsOK(void);
static void returnFromOptions(void);
static void unlockChallenges(void);
static void quit(void);
static void updateChallengeMissionData(void);
static char *listRestrictions(void);

static SDL_Texture *background;
static int startIndex;
static Widget *start;
static int completedChallenges;
static SDL_Texture *planetTexture;
static PointF planet;
static int totalChallenges;
static int show;
static char challenge[3][MAX_DESCRIPTION_LENGTH];
static int passed[3];
static char timeLimit[MAX_DESCRIPTION_LENGTH];
static char restrictions[MAX_DESCRIPTION_LENGTH];

void initChallengeHome(void)
{
	startSectionTransition();
	
	stopMusic();
	
	updateAllMissions();
	
	unlockChallenges();
	
	saveGame();
	
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	memset(&app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);
	
	background = getTexture(getBackgroundTextureName(rand()));
	planetTexture = getTexture(getPlanetTextureName(rand()));
	
	battle.camera.x =  battle.camera.y = 0;
	
	planet.x = rand() % SCREEN_WIDTH;
	planet.y = rand() % SCREEN_HEIGHT;
	
	game.currentMission = NULL;
	
	startIndex = 0;
	
	show = SHOW_CHALLENGES;
	
	initBackground();
	
	start = getWidget("start", "challenges");
	start->enabled = 0;
	start->action = startChallengeMission;
	
	getWidget("resume", "challengesMenu")->action = resume;
	getWidget("stats", "challengesMenu")->action = stats;
	getWidget("options", "challengesMenu")->action = options;
	getWidget("quit", "challengesMenu")->action = quit;
	
	getWidget("ok", "stats")->action = statsOK;
	
	endSectionTransition();
	
	playMusic("music/covert_operations.mp3");
}

static void unlockChallenges(void)
{
	Mission *m;
	int i;
	
	i = completedChallenges = totalChallenges = 0;
	
	for (m = game.challengeMissionHead.next ; m != NULL ; m = m->next)
	{
		m->available = i <= completedChallenges;
		
		completedChallenges += m->completedChallenges;
		totalChallenges += m->totalChallenges;
		
		i++;
	}
}

static void logic(void)
{
	handleKeyboard();
	
	scrollBackground(-0.25, 0);
	
	doStars(0.5, 0);
	
	planet.x -= 0.25;
	if (planet.x <= -200)
	{
		planet.x = SCREEN_WIDTH + 128 + (rand() % SCREEN_WIDTH);
		planet.y = (rand() % SCREEN_HEIGHT - 128);
	}
	
	switch (show)
	{
		case SHOW_CHALLENGES:
			doChallenges();
			break;
			
		case SHOW_MENU:
			break;
			
		case SHOW_STATS:
			break;
		
		case SHOW_OPTIONS:
			break;
	}
	
	doWidgets();
}

static void doChallenges(void)
{
	Mission *c;
	
	for (c = game.challengeMissionHead.next ; c != NULL ; c = c->next)
	{
		if (app.mouse.button[SDL_BUTTON_LEFT] && collision(app.mouse.x, app.mouse.y, 3, 3, c->rect.x, c->rect.y, c->rect.w, c->rect.h))
		{
			if (c->available)
			{
				game.currentMission = c;
				
				updateChallengeMissionData();
			
				start->enabled = c->available;
			}
			
			app.mouse.button[SDL_BUTTON_LEFT] = 0;
		}
	}
}

static void updateChallengeMissionData(void)
{
	Challenge *c;
	int i;
	
	STRNCPY(timeLimit, timeToString(game.currentMission->challengeData.timeLimit, 0), MAX_DESCRIPTION_LENGTH);
	sprintf(restrictions, "%s", listRestrictions());
	
	i = 0;
	
	for (i = 0 ; i < 3 ; i++)
	{
		strcpy(challenge[i], "");
		passed[i] = 0;
	}
	
	i = 0;
	
	for (c = game.currentMission->challengeData.challengeHead.next ; c != NULL ; c = c->next)
	{
		STRNCPY(challenge[i], getChallengeDescription(c), MAX_DESCRIPTION_LENGTH);
		passed[i] = c->passed;
		
		i++;
	}
}

static char *listRestrictions(void)
{
	return "";
}

static void draw(void)
{
	drawBackground(background);
	
	blit(planetTexture, planet.x, planet.y, 1);
	
	drawStars();
	
	drawText(SCREEN_WIDTH / 2, 50, 30, TA_CENTER, colors.white, _("Challenges"));
	
	drawText(SCREEN_WIDTH / 2, 100, 24, TA_CENTER, colors.white, "%d / %d", completedChallenges, totalChallenges);
	
	drawChallenges();
	
	switch (show)
	{
		case SHOW_CHALLENGES:
			drawWidgets("challenges");
			break;
			
		case SHOW_MENU:
			drawMenu();
			break;
			
		case SHOW_STATS:
			drawStats();
			break;
			
		case SHOW_OPTIONS:
			drawOptions();
			break;
	}
}

static void drawChallenges(void)
{
	Mission *c;
	SDL_Rect r;
	int i, endIndex;
	
	r.x = 135;
	r.y = 165;
	r.w = r.h = 96;
	
	endIndex = startIndex + MAX_ITEMS;
	i = 0;
	
	for (c = game.challengeMissionHead.next ; c != NULL ; c = c->next)
	{
		c->rect = r;
		
		if (i >= startIndex && i <= endIndex)
		{
			SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 0);
			SDL_RenderFillRect(app.renderer, &r);
			
			if (game.currentMission == c)
			{
				SDL_SetRenderDrawColor(app.renderer, 64, 128, 200, SDL_ALPHA_OPAQUE);
				SDL_RenderFillRect(app.renderer, &r);
				SDL_SetRenderDrawColor(app.renderer, 128, 192, 255, SDL_ALPHA_OPAQUE);
				SDL_RenderDrawRect(app.renderer, &r);
			}
			else
			{
				SDL_SetRenderDrawColor(app.renderer, 64, 64, 64, SDL_ALPHA_OPAQUE);
				SDL_RenderDrawRect(app.renderer, &r);
			}
			
			drawText(r.x + (r.w / 2), r.y + 28, 30, TA_CENTER, colors.white, "%d", i + 1);
			
			if (c->available)
			{
				drawText(r.x + (r.w / 2), r.y + r.w + 5, 18, TA_CENTER, (c->completedChallenges < c->totalChallenges) ? colors.white : colors.green, "%d / %d", c->completedChallenges, c->totalChallenges);
			}
			else
			{
				drawText(r.x + (r.w / 2), r.y + r.w + 5, 18, TA_CENTER, colors.lightGrey, "[Locked]");
			}
			
			r.x += 150;
			
			if (r.x > SCREEN_WIDTH - 800)
			{
				r.y += 165;
				r.x = 135;
			}
		}
		
		i++;
	}
	
	r.y = SCREEN_HEIGHT - 245;
	r.x = 100;
	r.w = SCREEN_WIDTH - 200;
	r.h = 150;
	
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 128);
	SDL_RenderFillRect(app.renderer, &r);
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, 64, 64, 64, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(app.renderer, &r);
	
	r.y = 240;
	
	if (game.currentMission)
	{
		drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - r.y, 24, TA_CENTER, colors.white, game.currentMission->description);
		
		r.y -= 50;
		drawText((SCREEN_WIDTH / 2) - 25, SCREEN_HEIGHT - r.y, 18, TA_RIGHT, colors.white, "Craft: %s", game.currentMission->craft);
		drawText((SCREEN_WIDTH / 2) + 25, SCREEN_HEIGHT - r.y, 18, TA_LEFT, (passed[0]) ? colors.green : colors.white, challenge[0]);
		
		r.y -= 30;
		drawText((SCREEN_WIDTH / 2) - 25, SCREEN_HEIGHT - r.y, 18, TA_RIGHT, colors.white, "Time Limit: %s", timeLimit);
		drawText((SCREEN_WIDTH / 2) + 25, SCREEN_HEIGHT - r.y, 18, TA_LEFT, (passed[0]) ? colors.green : colors.white, challenge[1]);
		
		r.y -= 30;
		drawText((SCREEN_WIDTH / 2) - 25, SCREEN_HEIGHT - r.y, 18, TA_RIGHT, colors.white, "Restrictions: %s", restrictions);
		drawText((SCREEN_WIDTH / 2) + 25, SCREEN_HEIGHT - r.y, 18, TA_LEFT, (passed[0]) ? colors.green : colors.white, challenge[2]);
	}
}

static void drawMenu(void)
{
	SDL_Rect r;
	
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 128);
	SDL_RenderFillRect(app.renderer, NULL);
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
	
	r.w = 400;
	r.h = 400;
	r.x = (SCREEN_WIDTH / 2) - r.w / 2;
	r.y = (SCREEN_HEIGHT / 2) - r.h / 2;
	
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 0);
	SDL_RenderFillRect(app.renderer, &r);
	SDL_SetRenderDrawColor(app.renderer, 200, 200, 200, 255);
	SDL_RenderDrawRect(app.renderer, &r);
		
	drawWidgets("challengesMenu");
}

static void resume(void)
{
	show = SHOW_CHALLENGES;
}

static void options(void)
{
	show = SHOW_OPTIONS;
	
	initOptions(returnFromOptions);
}

static void stats(void)
{
	selectWidget("ok", "stats");
	
	show = SHOW_STATS;
	
	initStatsDisplay();
}

static void statsOK(void)
{
	selectWidget("resume", "challengesMenu");
	
	show = SHOW_MENU;
}

static void returnFromOptions(void)
{
	show = SHOW_MENU;
	
	selectWidget("resume", "challengesMenu");
}

static void quit(void)
{
	initTitle();
}

static void handleKeyboard(void)
{
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		switch (show)
		{
			case SHOW_CHALLENGES:
				selectWidget("resume", "challengesMenu");
				show = SHOW_MENU;
				playSound(SND_GUI_CLOSE);
				break;
			
			case SHOW_MENU:
				show = SHOW_CHALLENGES;
				break;
				
			case SHOW_OPTIONS:
			case SHOW_STATS:
				show = SHOW_MENU;
				selectWidget("resume", "challengesMenu");
				break;
		}
		
		playSound(SND_GUI_CLOSE);
		
		memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);
	}
}

static void startChallengeMission(void)
{
	initBattle();
	
	loadMission(game.currentMission->filename);
}