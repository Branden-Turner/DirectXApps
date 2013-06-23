
#include "precompiled.h"
#include "script_updates.h"
#include "object.h"
#include "cmpData.h"
#include "cmpgfx.h"
#include "factory.h"
#include "graphics.h"
#include "scriptGlobal.h"
#include "gfxparticlesystem.h"
#include "compbody.h"
#include "physics.h"
#include "uisystem.h"
#include "dx9resourcemanager.h"
#include "dx9device.h"
#include "script_robot.h"
//all functions must begin with event_, and the value passed to
// the macro should be the rest of the name
#define BIND_UPDATE_FUNCTION(f) updateTable.Func(#f, event_##f)

namespace Grimelins
{
#define SAFE_GET(type, var, expr)\
  type var = expr;\
  ErrorIf(var == NULL, "%s failed to return a valid pointer\n", #expr)

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // P1_ANIMMANAGER
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void event_p1_animManager(Object* o)
  {
    ErrorIf(o==NULL, "event_p1_animManager update sent a null object\n");
    SAFE_GET(Data*, d, gFactory->getData(*o));
    SAFE_GET(gfxModel*, m, gFactory->getgfxModel(*o));
    s32 cp = d->gets32("anim_priority");
    bool changed = d->getbool("input_changed");
    s32 held = d->gets32("input_heldObject");
    s32 strafeDir = d->gets32("input_mov");

    //DETERMINE BASE ANIMATION
    ///////////////////////////
    Text base;
    if(held) {
      if(changed) {
        switch(strafeDir) {
        case 0: base = "_runforward";  break; //CARRY FWD
        case 1: base = "_runleft";     break; //CARRY LEFT
        case 2: base = "_runbackward"; break; //CARRY BACK
        case 3: base = "_runright";    break; //CARRY RIGHT
        default: ErrorIf(true, "ERROR** invalid strafe Direction in event_p1_animManager.nut\n");
        }
      }
      else {
        base = "_catchstance";                //CARRY IDLE
      }
    } else if(changed) {
      base = "_sprint";                       //FREE RUN
    } else {
      base = "_idle";                  //FREE RUN IDLE
    }

    //APPLY BASE ANIMATION (If not currently interrupted)
    if(!cp && base != d->gettext("anim_base"))
    {
      d->settext("anim_base", base);
      Text anim(d->gettext("character") + base);
      m->ChangeAnimNOW(anim);
    }
    else
      m->ChangeAnim(d->gettext("character") + d->gettext("anim_base"));
    //FACE PLAYER
    m->YawTo(d->getf32("input_lookAngle"));
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // A2_P1_INPUT
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void event_a2_p1_input(Object* p)
  {
    SAFE_GET(Data*, d, gFactory->getData(*p));
    ScriptVec3 m = d->getvec3("input_dir");
    if(!m.x && !m.z)
    {
      d->setbool("input_changed", false);
      return;
    }
    d->setbool("input_changed", true);
    m.Normalize();

    ScriptVec3 bp = SCRIPTGLOBALS->getBoss()->getPosition();
    ScriptVec3 pp = p->getPosition();
    ScriptVec3 input_p1_boss = bp - pp;
    input_p1_boss.y = 0;
    input_p1_boss.Normalize();
    d->setvec3("input_p1_boss", input_p1_boss);
    f32 angle;
    f32 prevA = d->getf32("input_lookAngle");
    f32 nextA;
    if(!d->gets32("input_heldObject"))
    {
      nextA = acos(-(m.z));
      if(m.x>0) nextA = PITWO - nextA;
      d->setvec3("input_modelFace", m);
    }
    else
    {
      nextA = input_p1_boss.x<0 ? acos(-(input_p1_boss.z)) : -acos(-(input_p1_boss.z));
      d->setvec3("input_modelFace", input_p1_boss);
    }

    //inline script spin function from a2_p1_input
    f32 high = std::max(prevA,nextA);
    f32 low = std::min(prevA, nextA);
    f32 low2 = low + PITWO;
    f32 ccw = high - low;
    f32 cw = low2 - high;
    if(ccw < cw)
    {
      if(prevA<nextA)
        angle = prevA + ccw*d->getf32("tweak_anim_rot");
      else
        angle = prevA - ccw*d->getf32("tweak_anim_rot");
    }
    else
    {
      f32 total;
      if(prevA<nextA)
        total = prevA - cw*d->getf32("tweak_anim_rot");
      else
        total = prevA + cw*d->getf32("tweak_anim_rot");
      if(total > PITWO) total -= PITWO;
      else if(total < 0) total += PITWO;
      angle = total;
    }
    d->setf32("input_lookAngle", angle);
    //these should be defined by the variables/tweakables script
    p->REQ_VAL("p1_sprint_speed", "f32");
    p->REQ_VAL("p1_carry_speed", "f32");

    //if the player is carrying an object, use carry speed variable for speed scalar
    f32 speed = (d->gets32("input_heldObject")==0) ? d->getf32("p1_sprint_speed") : d->getf32("p1_carry_speed");
    //move...
    pp.x += m.x * speed;
    pp.z += m.z * speed;
    p->setPosition(pp);

    //zero out move vector
    d->setvec3("input_dir", ScriptVec3());
    //record previous move vector
    d->setvec3("input_dirPrev", m);
  }


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // A2_BOUNDARY
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void event_a2_boundary(Object* p)
  {
    static const f32 front = -505;
    static const f32 back = 450;
    static const f32 left = -490;
    static const f32 right = 490;
    static const f32 bossFront = 40;
    static const f32 bossLeft = -95;
    static const f32 bossRight = 80;

    ScriptVec3 pos = p->getPosition();
    pos.z = clamp(pos.z, front, back);
    pos.x = clamp(pos.x, left, right);
    if(bossFront > pos.z) pos.x = clampOut(pos.x, bossLeft, bossRight);
    p->setPosition(pos);
  }

  void event_a2_vertScroll(Object* o)
  {
    ScriptVec3 pos = o->getPosition();
    pos.y += 5;

    if(pos.y > 7900)
      pos.y = -8000;

    o->setPosition(pos);
  }

  void event_a2_pickup_indicator(Object* o)
  {
    static BillBoard* bbSmall = GRAPHICS->makeRawBillBoard(Text("pickupbb.png"), ScriptVec3(0,50,0), 25.0f, 25.0f, true);
    static DX9Texture* pickupIcon  = GRAPHICS->GetDevice()->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "pickupbb.png");

    static UIElement pickupElem;
    static vec2 firstPos(0.5f,0.95f);
    static vec2 dimensions(0.25f,0.25f);


    SAFE_GET(Data*, d, gFactory->getData(*o));
    SAFE_GET(gfxModel*, m, gFactory->getgfxModel(*o));
    SAFE_GET(Body*, b, gFactory->getBody(*o));
    s32 held = d->gets32("input_heldObject");
    s32 closest = d->gets32("input_nearestObject");

    if(!held)
    {
//       if(closest)
//       {
//         Object* close = gFactory->getObjectByID(closest);
// 
//         if(close)
//         {
//           gfxModel* model = close->getComponentT<gfxModel>();
// 
//           if(model)
//           {
//             vec3 bombPos = model->GetPos();
//             bbSmall->Hide();
//             pickupElem.tex = pickupIcon;
//             pickupElem.pos = firstPos;
//             pickupElem.dim = dimensions;
//             pickupElem.type = HUD;
// 
//             GRAPHICS->GetUISystem()->AddUIElement(pickupElem);
//           }
//         }
//       }
//       else
//       {

        Body* bodyOfClosest = NULL;
        f32 sqDistMin = FLT_MAX;
        std::list<Object*>::iterator it = minesList.begin();
        for(u32 i = 0; i < minesList.size(); ++i)
        {
          Object* mine = *it;

          if(mine)
          {
            Body* mineBod = mine->getComponentT<Body>();

            if(mineBod)
            {
              vec3 bodyPos = mineBod->getPos();

              Object* player = SCRIPTGLOBALS->getPlayer1();
              if(player)
              {
                f32 distSq = (bodyPos - player->getPosition()).LengthSq();

                if(distSq < sqDistMin)
                {
                  sqDistMin = distSq;
                  bodyOfClosest = mineBod;
                }
              }

            }

          }

          it++;
        }

        if(bodyOfClosest)
        {
          vec3 bombPos = bodyOfClosest->getPos();
          bbSmall->SetPos(bombPos + vec3(0,50,0));
          bbSmall->Show();
        }
        else
          bbSmall->Hide();
//      }
    }
    else
    {
      bbSmall->Hide();
    }
  }





  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //     FUNCTION BINDING SECTION
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void bind_update_functions(void)
  {
    HSQUIRRELVM svm = CoreInstance()->Scripter;
    Sqrat::Table updateTable(svm);

    // ----- place bindings here ------
    BIND_UPDATE_FUNCTION(p1_animManager);
    BIND_UPDATE_FUNCTION(a2_p1_input);
    BIND_UPDATE_FUNCTION(a2_boundary);
    BIND_UPDATE_FUNCTION(a2_vertScroll);
    BIND_UPDATE_FUNCTION(a2_pickup_indicator);
    
    // ----- end bindings here ------
    Sqrat::RootTable(svm).Bind("update", updateTable);
  }
}