
#include "precompiled.h"
#include "script_robot.h"
#include "object.h"
#include "cmpData.h"
#include "cmpgfx.h"
#include "factory.h"
#include "sysinput.h"
#include "graphics.h"
#include "camera.h"
#include "uisystem.h"
#include "dx9resourcemanager.h"
#include "dx9device.h"
#include "scriptGlobal.h"
#include "userinterface.h"
#include "syssound.h"

//all functions must begin with event_, and the value passed to
// the macro should be the rest of the name
#define BIND_UPDATE_FUNCTION(f) updateTable.Func(#f, event_##f)
#define SAFE_GET(type, var, expr)\
  type var = expr;\
  ErrorIf(var == NULL, "%s failed to return a valid pointer\n", #expr)

namespace Grimelins
{  
  std::list<Object*> minesList;
  int platformMines[4] = {0};
  namespace
  {
   
  // *******  THESE ARE TWEAKABLES **********
  //time variables for how long an action takes (array elements are per phase)
  //       PHASE :      One    Two    Three
  f32 counterT[]    = { 5.2f,  6.0f,  0.0f}; // counterattack
  f32 exposeT[]     = { 6.0f,  6.59f,  6.0f}; // weak spot exposed
  f32 hitT[]        = { 0.3f,  0.3f,  0.3f}; // regular hit
  f32 crittT[]      = { 1.0f,  1.0f,  3.5f}; // hit in weak spot (critical)
  f32 resumeT[]     = { 3.1f,  3.5f,  0.5f}; // return from exposed
  f32 introInT[]    = { 5.2f,  3.0f,  2.5f}; // cinematic begins
  f32 introT[]      = { 4.0f,  6.59f,  0.0f}; // cinematic loops
  f32 introOutT[]   = { 2.0f,  0.0f,  0.5f}; // cinematic ends
  // ****************************************

  typedef void (*voidvoid)(void);
  void nop(void){;};
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // FORWARD DECLARATIONS
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //non phase functions
  void playerWin(void); void playerLose(void); void loadNextLevel(void); void hitPlayer(void); void spillMines(void); void spillMinesTwoFists(int phase);
  //base state rotation
  void idle(void); void poundAttack(void);
  //phase counterattacks (invoked by generic interface)
  void p1Update(void); void p1DmgPlayer(void); void p1Counter(void); void p1Expose(void); void p1Hit(void); void p1Critt(void); void p1Resume(void); void p1IntroIn(void); void p1Intro(void); void p1IntroOut(void);
  void p2Update(void); void p2DmgPlayer(void); void p2Counter(void); void p2Expose(void); void p2Hit(void); void p2Critt(void); void p2Resume(void); void p2IntroIn(void); void p2Intro(void); void p2IntroOut(void);
  void p3Update(void); void p3DmgPlayer(void); void p3Counter(void); void p3Expose(void); void p3Hit(void); void p3Critt(void); void p3Resume(void); void p3IntroIn(void); void p3Intro(void); void p3IntroOut(void);

  //Generic Phase Interface
  void switchPhase(void);
  void doIntroIn(void);
  void doIntro(void);
  void doIntroOut(void);
  void doCounter(void);
  void doExpose(void);
  void doHit(void);
  void doCritt(void);
  void doResume(void);

  //Utility
  inline Object* player1(void);
  inline Object* robot(void);
  inline void pause(void);
  inline void unpause(void);
  inline void timerReset(void);
  inline bool timeEllapsed(float);
  inline void nextAction(voidvoid, f32);
  inline void togglePlayerControls(bool);
  inline void addScriptTo(Object*, Text const&);
  inline void remScriptFrom(Object*, Text const&);
  void drawCutsceneEdges(void);

  //Special Robot Functionality
  Text getFacedPlatformName(void);
  Object* getFacedPlatform(void);
  void facePlayer(void);
  void facePlatform(void);
  void faceLock(void);
  void faceDir(void);
  void setPlatformTarget(vec3 const&);//identifies which platform the parameter points to, and sets platformLock global
  voidvoid faceDirFN = faceDir;
  //Robot facing variables.
  static s32 platformLock=0; //used to indicate which platform to face towards. Used in facePlatform function
  static bool downPlatforms[4] = {false, false, false, false};
  static bool playerDown = false;
  static f32 rotSpeed = 1.0f;//speed to rotate to next facing direction
  static vec3 faceDirVec(0,0,1);//used by faceDir function
    //don't change the next variable, even with your code
  static f32 robotAngle=PI; //used by all facing functions;
  Object* GODLaser=NULL;
  Object* FOLLOWLaser=NULL;
  static bool sparks = false;
  static FrameTimer sparkTimer(false);
  static float fallSpeed = 0.0f;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Robot state variables
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  static const Text scriptname = "event_a2_robot";
  bool overrideNext = false;
  int curPhase = 0;//this is the array index variable, each phase start will increment it.
  bool canBeHit = false;
  bool vulnerable = false;
  int crittCount = 0;
  f32 pauseTime = 99999;
  f32 r_markTime = 0;
  f32 r_waitTime = 0;
  f32 debugTime = 0;
  bool cutScene = true;
  FrameTimer r_time;
  bool pIntroComplete = false;//use this to indicate you wish to stay in the pIntro function (once false, pIntro will yield to pIntroOut)
  int pIntroLoopCount = 0;//counts number of times pIntro was called, for user reference
  //single actionable function for being hit, and taking an action on update
  voidvoid hitResponseFN = nop;
  voidvoid nextActionFN = nop;
  //counterattack generic call variables
  voidvoid pUpdateFN[]      = {p1Update,    p2Update,     p3Update,     nop};
  voidvoid pDmgPlayerFN[]   = {p1DmgPlayer, p2DmgPlayer,  p3DmgPlayer,  nop};
  voidvoid pIntroInFN[]     = {p1IntroIn,   p2IntroIn,    p3IntroIn,    nop};
  voidvoid pIntroFN[]       = {p1Intro,     p2Intro,      p3Intro,      nop};
  voidvoid pIntroOut[]      = {p1IntroOut,  p2IntroOut,   p3IntroOut,   nop};
  voidvoid pHitFN[]         = {p1Hit,       p2Hit,        p3Hit,        nop};
  voidvoid pCrittFN[]       = {p1Critt,     p2Critt,      p3Critt,      nop};
  voidvoid pCounterFN[]     = {p1Counter,   p2Counter,    p3Counter,    nop};
  voidvoid pExposeFN[]      = {p1Expose,    p2Expose,     p3Expose,     nop};
  voidvoid pResumeFN[]      = {p1Resume,    p2Resume,     p3Resume,     nop};
  
  enum RobotActions
  {
    R_Idle,
    R_Pound,
    R_IntroIn,
    R_Intro,
    R_IntroOut,
    R_Hit,
    R_Critt,
    R_Counter,
    R_Expose,
    R_Resume,
    R_Win,
    R_Lose,
    R_numActions
  };

  RobotActions currentAction = R_Idle;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Non Phase Functions
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  
  void playerWin(void)
  {
    //TODO: move player to platform center and face them to middle (make sure to set input_modelFace vector)
    currentAction = R_Win;
    cutScene = true;
    minesList.clear();
    vec3 dir;
    //control removal
    togglePlayerControls(false);
    //animation manager removal
    if(player1()->hasBehavior("event_p1_animManager")) player1()->scriptRemoveBehavior("event_p1_animManager");
    player1()->position = vec3(0,30, 300);
    player1()->getComponentT<gfxModel>()->YawTo(PI);

    //attach camera
    GRAPHICS->getSceneCamera()->switchScript("camera_a2win.nut");
    //animate character
    Text prefix = player1()->getComponentT<Data>()->gettext("character");
    player1()->getComponentT<gfxModel>()->ChangeAnimNOW(prefix + "_win");
    player1()->getComponentT<gfxModel>()->ChangeAnim(prefix + "_idle");

    hitResponseFN = nop;
    nextActionFN = nop;
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_off");
    if(GODLaser && GODLaser->hasBehavior("event_a2_goddEnabled"))
    {
      GODLaser->scriptRemoveBehavior("event_a2_goddEnabled");
      FOLLOWLaser->scriptRemoveBehavior("event_a2_goddEnabled");
    }
    nextAction(loadNextLevel, 3.0f);
  }
  void handleLoss(void)
  {
    gCore->Events.Post("game_over");
  }
  void playerLose(void)
  {
    minesList.clear();
    hitResponseFN = nop;
    nextActionFN = nop;
    nextAction(handleLoss, 0.0f);
  }

  void spillMinesTwoFists(int phase)
  {
    vec3 startPos;
    vec3 shootDir;

    if(phase == 3)
    {

      startPos = vec3(434.0f,65.0f,450.0);
      shootDir = vec3(-5,0.1f,-1);
      if(minesList.size() < 10)
      {
        Object* mine = FACTORY->createObject("a2mine");
        Data* mineData = FACTORY->getData(*mine);
        minesList.push_back(mine);
        mine->position = startPos;
        shootDir.x = SCRIPTGLOBALS->randfr(shootDir.x, shootDir.y);
        //Tweak hight of mines
        shootDir.y = 0.4;
        shootDir.Normalize();
        mineData->setvec3("startPos", startPos);
        mineData->setvec3("shootDir", shootDir);
        mineData->setf32("shootVel", SCRIPTGLOBALS->randfr(1.0f, 18.0f));
      }
      startPos = vec3(-430.0f,65.0f,450.0f);
      shootDir = vec3(-0.1f,5,-1);
      if(minesList.size() < 10)
      {
        Object* mine = FACTORY->createObject("a2mine");
        Data* mineData = FACTORY->getData(*mine);
        minesList.push_back(mine);
        mine->position = startPos;
        shootDir.x = SCRIPTGLOBALS->randfr(shootDir.x, shootDir.y);
        //Tweak hight of mines
        shootDir.y = 0.4;
        shootDir.Normalize();
        mineData->setvec3("startPos", startPos);
        mineData->setvec3("shootDir", shootDir);
        mineData->setf32("shootVel", SCRIPTGLOBALS->randfr(1.0f, 18.0f));
      }

      return;
    }

    //TWEAK VAR
    int numMinesToDispence = 3;
    int currentPlatform;

    if(faceDirVec.x > 0)
    {
      if(faceDirVec.z > 0)
        currentPlatform = 2;
      else
        currentPlatform = 3;
    }
    else
    {
      if(faceDirVec.z > 0)
        currentPlatform = 1;
      else
        currentPlatform = 0;
    }

    Data* robotData = FACTORY->getData(*robot());
    if(!robotData->exists("numMines", "s32"))
      robotData->sets32("numMines", 0);
    int numMines = robotData->gets32("numMines");

    for(int i = 0; i < 4; ++i)
    {
      if(i == currentPlatform)
        continue;

      switch (i)
      {
      case 0:
        startPos = vec3(-409.0f,65.0f,-505.0f);
        shootDir = vec3(-0.5f,3,1);
        break;

      case 1:
        startPos = vec3(-430.0f,65.0f,450.0f);
        shootDir = vec3(-0.1f,5,-1);
        break;

      case 2:
        startPos = vec3(434.0f,65.0f,450.0);
        shootDir = vec3(-5,0.1f,-1);
        break;

      case 3:
        startPos = vec3(404.0f,65.0f,-505.0);
        shootDir = vec3(-3,0.5f,1);
        break;
      }


      if(minesList.size() < 10 && platformMines[i] < 4)
      {
        Object* mine = FACTORY->createObject("a2mine");
        Data* mineData = FACTORY->getData(*mine);
        minesList.push_back(mine);
        mine->position = startPos;
        shootDir.x = SCRIPTGLOBALS->randfr(shootDir.x, shootDir.y);

        //Tweak hight of mines
        shootDir.y = 0.4;
        shootDir.Normalize();
        mineData->setvec3("startPos", startPos);
        mineData->setvec3("shootDir", shootDir);
        mineData->setf32("shootVel", SCRIPTGLOBALS->randfr(1.0f, 18.0f));
        ++platformMines[i];
        ++numMines;
        printDebug("P1:%d - P2: %d - P3: %d - P4 %d \n", platformMines[0], platformMines[1], platformMines[2], platformMines[3]);
      }
    }

    robotData->sets32("numMines", numMines);
    nextAction(idle, 0.92f);
  }

  void spillMines(void)
  {
    vec3 startPos;
    vec3 shootDir;

    //TWEAK VAR
    int numMinesToDispence = 3;
    int currentPlatform;

    if(faceDirVec.x > 0)
    {
      if(faceDirVec.z > 0)
        currentPlatform = 2;
      else
        currentPlatform = 3;
    }
    else
    {
      if(faceDirVec.z > 0)
        currentPlatform = 1;
      else
        currentPlatform = 0;
    }

    switch (currentPlatform)
    {
    case 0:
      startPos = vec3(-409.0f,65.0f,-505.0f);
      shootDir = vec3(-0.5f,3,1);
    	break;

    case 1:
      startPos = vec3(-430.0f,65.0f,450.0f);
      shootDir = vec3(-0.1f,5,-1);
      break;

    case 2:
      startPos = vec3(434.0f,65.0f,450.0);
      shootDir = vec3(-5,0.1f,-1);
      break;

    case 3:
      startPos = vec3(404.0f,65.0f,-505.0);
      shootDir = vec3(-3,0.5f,1);
      break;
    }

    Data* robotData = FACTORY->getData(*robot());
    if(!robotData->exists("numMines", "s32"))
      robotData->sets32("numMines", 0);
   
    int numMines = robotData->gets32("numMines");
    
    for(int i = 0; i < numMinesToDispence && minesList.size() < 10 && platformMines[currentPlatform] < 4; ++i)
    {
      Object* mine = FACTORY->createObject("a2mine");
      Data* mineData = FACTORY->getData(*mine);
      minesList.push_back(mine);
      mine->position = startPos;
      shootDir.x = SCRIPTGLOBALS->randfr(shootDir.x, shootDir.y);

      //Tweak hight of mines
      shootDir.y = 0.4;
      shootDir.Normalize();
      mineData->setvec3("startPos", startPos);
      mineData->setvec3("shootDir", shootDir);
      mineData->setf32("shootVel", SCRIPTGLOBALS->randfr(1.0f, 12.0f));
      ++platformMines[currentPlatform];
      ++numMines;
      printDebug("P1:%d - P2: %d - P3: %d - P4 %d \n", platformMines[0], platformMines[1], platformMines[2], platformMines[3]);
    }
    
    gfxModel* curPlatform = getFacedPlatform()->getComponentT<gfxModel>();
    curPlatform->ChangeAnimNOW(getFacedPlatformName() + "_shake");

    curPlatform->ChangeAnim(getFacedPlatformName() + "_static");
    robotData->sets32("numMines", numMines);

    gfxModel* robMod = robot()->getComponentT<gfxModel>();

    if(robMod)
    {
      vec3 dustPos = robot()->getPosition() + robMod->GetDirection() * 500.0f; 
      dustPos.y = 25.0f;
      GRAPHICS->dust(dustPos);
    }

    nextAction(idle, 0.92f);
  }

  void loadNextLevel(void)
  {
    gCore->Events.Post("player_win");
  }

  void hitPlayer( void )
  {
    printDebug("HA HA HA I will crush you little rock!\n");
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Base State Rotation
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void idle(void)
  {
    printDebugT("Idle Start.\n");
    currentAction = R_Idle;
    canBeHit = true;

    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_idle");
    nextAction(poundAttack, 2.5f);   
    faceDirFN = facePlayer;
  }

  void poundAttack(void)
  {
    currentAction = R_Pound;
    //SOUND->ScriptPostEvent("robot_crush", robot()->getID(), 0);
    robot()->getComponentT<Data>()->setbool("handsBlowMines", true);
    gCore->Events.PostDelayed(H("robot_fistMineToggle"), 2.0f, true);

    setPlatformTarget(player1()->getPosition());
    faceDirFN = faceLock;
    if((player1()->getPosition() - robot()->getPosition()).Length() < 525)
      FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_pound");
    else
      FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_poundfar");

    vec3 pp = player1()->position;
    pp.y = 0;
    INPUT->vibrateAdd(0.5f, 0.7f, 1.5f, 0.1f, 0.8f, pp );

    nextAction(spillMines, 1.3f);
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Phase 1 : Sweep
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void p1Update(void)
  {
    //Every half second spit out sparks
    if(sparks && sparkTimer.Get() > 0.1f)
    {
      //Calculate the spark vector
      vec3 startDir;
      vec3 sparkDir;

      int finger = rand() % 3;
      vec3 pos;

      switch(finger)
      {
        case 0:
          pos = GRAPHICS->getBoneWorldPos("Bone_Finger_MidTip_L");
          pos.y = 25.0f;

          //Calculate the spark vector
          startDir = (pos - robot()->getPosition());
          startDir.Normalize();
          sparkDir = vec3(-startDir.z, -startDir.y, startDir.x) * 20.0f;

          GRAPHICS->sparkStream(pos, sparkDir);
        break;

        case 1:
          pos = GRAPHICS->getBoneWorldPos("Bone_Finger_PinkyTip_L");
          pos.y = 25.0f;

          //Calculate the spark vector
          startDir = (pos - robot()->getPosition());
          startDir.Normalize();
          sparkDir = vec3(-startDir.z, -startDir.y, startDir.x) * 20.0f;

          GRAPHICS->sparkStream(pos, sparkDir);
        break;

        case 2:
          pos = GRAPHICS->getBoneWorldPos("Bone_Finger_PointerTip_L");
          pos.y = 25.0f;

          //Calculate the spark vector
          startDir = (pos - robot()->getPosition());
          startDir.Normalize();
          sparkDir = vec3(-startDir.z, -startDir.y, startDir.x) * 20.0f;

          GRAPHICS->sparkStream(pos, sparkDir);
        break;
      }

      sparkTimer.StartTimer();
    }

  }
  void p1DmgPlayer(void)
  {
    //find out what the robot was doing, respond accordingly
    switch(currentAction)
    {
    case R_Pound:
      {
        //toss player CCW
        vec3 rear = (player1()->getPosition() - robot()->getPosition());
        rear.y = 0;
        rear.Normalize();
        player1()->setPosition(player1()->getPosition() + vec3(rear*25));
        INPUT->vibrateAdd(0.7f, 0.2f, 0.0f, 0.0f, 0.3f );
        s32 health = player1()->getComponentT<Data>()->gets32("health");
        player1()->getComponentT<Data>()->sets32("health", health - 400);
        gCore->Events.PostToNow(player1()->GetMyListenerHandle(), "p1_hit");
        break;
      }
    case R_Counter:
      {
        //toss player CCW
        vec3 v = (robot()->getPosition() - player1()->getPosition());
        v.y = 0;
        v.Normalize();
        vec3 r = v.Cross(vec3(0,1,0));
        r += v*-0.5f;
        player1()->setPosition(player1()->getPosition() + vec3(r*5));
        INPUT->vibrateAdd(0.5f, 0.2f, 0.0f, 0.0f, 0.6f );
        s32 health = player1()->getComponentT<Data>()->gets32("health");
        player1()->getComponentT<Data>()->sets32("health", health - 50);
        gCore->Events.PostToNow(player1()->GetMyListenerHandle(), "p1_hit");

        break;
      }
    }
  }
  void p1IntroIn(void)
  {
    GRAPHICS->getSceneCamera()->switchScript("camera_a2orbit.nut");
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_sweep");
    faceDirVec = vec3(0,0,1);
    rotSpeed = 1.0f;
    faceDirFN = faceDir;
    INPUT->vibrateAdd(0.2f, 7.3f, 2.5f, 0.0f, 0.25f);
    //SOUND->ScriptPostEvent("robot_f_restest", robot()->getID(), 0);
  }
  void p1Intro(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_powerdown");
    FACTORY->getgfxModel(*robot())->ChangeAnim("robot_off");
    pIntroComplete = true;//no looping required
    //SOUND->ScriptPostEvent("robot_f_evalcomp", robot()->getID(), 0);
  }  
  void p1IntroOut(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnim("robot_powerup");
    GRAPHICS->getSceneCamera()->switchScript("camera_a2close.nut");
    INPUT->vibrateAdd(1.0f, 1.0f, 0.0f, 0.1f, 0.7f );
    //SOUND->ScriptPostEvent("robot_f_resistant", robot()->getID(), 0);
  }    
  void p1Counter(void)
  {
    faceDirVec = vec3(0,0,1);
    rotSpeed = 0.2f;
    faceDirFN = faceDir;
    INPUT->vibrateAdd(0.2f, 7.3f, 2.5f, 0.0f, 0.25f);
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_sweep");
    robot()->getComponentT<Data>()->setbool("robotsweep", true);
    //SOUND->ScriptPostEvent("robot_f_restest", robot()->getID(), 0);

    sparks = true;
    sparkTimer.StartTimer();
  }
  void p1Expose(void)
  {
    //SOUND->ScriptPostEvent("robot_f_evalcomp", robot()->getID(), 0);
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_powerdown");
    FACTORY->getgfxModel(*robot())->ChangeAnim("robot_off");
    robot()->getComponentT<Data>()->setbool("robotsweep", false);
    INPUT->vibrateAdd(0.3f, 0.2f, 0.3f, 0.0f, 0.9f );
    sparks = false;
  }
  void p1Hit(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_ouchsm");
    INPUT->vibrateAdd(0.2f, 0.2f, 0.1f, 0.0f, 0.3f );
  }
  void p1Critt(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_ouchlg");
    INPUT->vibrateAdd(0.4f, 0.4f, 0.0f, 0.6f, 0.0f );
  }
  void p1Resume(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnim("robot_powerup");
    faceDirFN = facePlayer;
    //SOUND->ScriptPostEvent("robot_f_resistant", robot()->getID(), 0);

  } 

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Phase 2 : Smash
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void p2Update(void)
  {
  }
  void p2DmgPlayer(void)
  {
    //find out what the robot was doing, respond accordingly
    switch(currentAction)
    {
    case R_Pound:
      {
        vec3 rear = (player1()->getPosition() - robot()->getPosition());
        rear.y = 0;
        rear.Normalize();
        player1()->setPosition(player1()->getPosition() + vec3(rear*25));
        INPUT->vibrateAdd(0.7f, 0.2f, 0.0f, 0.0f, 0.3f );
        s32 health = player1()->getComponentT<Data>()->gets32("health");
        player1()->getComponentT<Data>()->sets32("health", health - 500);
        gCore->Events.PostToNow(player1()->GetMyListenerHandle(), "p1_hit");

        break;
      }
    case R_Counter:
      {
        vec3 rear = (player1()->getPosition() - robot()->getPosition());
        rear.y = 0;
        rear.Normalize();
        player1()->setPosition(player1()->getPosition() + vec3(rear*5));
        INPUT->vibrateAdd(0.9f, 0.3f, 0.0f, 0.0f, 0.33f );
        s32 health = player1()->getComponentT<Data>()->gets32("health");
        player1()->getComponentT<Data>()->sets32("health", health - 250);
        gCore->Events.PostToNow(player1()->GetMyListenerHandle(), "p1_hit");

        break;
      }
    }
  }
  void lightWarning(bool on)
  {
    if(!on)
    {
      FACTORY->getObjectByID(robot()->getComponentT<Data>()->gets32("siren"))->position = vec3(0,100000,0);
    }
    else
    {
      Object* siren = FACTORY->getObjectByID(robot()->getComponentT<Data>()->gets32("siren"));
      if(platformLock == 0)
        siren->position = vec3(-390,120,-595);//bl
      else if(platformLock == 1)
        siren->position = vec3(-435,105,550);//"fl";
      else if(platformLock == 2)
        siren->position = vec3(425,105,545);//"fr";
      else
        siren->position = vec3(390,120,-595);//br";
      //SOUND->ScriptPostEvent("robot_f_platform", robot()->getID(), 0);

    }
  }
  void p2IntroIn(void)
  {
    GRAPHICS->getSceneCamera()->switchScript("camera_a2orbit.nut");
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_smashclosed");
    //SOUND->ScriptPostEvent("robot_decimate", robot()->getID(), 0);

    robot()->getComponentT<Data>()->setbool("handsBlowMines", true);
    gCore->Events.PostDelayed(H("robot_fistMineToggle"), 2.0f, true);

    faceDirVec = (-player1()->getPosition()).Normalize();
    setPlatformTarget(faceDirVec);//negative so it will be opposite platform
    faceDirFN = facePlatform;
    lightWarning(true);
    INPUT->vibrateAdd(0.5f, 0.3f, 0.0f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 0.6f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 1.2f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 1.8f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 2.4f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 3.0f, 0.0f, 0.33f);
    gfxModel* curPlatform = getFacedPlatform()->getComponentT<gfxModel>();
    curPlatform->ChangeAnimNOW(getFacedPlatformName() + "_shake");

  }
  void p2Intro(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_fall");
    FACTORY->getgfxModel(*robot())->ChangeAnim("robot_stuck");
    gfxModel* curPlatform = getFacedPlatform()->getComponentT<gfxModel>();
    curPlatform->ChangeAnimNOW(getFacedPlatformName() + "_fall");
    curPlatform->ChangeAnim(getFacedPlatformName() + "_hang");
    INPUT->vibrateAdd(1.0f, 0.1f, 0.5f, 0.0f, 0.0f);
    INPUT->vibrateAdd(0.6f, 0.1f, 1.5f, 0.0f, 0.3f);
    INPUT->vibrateAdd(0.6f, 0.1f, 2.5f, 0.0f, 0.3f);
    INPUT->vibrateAdd(0.6f, 0.1f, 3.5f, 0.0f, 0.3f);
    INPUT->vibrateAdd(0.1f, 4.1f, 0.5f, 0.0f, 0.3f);
    pIntroComplete = true;
  }
  void p2IntroOut(void)
  {
    gfxModel* curPlatform = getFacedPlatform()->getComponentT<gfxModel>();
    curPlatform->ChangeAnimNOW(getFacedPlatformName() + "_up");
    curPlatform->ChangeAnim(getFacedPlatformName() + "_static");
    GRAPHICS->getSceneCamera()->switchScript("camera_a2close.nut");
    lightWarning(false);
  }    
  void p2Counter(void)
  {
    //SOUND->ScriptPostEvent("robot_decimate", robot()->getID(), 0);
    vec3 dir = player1()->position; dir.z-=50;
    setPlatformTarget(dir);
    facePlatform();
    faceDirFN = facePlatform;
    lightWarning(true);

    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_smashclosed");
    INPUT->vibrateAdd(0.5f, 0.3f, 0.0f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 0.6f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 1.2f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 1.8f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 2.4f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 3.0f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 3.6f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 4.2f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 4.8f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 5.4f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 6.0f, 0.0f, 0.33f);
    gfxModel* curPlatform = getFacedPlatform()->getComponentT<gfxModel>();
    curPlatform->ChangeAnimNOW(getFacedPlatformName() + "_shake");
  }
  void p2Expose(void)
  {
    spillMinesTwoFists(2);
    gfxModel* curPlatform = getFacedPlatform()->getComponentT<gfxModel>();
    curPlatform->ChangeAnimNOW(getFacedPlatformName() + "_fall");
    curPlatform->ChangeAnim(getFacedPlatformName() + "_hang");
    downPlatforms[platformLock] = true;

    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_fall");
    FACTORY->getgfxModel(*robot())->ChangeAnim("robot_stuck");
    INPUT->vibrateAdd(1.0f, 0.1f, 0.5f, 0.0f, 0.0f);
    INPUT->vibrateAdd(0.6f, 0.1f, 1.5f, 0.0f, 0.3f);
    INPUT->vibrateAdd(0.6f, 0.1f, 2.5f, 0.0f, 0.3f);
    INPUT->vibrateAdd(0.6f, 0.1f, 3.5f, 0.0f, 0.3f);
    INPUT->vibrateAdd(0.1f, 4.1f, 0.5f, 0.0f, 0.3f);
  }
  void p2Hit(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_ouchsm");
    INPUT->vibrateAdd(0.2f, 0.2f, 0.1f, 0.0f, 0.3f );
  }
  void p2Critt(void)
  {

    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_ouchlg");
    INPUT->vibrateAdd(0.4f, 0.4f, 0.0f, 0.6f, 0.0f );
    lightWarning(false);

    gfxModel* curPlatform = getFacedPlatform()->getComponentT<gfxModel>();
    downPlatforms[platformLock] = false;
    curPlatform->ChangeAnimNOW(getFacedPlatformName() + "_up");
    curPlatform->ChangeAnim(getFacedPlatformName() + "_static");
  }
  void p2Resume(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_idle");
    faceDirFN = facePlayer;
    lightWarning(false);
    //SOUND->ScriptPostEvent("robot_donot_fly", robot()->getID(), 0);

    gfxModel* curPlatform = getFacedPlatform()->getComponentT<gfxModel>();
    downPlatforms[platformLock] = false;
    curPlatform->ChangeAnimNOW(getFacedPlatformName() + "_up");
    curPlatform->ChangeAnim(getFacedPlatformName() + "_static");
  } 

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Phase 3 : Smash and GOD Laser
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void p3Update(void)
  {

  }
  void p3DmgPlayer(void)
  {
    //find out what the robot was doing, respond accordingly
    switch(currentAction)
    {
    case R_Pound:
      {
        //toss player
        vec3 rear = (player1()->getPosition() - robot()->getPosition());
        rear.y = 0;
        rear.Normalize();
        player1()->setPosition(player1()->getPosition() + vec3(rear*25));
        INPUT->vibrateAdd(0.7f, 0.2f, 0.0f, 0.0f, 0.3f );
        s32 health = player1()->getComponentT<Data>()->gets32("health");
        player1()->getComponentT<Data>()->sets32("health", health - 500);
        gCore->Events.PostToNow(player1()->GetMyListenerHandle(), "p1_hit");

        break;
      }
    case R_Expose:
      {
        //toss player
        vec3 rear = (player1()->getPosition() - robot()->getPosition());
        rear.y = 0;
        rear.Normalize();
        player1()->setPosition(player1()->getPosition() + vec3(rear*5));
        INPUT->vibrateAdd(0.7f, 0.2f, 0.0f, 0.0f, 0.3f );
        s32 health = player1()->getComponentT<Data>()->gets32("health");
        player1()->getComponentT<Data>()->sets32("health", health - 250);
        gCore->Events.PostToNow(player1()->GetMyListenerHandle(), "p1_hit");

        break;
      }
    }
  }

  void p3PrepIntroReady(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_goddtell");
    FACTORY->getgfxModel(*robot())->ChangeAnim("robot_godd");
    nextAction(doIntro, 2.0f);
  }
  void p3PrepTurnToCenter(void)
  {
    rotSpeed = 0.02f;
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_idle");
    gfxModel* backRight = FACTORY->getObjectByID( robot()->getComponentT<Data>()->gets32("a2platformbr") )->getComponentT<gfxModel>();
    backRight->ChangeAnimNOW("a2platformbr_fall");
    backRight->ChangeAnim("a2platformbr_hang");
    faceDirVec = (vec3(0,0,1)).Normalize();
    faceDirFN = faceDir;
    nextAction(p3PrepIntroReady, 2.0f);
  }
  void p3PrepIntroRight(void)
  {
    platformLock = 3;
    faceDirFN = facePlatform;
    
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_smash");
    INPUT->vibrateAdd(0.5f, 0.3f, 0.0f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 0.6f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 1.2f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 1.8f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 2.4f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 3.0f, 0.0f, 0.33f);
    FACTORY->getObjectByID( robot()->getComponentT<Data>()->gets32("a2platformbr") )->getComponentT<gfxModel>()->ChangeAnimNOW("a2platformbr_shake");
    nextAction(p3PrepTurnToCenter, 3.0f);
  }
  void p3PrepIntroTurn(void)
  {
    rotSpeed = 0.2f;
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_idle");
    gfxModel* backLeft = FACTORY->getObjectByID( robot()->getComponentT<Data>()->gets32("a2platformbl") )->getComponentT<gfxModel>();
    backLeft->ChangeAnimNOW("a2platformbl_fall");
    backLeft->ChangeAnim("a2platformbl_hang");

    faceDirVec = (vec3(1,0,-1)).Normalize();
    setPlatformTarget(faceDirVec);
    faceDirFN = faceDir;

    nextAction(p3PrepIntroRight, 2.0f);
  }
  void p3PrepIntroLeft(void)
  {
    platformLock = 0;
    faceDirFN = facePlatform;

    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_smash");
    INPUT->vibrateAdd(0.5f, 0.3f, 0.0f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 0.6f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 1.2f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 1.8f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 2.4f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 3.0f, 0.0f, 0.33f);

    FACTORY->getObjectByID(robot()->getComponentT<Data>()->gets32("a2platformbl"))->getComponentT<gfxModel>()->ChangeAnimNOW("a2platformbl_shake");

    nextAction(p3PrepIntroTurn, 3.0f);
  }

  void p3IntroIn(void)
  {   
    GRAPHICS->getSceneCamera()->switchScript("camera_a2orbit.nut");
    overrideNext = true;
    GODLaser = FACTORY->createObject("a2goddLaser");
    FOLLOWLaser = FACTORY->createObject("a2laser");

    platformLock = 0;
    faceDirFN = facePlatform;

    player1()->position = vec3(350.0f, player1()->position.y,300.0f);

    nextAction(p3PrepIntroLeft, 2.0f);
  }

  void p3Intro(void)
  {
    if(pIntroLoopCount == 0)
    {
      GODLaser->scriptAddBehavior("event_a2_goddEnabled");
      FOLLOWLaser->scriptAddBehavior("event_a2_goddEnabled");
    }

    if((GODLaser->position-player1()->position).Magnitude() < 300)
      pIntroComplete = true;
  }

  void p3IntroOut(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_idle");
    GRAPHICS->getSceneCamera()->switchScript("camera_a2close.nut");
    downPlatforms[0] = true;
    downPlatforms[3] = true;

  }  

  void p3Counter(void)
  {
  } 

  void p3Expose(void)
  {
    setPlatformTarget(player1()->getPosition());
    faceDirFN = facePlatform;
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_smash");
    INPUT->vibrateAdd(0.5f, 0.3f, 0.0f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 0.6f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 1.2f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 1.8f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 2.4f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 3.0f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 3.6f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 4.2f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 4.8f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 5.4f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 6.0f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 6.6f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 7.2f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 7.8f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 8.4f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 9.0f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 9.6f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 10.2f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 10.8f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 11.8f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 12.4f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 12.0f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 13.6f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 13.2f, 0.0f, 0.33f);
    INPUT->vibrateAdd(0.5f, 0.3f, 14.8f, 0.0f, 0.33f);
    spillMinesTwoFists(3);
  }

  void p3Hit(void)
  {
    INPUT->vibrateAdd(0.2f, 0.2f, 0.1f, 0.0f, 0.3f );
    if(GODLaser->hasBehavior("event_a2_goddEnabled"))
    {
      GODLaser->scriptRemoveBehavior("event_a2_goddEnabled");
      FOLLOWLaser->scriptRemoveBehavior("event_a2_goddEnabled");
    }
  }

  void p3Critt(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_goddtell");
    FACTORY->getgfxModel(*robot())->ChangeAnim("robot_godd");
    INPUT->vibrateAdd(0.4f, 0.4f, 0.0f, 0.6f, 0.0f );
      
    if(! GODLaser->hasBehavior("event_a2_goddEnabled"))
    {
      GODLaser->scriptAddBehavior("event_a2_goddEnabled");
      FOLLOWLaser->scriptAddBehavior("event_a2_goddEnabled");
    }
  }

  void p3Resume(void)
  {
    FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_idle");
    faceDirFN = facePlayer;
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Generic Phase Handling Interface
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void switchPhase(void)
  {
    ++curPhase;
    canBeHit = false;
    vulnerable = false;
    crittCount = 0;
    hitResponseFN = doHit;
    if(curPhase==3)
      playerWin();
    else
      nextAction(doIntroIn, 0);
  }

  void doIntroIn(void)
  {
//     //BRANDEN REMOVE THE NEXT LINE (AND the Return) TO MAKE IT NOT WIN IMMIDIATELY
//     playerWin();
//     return;
    currentAction = R_IntroIn;
    cutScene = true;
    togglePlayerControls(false);
    pIntroInFN[curPhase]();
    pIntroComplete = false;
    pIntroLoopCount = 0;
    if(overrideNext==false)
      nextAction(doIntro, introInT[curPhase]);
  }

  void doIntro(void)
  {
    currentAction = R_Intro;
    pIntroFN[curPhase]();
    if(pIntroComplete)
      nextAction(doIntroOut, introT[curPhase]);
    else
      ++pIntroLoopCount;
    ErrorIf(pIntroLoopCount == (u32)(-1), "Intro should not last this many frames\n");
  }

  void doIntroOut(void)
  {
    currentAction = R_IntroOut;
    pIntroOut[curPhase]();
    canBeHit = true;
    togglePlayerControls(true);
    nextAction(idle, introOutT[curPhase]);
    cutScene = false;
  }

  void doCounter(void)
  {
    currentAction = R_Counter;
    pCounterFN[curPhase]();
    nextAction(doExpose, counterT[curPhase]);
  }

  void doExpose(void)
  {
    currentAction = R_Expose;
    vulnerable = true;
    canBeHit = true;
    hitResponseFN = doCritt;
    pExposeFN[curPhase]();
    nextAction(doResume, exposeT[curPhase]);
  }

  void doHit(void)
  {
    currentAction = R_Hit;
    canBeHit = false;
    pHitFN[curPhase]();
    nextAction(doCounter, hitT[curPhase]);
  }

  void doCritt(void)
  {
    currentAction = R_Critt;
    vulnerable = false;
    canBeHit = false;
    hitResponseFN = doHit;
    pCrittFN[curPhase]();
    if(++crittCount == 3)
      switchPhase();
    else
      nextAction(idle, crittT[curPhase]);
  }

  void doResume(void)
  {
    currentAction = R_Resume;
    canBeHit = false;
    vulnerable = false;
    hitResponseFN = doHit;
    pResumeFN[curPhase]();
    nextAction(idle, resumeT[curPhase]);
  }


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Script Event Handles
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void event_create(void)
  {
    curPhase = -1;
    SecureZeroMemory(platformMines, sizeof(int)*4);
    faceDirFN = faceDir;
    platformLock=0;
    downPlatforms[0] = downPlatforms[1] = downPlatforms[2] = downPlatforms[3] = false;
    playerDown = false;
    rotSpeed = 1.0f;
    faceDirVec = vec3(0,0,1);
    robotAngle=PI;
    GODLaser=NULL;
    FOLLOWLaser=NULL;
    sparks = false;
    sparkTimer.StartTimer();
    overrideNext = false;
    canBeHit = false;
    vulnerable = false;
    crittCount = 0;
    pauseTime = 99999;
    r_markTime = 0;
    r_waitTime = 0;
    debugTime = 0;
    cutScene = true;
    r_time.StartTimer();
    pIntroComplete = false;
    pIntroLoopCount = 0;
    hitResponseFN = nop;
    nextActionFN = nop;
    currentAction = R_Idle;
    fallSpeed = -3.0f;
    switchPhase();

  }

  void event_removeMineFromList(int platform, int ID)
  {    
    minesList.remove(FACTORY->getObjectByID(ID));
    ErrorIf(platform>3 || platform<0, "Platform out of range\n");
    printDebug("Number of mines on this plaform went from %d to %d \n", platformMines[platform], platformMines[platform] - 1);
    --platformMines[platform];
  }


  void event_update(void)
  {
    drawCutsceneEdges();
    if(playerDown)
    {
      //PLAYER falls
      vec3 pos = player1()->getPosition();
      fallSpeed *= 1.06f;
      if(fallSpeed < -20) fallSpeed = -10;
      pos.y += fallSpeed;
      player1()->position = pos;
    }
    if(gCore->GamePaused)
      return;

    s32 health = player1()->getComponentT<Data>()->gets32("health");


    if(currentAction != R_Lose)
    {
      if(health<=0)
      {
        curPhase=3;
        canBeHit = false;
        vulnerable = false;
        crittCount = 0;
        hitResponseFN = nop;
        currentAction = R_Lose;
        FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_goddtell");
        FACTORY->getgfxModel(*robot())->ChangeAnim("robot_godd");
        //.......................................... insert regular lose code here.................................................................

        //animation manager removal
        togglePlayerControls(false);
        if(player1()->hasBehavior("event_p1_animManager")) player1()->scriptRemoveBehavior("event_p1_animManager");
        player1()->position = vec3(0,30, 300);
        player1()->getComponentT<gfxModel>()->YawTo(0);

        //attach camera
        GRAPHICS->getSceneCamera()->switchScript("camera_a2lose.nut");
        //animate character
        Text prefix = player1()->getComponentT<Data>()->gettext("character");
        player1()->getComponentT<gfxModel>()->ChangeAnimNOW(prefix + "_death");


        curPhase=3;
        currentAction = R_Lose;
        if(GODLaser && GODLaser->hasBehavior("event_a2_goddEnabled"))
        {
          GODLaser->scriptRemoveBehavior("event_a2_goddEnabled");
          FOLLOWLaser->scriptRemoveBehavior("event_a2_goddEnabled");
        }
        cutScene = true;

        nextAction(playerLose, 2);
      }
      else if (!playerDown)
      {
        vec3 pp = player1()->getPosition();
        int player_platform = 3;
        if(pp.x < 0) { 
          if(pp.z<0) player_platform = 0;
          else player_platform = 1;
        }
        else if(pp.z>0) player_platform = 2;

        if(downPlatforms[player_platform] )
        {
          //THIS IS WHERE THE PLAYER BEGINS TO FALL OFF THE PLATFORM *********************
          playerDown = true;
          canBeHit = false;
          vulnerable = false;
          crittCount = 0;
          hitResponseFN = nop;
          FACTORY->getgfxModel(*robot())->ChangeAnimNOW("robot_fall");
          FACTORY->getgfxModel(*robot())->ChangeAnim("robot_winfall");
          curPhase=3;
          platformLock = player_platform;
          faceDirFN = facePlatform;

          //animation manager removal
          togglePlayerControls(false);
          if(player1()->hasBehavior("event_p1_animManager")) player1()->scriptRemoveBehavior("event_p1_animManager");
          //attach camera
          GRAPHICS->getSceneCamera()->switchScript("camera_a2fall.nut");
          //animate character
          Text prefix = player1()->getComponentT<Data>()->gettext("character");
          player1()->getComponentT<gfxModel>()->ChangeAnimNOW(prefix + "_fall");
          player1()->getComponentT<gfxModel>()->YawTo(PI);


          currentAction = R_Lose;
          if(GODLaser && GODLaser->hasBehavior("event_a2_goddEnabled"))
          {
            GODLaser->scriptRemoveBehavior("event_a2_goddEnabled");
            FOLLOWLaser->scriptRemoveBehavior("event_a2_goddEnabled");
          }
          cutScene = true;

          nextAction(playerLose, 10.0);
        }
      }
    }

    faceDirFN();
    pUpdateFN[curPhase]();
    if(timeEllapsed(r_waitTime))
      nextActionFN();
  }
  void event_hit(void)
  {
    if(canBeHit)
      hitResponseFN();
    else
    {
      //immune to hits at this point in time
    }
  }
  void event_pause(void)
  {
    if(!pauseTime)
      pause();
    else
      unpause();
  }
  void event_hitPlayer(void)
  {
    hitPlayer();
    pDmgPlayerFN[curPhase]();
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Purely Utility functionality
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  inline void timerReset(void) {
    r_markTime = r_time.Get();
  }
  inline bool timeEllapsed(float duration) {
    debugTime = r_time.Get();
    return ((debugTime - r_markTime) > duration);
  }
  inline void nextAction(voidvoid fn, f32 wait) {
    nextActionFN = fn;
    r_waitTime = wait;
    timerReset();
  }
  inline Object* player1(void) {
    ErrorIf(SCRIPTGLOBALS->getPlayer1()==NULL, "Robot state machine needed the player pointer but failed to aquire it.\n");
    return SCRIPTGLOBALS->getPlayer1();
  }
  inline Object* robot(void) {
    ErrorIf(SCRIPTGLOBALS->getBoss()==NULL, "Robot state machine needed the boss pointer but failed to aquire it.\n");
    return SCRIPTGLOBALS->getBoss();
  }

  inline void togglePlayerControls(bool on) {
    if(on) {
      addScriptTo(player1(), "event_p1_animManager");
      addScriptTo(player1(), "event_a2_p1_input");
    } else {
      remScriptFrom(player1(), "event_a2_p1_input");
      remScriptFrom(player1(), "event_p1_animManager");
    }
  }

  inline void pause(void) {
    pauseTime = r_time.Get() - r_markTime;
    ErrorIf(pauseTime==0,"Cant pause with zero time\n");
  }
  inline void unpause(void) {
    r_markTime = r_time.Get() - pauseTime;
    pauseTime = 0;
  }
  inline void addScriptTo(Object* obj, Text const& script)
  {
    ErrorIf(obj==NULL, "Cant add scripts to NULL object\n");
    ErrorIf(obj->hasBehavior(script), "%s already attached to %s (%s)\n", script.c_str(), obj->getArchetype().c_str(), obj->getInstanceName().c_str());
    obj->addBehavior(script);
    ErrorIf(!obj->hasBehavior(script), "%s failed to be attached to %s (%s)\n", script.c_str(), obj->getArchetype().c_str(), obj->getInstanceName().c_str());

  }
  inline void remScriptFrom(Object* obj, Text const& script)
  {
    ErrorIf(obj==NULL, "Cant remove scripts from NULL object\n");
    ErrorIf(!obj->hasBehavior(script), "%s was not attached to %s (%s)\n", script.c_str(), obj->getArchetype().c_str(), obj->getInstanceName().c_str());
    obj->removeBehavior(script);
    ErrorIf(obj->hasBehavior(script), "%s failed to be removed from %s (%s)\n", script.c_str(), obj->getArchetype().c_str(), obj->getInstanceName().c_str());

  }


  void drawCutsceneEdges(void)
  {
    static const f32 height = 0.3f;// % of screen height
    static const f32 speed = 0.01f;// % of screen height per frame

    static DX9Texture* edgeTex   = GRAPHICS->GetDevice()->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "black.png");
    static UIElement edgeElem;
    static vec2 cPos[] = {vec2(0.5f, 0), vec2(0.5f, 1.0f)};
    static f32 cdim = 0;
    static vec2 dimensions(1.0f,height);
    if(cutScene || cdim >0)
    {
      if(!cutScene) cdim -= speed;
      else if (cdim < height) cdim += speed;
      for (int i = 0; i < 2; ++i)
      {
        edgeElem.tex = edgeTex;
        edgeElem.pos = cPos[i];
        edgeElem.dim = vec2(1.0f, cdim);
        edgeElem.type = HUD;
        GRAPHICS->GetUISystem()->AddUIElement(edgeElem);
      }
    }
    if(gCore->GamePaused)
    {
      USERINTERFACE->vUpdateSystem();
    }
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Robot specific functions
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  Text getFacedPlatformName(void)
  {
    setPlatformTarget(faceDirVec);
    Text ext = "";
    if(platformLock == 0)
      ext = "bl";
    else if(platformLock == 1)
      ext = "fl";
    else if(platformLock == 2)
      ext = "fr";
    else
      ext = "br";

    return "a2platform"  + ext;

  }
  Object* getFacedPlatform(void)
  {

    int id = robot()->getComponentT<Data>()->gets32(getFacedPlatformName());
    return FACTORY->getObjectByID(id);
  }
  
  void facePlayer(void)
  {
    static const f32 faceSpeed = 0.02f;
    rotSpeed = faceSpeed;
    faceDirVec = player1()->getPosition() - robot()->getPosition();
    faceDirVec.y=0;
    faceDirVec.Normalize();
    faceDir();
  }
  void faceLock(void)
  {
    faceDir();
  }
  void facePlatform(void)
  {
    static const f32 faceSpeed = 0.7f;
    static const vec3 platforms[] = {vec3(-1,0,-0.65f).Normalize(), vec3(-0.65f,0,1).Normalize(), vec3(1,0,0.85f).Normalize(), vec3(1,0,-1).Normalize()};
    ErrorIf(platformLock<0||platformLock>3, "Valid range for platformLock is 0 to 3, CCW from back left.\n");
    if(platformLock<0 || platformLock>3) platformLock = 0;
    faceDirVec = platforms[platformLock];
    rotSpeed = faceSpeed;
    faceDir();
  }
  void faceDir(void) //assumes facePlayer is current facing
  {
    ErrorIf(rotSpeed>1.0||rotSpeed<=0,"rotSpeed is out of bounds [0-1]\n");
    ErrorIf(faceDirVec.Normalized() != faceDirVec, "faceDirVec was not normalized!\n");
    f32 drag = 1.0f - rotSpeed;
    vec3 look(faceDirVec.x, 0, -faceDirVec.z);
    f32 angle = acos(look.z);
    if(look.x > 0.0f)
      angle = PITWO-angle;
    angle = angle*rotSpeed + robotAngle*drag;
    robotAngle = angle;
    FACTORY->getgfxModel(*robot())->YawTo(angle);
    s32 gearID = robot()->getComponentT<Data>()->gets32("gearbox");
    Object* gearbox = FACTORY->getObjectByID(gearID);
    FACTORY->getgfxModel(*gearbox)->YawTo(angle);

  }

  void setPlatformTarget( vec3 const& p )
  {
    platformLock = (p.x<0) ? (p.z<0 ? 0 : 1) : (p.z>0? 2 : 3);
  }


  }//unnamed namespace

  void bind_robot_functions(void)
  {
    HSQUIRRELVM svm = CoreInstance()->Scripter;
    Sqrat::Table updateTable(svm);

    // ----- place bindings here ------
    BIND_UPDATE_FUNCTION(create);
    BIND_UPDATE_FUNCTION(removeMineFromList);
    BIND_UPDATE_FUNCTION(update);
    BIND_UPDATE_FUNCTION(hit);
    BIND_UPDATE_FUNCTION(pause);
    BIND_UPDATE_FUNCTION(hitPlayer);
    // ----- end bindings here ------

    Sqrat::RootTable(svm).Bind("robot", updateTable);
  }
}//namespace
