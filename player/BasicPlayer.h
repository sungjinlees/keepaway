/*
Copyright (c) 2004, Gregory Kuhlmann, Peter Stone
University of Texas at Austin
All right reserved

Based On:

Copyright (c) 2000-2003, Jelle Kok, University of Amsterdam
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the University of Amsterdam nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! \file BasicPlayer.h
<pre>
<b>File:</b>          BasicPlayer.h
<b>Project:</b>       Robocup Soccer Simulation Team: UvA Trilearn
<b>Authors:</b>       Jelle Kok
<b>Created:</b>       10/12/2000
<b>Last Revision:</b> $ID$
<b>Contents:</b>      This file contains the class definitions for the
               BasicPlayer. The BasicPlayer is the class where the
               available skills for the agent are defined.
<hr size=2>
<h2><b>Changes</b></h2>
<b>Date</b>             <b>Author</b>          <b>Comment</b>
10/12/2000       Jelle Kok       Initial version created
</pre>
*/

#ifndef _BASICPLAYER_
#define _BASICPLAYER_

#include "ActHandler.h"
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


extern Logger Log; /*!< This is a reference to Logger to write log info to*/

#define SCOPED_LOG EntryRaiiObject obj ## __LINE__ (__PRETTY_FUNCTION__);

struct EntryRaiiObject {
  EntryRaiiObject(const char *f) : f_(f) {
    Log.log(101, "Entered into >> %s", f_);
  }

  ~EntryRaiiObject() {
    Log.log(101, "Exited from << %s", f_);
  }

  const char *f_;
};

class FileLock {
private:
  int lock;
  std::string lockName;

public:
  FileLock(const std::string &name);

  ~FileLock();
};

class ScopedLock {
private:
  sem_t *sem;

public:
  ScopedLock(sem_t *sem);

  ~ScopedLock();
};

class Barrier {
public:
  Barrier(int n, size_t hash, const string name);

  ~Barrier();

  void wait();

private:
  void phase1();

  void phase2();

private:
  string name;
  int n;
  int *count;
  sem_t *mutex;
  sem_t *turnstile;
  sem_t *turnstile2;
  int wait_id;
  unordered_map<string, string> sharedMemory;
};

inline void SemTimedWait(sem_t *sem, int ms = 0) {
  if (ms > 0) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += ms * 1000000;
    ts.tv_sec += ts.tv_nsec / 1000000000;
    ts.tv_nsec %= 1000000000;
    sem_timedwait(sem, &ts);
  } else {
    sem_wait(sem);
  }
}

/*! This class defines the skills that can be used by an agent. No
    functionality is available that chooses when to execute which skill, this
    is done in the Player class. The WorldModel is used to determine the way
    in which the skills are performed. */
class BasicPlayer {
public:
  ActHandler *ACT; /*!< ActHandler to which commands can be sent        */
  WorldModel *WM;  /*!< WorldModel that contains information of world   */
  ServerSettings *SS;  /*!< All parameters used by the server               */
  PlayerSettings *PS;  /*!< All parameters used for the player              */

  ////////////////////////// LOW-LEVEL SKILLS /////////////////////////////////

  SoccerCommand alignNeckWithBody();

  SoccerCommand turnBodyToPoint(VecPosition pos,
                                int iPos = 1);

  SoccerCommand turnBackToPoint(VecPosition pos,
                                int iPos = 1);

  SoccerCommand turnNeckToPoint(VecPosition pos,
                                SoccerCommand com);

  SoccerCommand searchBall();

  SoccerCommand dashToPoint(VecPosition pos,
                            int iCycles = 1);

  SoccerCommand freezeBall();

  SoccerCommand kickBallCloseToBody(AngDeg ang,
                                    double dKickRatio = 0.16);

  SoccerCommand accelerateBallToVelocity(VecPosition vel);

  SoccerCommand catchBall();

  SoccerCommand communicate(char *str);

  SoccerCommand teleportToPos(VecPosition pos);

  SoccerCommand listenTo(ObjectT obj);

  SoccerCommand tackle();

  ////////////////////////// INTERMEDIATE SKILLS //////////////////////////////

  SoccerCommand turnBodyToObject(ObjectT o);

  SoccerCommand turnNeckToObject(ObjectT o,
                                 SoccerCommand com);

  SoccerCommand directTowards(VecPosition posTo,
                              AngDeg angWhenToTurn,
                              VecPosition *pos = NULL,
                              VecPosition *vel = NULL,
                              AngDeg *angBody = NULL);

  SoccerCommand moveToPos(VecPosition posTo,
                          AngDeg angWhenToTurn,
                          double dDistDashBack = 0.0,
                          bool bMoveBack = false,
                          int iCycles = 1);

  SoccerCommand collideWithBall();

  SoccerCommand interceptClose();

  SoccerCommand interceptCloseGoalie();

  SoccerCommand kickTo(VecPosition posTarget,
                       double dEndSpeed);

  SoccerCommand turnWithBallTo(AngDeg ang,
                               AngDeg angKickThr,
                               double dFreezeThr);

  SoccerCommand moveToPosAlongLine(VecPosition pos,
                                   AngDeg ang,
                                   double dDistThr,
                                   int iSign,
                                   AngDeg angThr,
                                   AngDeg angCorr);


  ////////////////////////// HIGH-LEVEL SKILLS ////////////////////////////////

  SoccerCommand intercept(bool isGoalie);

  SoccerCommand dribble(AngDeg ang,
                        DribbleT d);

  SoccerCommand directPass(VecPosition pos,
                           PassT passType);

  SoccerCommand leadingPass(ObjectT o,
                            double dDist,
                            DirectionT dir = DIR_NORTH);

  SoccerCommand throughPass(ObjectT o,
                            VecPosition posEnd,
                            AngDeg *angMax = NULL);

  SoccerCommand outplayOpponent(ObjectT o,
                                VecPosition pos,
                                VecPosition *posTo = NULL);

  SoccerCommand clearBall(ClearBallT type,
                          SideT s = SIDE_ILLEGAL,
                          AngDeg *angMax = NULL);

  SoccerCommand mark(ObjectT o,
                     double dDist,
                     MarkT mark);

  SoccerCommand defendGoalLine(double dDist);

  SoccerCommand interceptScoringAttempt();

  SoccerCommand holdBall(double dDistThr = 0.7);

  ////////////////////////// UTILITY METHODS //////////////////////////////////

  VecPosition getThroughPassShootingPoint(ObjectT objTeam,
                                          VecPosition posEnd,
                                          AngDeg *angMax);

  VecPosition getInterceptionPointBall(int *iCyclesBall,
                                       bool isGoalie);

  VecPosition getActiveInterceptionPointBall
      (int *iCyclesBall,
       bool isGoalie);

  VecPosition getDribblePoint(DribbleT dribble,
                              double *dDist);

  VecPosition getShootPositionOnLine(VecPosition p1,
                                     VecPosition p2,
                                     AngDeg *angLargest = NULL);

  double getEndSpeedForPass(ObjectT o,
                            VecPosition posPass);

  VecPosition getMarkingPosition(ObjectT o,
                                 double dDist,
                                 MarkT mark);

  /////////////////////// KEEPAWAY SKILLS //////////////////////////////////

  SoccerCommand getOpenForPassFromInRectangle(Rect rect,
                                              VecPosition posFrom);

  SoccerCommand markMostOpenOpponent(ObjectT withBall);

  VecPosition leastCongestedPointForPassInRectangle(Rect rect,
                                                    VecPosition posFrom);
};

#endif
