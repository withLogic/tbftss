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

#include "challenges.h"

static void updateTimeChallenge(Challenge *c);
static void updateAccuracyChallenge(Challenge *c);
static void updateArmourChallenge(Challenge *c);
static void updateLossesChallenge(Challenge *c);
static void updatePlayerKillsChallenge(Challenge *c);
static void updateDisabledChallenge(Challenge *c);
static void completeChallenge(void);
static void failChallenge(void);
static void updateChallenges(void);
static char *getFormattedChallengeDescription(const char *format, ...);
char *getChallengeDescription(Challenge *c);

static char descriptionBuffer[MAX_DESCRIPTION_LENGTH];
static const char *challengeDescription[CHALLENGE_MAX];

void initChallenges(void)
{
	Mission *mission, *tail;
	char **filenames;
	char path[MAX_FILENAME_LENGTH];
	int count, i;
	
	challengeDescription[CHALLENGE_ARMOUR] = _("Retain at least %d%% armour");
	challengeDescription[CHALLENGE_TIME] = _("Complete challenge in %d seconds or less");
	challengeDescription[CHALLENGE_ACCURACY] = _("Attain a %d%% hit accuracy");
	challengeDescription[CHALLENGE_NO_LOSSES] = _("Do not lose any team mates");
	challengeDescription[CHALLENGE_1_LOSS] = _("Do not lose more than 1 team mate");
	challengeDescription[CHALLENGE_LOSSES] = _("Do not lose more than %d team mates");
	challengeDescription[CHALLENGE_PLAYER_KILLS] = _("Take down %d enemy targets");
	challengeDescription[CHALLENGE_DISABLE] = _("Disable %d or more enemy fighters");
	challengeDescription[CHALLENGE_TIME_MINS] = _("Complete challenge in %d minutes or less");
	
	tail = &game.challengeMissionHead;
	
	filenames = getFileList(getFileLocation("data/challenges"), &count);
	
	for (i = 0 ; i < count ; i++)
	{
		sprintf(path, "data/challenges/%s", filenames[i]);
		
		mission = loadMissionMeta(path);
		tail->next = mission;
		tail = mission;
		
		free(filenames[i]);
	}
	
	free(filenames);
}

void doChallenges(void)
{
	if (battle.challengeData.isChallenge && battle.status == MS_IN_PROGRESS)
	{
		if (battle.challengeData.timeLimit > 0 && battle.stats[STAT_TIME] / FPS >= battle.challengeData.timeLimit)
		{
			failChallenge();
		}
		
		if (battle.challengeData.killLimit > 0 && battle.stats[STAT_ENEMIES_KILLED_PLAYER] >= battle.challengeData.killLimit)
		{
			completeChallenge();
		}
		
		if (battle.status != MS_IN_PROGRESS)
		{
			updateChallenges();
		}
	}
}

static void updateChallenges(void)
{
	Challenge *c;
	
	for (c = game.currentMission->challengeHead.next ; c != NULL ; c = c->next)
	{
		if (!c->passed)
		{
			switch (c->type)
			{
				case CHALLENGE_TIME:
				case CHALLENGE_TIME_MINS:
					updateTimeChallenge(c);
					break;
					
				case CHALLENGE_ACCURACY:
					updateAccuracyChallenge(c);
					break;
					
				case CHALLENGE_ARMOUR:
					updateArmourChallenge(c);
					break;
					
				case CHALLENGE_NO_LOSSES:
				case CHALLENGE_1_LOSS:
				case CHALLENGE_LOSSES:
					updateLossesChallenge(c);
					break;
					
				case CHALLENGE_PLAYER_KILLS:
					updatePlayerKillsChallenge(c);
					break;
					
				case CHALLENGE_DISABLE:
					updateDisabledChallenge(c);
					break;
			}
		}
	}
}

static void updateTimeChallenge(Challenge *c)
{
	switch (c->type)
	{
		case CHALLENGE_TIME:
			if (battle.stats[STAT_TIME] / FPS < c->value)
			{
				c->passed = 1;
			}
			break;
		
		case CHALLENGE_TIME_MINS:
			if ((battle.stats[STAT_TIME] / FPS) / 60 < c->value)
			{
				c->passed = 1;
			}
			break;
	}
}

static void updateAccuracyChallenge(Challenge *c)
{
	float percent;
	
	percent = battle.stats[STAT_SHOTS_HIT];
	percent /= battle.stats[STAT_SHOTS_FIRED];
	percent *= 100;
	
	if (percent >= c->value)
	{
		c->passed = 1;
	}
}

static void updateArmourChallenge(Challenge *c)
{
	float percent;
	
	percent = player->health;
	percent /= player->maxHealth;
	percent *= 100;
	
	if (percent >= c->value)
	{
		c->passed = 1;
	}
}

static void updateLossesChallenge(Challenge *c)
{
	if (!c->passed)
	{
		c->passed = battle.stats[STAT_ALLIES_KILLED] <= c->value;
	}
}

static void updatePlayerKillsChallenge(Challenge *c)
{
	if (!c->passed)
	{
		c->passed = battle.stats[STAT_ENEMIES_KILLED_PLAYER] >= c->value;
	}
}

static void updateDisabledChallenge(Challenge *c)
{
	if (!c->passed)
	{
		c->passed = battle.stats[STAT_ENEMIES_DISABLED] >= c->value;
	}
}

char *getChallengeDescription(Challenge *c)
{
	return getFormattedChallengeDescription(challengeDescription[c->type], c->value);
}

Challenge *getChallenge(Mission *mission, int type, int value)
{
	Challenge *c;
	
	for (c = mission->challengeHead.next ; c != NULL ; c = c->next)
	{
		if (c->type == type && c->value == value)
		{
			return c;
		}
	}
	
	return NULL;
}

static char *getFormattedChallengeDescription(const char *format, ...)
{
	va_list args;

	memset(&descriptionBuffer, '\0', sizeof(descriptionBuffer));

	va_start(args, format);
	vsprintf(descriptionBuffer, format, args);
	va_end(args);
	
	return descriptionBuffer;
}

void updateChallengeMissions(void)
{
	Mission *m;
	Challenge *c;
	
	for (m = game.challengeMissionHead.next ; m != NULL ; m = m->next)
	{
		m->totalChallenges = m->completedChallenges = 0;
		
		for (c = m->challengeHead.next ; c != NULL ; c = c->next)
		{
			m->totalChallenges++;
			
			if (c->passed)
			{
				m->completedChallenges++;
			}
		}
	}
}

static void completeChallenge(void)
{
	if (battle.status == MS_IN_PROGRESS)
	{
		battle.status = MS_COMPLETE;
		battle.missionFinishedTimer = FPS;
		selectWidget("continue", "battleWon");
		
		game.stats[STAT_CHALLENGES_COMPLETED]++;
		
		retreatAllies();
		
		retreatEnemies();
		
		player->flags |= EF_IMMORTAL;
	}
}

static void failChallenge(void)
{
	if (battle.status == MS_IN_PROGRESS)
	{
		battle.status = MS_FAILED;
		battle.missionFinishedTimer = FPS;
		selectWidget("retry", "battleLost");
		
		retreatAllies();
		
		retreatEnemies();
		
		player->flags |= EF_IMMORTAL;
	}
}